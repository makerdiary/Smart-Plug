/*****************************************************************************/
/*  Copyright (c) 2009 NXP B.V.  All rights are reserved.                    */
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

#ifndef _PCB_H_
#define _PCB_H_

// Ports
typedef enum _PORT_T_
{
  PORT0,
  PORT1,
  PORT2,
  PORT3
} port_t;

// General sizes
#define PCB_NUMPORTS     4
#define PCB_NUMPINS      12

// Values for portpin
#define PCB_PORTPIN( port, pin )    ( ((port) << 8) | (pin) )
#define PCB_PORTPIN2PORT( portpin ) ( ((portpin)>>8) & 0x000000ff )
#define PCB_PORTPIN2PIN( portpin )  ( (portpin) & 0x000000ff )

// Values for func (pin specific, so no useful constants here)
typedef enum _PCB_FUNC_T_
{
  PCB_FUNC_0,
  PCB_FUNC_1,
  PCB_FUNC_2,
  PCB_FUNC_3
} pcb_func_t;

// Values for mode
typedef enum _PCB_MODE_T_
{
  PCB_MODE_PULLNONE, // 00 pin has neither pull-up nor pull-down
  PCB_MODE_PULLDOWN, // 01 pin has a pull-down resistor enabled
  PCB_MODE_PULLUP,   // 10 pin has a pull-up resistor enabled (HW default)
  PCB_MODE_REPEATER  // 11 Repeater mode
} pcb_mode_t;

void pcb_init  ( void );
void pcb_alloc ( int portpin, pcb_func_t func, pcb_mode_t mode );
void pcb_free  ( int portpin );


#endif
