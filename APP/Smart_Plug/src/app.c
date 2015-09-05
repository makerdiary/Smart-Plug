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

/*******************************************************************
 * standard include files
 *******************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

/*******************************************************************
 * project specific include files
 *******************************************************************/

#include "app_config.h"
#include "rtk.h"
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "dprint_macros.h"
#include "bsp.h"
#include "gpio.h"
#include "wdt.h"
#include "app.h"
#include "rfhandler.h"
#ifdef SUPPORT_LOCAL_LCD
#include "lcd.h"
#endif
#include "nvmem.h"
#include "string_functions.h"

#include "znp_interface.h"

/*******************************************************************
 * Local types
 *******************************************************************/

//#define DEBUG_STACK_USAGE

#define BOD_RSTLEV_VAL   0   // possible values 0 till 3
#define BOD_INTVAL_VAL   3   // possible values 0 till 3
#define BOD_RSTENA_VAL   0   // possible values 0 or 1
#define BOD_TIMEOUT      10  // in seconds

#define MINIMUM_RELAY_SWITCHING_TIME   10

#define NR_CALIBRATION_VALUES 5

typedef struct _calibration_data_t_
{
  uint8_t   state;
  int8_t    counter;
  uint16_t  txtime;
  float     vpp;
  float     i1pp;
  float     i2pp;
  float     deltaphi1;
  float     deltaphi2;
  float     vref;
  float     i1ref;
  float     i2ref;
} calibration_data_t;

/*******************************************************************
 * global variables and struct definitions
 *******************************************************************/
unsigned int unique_id  = 0;

extern xTaskHandle         pAppStartHandle;
static xTaskHandle         pRfHandle;
static rtk_sem_t           ipc_mutex;
static meter_result_t      ipc_calc_result;
static metrology_ranges_t  metrology_ranges;
#ifdef SUPPORT_CALIBRATE_EMETER
static metrology_ranges_t  calibration_ranges;
#endif
static calibration_data_t  CalData;
static char                relay_state = 'C';  // 'O' = open, 'C' = close
static volatile uint16_t   relay_switching_interval = MINIMUM_RELAY_SWITCHING_TIME;
static uint8_t             bod_sec_counter;    // values 0 till BOD_TIMEOUT

extern unsigned char znpBuf[100];
extern signed int znpResult;                    //from znp_interface

/******************************************************************************
 * application
 *****************************************************************************/

#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
static void app_print_info(void)
{
    char tmp_str[50];

    DO_SPRINTF   (tmp_str, STR_EOL "Smart Plug (v%s)" STR_EOL, SW_VERSION);
    DPRINT_NOARG (tmp_str);
    DO_SPRINTF   (tmp_str, "Firmware build on %s, at %s" STR_EOL, __DATE__, __TIME__);
    DPRINT_NOARG (tmp_str);
    DO_SPRINTF   (tmp_str, "CPU clock %d Hz" STR_EOL STR_EOL, LPC_CORE_CLOCKSPEED_HZ);
    DPRINT_NOARG (tmp_str);
    
}

static void app_show_stack_usage(void)
{
    char tmp_str[15];

    DPRINT_NOARG ("Bytes free: ");
    DO_SPRINTF   (tmp_str, "main=%d,", uxTaskGetStackHighWaterMark(pAppStartHandle) * 4);
    DPRINT_NOARG (tmp_str);
    DO_SPRINTF   (tmp_str, "rf=%d,", uxTaskGetStackHighWaterMark(pRfHandle) * 4);
    DPRINT_NOARG (tmp_str);
    DO_SPRINTF   (tmp_str, "heap=%d" STR_EOL, configTOTAL_HEAP_SIZE - GetMemInUse());
    DPRINT_NOARG (tmp_str);
}

