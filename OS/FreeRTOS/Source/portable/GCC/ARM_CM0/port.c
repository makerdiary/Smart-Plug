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

/*-----------------------------------------------------------
 * Implementation of functions defined in portable.h for the ARM CM0 port.
 *----------------------------------------------------------*/

/* Scheduler includes. */
#include "FreeRTOS.h"
#include "task.h"

/* For backward compatibility, ensure configKERNEL_INTERRUPT_PRIORITY is
defined.  The value should also ensure backward compatibility.
FreeRTOS.org versions prior to V4.4.0 did not include this definition. */
#ifndef configKERNEL_INTERRUPT_PRIORITY
  #define configKERNEL_INTERRUPT_PRIORITY 3
#endif

/* Constants required to manipulate the NVIC. */
#define portNVIC_PENDSVSET      0x10000000

/* Constants required to set up the initial stack. */
#define portINITIAL_XPSR      ( 0x01000000 )

/* Each task maintains its own interrupt status in the critical nesting
variable. */
static unsigned portBASE_TYPE uxCriticalNesting = 0xaaaaaaaa;

/*
 * Exception handlers.
 */
void xPortPendSVHandler( void ) __attribute__ (( naked ));
void xPortSysTickHandler( void );
void vPortSVCHandler( void ) __attribute__ (( naked ));

/*
 * Start first task is a separate function so it can be tested in isolation.
 */
void vPortStartFirstTask( void ) __attribute__ (( naked ));

/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portSTACK_TYPE *pxPortInitialiseStack( portSTACK_TYPE *pxTopOfStack, pdTASK_CODE pxCode, void *pvParameters )
{
  /* Simulate the stack frame as it would be created by a context switch
  interrupt. */
  *pxTopOfStack = portINITIAL_XPSR;                /* xPSR */
  pxTopOfStack--;
  *pxTopOfStack = ( portSTACK_TYPE ) pxCode;       /* Return address (new PC) */
  pxTopOfStack--;
  *pxTopOfStack = 0;                               /* LR (R14) */
  pxTopOfStack -= 5;                               /* R12, R3, R2, R1 */
  *pxTopOfStack = ( portSTACK_TYPE ) pvParameters; /* and R0 */
  pxTopOfStack -= 8;                               /* R7, R6, R5, R4, R11, R10, R9, and R8. */
  return pxTopOfStack;
}
/*-----------------------------------------------------------*/

void __attribute__ (( naked )) vPortSVCHandler( void )
{
    asm volatile(
        " ldr r3, pxCurrentTCBConst2 \n" /* Restore the context. */
        " ldr r1, [r3]		         \n" /* Use pxCurrentTCBConst to get the pxCurrentTCB address. */
        " ldr r0, [r1]		         \n" /* The first item in pxCurrentTCB is the task top of stack. */
        " ldm r0!, {r4-r7}		     \n"
        " mov r8,  r4		         \n"
        " mov r9,  r5		         \n"
        " mov r10, r6		         \n"
        " mov r11, r7		         \n"
        " ldm r0!, {r4-r7}		     \n"
        " msr psp, r0		         \n" /* Restore the task stack pointer. */
        " ldr r0, =0		         \n"
        " msr primask, r0		     \n"
        " mov r1,r14		         \n"
        " movs r2,#13		         \n"
        " orr r1,r1,r2		         \n"
        " mov r14,r1		         \n"
        " bx r14		             \n"
        "							 \n"
        " .align 2					 \n"
        "pxCurrentTCBConst2: .word pxCurrentTCB	\n"
    );
}
/*-----------------------------------------------------------*/

void __attribute__ (( naked )) vPortStartFirstTask( void )
{
    asm volatile(
        " ldr r0, =0    \n" /* cm0 has fixed vector table at address 0x0 */
        " ldr r0, [r0]  \n"
        " msr msp, r0   \n" /* Set the msp back to the start of the stack. */
        " svc 0         \n" /* System call to start first task. */
    );
}
/*-----------------------------------------------------------*/

/*
 * See header file for description.
 */
