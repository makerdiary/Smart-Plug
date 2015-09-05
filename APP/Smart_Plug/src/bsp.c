/*****************************************************************************/
/*  Copyright (c) 2010 NXP B.V.  All rights are reserved.                    */
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

/*******************************************************************
 * project specific include files
 *******************************************************************/

#include "app_config.h"
#include "bsp.h"
#include "rtk.h"
#include "pcb.h"
#include "gpio.h"
#include "i2c.h"
#include "uart.h"
#include "wdt.h"
#include "znp_hal.h"
#include "znp_interface.h"



/******************************************************************************
 * global functions
 *****************************************************************************/
void bsp_init(void)
{
#ifdef SUPPORT_FOR_SERIAL_PORT
  uart_config_t config = {RATE_19200_BAUD, BITS_8, BITS_1, NO_PARITY};
#endif
  pcb_init();
  gpio_init();
#ifdef SUPPORT_I2C_BUS
  /* Configuring I2C pins SCL/SDA */
  pcb_alloc(I2C_SCL_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLUP);
  pcb_alloc(I2C_SDA_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLUP);
  I2C_Init(I2CMASTER,0,NULL,NULL);
#endif
#ifdef SUPPORT_FOR_SERIAL_PORT
  /* Configuring UART pins RX/TX */
  pcb_alloc(UART_RX_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLUP);
  pcb_alloc(UART_TX_PORTPIN, PCB_FUNC_1, PCB_MODE_PULLUP);
  Ser_Init(config);
#else
  gpio_alloc(MEASURE_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);
#endif

  /* Configuring LED pin */
  gpio_alloc(RLED_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);
  gpio_alloc(BLED_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);

  /* Configuring CC2530ZNP*/
#ifdef SUPPORT_CC2530ZNP
  znp_hal_init();
  znpInit();
#endif
  
  /* Configuring Relay On/Off pin */
  gpio_alloc(RELAY_CLOSE_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);
  gpio_clr(RELAY_CLOSE_PORTPIN);  /* when low, the closing coin is cut-off */
  gpio_alloc(RELAY_OPEN_PORTPIN, PCB_MODE_PULLNONE, GPIO_OUTPUT);
  gpio_clr(RELAY_OPEN_PORTPIN);   /* when low, the opening coin is cut-off */

  WDT_Init();
}

/* End of file */