void print_meter_result(meter_result_t *meter_result)
{
    int   len;
    char  output_str[60];

    len  = 0;
    len  = DO_SPRINTF(&output_str[len], "Reading: V=");
    len += ftos(&output_str[len], (meter_result->meter_data).V, 3);
    len += DO_SPRINTF(&output_str[len], " I=");
    len += ftos(&output_str[len], (meter_result->meter_data).I, 3);
    len += DO_SPRINTF(&output_str[len], " S=");
    len += ftos(&output_str[len], (meter_result->meter_data).S, 3);
    len += DO_SPRINTF(&output_str[len], " PF=");
    len += ftos(&output_str[len], (meter_result->meter_data).PF, 2);
    len += DO_SPRINTF(&output_str[len], STR_EOL);
    DPRINT_NOARG(output_str);

    len  = 0;
    len  = DO_SPRINTF(&output_str[len], "Reading: P=");
    len += ftos(&output_str[len], (meter_result->meter_data).P, 3);
    len += DO_SPRINTF(&output_str[len], " S1=");
    len += ftos(&output_str[len], (meter_result->meter_data).S1, 3);
    len += DO_SPRINTF(&output_str[len], " Q1=");
    len += ftos(&output_str[len], (meter_result->meter_data).Q1, 3);
    len += DO_SPRINTF(&output_str[len], " PF1=");
    len += ftos(&output_str[len], (meter_result->meter_data).PF1, 2);
    len += DO_SPRINTF(&output_str[len], STR_EOL);
    DPRINT_NOARG(output_str);

    len  = 0;
    len += DO_SPRINTF(&output_str[len], "Reading: SN=");
    len += ftos(&output_str[len], (meter_result->meter_data).SN, 3);
    len += DO_SPRINTF(&output_str[len], " N=");
    len += ftos(&output_str[len], (meter_result->meter_data).N, 3);
    len += DO_SPRINTF(&output_str[len], " THDI=");
    len += ftos(&output_str[len], (meter_result->meter_data).THDI, 3);
    len += DO_SPRINTF(&output_str[len], STR_EOL);
    DPRINT_NOARG(output_str);
    
    len  = 0;
    len += DO_SPRINTF(&output_str[len], "Reading: +T=");
    len += ftos(&output_str[len], meter_result->ecounter, 3);
    len += DO_SPRINTF(&output_str[len], " -T=");
    len += ftos(&output_str[len], meter_result->pcounter, 3);
    len += DO_SPRINTF(&output_str[len], STR_EOL);
    DPRINT_NOARG(output_str);
    
    len  = 0;
    len += DO_SPRINTF(&output_str[len], "Reading: Freq=");
    len += DO_SPRINTF(&output_str[len], "%d",meter_result->mains_freq);
    len += DO_SPRINTF(&output_str[len], " On Time: ");
    len += getTimeFormat(&output_str[len], meter_result->sec_cnt);
    len += DO_SPRINTF(&output_str[len], STR_EOL STR_EOL);
    DPRINT_NOARG(output_str);
}
#endif

static void setup_BOD(void)
{
    bod_sec_counter = BOD_TIMEOUT;
    SYSCON->BODCTRL = (BOD_RSTENA_VAL << 4) |
                      (BOD_INTVAL_VAL << 2) |
                      (BOD_RSTLEV_VAL << 0);
    NVIC_SetPriority(BOD_IRQn, BODINT_PRIORITY);
    NVIC_EnableIRQ(BOD_IRQn);
}

void BOD_IRQHandler(void)
{
    if (bod_sec_counter >= BOD_TIMEOUT)
    {
        nvmem_store_in_flash();
        bod_sec_counter = 0;
    }
}

static void clear_calc_result(int reset_all)
{
    if (reset_all)
    {
        ipc_calc_result.data_changed = 0;
        ipc_calc_result.sec_cnt      = 0;
        ipc_calc_result.mains_freq   = EM_AUTO;
        ipc_calc_result.pcounter     = (float)0.0;
        ipc_calc_result.ecounter     = (float)0.0;
        ipc_calc_result.meter_data.V = (float)0.0;
    }
    ipc_calc_result.meter_data.I     = (float)0.0;
    ipc_calc_result.meter_data.P     = (float)0.0;
    ipc_calc_result.meter_data.Q1    = (float)0.0;
    ipc_calc_result.meter_data.S     = (float)0.0;
    ipc_calc_result.meter_data.S1    = (float)0.0;
    ipc_calc_result.meter_data.PF    = (float)1.0;
    ipc_calc_result.meter_data.PF1   = (float)1.0;
    ipc_calc_result.meter_data.SN    = (float)0.0;
    ipc_calc_result.meter_data.N     = (float)0.0;
    ipc_calc_result.meter_data.THDI  = (float)0.0;
}

