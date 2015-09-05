/*
  FreeRTOS.org V5.3.0 - Copyright (C) 2003-2009 Richard Barry.

  This file is part of the FreeRTOS.org distribution.

  FreeRTOS.org is free software; you can redistribute it and/or modify it
  under the terms of the GNU General Public License (version 2) as published
  by the Free Software Foundation and modified by the FreeRTOS exception.
  **NOTE** The exception to the GPL is included to allow you to distribute a
  combined work that includes FreeRTOS.org without being obliged to provide
  the source code for any proprietary components.  Alternative commercial
  license and support terms are also available upon request.  See the
  licensing section of http://www.FreeRTOS.org for full details.

  FreeRTOS.org is distributed in the hope that it will be useful, but WITHOUT
  ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
  FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
  more details.

  You should have received a copy of the GNU General Public License along
  with FreeRTOS.org; if not, write to the Free Software Foundation, Inc., 59
  Temple Place, Suite 330, Boston, MA  02111-1307  USA.


  ***************************************************************************
  *                                                                         *
  * Get the FreeRTOS eBook!  See http://www.FreeRTOS.org/Documentation      *
  *                                                                         *
  * This is a concise, step by step, 'hands on' guide that describes both   *
  * general multitasking concepts and FreeRTOS specifics. It presents and   *
  * explains numerous examples that are written using the FreeRTOS API.     *
  * Full source code for all the examples is provided in an accompanying    *
  * .zip file.                                                              *
  *                                                                         *
  ***************************************************************************

  1 tab == 4 spaces!

  Please ensure to read the configuration and relevant port sections of the
  online documentation.

  http://www.FreeRTOS.org - Documentation, latest information, license and
  contact details.

  http://www.SafeRTOS.com - A version that is certified for use in safety
  critical systems.

  http://www.OpenRTOS.com - Commercial support, development, porting,
  licensing and training services.
*/

/*
  Change from V4.2.1:

  + Introduced usage of configKERNEL_INTERRUPT_PRIORITY macro to set the
    interrupt priority used by the kernel.
*/

  
  RSEG    CODE:CODE(2)
  THUMB

  EXTERN pxCurrentTCB
  EXTERN vTaskSwitchContext

  PUBLIC xPortPendSVHandler
  PUBLIC vPortSVCHandler
  PUBLIC vPortStartFirstTask

#ifndef configMAX_SYSCALL_INTERRUPT_PRIORITY
#define configMAX_SYSCALL_INTERRUPT_PRIORITY    1
#endif

/*-----------------------------------------------------------*/

xPortPendSVHandler:
  mrs r0, psp           
  ldr r3, =pxCurrentTCB     /* Get the location of the current TCB. */
  ldr r2, [r3]            

  subs r0, #16
  stm r0!,{r4-r7}
  mov r4, r8
  mov r5, r9
  mov r6, r10
  mov r7, r11
  subs r0, #32
  stm r0!,{r4-r7}
  subs r0, #16
  str r0, [r2]          /* Save the new top of stack into the first member of the TCB. */

  push {r3, lr}
  ldr r0, =configMAX_SYSCALL_INTERRUPT_PRIORITY
  msr primask, r0
  bl vTaskSwitchContext     
  ldr r0, =0
  msr primask, r0
  pop {r3, r4}
  mov r14, r4

  ldr r1, [r3]
  ldr r0, [r1]          /* The first item in pxCurrentTCB is the task top of stack. */
  ldm r0!, {r4-r7}        /* Pop r8-r11 */
  mov r8,  r4
  mov r9,  r5
  mov r10, r6
  mov r11, r7
  ldm r0!, {r4-r7}        /* Pop r4-r7 */
  msr psp, r0
  bx r14

/*-----------------------------------------------------------*/

vPortSVCHandler:
  ldr r3, =pxCurrentTCB
  ldr r1, [r3]
  ldr r0, [r1]
  ldm r0!, {r4-r7}
  mov r8,  r4
  mov r9,  r5
  mov r10, r6
  mov r11, r7
  ldm r0!, {r4-r7}
  msr psp, r0
  ldr r0, =0
  msr primask, r0
  mov r1,r14
  movs r2,#13
  orrs r1,r1,r2
  mov r14,r1
  bx r14

/*-----------------------------------------------------------*/

vPortStartFirstTask:
  /* cm0 has fixed vector table at address 0x0 */
  ldr r0, =0
  ldr r0, [r0]
  /* Set the msp back to the start of the stack. */
  msr msp, r0
  /* Call SVC to start the first task. */
  svc 0

  END
  