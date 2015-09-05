/******************************************************************************/
/*  Copyright (c) 2011 NXP B.V.  All rights are reserved.                     */
/*  Reproduction in whole or in part is prohibited without the prior          */
/*  written consent of the copyright owner.                                   */
/*                                                                            */
/*  This software and any compilation or derivative thereof is, and           */
/*  shall remain the proprietary information of NXP and is                    */
/*  highly confidential in nature. Any and all use hereof is restricted       */
/*  and is subject to the terms and conditions set forth in the               */
/*  software license agreement concluded with NXP B.V.                        */
/*                                                                            */
/*  Under no circumstances is this software or any derivative thereof         */
/*  to be combined with any Open Source Software, exposed to, or in any       */
/*  way licensed under any Open License Terms without the express prior       */
/*  written permission of the copyright owner.                                */
/*                                                                            */
/*  For the purpose of the above, the term Open Source Software means         */
/*  any software that is licensed under Open License Terms. Open              */
/*  License Terms means terms in any license that require as a                */
/*  condition of use, modification and/or distribution of a work              */
/*                                                                            */
/*  1. the making available of source code or other materials                 */
/*     preferred for modification, or                                         */
/*                                                                            */
/*  2. the granting of permission for creating derivative                     */
/*     works, or                                                              */
/*                                                                            */
/*  3. the reproduction of certain notices or license terms                   */
/*     in derivative works or accompanying documentation, or                  */
/*                                                                            */
/*  4. the granting of a royalty-free license to any party                    */
/*     under Intellectual Property Rights                                     */
/*                                                                            */
/*  regarding the work and/or any work that contains, is combined with,       */
/*  requires or otherwise is based on the work.                               */
/*                                                                            */
/*  This software is provided for ease of recompilation only.                 */
/*  Modification and reverse engineering of this software are strictly        */
/*  prohibited.                                                               */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <stdbool.h>

/*******************************************************************************
 * Project include files
 ******************************************************************************/

#include "app_config.h"
#include "FreeRTOS.h"
#include "nvmem.h"

/*******************************************************************************
 * Typedefs and defines
 ******************************************************************************/

#define FLASH_SECTOR_FOR_DATA  7            // Highest Flash sector number
#define START_OF_FLASH_SECTOR  0x00007000   // Start address of FLASH_SECTOR_FOR_DATA
#define END_OF_FLASH_SECTOR    0x00008000   // End address of FLASH_SECTOR_FOR_DATA
#define CUR_BLOCK_SIZE         256          // Length of data (in bytes) to store in Flash

#define IAP_LOCATION   0x1FFF1FF1
typedef void (*IAP)(unsigned int *pcommand, unsigned int *presult);

#define NVM_VERSION_MAJOR 2
#define NVM_VERSION_MINOR 0

typedef struct _version_t_
{
    uint16_t major;
    uint16_t minor;
} version_t;

typedef union _nvm_version_t_
{
    uint32_t  version;
    version_t ver;
} nvm_version_t;

typedef struct _nvm_layout_t_
{
    nvm_version_t       version;
    metrology_ranges_t  ranges_50Hz;
    metrology_ranges_t  ranges_60Hz;
    float               produced_counter;   /* feed back into grid */
    float               consumed_counter;
} nvm_layout_t;

/*******************************************************************************
 * local data
 ******************************************************************************/

static nvm_layout_t  nvm_storage;
static uint8_t      *pwrite_pos;

/*******************************************************************************
 * local functions
 ******************************************************************************/

static uint32_t erase_flash_sector(void)
{
    IAP      iap_entry = (IAP)IAP_LOCATION;
    uint32_t command[5];
    uint32_t result[5];

    portDISABLE_INTERRUPTS();
    command[0] = 50;                         // Prepare sector for erase
    command[1] = FLASH_SECTOR_FOR_DATA;
    command[2] = FLASH_SECTOR_FOR_DATA;      // Only 1 sector, so end sector # is start sector #
    iap_entry(command, result);
    if (result[0] == 0)
    {
        command[0] = 52;                     // Erase sector
        command[1] = FLASH_SECTOR_FOR_DATA;
        command[2] = FLASH_SECTOR_FOR_DATA;  // Only 1 sector, so end sector # is start sector #
        command[3] = SystemCoreClock / 1000;
        iap_entry(command, result);
    }
    portENABLE_INTERRUPTS();
    return result[0];
}

/*******************************************************************************
 * public functions
 ******************************************************************************/

