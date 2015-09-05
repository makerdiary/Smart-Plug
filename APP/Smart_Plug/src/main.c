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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

/*****************************************************************************
 * project include files
 *****************************************************************************/

#include "EM7xx.h"
#include "bsp.h"
#include "rtk.h"
#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "app.h"

xTaskHandle  pAppStartHandle;
static unsigned int   sys_seconds_cnt     = 0;
static unsigned short sys_millisec_cnt    = 0;
static unsigned int   app_millisec_cnt[2] = {0,0};

#if (configGENERATE_RUN_TIME_STATS == 1)
static void calibrateIdleCount(void);
static void updateIdleCount(int millisec_increment);
#endif

/*-----------------------------------------------------------*/

static void AppStart(void *p_arg)
{
    // (4) The function call calibrateIdleCount() must be the first instruction in this task.
    //     Beside this task, only the idle task should run in order to have a good working
    //     CPU load measurement via the function GetCpuLoad()
    
#if (configGENERATE_RUN_TIME_STATS == 1)
    calibrateIdleCount();
#endif
    
    // (5) All communication interfaces (USB, ethernet, etc) must be inited after calibrateIdleCount()
    

    // (6) Now the application (and all other tasks) can be started
    
    app_main(p_arg);
}

int main (void)
{
    // (1) Setup the system, eg. setup all PLLs, set the CPU clock, init the UART...
    
    SystemInit();
    bsp_init();

    // (2) Only create the applications main task before starting the scheduler,
    //     all other tasks should be created after calling calibrateIdleCount()
    
    xTaskCreate(AppStart, (const signed portCHAR *)"MAIN",
                MAIN_TASK_STK_SIZE, (void *) NULL,
                MAIN_TASK_PRIORITY, &pAppStartHandle);

    // (3) Now all the tasks have been started - start the scheduler.
    //     NOTE : Tasks run in system mode and the scheduler runs in Supervisor mode.
    //     The processor MUST be in supervisor mode when vTaskStartScheduler is
    //     called.  The demo applications included in the FreeRTOS.org download switch
    //     to supervisor mode prior to main being called.  If you are not using one of
    //     these demo application projects then ensure Supervisor mode is used here.

	vTaskStartScheduler();

    return 0;
}

// KdW 27 oktober 2010
// The original code is only correct if configTICK_RATE_HZ is 1000
// This code is correct if 1000 can be divided by configTICK_RATE_HZ so that
// one clock tick is an integral number of milliseconds.
// For instance, values of 1000, 500, 250, 100 and 50 can be used.
//
void vApplicationTickHook( void )
{
    int millisec_increment = 1000/configTICK_RATE_HZ;
    sys_millisec_cnt += millisec_increment;
    if( sys_millisec_cnt >= 1000 )
    {
        sys_millisec_cnt -= 1000;
        sys_seconds_cnt++;
    }
    app_millisec_cnt[0] += millisec_increment;
    app_millisec_cnt[1] += millisec_increment;
#if (configGENERATE_RUN_TIME_STATS == 1)
    updateIdleCount(millisec_increment);
#endif
}

#if (configGENERATE_RUN_TIME_STATS == 1)
//------------------------------------------------------------------------------
// For this CPU load measurement functionality to work, only one task should be
// created before vTaskStartScheduler() is called. This first created task must
// then immediately call the function calibrateIdleCount() as the first thing it
// does. The calibrateIdleCount() function will then measure how many times the
// vApplicationIdleHook() function can be called in a totally idle system.

#define CPU_LOAD_MEASURE_TIME   250

static unsigned int  CalibratedIdleCount = 0;
static unsigned int  RunningIdleCount    = 0;
static unsigned int  IdleCount           = 0;
static unsigned int  idle_millisec_cnt   = 0;
static unsigned char resetRIC            = 0; // reset RunningIdleCount

