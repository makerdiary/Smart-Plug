/*****************************************************************************
 *   wdt.c:  WDT API file for NXP EMxxx
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2009.08.25  ver 1.00    First version
 *
******************************************************************************/

#include "bsp.h"
#include "wdt.h"

/*****************************************************************************
 * Function name: WDT_Init
 * Description  : Initialize watchdog timer
 *                 The watchdog timeout = Twdtclk * WDT_FEED_VALUE * 4
 *                 The AHB clock is used for Twdtclk
 * Argument(s)  : none.
 * Return(s)    : none.
 *****************************************************************************/

#ifdef LPC17XX_BUILD
  #define WDT_ONE_SECOND_IRC  0x00100000                 // 1 second when internal RC is used
  #define WDT_FEED_VALUE      (7 * WDT_ONE_SECOND_IRC)   // 7 Seconds watchdog timeout
#else
  #define WDT_TIMEOUT_IN_SEC    7   // 7 Seconds watchdog timeout
#endif

void WDT_Init( void )
{
#ifndef DEBUG
 #ifdef LPC17XX_BUILD
    WDTC   = WDT_FEED_VALUE;  /* once WDEN is set, the WDT will start after feeding */
    WDMOD  = WDEN | WDRESET;
 #else
    uint32_t wdt_feed_value = 0;

    wdt_feed_value = WDT_clk_get();
    wdt_feed_value = (wdt_feed_value / 4) * WDT_TIMEOUT_IN_SEC;

    /* Enable AHB clock to the WDT. */
    SYSCON->SYSAHBCLKCTRL |= (1<<15);

	WDT->TC     = wdt_feed_value;  /* once WDEN is set, the WDT will start after feeding */
	WDT->MOD    = WDEN | WDRESET;
 #endif
// JvdG: First feed will be done at a later moment
//    WDT_Feed();
#endif
}

/*****************************************************************************
 * Function name: WDT_Feed
 * Description  : Feed watchdog timer to prevent it from timeout
 * Argument(s)  : none.
 * Return(s)    : none.
 *****************************************************************************/

void WDT_Feed( void )
{
#ifndef DEBUG
 #ifdef LPC17XX_BUILD
    WDFEED = 0xAA;            /* Feeding sequence */
    WDFEED = 0x55;
 #else
	WDT->FEED = 0xAA;            /* Feeding sequence */
	WDT->FEED = 0x55;
 #endif
#endif
}

/******************************************************************************
**                            End Of File
******************************************************************************/