bool nvmem_init(void)
{
    uint8_t       *fl_ptr = (uint8_t *)(END_OF_FLASH_SECTOR - CUR_BLOCK_SIZE);
    nvm_layout_t  *flash_nvm_ptr = (nvm_layout_t *)fl_ptr;
    bool           found = false;
    bool           erase_needed = false;

    // try to find latest emeter data in Flash sector
    while ((!found) && (flash_nvm_ptr >= (nvm_layout_t *)START_OF_FLASH_SECTOR))
    {
        if (flash_nvm_ptr->version.ver.major == NVM_VERSION_MAJOR)
        {
            found = true;
        }
        else
        {
            fl_ptr -= CUR_BLOCK_SIZE;
            flash_nvm_ptr = (nvm_layout_t *)fl_ptr;
        }
    }
    if (found)
    {
        nvm_storage.version          = flash_nvm_ptr->version;
        nvm_storage.ranges_50Hz      = flash_nvm_ptr->ranges_50Hz;
        nvm_storage.ranges_60Hz      = flash_nvm_ptr->ranges_60Hz;
        nvm_storage.produced_counter = flash_nvm_ptr->produced_counter;
        nvm_storage.consumed_counter = flash_nvm_ptr->consumed_counter;
        if (flash_nvm_ptr == (nvm_layout_t *)(END_OF_FLASH_SECTOR - CUR_BLOCK_SIZE))
        {
            erase_needed = true;
        }
    }
    else
    {
        nvm_storage.version.ver.major = NVM_VERSION_MAJOR;
        nvm_storage.version.ver.minor = NVM_VERSION_MINOR;
        nvm_storage.produced_counter  = (float)0.0;
        nvm_storage.consumed_counter  = (float)0.0;
        erase_needed = true;
    }
    if (erase_needed)
    {
        erase_flash_sector();
        pwrite_pos = (uint8_t *)START_OF_FLASH_SECTOR;
    }
    else
    {
        pwrite_pos = (fl_ptr + CUR_BLOCK_SIZE);
    }
    return found;
}

void nvmem_read_ranges(EM_mains_t Fmains, metrology_ranges_t *pRanges)
{
    if (Fmains == EM_50HZ)
    {
        *pRanges = nvm_storage.ranges_50Hz;
    }
    else
    {
        *pRanges = nvm_storage.ranges_60Hz;
    }
}

void nvmem_write_ranges(EM_mains_t Fmains, metrology_ranges_t *pRanges)
{
    if (Fmains == EM_50HZ)
    {
        nvm_storage.ranges_50Hz = *pRanges;
    }
    else
    {
        nvm_storage.ranges_60Hz = *pRanges;
    }
}

void nvmem_read_counters(float *pcounter, float *ecounter)
{
    *pcounter = nvm_storage.produced_counter;
    *ecounter = nvm_storage.consumed_counter;
}

void nvmem_write_counters(float pcounter, float ecounter)
{
    nvm_storage.produced_counter = pcounter;
    nvm_storage.consumed_counter = ecounter;
}

void nvmem_store_in_flash(void)
{
    IAP      iap_entry = (IAP)IAP_LOCATION;
    uint32_t command[5];
    uint32_t result[5];

    if (pwrite_pos == (uint8_t *)END_OF_FLASH_SECTOR)
    {
        erase_flash_sector();
        pwrite_pos = (uint8_t *)START_OF_FLASH_SECTOR;
    }
    portDISABLE_INTERRUPTS();
    command[0] = 50;                    // Prepare sector for write
    command[1] = FLASH_SECTOR_FOR_DATA;
    command[2] = FLASH_SECTOR_FOR_DATA; // Only 1 sector, so end sector # is start sector #
    iap_entry(command, result);
    if (result[0] == 0)
    {
        command[0] = 51;                // Copy RAM to Flash sector
        command[1] = (uint32_t)pwrite_pos;
        command[2] = (uint32_t)&nvm_storage;
        command[3] = CUR_BLOCK_SIZE;
        command[4] = SystemCoreClock / 1000;
        iap_entry(command, result);
        pwrite_pos += CUR_BLOCK_SIZE;
    }
    portENABLE_INTERRUPTS();
}

uint32_t nvmem_readID(void)
{
    IAP      iap_entry  = (IAP)IAP_LOCATION;
    uint32_t command[5] = {58,0,0,0,0}; // Read device serial number
    uint32_t result[5]  = { 0,0,0,0,0};

    iap_entry(command, result);
    if (result[0] != 0) return 0;        // not succesful
    return (((result[1] ^ result[2]) ^ result[3]) ^ result[4]);
}

/* EOF */