#ifdef SUPPORT_LOCAL_LCD
void updateLCD(void)
{
static          char new_lcd_text[(LCD_NR_OF_LINES * LCD_LINE_LENGTH) + 1];
static unsigned char sec_cnt = 0;
static unsigned char view    = 0;
       unsigned char len     = 0;

    len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.P, 1,0,4,1);
    len += DO_SPRINTF(&new_lcd_text[len], "W  ");
    len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.V, 0,0,3,1);
    len += DO_SPRINTF(&new_lcd_text[len], "V");
    len = LCD_LINE_LENGTH;
    switch (view)
    {
    case 0:
        len += DO_SPRINTF(&new_lcd_text[len], "+T ");
        len += flt2asc(&new_lcd_text[len], ipc_calc_result.ecounter, 0,1,5,3);
        len += DO_SPRINTF(&new_lcd_text[len], " kWh");
        break;
    case 1:
        len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.S, 1,0,4,1);
        len += DO_SPRINTF(&new_lcd_text[len], "VA ");
        len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.I, 0,0,2,2);
        len += DO_SPRINTF(&new_lcd_text[len], "A");
        break;
    case 2:
        len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.Q1, 1,0,4,1);
        len += DO_SPRINTF(&new_lcd_text[len], "VAr  ");
        len += flt2asc(&new_lcd_text[len], ipc_calc_result.meter_data.PF, 0,0,1,2);
        break;
    }
    for (; len < (LCD_NR_OF_LINES * LCD_LINE_LENGTH); len++) {
        new_lcd_text[len+0] = ' ';
        new_lcd_text[len+1] = '\0';
    }
    lcd_update(new_lcd_text);
    sec_cnt++;
    if (sec_cnt >= 10)
    {
        sec_cnt = 0;
        view++;
        if (view >= 3)
        {
            view = 0;
        }
    }
}
#endif

#ifdef SUPPORT_CALIBRATE_EMETER
/* when NR_CALIBRATION_VALUES is large, this code should be changed!!*/
static float CalcAverage(float *inp)
{
  int   i;
  int   low = 0;
  int   high = 0;
  float res = (float)0.0;
  float nr_of_values = (float)0.0;

  for (i = 1; i < NR_CALIBRATION_VALUES; i++)
  {
    if (inp[i] < inp[low])
    {
      low = i;
    }
    if (inp[i] > inp[high])
    {
      high = i;
    }
  }

  for (i = 0; i < NR_CALIBRATION_VALUES; i++)
  {
    if ((i != low) && (i != high))  
    {
      res += inp[i];
      nr_of_values += (float)1.0;
    }
  }

  return (res / (float)nr_of_values);
}

uint8_t em_get_state(void)
{
  return CalData.state;
}

void em_StartCalibration(float *values)
{
  if (!CalData.state)
  {
    if (ipc_calc_result.mains_freq == EM_50HZ)
    {
      nvmem_read_ranges(EM_50HZ, &metrology_ranges);
    }
    else //  EM_60HZ
    {
      nvmem_read_ranges(EM_60HZ, &metrology_ranges);
    }
    calibration_ranges.Vpp = values[0];
    calibration_ranges.I1pp = values[1];
    calibration_ranges.I2pp = values[2];
    CalData.vpp = calibration_ranges.Vpp;
    CalData.i1pp = calibration_ranges.I1pp;
    CalData.i2pp = calibration_ranges.I2pp;
    CalData.deltaphi1 =  (float)0.0;
    CalData.deltaphi2 =  (float)0.0;
    CalData.vref = values[3];
    CalData.i1ref = values[4];
    CalData.i2ref = values[5];
    CalData.counter = -5;
    CalData.txtime = get_tx_interval();
    metrology_set_ranges(&calibration_ranges);
    set_tx_interval(60);
    em_relay_open();
    nvmem_write_ranges(ipc_calc_result.mains_freq, &calibration_ranges);
    CalData.state = 1;
  }
}

void em_NextCalibrationState(void)
{
  if (CalData.state)
  {
    set_tx_interval(60);
    switch(CalData.state + 1)
    {
      case 3:
        em_relay_close();
        CalData.counter = -10;
        break;
      case 4:
        CalData.counter = -2;
        break;
      case 6:
        CalData.counter = -20;
        break;
      case 7:
        CalData.counter = -2;
        break;
      default:
        set_tx_interval(1);
        break;
    }
    metrology_set_ranges(&calibration_ranges);
    nvmem_write_ranges(ipc_calc_result.mains_freq, &calibration_ranges);
    CalData.state++;
  }
}