static void calibrateIdleCount(void)
{
    rtk_tsk_sleep(2 * CPU_LOAD_MEASURE_TIME);
    // Store the IdleCount in a totally idle system
    CalibratedIdleCount = IdleCount;
}

static void updateIdleCount(int millisec_increment)
{
    idle_millisec_cnt += millisec_increment;
    if (idle_millisec_cnt >= CPU_LOAD_MEASURE_TIME)
    {
        idle_millisec_cnt = 0;
        // Store the IdleCount of the system during the last time period,
        // which is defined by CPU_LOAD_MEASURE_TIME in milli seconds
        IdleCount = RunningIdleCount;
        RunningIdleCount = 0; // must be done here too in case CPU 100 % busy
        resetRIC = 1;
    }
}

unsigned int GetCpuLoad(void)
{
    unsigned int cpuLoad;
    if (IdleCount == 0)
    {
        cpuLoad = 100; // CPU is 100 % busy
    }
    else if (IdleCount >= CalibratedIdleCount)
    {
        cpuLoad = 0;   // CPU is 100 % idle
    }
    else
    {
        // CPU load (0 till 100 %) is rounded up to the nearest percent
        cpuLoad = 100 - ((IdleCount * 100) / CalibratedIdleCount);
    }
    return cpuLoad;
}
#endif

void vApplicationIdleHook(void)
{
#if (configGENERATE_RUN_TIME_STATS == 1)
    RunningIdleCount++;
    // The next if statement is needed because the above increment is not atomic
    if (resetRIC)
    {
        resetRIC = 0;
        RunningIdleCount = 0;
    }
#else
    SCB->SCR = 0;
    asm("WFI");
#endif
}

//------------------------------------------------------------------------------

void vApplicationStackOverflowHook(xTaskHandle *pxTask, signed portCHAR *pcTaskName)
{
  /* This function will get called if a task overflows its stack. */
	
  for(;;);
}

//------------------------------------------------------------------------------

#if (configGENERATE_RUN_TIME_STATS == 1)
void vConfigureTimerForRunTimeStats( void )
{
const unsigned long TCR_COUNT_RESET = 2, CTCR_CTM_TIMER = 0x00, TCR_COUNT_ENABLE = 0x01;

	/* This function configures a timer that is used as the time base when
	collecting run time statistical information - basically the percentage
	of CPU time that each task is utilising.  It is called automatically when
	the scheduler is started (assuming configGENERATE_RUN_TIME_STATS is set
	to 1. */
	
    SYSCON->SYSAHBCLKCTRL |= (1 << 9);  /* Power up the timer. */
	
	/* Reset Timer 0 */
	TIM32_0->TCR = TCR_COUNT_RESET;
	
	/* Just count up. */
	TIM32_0->CTCR = CTCR_CTM_TIMER;

	/* Prescale to a frequency that is good enough to get a decent resolution,
	but not too fast so as to overflow all the time. The counter should be at
    least 10 times the frequency of the tick count (we choose 20 times). */
	TIM32_0->PR =  ( configTICK_RATE_HZ * 20 ) - 1UL;

	/* Start the counter. */
	TIM32_0->TCR = TCR_COUNT_ENABLE;
}

unsigned long vGetRunTimeCounterValue( void )
{
    return TIM32_0->TC;
}
#endif

//------------------------------------------------------------------------------

void Reset_msec_cnt(int timer_nr)
{
  app_millisec_cnt[timer_nr] = 0;
}

unsigned int Get_msec_cnt(int timer_nr)
{
  return app_millisec_cnt[timer_nr];
}

unsigned int Get_sys_seconds_cnt(void)
{
  return sys_seconds_cnt;
}

void Get_sys_time(unsigned int *psec, unsigned short *pmsec)
{
    portENTER_CRITICAL();
    *psec  = sys_seconds_cnt;
    *pmsec = (short)sys_millisec_cnt;
    portEXIT_CRITICAL();
}

/* EOF */
