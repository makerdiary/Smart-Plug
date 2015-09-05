/*****************************************************************************/
/*  Copyright (c) 2011 NXP B.V.  All rights are reserved.                    */
/*  Reproduction in whole or in part is prohibited without the prior         */
/*  written consent of the copyright owner.                                  */
/*                                                                           */
/*  This software and any compilation or derivative thereof is, and          */
/*  shall remain the proprietary information of NXP and is                   */
/*  highly confidential in nature. Any and all use hereof is restricted      */
/*  and is subject to the terms and conditions set forth in the              */
/*  software license agreement concluded with NXP B.V.                       */
/*                                                                           */
/*  Under no circumstances is this software or any derivative thereof        */
/*  to be combined with any Open Source Software, exposed to, or in any      */
/*  way licensed under any Open License Terms without the express prior      */
/*  written permission of the copyright owner.                               */
/*                                                                           */
/*  For the purpose of the above, the term Open Source Software means        */
/*  any software that is licensed under Open License Terms. Open             */
/*  License Terms means terms in any license that require as a               */
/*  condition of use, modification and/or distribution of a work             */
/*                                                                           */
/*  1. the making available of source code or other materials                */
/*     preferred for modification, or                                        */
/*                                                                           */
/*  2. the granting of permission for creating derivative                    */
/*     works, or                                                             */
/*                                                                           */
/*  3. the reproduction of certain notices or license terms                  */
/*     in derivative works or accompanying documentation, or                 */
/*                                                                           */
/*  4. the granting of a royalty-free license to any party                   */
/*     under Intellectual Property Rights                                    */
/*                                                                           */
/*  regarding the work and/or any work that contains, is combined with,      */
/*  requires or otherwise is based on the work.                              */
/*                                                                           */
/*  This software is provided for ease of recompilation only.                */
/*  Modification and reverse engineering of this software are strictly       */
/*  prohibited.                                                              */
/*                                                                           */
/*****************************************************************************/

/*****************************************************************************
 * standard include files
 *****************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*****************************************************************************
 * project include files
 *****************************************************************************/

#include "app_config.h"
#include "dprint_macros.h"
#include "string_functions.h"
#include "rtk.h"
#include "task.h"
#include "bsp.h"
#include "app.h"
#include "nvmem.h"
#include "rfhandler.h"
#ifdef SUPPORT_FOR_SERIAL_PORT
  #include "uart.h"
#endif
#include "main.h"
#include "gpio.h"

/******************************************************************************
 * type definitions
 *****************************************************************************/


#ifdef SUPPORT_CC2530ZNP
  #define DEFAULT_TX_INTERVAL 3    /* default time in seconds between RF TX messages */
#else
  #define DEFAULT_TX_INTERVAL 1    /* default time in seconds between UART TX messages */
#endif



/******************************************************************************
 * global variables
 *****************************************************************************/

extern unsigned char znpBuf[100];

extern signed int znpResult;                    //from znp_interface

extern unsigned int unique_id;
volatile unsigned short tx_interval = DEFAULT_TX_INTERVAL;
volatile char rf_tx_config_msg      = 0;
volatile char rf_tx_calibration_msg = 0;


static EM_mains_t   mains_freq = EM_AUTO; 

/******************************************************************************
 * local functions
 *****************************************************************************/


#ifdef SUPPORT_CALIBRATE_EMETER
void trigger_tx_calibration_msg(void)
{
    Reset_msec_cnt(SW_TIMER_1);
    rf_tx_calibration_msg = 1;
}
#endif

void trigger_tx_config_msg(void)
{
    Reset_msec_cnt(SW_TIMER_1);
    rf_tx_config_msg = 1;
}

/******************************************************************************
 * global functions
 *****************************************************************************/
unsigned short get_tx_interval(void)
{
    return tx_interval;
}

void set_tx_interval(unsigned short value)
{
    tx_interval = value;
    if (tx_interval == 0) tx_interval = 1;
}

void *rf_thread(void *parg)
{
    return 0;
}