void em_PreviousCalibrationState(void)
{
  if (CalData.state)
  {
    switch(CalData.state)
    {
      case 2:
        calibration_ranges.Vpp = CalData.vpp;
        CalData.counter = -5;
        CalData.state = 1;
        em_relay_open();
        set_tx_interval(60);
        break;
      case 5:
        calibration_ranges.I1pp = CalData.i1pp;
        calibration_ranges.DeltaPhi1 = (float)0.0;
        CalData.counter = -10;
        CalData.state = 2;
        break;
      case 8:
        calibration_ranges.I2pp = CalData.i2pp;
        calibration_ranges.DeltaPhi2 = (float)0.0;
        CalData.counter = -20;
        CalData.state = 5;
        break;
    }
    metrology_set_ranges(&calibration_ranges);
    nvmem_write_ranges(ipc_calc_result.mains_freq, &calibration_ranges);
  }
}

void em_CancelCalibrationState(void)
{
  if(CalData.state)
  {
    metrology_set_ranges(&metrology_ranges);
    nvmem_write_ranges(ipc_calc_result.mains_freq, &metrology_ranges);
    set_tx_interval(CalData.txtime);
    trigger_tx_config_msg();
    CalData.state = 0;
  }
}

void em_StopCalibrationState(void)
{
  if (CalData.state)
  {
    metrology_ranges = calibration_ranges;
    nvmem_write_ranges(ipc_calc_result.mains_freq, &metrology_ranges);
    set_tx_interval(CalData.txtime);
    CalData.state = 0;
  }
}
#endif

void em_relay_open(void)
{
//  if (relay_switching_interval >= MINIMUM_RELAY_SWITCHING_TIME)
  {
    gpio_set(RELAY_OPEN_PORTPIN);
    rtk_tsk_sleep(30);
    gpio_clr(RELAY_OPEN_PORTPIN);
    relay_state = 'O';
    relay_switching_interval = 0;
  }
}

void em_relay_close(void)
{
  if (relay_switching_interval >= MINIMUM_RELAY_SWITCHING_TIME)
  {
    gpio_set(RELAY_CLOSE_PORTPIN);
    rtk_tsk_sleep(30);
    gpio_clr(RELAY_CLOSE_PORTPIN);
    relay_state = 'C';
    relay_switching_interval = 0;
  }
}

char em_get_relay_state(void)
{
  return relay_state;
}

