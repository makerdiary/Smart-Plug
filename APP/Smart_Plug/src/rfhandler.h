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

#ifndef RFHANDLER_H_INCLUDED
#define RFHANDLER_H_INCLUDED

#include "app_config.h"
#include "rf_cfg.h"
#include "app.h"
#ifdef SUPPORT_CC2530ZNP
  #include "znp_hal.h"
  #include "znp_interface.h"
  #include "znp_interface_spi.h"
  #include "application_configuration.h"
  #include "af_zdo.h"
#endif


#define USER_DEFINE_BYTE_1    0xA0
#define USER_DEFINE_BYTE_2    0xB0

/******************************************************************************
 * global functions
 *****************************************************************************/

unsigned short get_tx_interval(void);
void set_tx_interval(unsigned short value);
void trigger_tx_calibration_msg(void);
void trigger_tx_config_msg(void);
void *rf_thread(void *parg);
int  Packet_Tx_Msg(char *buf);
void Print_Rx_Msg(char *rxbuf);
signed int startZnp(unsigned char deviceType);
#endif