int Packet_Tx_Msg(char *buf)
{
     static meter_result_t local_calc_result;
     union
     {
         float fval;
         int   ival;
     }   rfdata;
     int size;
     
     local_calc_result.data_changed = 0;
     copy_calc_results(&local_calc_result);
     
     if (!local_calc_result.data_changed)
     {
         local_calc_result.meter_data.V    = (float)0.0;
         local_calc_result.meter_data.I    = (float)0.0;
         local_calc_result.meter_data.P    = (float)0.0;
         local_calc_result.meter_data.Q1   = (float)0.0;
         local_calc_result.meter_data.S    = (float)0.0;
         local_calc_result.meter_data.S1   = (float)0.0;
         local_calc_result.meter_data.PF   = (float)1.0;
         local_calc_result.meter_data.PF1  = (float)1.0;
         local_calc_result.meter_data.SN   = (float)0.0;
         local_calc_result.meter_data.N    = (float)0.0;
         local_calc_result.meter_data.THDI = (float)0.0;
         local_calc_result.sec_cnt        += tx_interval;
     }
     else
     {
         mains_freq = local_calc_result.mains_freq;
     }
     
     // add UNIRUE_ID to Data-field
     buf[0]  = (unique_id >>  0) & 0xFF;
     buf[1]  = (unique_id >>  8) & 0xFF;
     buf[2]  = (unique_id >> 16) & 0xFF;
     buf[3]  = (unique_id >> 24) & 0xFF;
     
     // add USER DEFINE FIELD
     buf[4]  = USER_DEFINE_BYTE_1;
     buf[5]  = USER_DEFINE_BYTE_2;
     
     // add the uptime (second counter) to the Data-field (wrap around after 194 days)
     buf[6]  = (local_calc_result.sec_cnt >>  0) & 0xFF;
     buf[7]  = (local_calc_result.sec_cnt >>  8) & 0xFF;
     buf[8]  = (local_calc_result.sec_cnt >> 16) & 0xFF;
     buf[9]  = (local_calc_result.sec_cnt >> 24) & 0xFF;
     
     // add V to Data-field
     rfdata.fval = local_calc_result.meter_data.V;
     buf[10] = (rfdata.ival >>  0) & 0xFF;
     buf[11] = (rfdata.ival >>  8) & 0xFF;
     buf[12] = (rfdata.ival >> 16) & 0xFF;
     buf[13] = (rfdata.ival >> 24) & 0xFF;
     
     //   add I to Data-field
     rfdata.fval = local_calc_result.meter_data.I;
     buf[14]     = (rfdata.ival >>  0) & 0xFF;
     buf[15]     = (rfdata.ival >>  8) & 0xFF;
     buf[16]     = (rfdata.ival >> 16) & 0xFF;
     buf[17]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add P to Data-field
     rfdata.fval = local_calc_result.meter_data.P;
     buf[18]     = (rfdata.ival >>  0) & 0xFF;
     buf[19]     = (rfdata.ival >>  8) & 0xFF;
     buf[20]     = (rfdata.ival >> 16) & 0xFF;
     buf[21]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add Q1 to Data-field
     rfdata.fval = local_calc_result.meter_data.Q1;
     buf[22]     = (rfdata.ival >>  0) & 0xFF;
     buf[23]     = (rfdata.ival >>  8) & 0xFF;
     buf[24]     = (rfdata.ival >> 16) & 0xFF;
     buf[25]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add S to Data-field
     rfdata.fval = local_calc_result.meter_data.S;
     buf[26]     = (rfdata.ival >>  0) & 0xFF;
     buf[27]     = (rfdata.ival >>  8) & 0xFF;
     buf[28]     = (rfdata.ival >> 16) & 0xFF;
     buf[29]     = (rfdata.ival >> 24) & 0xFF;
    
     //   add S1 to Data-field
     rfdata.fval = local_calc_result.meter_data.S1;
     buf[30]     = (rfdata.ival >>  0) & 0xFF;
     buf[31]     = (rfdata.ival >>  8) & 0xFF;
     buf[32]     = (rfdata.ival >> 16) & 0xFF;
     buf[33]     = (rfdata.ival >> 24) & 0xFF;
    
     //   add PF to Data-field
     rfdata.fval = local_calc_result.meter_data.PF;
     buf[34]     = (rfdata.ival >>  0) & 0xFF;
     buf[35]     = (rfdata.ival >>  8) & 0xFF;
     buf[36]     = (rfdata.ival >> 16) & 0xFF;
     buf[37]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add PF1 to Data-field
     rfdata.fval = local_calc_result.meter_data.PF1;
     buf[38]     = (rfdata.ival >>  0) & 0xFF;
     buf[39]     = (rfdata.ival >>  8) & 0xFF;
     buf[40]     = (rfdata.ival >> 16) & 0xFF;
     buf[41]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add SN to Data-field
     rfdata.fval = local_calc_result.meter_data.SN;
     buf[42]     = (rfdata.ival >>  0) & 0xFF;
     buf[43]     = (rfdata.ival >>  8) & 0xFF;
     buf[44]     = (rfdata.ival >> 16) & 0xFF;
     buf[45]     = (rfdata.ival >> 24) & 0xFF;
    
     //   add N to Data-field
     rfdata.fval = local_calc_result.meter_data.N;
     buf[46]     = (rfdata.ival >>  0) & 0xFF;
     buf[47]     = (rfdata.ival >>  8) & 0xFF;
     buf[48]     = (rfdata.ival >> 16) & 0xFF;
     buf[49]     = (rfdata.ival >> 24) & 0xFF;
    
     //   add THDI to Data-field
     rfdata.fval = local_calc_result.meter_data.THDI;
     buf[50]     = (rfdata.ival >>  0) & 0xFF;
     buf[51]     = (rfdata.ival >>  8) & 0xFF;
     buf[52]     = (rfdata.ival >> 16) & 0xFF;
     buf[53]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add the consumed energy to the Data-field
     rfdata.fval = local_calc_result.ecounter;
     buf[54]     = (rfdata.ival >>  0) & 0xFF;
     buf[55]     = (rfdata.ival >>  8) & 0xFF;
     buf[56]     = (rfdata.ival >> 16) & 0xFF;
     buf[57]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add the produced energy to the Data-field
     rfdata.fval = local_calc_result.pcounter;
     buf[58]     = (rfdata.ival >>  0) & 0xFF;
     buf[59]     = (rfdata.ival >>  8) & 0xFF;
     buf[60]     = (rfdata.ival >> 16) & 0xFF;
     buf[61]     = (rfdata.ival >> 24) & 0xFF;
     
     //   add the mains frequency
     buf[62]     = (char)mains_freq;
    
     //   add the relay state
     buf[63]     = em_get_relay_state();                    // Relay status ('O' = open, 'C' = close)
    
     //   add the transmit interval
     buf[64]     = (tx_interval >> 0) & 0xFF;               // current txtime LSB
     buf[65]     = (tx_interval >> 8) & 0xFF;               // current txtime MSB
     
     size = 66;
     
     return(size);
}