void app_main(void *p_arg)
{
    metrology_result_t  meter_result;
    float               preal_kwh;
    int                 i;
    char                result_buf[66];
    
#ifdef SUPPORT_CALIBRATE_EMETER
    float               cal_value1[NR_CALIBRATION_VALUES];
    float               cal_value2[NR_CALIBRATION_VALUES];
#endif

    (void)p_arg;

    CalData.state = 0;
    unique_id = nvmem_readID();
    
    if (!nvmem_init())
    {
        metrology_ranges.Vpp  = (float)945.4793636;
        metrology_ranges.I1pp = (float)2.546045455;
        metrology_ranges.I2pp = (float)39.98545455;
        metrology_ranges.DeltaPhi1 = (float)-0.020022045;
        metrology_ranges.DeltaPhi2 = (float)-0.016616318;
        nvmem_write_ranges(EM_50HZ, &metrology_ranges);
        metrology_ranges.Vpp  = (float)944.3160455;
        metrology_ranges.I1pp = (float)2.569045455;
        metrology_ranges.I2pp = (float)39.90036364;
        metrology_ranges.DeltaPhi1 = (float)-0.0117395;
        metrology_ranges.DeltaPhi2 = (float)-0.012733636;
        nvmem_write_ranges(EM_60HZ, &metrology_ranges);
    }

    clear_calc_result(1);
    
    em_relay_close();

#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
    app_print_info();
#endif

#ifdef SUPPORT_LOCAL_LCD
    lcd_init();
    lcd_update("NXP EM773  plus CC2530 RFtranscv");
#endif
    
    signed int znpReturn ; 
    while ((znpReturn = startZnp(ZNP_DEVICETYPE)) != ZNP_SUCCESS)
    {
        printf("\r\nStart Network failed! Start again atfer %ds...\r\n",ZNP_START_DELAY_IF_FAIL_MS/1000);
        rtk_tsk_sleep(ZNP_START_DELAY_IF_FAIL_MS);
    }
    DPRINT_NOARG("On Network!\r\n");
    
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
    /* On network, display info about this network */   
    getNetworkConfigurationParameters();                
    getDeviceInformation();
#endif
    
    /* Now the network is running - wait for any received messages from the ZNP */
#ifdef VERBOSE_MESSAGE_DISPLAY    
    printAfIncomingMsgHeaderNames();
#endif
    
    
    /* give the front end hardware time to settle */
    rtk_tsk_sleep(1000);

    ipc_mutex = rtk_sem_create( 1, "pmdata" );

    ipc_calc_result.mains_freq = metrology_init(configCPU_CLOCK_HZ, EM_AUTO);

    if (ipc_calc_result.mains_freq == EM_50HZ)
    {
        nvmem_read_ranges(EM_50HZ, &metrology_ranges);
    }
    else //  EM_60HZ
    {
        nvmem_read_ranges(EM_60HZ, &metrology_ranges);
    }
    nvmem_read_counters(&ipc_calc_result.pcounter, &ipc_calc_result.ecounter);
    ipc_calc_result.data_changed = 1;

    metrology_set_ranges(&metrology_ranges);
    metrology_start();
    for (i = 0; i < 3; i++)
    {
        while (metrology_read_data(&meter_result) != EM_VALID)
        {
            rtk_tsk_sleep(100);
        }
    }
/*
    xTaskCreate((pdTASK_CODE) rf_thread,
                ( const signed portCHAR * )"RF",
                RF_TASK_STK_SIZE,
                (void *) NULL,
                RF_TASK_PRIORITY,
                &pRfHandle );
*/
    setup_BOD();
    
    BLED_ON();
    RLED_OFF();
    
    while (1)
    {
        if (metrology_get_gainchannel() == EM_CHANNEL1)
        {
            BLED_BLINK();
        }
        else
        {
            BLED_ON();
        }
        
        RLED_BLINK();
        
//        gpio_set(MEASURE_PORTPIN);
        if (metrology_read_data(&meter_result) == EM_VALID)
        {
//            gpio_clr(MEASURE_PORTPIN);
            switch(CalData.state)
            {
#ifdef SUPPORT_CALIBRATE_EMETER
            case 1:
                if (CalData.counter >= 0)
                {
                    cal_value1[CalData.counter] = meter_result.V;
                }
                CalData.counter++;
                if (CalData.counter == NR_CALIBRATION_VALUES)
                {
                    calibration_ranges.Vpp = calibration_ranges.Vpp * (CalData.vref / CalcAverage(cal_value1));
                    em_NextCalibrationState();
                    trigger_tx_config_msg();
                    trigger_tx_calibration_msg();
                }
                break;
            case 3:
                if (CalData.counter >= 0)
                {
                    cal_value1[CalData.counter] = meter_result.I;
                }
                CalData.counter++;
                if (CalData.counter == NR_CALIBRATION_VALUES)
                {
                    calibration_ranges.I1pp = calibration_ranges.I1pp * (CalData.i1ref / CalcAverage(cal_value1));
                    em_NextCalibrationState();
                    trigger_tx_config_msg();
                    trigger_tx_calibration_msg();
                }
                break;
            case 4:
                if (CalData.counter >= 0)
                {
                    cal_value1[CalData.counter] = meter_result.P;
                    cal_value2[CalData.counter] = meter_result.Q1;
                }
                CalData.counter++;
                if (CalData.counter == NR_CALIBRATION_VALUES)
                {
                    calibration_ranges.DeltaPhi1 = atanf(CalcAverage(cal_value2) / CalcAverage(cal_value1));
                    em_NextCalibrationState();
                    trigger_tx_config_msg();
                    trigger_tx_calibration_msg();
                }
                break;
            case 6:
                if (CalData.counter >= 0)
                {
                    cal_value1[CalData.counter] = meter_result.I;
                }
                CalData.counter++;
                if (CalData.counter == NR_CALIBRATION_VALUES)
                {
                    calibration_ranges.I2pp = calibration_ranges.I2pp * (CalData.i2ref / CalcAverage(cal_value1));
                    em_NextCalibrationState();
                    trigger_tx_config_msg();
                    trigger_tx_calibration_msg();
                }
                break;
            case 7:
                if (CalData.counter >= 0)
                {
                    cal_value1[CalData.counter] = meter_result.P;
                    cal_value2[CalData.counter] = meter_result.Q1;
                }
                CalData.counter++;
                if (CalData.counter == NR_CALIBRATION_VALUES)
                {
                    calibration_ranges.DeltaPhi2 = atanf(CalcAverage(cal_value2) / CalcAverage(cal_value1));
                    em_NextCalibrationState();
                    trigger_tx_config_msg();
                    trigger_tx_calibration_msg();
                }
                break;
#endif
            default:
      /* delay the hand-over of the measurement data until the 1/2 Vcc value on the biasing network is settled*/
      /* TODO: calculations in the start-interval should not be taken into account for the metrology data*/
              
                rtk_sem_acquire( ipc_mutex );
                ipc_calc_result.meter_data = meter_result;
                if ((ipc_calc_result.meter_data.P < (float)0.100) &&
                    (ipc_calc_result.meter_data.P > (float)-0.200))
                {
                    /* if the real power is below threshold, the resulting power and current should also be made zero */
                    clear_calc_result(0);
                }
                preal_kwh = ipc_calc_result.meter_data.P / (float)(3600.0 * 1000.0);
                if (preal_kwh > (float)0.0)
                {
                    ipc_calc_result.ecounter += preal_kwh;  /* update the consumption meter in kWh */
                }
                else
                {
                    ipc_calc_result.pcounter -= preal_kwh;  /* update the production meter in kWh */
                }
                ipc_calc_result.data_changed = 1;
                ipc_calc_result.sec_cnt = Get_sys_seconds_cnt();
                rtk_sem_release( ipc_mutex );
                nvmem_write_counters(ipc_calc_result.pcounter, ipc_calc_result.ecounter);
                if (bod_sec_counter < BOD_TIMEOUT) bod_sec_counter += 1;
#ifdef SUPPORT_LOCAL_LCD
                updateLCD();
#endif
                relay_switching_interval++;
                if (relay_switching_interval > MINIMUM_RELAY_SWITCHING_TIME)
                {
                    relay_switching_interval = MINIMUM_RELAY_SWITCHING_TIME;
                }
                
                unsigned char tx_len = Packet_Tx_Msg(result_buf);
#ifdef DEBUG_OUTPUT_VIA_SERIAL_PORT
                Print_Rx_Msg(result_buf);
#endif

#if defined(SMARTPLUG_COORDINATOR)
                while (SRDY_IS_HIGH());      //wait until SRDY goes low indicating a message has been received.   
                displayMessages();
#elif defined(SMARTPLUG_ROUTER)
                afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, (unsigned char *)result_buf, tx_len);
                if (znpResult != ZNP_SUCCESS)
                {
                    printf("afSendData error %i\r\n",znpResult);
                    rtk_tsk_sleep(ZNP_RESTART_DELAY_IF_MESSAGE_FAIL_MS);
                }
                
#else //End Device
                afSendData(DEFAULT_ENDPOINT,DEFAULT_ENDPOINT,0, TEST_CLUSTER, (unsigned char *)result_buf, tx_len);
                if (znpResult != ZNP_SUCCESS)
                {
                    printf("afSendData error %i\r\n",znpResult);
                    rtk_tsk_sleep(ZNP_RESTART_DELAY_IF_MESSAGE_FAIL_MS);
                }
                
#endif
                break;
            }
        }
        
        
//        gpio_clr(MEASURE_PORTPIN);
        int sleeptime = get_tx_interval()*1000;
        rtk_tsk_sleep(500);
#ifdef DEBUG_STACK_USAGE
        app_show_stack_usage();
#endif
    }
}

