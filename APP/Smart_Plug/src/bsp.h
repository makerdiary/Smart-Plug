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

#ifndef _BSP_H
#define _BSP_H

#include "app_config.h"
#include "EM7xx.h"
#include "pcb.h"

/* Define the priorities of the supported interrupts */
/* value: 0 .. 3   (0 has highest prio) */
/* ISRs that call FreeRTOS API functions must have a priority between */
/* configMAX_SYSCALL_INTERRUPT_PRIORITY (1) and configKERNEL_INTERRUPT_PRIORITY (3) */
/* see FreeRTOSConfig.h */

#define BODINT_PRIORITY     0
#define RFINT_DATA_PRIORITY 2
#define SSPINT_PRIORITY     3
#define UARTINT_PRIORITY    3
#define I2CINT_PRIORITY     3

#define UART_RX_PORTPIN     PCB_PORTPIN(PORT1, 6)
#define UART_TX_PORTPIN     PCB_PORTPIN(PORT1, 7)

#define I2C_SCL_PORTPIN     PCB_PORTPIN(PORT0, 4)
#define I2C_SDA_PORTPIN     PCB_PORTPIN(PORT0, 5)

#define RLED_PORTPIN        PCB_PORTPIN(PORT1, 1)
#define BLED_PORTPIN        PCB_PORTPIN(PORT1, 2)

#define MRDY_PORTPIN        PCB_PORTPIN(PORT0, 7)
#define SRDY_PORTPIN        PCB_PORTPIN(PORT1, 5)

#define RELAY_OPEN_PORTPIN  PCB_PORTPIN(PORT3, 4)
#define RELAY_CLOSE_PORTPIN PCB_PORTPIN(PORT3, 5)

#define MEASURE_PORTPIN     UART_TX_PORTPIN       /* TX pin of UART connector */


#define RLED_ON()           gpio_clr(RLED_PORTPIN)
#define RLED_OFF()          gpio_set(RLED_PORTPIN)
#define RLED_BLINK()        gpio_blink(RLED_PORTPIN)

#define BLED_ON()           gpio_clr(BLED_PORTPIN)
#define BLED_OFF()          gpio_set(BLED_PORTPIN)
#define BLED_BLINK()        gpio_blink(BLED_PORTPIN) 

void bsp_init(void);

#endif

