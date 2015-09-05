/******************************************************************************/
/*  Copyright (c) 2010-2011 NXP B.V.  All rights are reserved.                */
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

#ifndef _METROLOGY_H_
#define _METROLOGY_H_

#include <stdint.h>

/************************************************/
/* General definitions                          */
/************************************************/

typedef struct metrology_ranges_tag
{
    float Vpp;          /* V peak-peak in Volt               */
    float I1pp;         /* I peak-peak channel1 in Ampere    */
    float I2pp;         /* I peak-peak channel2 in Ampere    */
    float DeltaPhi1;    /* phi correction in radian channel1 */
    float DeltaPhi2;    /* phi correction in radian channel2 */
} metrology_ranges_t;

typedef struct metrology_result_tag
{
    float V;            /* Voltage rms in Volt                      */
    float I;            /* Current rms in Ampere                    */
    float P;            /* Active power in Watt                     */
    float Q1;           /* Fundamental Reactive power in var        */
    float S;            /* Apparent power in VA                     */
    float S1;           /* Fundamental Apparent power in VA         */
    float PF;           /* Power Factor 0.00-1.00                   */
    float PF1;          /* Fundamental Power Factor 0.00-1.00       */
    float SN;           /* Nonfundamental Apparent power in VA      */
    float N;            /* Nonactive Power in var                   */
    float THDI;         /* Total Harmonic Distortion of the current */
} metrology_result_t;

typedef enum _EM_current_channel_t
{
    EM_CHANNEL1,        /* I_HIGHGAIN */
    EM_CHANNEL2         /* I_LOWGAIN  */
} EM_current_channel_t;

typedef enum _EM_mains_t
{
    EM_AUTO,
    EM_50HZ = 50,
    EM_60HZ = 60
} EM_mains_t;

typedef enum _EM_return_t
{
    EM_VALID,
    EM_NOT_VALID,
    EM_NOT_VALID_OVERFLOW
} EM_return_t;

/************************************************/
/* Metrology engine interface                   */
/************************************************/

/******************************************************************************/
/* Name        : metrology_init                                               */
/* Description : Initializes the metrology engine according input parameters  */
/* Parameters  :                                                              */
/*  (in) AHBClkFrequency - AHB clock in Hz as initialized by the application, */
/*                         must be 12000000, 24000000, 36000000 or 48000000.  */
/*  (in) Fmains          - The mains supply frequency (EM_50HZ or EM_60HZ),   */
/*                         or EM_AUTO to let the metrology engine detect the  */
/*                         mains supply frequency.                            */
/* Returns     : EM_50HZ or EM_60HZ.                                          */
/*               When Fmains equals EM_AUTO the metrology engine returns      */
/*               the detected mains supply frequency (default EM_60HZ)        */
/******************************************************************************/

EM_mains_t metrology_init ( uint32_t AHBClkFrequency, EM_mains_t Fmains );

/******************************************************************************/
/* Name        : metrology_set_ranges                                         */
/* Description : Set ranges for the metrology engine inputs                   */
/* Parameters  :                                                              */
/*  (in) ranges - pointer to initialized struct allocated by caller           */
/*                (see metrology_ranges_t)                                    */
/* Returns     : none                                                         */
/******************************************************************************/

void metrology_set_ranges ( metrology_ranges_t * ranges );

/******************************************************************************/
/* Name        : metrology_start                                              */
/* Description : Starts the metrology engine                                  */
/* Parameters  : none                                                         */
/* Returns     : none                                                         */
/******************************************************************************/
  
void metrology_start ( void );

/******************************************************************************/
/* Name        : metrology_stop                                               */
/* Description : Stops the metrology engine                                   */
/* Parameters  : none                                                         */
/* Returns     : none                                                         */
/******************************************************************************/

void metrology_stop ( void );

/******************************************************************************/
/* Name        : metrology_read_data                                          */
/* Description : Reads and returns the metrology engine result data           */
/*               Note: This fuction must be called at least every 950 ms.     */
/* Parameters  :                                                              */
/*  (in) metrology_result - pointer to struct allocated by caller, in which   */
/*                          metrology_result data is returned when available. */
/* Returns     : EM_VALID              - when data available,                 */
/*               EM_NOT_VALID          - when data not jet available,         */
/*               EM_NOT_VALID_OVERFLOW - when data wasn't read in time.       */
/******************************************************************************/

EM_return_t metrology_read_data ( metrology_result_t * metrology_result );

/******************************************************************************/
/* Name        : metrology_get_gainchannel                                    */
/* Description : Returns the by the engine selected current gain input        */
/* Parameters  : none                                                         */
/* Returns     : EM_CHANNEL1 - when I_HIGHGAIN is selected,                   */
/*               EM_CHANNEL2 - when I_LOWGAIN is selected.                    */
/******************************************************************************/

EM_current_channel_t metrology_get_gainchannel ( void );

/******************************************************************************/
/* Name        : metrology_IRQHandler                                         */
/* Description : Metrology engine interrupt service routine                   */
/*               Note: This fuction must be called for interrupt 17.          */
/* Parameters  : none                                                         */
/* Returns     : none                                                         */
/******************************************************************************/

void metrology_IRQHandler ( void );

#endif

/* End of file */