void copy_calc_results(meter_result_t * ptr_calc_result)
{
    rtk_sem_acquire( ipc_mutex );
    if (ipc_calc_result.data_changed)
    {
        *ptr_calc_result = ipc_calc_result;
        ipc_calc_result.data_changed = 0;
    }
    rtk_sem_release( ipc_mutex );
}


void displayMessages()
{
    spiPoll();
    unsigned char len;
    if (znpBuf[SRSP_LENGTH_FIELD] > 0)
    {
        len = znpBuf[SRSP_LENGTH_FIELD] + SRSP_HEADER_SIZE;
        if(len > 0 && znpBuf[SRSP_HEADER_SIZE+17+4] == USER_DEFINE_BYTE_1 && znpBuf[SRSP_HEADER_SIZE+17+5] == USER_DEFINE_BYTE_2)
        {
            Print_Rx_Msg((char*)&znpBuf[SRSP_HEADER_SIZE+17]);
        }
        else
        {
            DPRINT_NOARG("Invalid Rx Data!!!\r\n\r\n");
        }
        znpBuf[SRSP_LENGTH_FIELD] = 0;
    } 
}

/** Note: can also use interrupts to detect when incoming message is received.
Configure high>low edge triggered interrupt for SRDY signal from ZNP.
When SRDY goes low, the interrupt will fire
in the interrupt service routine, first check MRDY. 
If MRDY is low (active) then ignore the interrupt, since transaction is happening
If MRDY is not low then disable interrupt, poll(), and set msg received flag, and enable interrupt
Also be sure to disable the interrupt in methods that access the ZNP, sendSreq(), spiPoll(), etc.
*/


/* End of file */