void Print_Rx_Msg(char *rxbuf)
{
     union
     {
         float fval;
         int   ival;
     }   rfdata;
     
     meter_result_t rx_calc_result;
     unsigned int   rx_unique_id = 0;
     char           rx_relay_state = 'C';
     unsigned short rx_tx_interval;
     
     // UNIQUE_ID
     rx_unique_id = (rxbuf[3] << 24) | (rxbuf[2] << 16) | (rxbuf[1] << 8) | rxbuf[0];
     
     // USER DEFINE BYTES
     
     
     // uptime
     rx_calc_result.sec_cnt = (rxbuf[9] << 24) | (rxbuf[8] << 16) | (rxbuf[7] << 8) | rxbuf[6];
     
     // V
     rfdata.ival = (rxbuf[13] << 24) | (rxbuf[12] << 16) | (rxbuf[11] << 8) | rxbuf[10];
     rx_calc_result.meter_data.V = rfdata.fval;
     
     // I
     rfdata.ival = (rxbuf[17] << 24) | (rxbuf[16] << 16) | (rxbuf[15] << 8) | rxbuf[14];
     rx_calc_result.meter_data.I = rfdata.fval;
     
     // P
     rfdata.ival = (rxbuf[21] << 24) | (rxbuf[20] << 16) | (rxbuf[19] << 8) | rxbuf[18];
     rx_calc_result.meter_data.P = rfdata.fval;
     
     // Q1
     rfdata.ival = (rxbuf[25] << 24) | (rxbuf[24] << 16) | (rxbuf[23] << 8) | rxbuf[22];
     rx_calc_result.meter_data.Q1 = rfdata.fval;
     
     // S
     rfdata.ival = (rxbuf[29] << 24) | (rxbuf[28] << 16) | (rxbuf[27] << 8) | rxbuf[26];
     rx_calc_result.meter_data.S = rfdata.fval;

     // S1
     rfdata.ival = (rxbuf[33] << 24) | (rxbuf[32] << 16) | (rxbuf[31] << 8) | rxbuf[30];
     rx_calc_result.meter_data.S1 = rfdata.fval;
     
     // PF
     rfdata.ival = (rxbuf[37] << 24) | (rxbuf[36] << 16) | (rxbuf[35] << 8) | rxbuf[34];
     rx_calc_result.meter_data.PF = rfdata.fval;

     // PF1
     rfdata.ival = (rxbuf[41] << 24) | (rxbuf[40] << 16) | (rxbuf[39] << 8) | rxbuf[38];
     rx_calc_result.meter_data.PF1 = rfdata.fval;
     
     // SN
     rfdata.ival = (rxbuf[45] << 24) | (rxbuf[44] << 16) | (rxbuf[43] << 8) | rxbuf[42];
     rx_calc_result.meter_data.SN = rfdata.fval;
    
     // N
     rfdata.ival = (rxbuf[49] << 24) | (rxbuf[48] << 16) | (rxbuf[47] << 8) | rxbuf[46];
     rx_calc_result.meter_data.N = rfdata.fval;

     // THDI
     rfdata.ival = (rxbuf[53] << 24) | (rxbuf[52] << 16) | (rxbuf[51] << 8) | rxbuf[50];
     rx_calc_result.meter_data.THDI = rfdata.fval;
     
     // consumed energy
     rfdata.ival = (rxbuf[57] << 24) | (rxbuf[56] << 16) | (rxbuf[55] << 8) | rxbuf[54];
     rx_calc_result.ecounter = rfdata.fval;
     
     // produced energy
     rfdata.ival = (rxbuf[61] << 24) | (rxbuf[60] << 16) | (rxbuf[59] << 8) | rxbuf[58];
     rx_calc_result.pcounter = rfdata.fval;
     
     // the mains frequency
     rx_calc_result.mains_freq = (EM_mains_t)rxbuf[62];
    
     // the relay state
     rx_relay_state = rxbuf[63];
     
     // the transmit interval
     rx_tx_interval = (rxbuf[65] << 8) | rxbuf[64];

     printf("Plug ID: %u\r\n",rx_unique_id);
     printf("Relay: %s  Transmit interval: %ds\r\n", rx_relay_state == 'C'? "ON":"OFF", rx_tx_interval);
     print_meter_result(&rx_calc_result);
     
}


