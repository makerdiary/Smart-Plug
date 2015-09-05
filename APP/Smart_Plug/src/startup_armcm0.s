;/******************************************************************************
; * @file:    startup_armcm0.s
; * @purpose: CMSIS ARM Cortex-M0 Core Device Startup File
; * @version: V0.01
; * @date:    17. Feb. 2009
; *---------------------------------------------------------------------------
; *
; * Copyright (C) 2009 ARM Limited. All rights reserved.
; *
; * ARM Limited (ARM) is supplying this software for use with Cortex-Mx
; * processor based microcontrollers.  This file can be freely distributed
; * within development tools that are supporting such ARM based processors.
; *
; * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
; * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
; * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
; * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
; * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
; *
; ******************************************************************************/

;
; The modules in this file are included in the libraries, and may be replaced
; by any user-defined modules that define the PUBLIC symbol _program_start or
; a user defined start symbol.
; To override the cstartup defined in the library, simply add your modified
; version to the workbench project.
;
; The vector table is normally located at address 0.
; When debugging in RAM, it can be located in RAM, aligned to at least 2^6.
; The name "__vector_table" has special meaning for C-SPY:
; it is where the SP start value is found, and the NVIC vector
; table register (VTOR) is initialized to this address if != 0.
;
; Cortex-M version
;

#include "app_config.h"

        EXTERN  vPortSVCHandler
        EXTERN  xPortPendSVHandler
        EXTERN  xPortSysTickHandler
        EXTERN  metrology_IRQHandler
        EXTERN  I2C_IRQHandler
        EXTERN  UART_IRQHandler
        EXTERN  SSP_IRQHandler
        EXTERN  BOD_IRQHandler

        MODULE  ?cstartup

        ;; Forward declaration of sections.
        SECTION CSTACK:DATA:NOROOT(3)

        SECTION .intvec:CODE:NOROOT(2)

        EXTERN  __iar_program_start
        PUBLIC  __vector_table
        PUBLIC  __vector_table_0x1c

        DATA
__vector_table
        DCD     sfe(CSTACK)             /* The initial stack pointer */
        DCD     __iar_program_start     /* Exception #1:  The reset handler */
        DCD     NMI_Handler             /* Exception #2:  The NMI handler */
        DCD     HardFault_Handler       /* Exception #3:  The hard fault handler */
        DCD     0                       /* Exception #4:  Reserved */
        DCD     0                       /* Exception #5:  Reserved */
        DCD     0                       /* Exception #6:  Reserved */
__vector_table_0x1c
        DCD     0                       /* Exception #7:  Reserved */
        DCD     0                       /* Exception #8:  Reserved */
        DCD     0                       /* Exception #9:  Reserved */
        DCD     0                       /* Exception #10: Reserved */
        DCD     vPortSVCHandler         /* Exception #11: SVCall handler */
        DCD     0                       /* Exception #12: Reserved */
        DCD     0                       /* Exception #13: Reserved */
        DCD     xPortPendSVHandler      /* Exception #14: The PendSV handler */
        DCD     xPortSysTickHandler     /* Exception #15: The SysTick handler */
        DCD     Default_Handler         /* Exception #16: WAKEUP0_IRQn */
        DCD     Default_Handler         /* Exception #17: WAKEUP1_IRQn */
        DCD     Default_Handler         /* Exception #18: WAKEUP2_IRQn */
        DCD     Default_Handler         /* Exception #19: WAKEUP3_IRQn */
        DCD     Default_Handler         /* Exception #20: WAKEUP4_IRQn */
        DCD     Default_Handler         /* Exception #21: WAKEUP5_IRQn */
        DCD     Default_Handler         /* Exception #22: WAKEUP6_IRQn */
        DCD     Default_Handler         /* Exception #23: WAKEUP7_IRQn */
        DCD     Default_Handler         /* Exception #24: WAKEUP8_IRQn */
        DCD     Default_Handler         /* Exception #25: WAKEUP9_IRQn */
        DCD     Default_Handler         /* Exception #26: WAKEUP10_IRQn */
        DCD     Default_Handler         /* Exception #27: WAKEUP11_IRQn */
        DCD     Default_Handler         /* Exception #28: WAKEUP12_IRQn */
        DCD     Default_Handler         /* Exception #29: WAKEUP13_IRQn */
        DCD     Default_Handler         /* Exception #30: WAKEUP14_IRQn */
#ifdef SUPPORT_I2C_BUS
        DCD     I2C_IRQHandler          /* Exception #31: I2C_IRQn */
#else
        DCD     Default_Handler         /* Exception #31: I2C_IRQn */
#endif
        DCD     Default_Handler         /* Exception #32: TIMER_16_0_IRQn */
        DCD     metrology_IRQHandler    /* Exception #33: METROLOGY_IRQn */
        DCD     Default_Handler         /* Exception #34: TIMER_32_0_IRQn */
        DCD     Default_Handler         /* Exception #35: TIMER_32_1_IRQn */
#ifdef SUPPORT_OL2381
        DCD     SSP_IRQHandler          /* Exception #36: SSP_IRQn */
#else
        DCD     Default_Handler         /* Exception #36: SSP_IRQn */
#endif
#ifdef SUPPORT_FOR_SERIAL_PORT
        DCD     UART_IRQHandler         /* Exception #37: UART_IRQn */
#else
        DCD     Default_Handler         /* Exception #37: UART_IRQn */
#endif
        DCD     Default_Handler         /* Exception #38: RESERVED */
        DCD     Default_Handler         /* Exception #39: RESERVED */
        DCD     Default_Handler         /* Exception #40: RESERVED */
        DCD     Default_Handler         /* Exception #41: WDT_IRQn   (Watchdog Timer) */
        DCD     BOD_IRQHandler          /* Exception #42: BOD_IRQn   (Brownout Detect) */
        DCD     Default_Handler         /* Exception #43: RESERVED */
        DCD     Default_Handler         /* Exception #44: EINT3_IRQn */
        DCD     Default_Handler         /* Exception #45: EINT2_IRQn */
        DCD     Default_Handler         /* Exception #46: EINT1_IRQn */
        DCD     Default_Handler         /* Exception #47: EINT0_IRQn */

;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;;
;;
;; Default interrupt handlers.
;;
        THUMB

        PUBWEAK NMI_Handler
        SECTION .text:CODE:REORDER(1)
NMI_Handler
        B NMI_Handler

        PUBWEAK HardFault_Handler
        SECTION .text:CODE:REORDER(1)
HardFault_Handler
        B HardFault_Handler

        PUBWEAK Default_Handler
        SECTION .text:CODE:REORDER(1)
Default_Handler
        B Default_Handler

        END