portBASE_TYPE xPortStartScheduler( void )
{
	/*
	 * Setup the systick timer to generate the tick interrupts at the required
	 * frequency. Interrupts are disabled here already.
	 */
    SysTick_Config( configCPU_CLOCK_HZ / configTICK_RATE_HZ );

    /* Make PendSV the same priroity as SysTick and the kernel. */
    NVIC_SetPriority (PendSV_IRQn, configKERNEL_INTERRUPT_PRIORITY);

    /* Initialise the critical nesting count ready for the first task. */
    uxCriticalNesting = 0;

    /* Needed here since CM0 doesn't have interrupt priority masking */
    portENABLE_INTERRUPTS();

    /* Start the first task. */
    vPortStartFirstTask();

    /* Should not get here! */
    return 0;
}
/*-----------------------------------------------------------*/

void vPortEndScheduler( void )
{
  /* It is unlikely that the CM0 port will require this function as there
  is nothing to return to.  */
}
/*-----------------------------------------------------------*/

void vPortYieldFromISR( void )
{
  /* Set a PendSV to request a context switch. */
  SCB->ICSR = portNVIC_PENDSVSET;
}
/*-----------------------------------------------------------*/

void vPortEnterCritical( void )
{
  portDISABLE_INTERRUPTS();
  uxCriticalNesting++;
}
/*-----------------------------------------------------------*/

void vPortExitCritical( void )
{
  uxCriticalNesting--;
  if( uxCriticalNesting == 0 )
  {
    portENABLE_INTERRUPTS();
  }
}
/*-----------------------------------------------------------*/

void __attribute__ (( naked )) xPortPendSVHandler( void )
{
    asm volatile(
        " mrs r0, psp               \n"
        "                           \n"
        " ldr r3, pxCurrentTCBConst \n" /* Get the location of the current TCB. */
        " ldr r2, [r3]              \n"
        "                           \n"
        " sub r0, #16               \n"
        " stm r0!,{r4-r7}           \n"
        " mov r4, r8                \n"
        " mov r5, r9                \n"
        " mov r6, r10               \n"
        " mov r7, r11               \n"
        " sub r0, #32               \n"
        " stm r0!,{r4-r7}           \n"
        " sub r0, #16               \n"
        " str r0, [r2]              \n" /* Save the new top of stack into the first member of the TCB. */
        "                           \n"
        " push {r3, lr}             \n"
        " mov r0, %0                \n"
        " msr primask, r0           \n"
        " bl vTaskSwitchContext     \n"
        " ldr r0, =0                \n"
        " msr primask, r0           \n"
        " pop {r3, r4}              \n"
        " mov r14, r4               \n"
        "							\n"	/* Restore the context, including the critical nesting count. */
        " ldr r1, [r3]              \n"
        " ldr r0, [r1]              \n" /* The first item in pxCurrentTCB is the task top of stack. */
        " ldm r0!, {r4-r7}          \n" /* Pop r8-r11 */
        " mov r8,  r4               \n"
        " mov r9,  r5               \n"
        " mov r10, r6               \n"
        " mov r11, r7               \n"
        " ldm r0!, {r4-r7}          \n" /* Pop r4-r7 */
        " msr psp, r0               \n"
        " bx r14                    \n"
        "							\n"
        " .align 2					\n"
        "pxCurrentTCBConst: .word pxCurrentTCB \n"
        ::"i"(configMAX_SYSCALL_INTERRUPT_PRIORITY)
    );
}
/*-----------------------------------------------------------*/

void xPortSysTickHandler( void )
{
unsigned portLONG ulDummy;

  /* If using preemption, also force a context switch. */
  #if configUSE_PREEMPTION == 1
    SCB->ICSR = portNVIC_PENDSVSET;
  #endif

  ulDummy = portSET_INTERRUPT_MASK_FROM_ISR();
  {
    vTaskIncrementTick();
  }
  portCLEAR_INTERRUPT_MASK_FROM_ISR( ulDummy );
}
/*-----------------------------------------------------------*/