#define WAIT_FOR_DEVICE_STATE_TIMEOUT 4000000l
/** Wait until a message is received. Exits if received message is a ZDO_STATE_CHANGE_IND
and the state matches what we want. Else loops. 
@param expectedState the deviceState we are expecting - DEV_ZB_COORD etc.
@return 0 if success, -1 if timeout
*/
static signed int waitForDeviceState(unsigned char expectedState)
{
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
    printf("Waiting for network... \r\n");
#endif
    unsigned char state = 0xFF;
    unsigned long timeout = WAIT_FOR_DEVICE_STATE_TIMEOUT;
    while (state != expectedState)
    {
        while ((SRDY_IS_HIGH()) && (timeout > 0))
            timeout--;
        if (timeout == 0)
            return -1;  //error
        
        spiPoll();
        if (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) == ZDO_STATE_CHANGE_IND)
        {
            state = znpBuf[SRSP_PAYLOAD_START];
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
            printf("%s ", getDeviceStateName(state));
#endif
        }
    }
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
    printf("\r\n");
#endif
    return 0;
}


/** Attempts to start the ZNP.
@param the type of device we're starting, e.g. ROUTER
@post znpResult contains the ZNP library error code, if any.
@return ZNP_SUCCESS if successful, else an error code indicating where it failed.
@see Communications Examples for more information about each of these steps.
*/
signed int startZnp(unsigned char deviceType)
{
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
    printf("\r\nZNP Startup...\r\n");
#endif
    znpInit(); 
    if (znpResult != ZNP_SUCCESS) 
        return -1; 
    
    setStartupOptions(STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE);
    if (znpResult != ZNP_SUCCESS) 
        return -2; 
    
    znpReset();
    if (znpResult != ZNP_SUCCESS) 
        return -3; 
    
    setZigbeeDeviceType(deviceType);
    if (znpResult != ZNP_SUCCESS) 
        return -4; 
    
    setChannel(DEFAULT_CHANNEL);
    if (znpResult != ZNP_SUCCESS) 
        return -5; 
    
    setCallbacks(CALLBACKS_ENABLED);
    if (znpResult != ZNP_SUCCESS) 
        return -6; 
    
    afRegisterGenericApplication();
    if (znpResult != ZNP_SUCCESS) 
        return -8; 
    
    zdoStartApplication();
    if (znpResult != ZNP_SUCCESS) 
        return -9; 
    
    /* Wait until this device has joined a network.
    Device State will change to DEV_ROUTER or DEV_COORD to indicate that the device has correctly joined a network. */
    unsigned char deviceState = 0;
    switch (deviceType)
    {
    case ROUTER: deviceState = DEV_ROUTER; break;
    case END_DEVICE: deviceState = DEV_END_DEVICE; break;
    case COORDINATOR: deviceState = DEV_ZB_COORD; break;
    default:
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
      printf("ERROR - UNKNOWN DEVICE TYPE\r\n");
#endif
      return -10;
    }
    
    znpResult = waitForDeviceState(deviceState);
    if (znpResult != ZNP_SUCCESS) 
        return -10; 
    
    return ZNP_SUCCESS;
}





/* End of file */
