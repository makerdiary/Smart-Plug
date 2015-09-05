/******************************************************************************
 * @file:    system_EMxxx.c
 * @brief    CMSIS ARM Cortex-M0 Device Peripheral Access Layer Source File
 *           for the NXP EMxxx
 * @version  V1.02
 * @date     08. September 2009
 *
* @note
 * Copyright (C) 2009 ARM Limited. All rights reserved.
 *
 * @par
 * ARM Limited (ARM) is supplying this software for use with Cortex-M
 * processor based microcontrollers.  This file can be freely distributed
 * within development tools that are supporting such ARM based processors.
 *
 * @par
 * THIS SOFTWARE IS PROVIDED "AS IS".  NO WARRANTIES, WHETHER EXPRESS, IMPLIED
 * OR STATUTORY, INCLUDING, BUT NOT LIMITED TO, IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE APPLY TO THIS SOFTWARE.
 * ARM SHALL NOT, IN ANY CIRCUMSTANCES, BE LIABLE FOR SPECIAL, INCIDENTAL, OR
 * CONSEQUENTIAL DAMAGES, FOR ANY REASON WHATSOEVER.
 *
 ******************************************************************************/


#include <stdint.h>
#include <EM7xx.h>
#include "app_config.h"

/*
//-------- <<< Use Configuration Wizard in Context Menu >>> ------------------
*/

/*--------------------- Clock Configuration ----------------------------------
//
// <e> Clock Configuration
//   <h> System Controls and Status Register (SCS)
//     <o1.4>    OSCRANGE: Main Oscillator Range Select
//                     <0=>  1 MHz to 20 MHz
//                     <1=> 15 MHz to 24 MHz
//     <e1.5>       OSCEN: Main Oscillator Enable
//     </e>
//   </h>
//
//   <h> Clock Source Select Register (CLKSRCSEL)
//     <o2.0..1>   CLKSRC: PLL Clock Source Selection
//                     <0=> Internal RC oscillator
//                     <1=> Main oscillator
//                     <2=> RTC oscillator
//   </h>
//
//   <e3> PLL0 Configuration (Main PLL)
//     <h> PLL0 Configuration Register (PLL0CFG)
//                     <i> F_cco0 = (2 * M * F_in) / N
//                     <i> F_in must be in the range of 32 kHz to 50 MHz
//                     <i> F_cco0 must be in the range of 275 MHz to 550 MHz
//       <o4.0..14>  MSEL: PLL Multiplier Selection
//                     <6-32768><#-1>
//                     <i> M Value
//       <o4.16..23> NSEL: PLL Divider Selection
//                     <1-256><#-1>
//                     <i> N Value
//     </h>
//   </e>
//
//   <e5> PLL1 Configuration (USB PLL)
//     <h> PLL1 Configuration Register (PLL1CFG)
//                     <i> F_usb = M * F_osc or F_usb = F_cco1 / (2 * P)
//                     <i> F_cco1 = F_osc * M * 2 * P
//                     <i> F_cco1 must be in the range of 156 MHz to 320 MHz
//       <o6.0..4>   MSEL: PLL Multiplier Selection
//                     <1-32><#-1>
//                     <i> M Value (for USB maximum value is 4)
//       <o6.5..6>   PSEL: PLL Divider Selection
//                     <0=> 2
//                     <1=> 4
//                     <2=> 8
//                     <3=> 16
//                     <i> P Value
//     </h>
//   </e>
//
//   <h> CPU Clock Configuration Register (CCLKCFG)
//     <o7.0..7>  CCLKSEL: Divide Value for CPU Clock from PLL0
//                     <0-255>
//                     <i> Divide is CCLKSEL + 1. Only 0 and odd values are valid.
//   </h>
//
//   <h> USB Clock Configuration Register (USBCLKCFG)
//     <o8.0..3>   USBSEL: Divide Value for USB Clock from PLL1
//                     <0-15>
//                     <i> Divide is USBSEL + 1
//   </h>
//
// </e>
*/
#define CLOCK_SETUP           1

#define MAIN_PLL_SETUP        1
#define MAIN_CLKSRCSEL_Val    0x00000001

#if ( LPC_CORE_CLOCKSPEED_HZ == 12000000 )
  #define MAIN_PLL_M_Val      0x00
  #define MAIN_PLL_P_Val      0x00000003
#endif
#if ( LPC_CORE_CLOCKSPEED_HZ == 24000000 )
  #define MAIN_PLL_M_Val      0x01
  #define MAIN_PLL_P_Val      0x00000002
#endif
#if ( LPC_CORE_CLOCKSPEED_HZ == 36000000 )
  #define MAIN_PLL_M_Val      0x02
  #define MAIN_PLL_P_Val      0x00000002
#endif
#if ( LPC_CORE_CLOCKSPEED_HZ == 48000000 )
  #define MAIN_PLL_M_Val      0x03
  #define MAIN_PLL_P_Val      0x00000001
#endif

#ifndef MAIN_PLL_M_Val
#error "Non supported cpu clock setting for M"
#endif

#ifndef MAIN_PLL_P_Val
#error "Non supported cpu clock setting for P"
#endif

#define SYS_AHB_DIV_Val       1			/* 1 through 255, typical is 1 or 2 or 4 */

#define WDT_CLK_SETUP         1         /* If WDT is enabled, this clock has to be set. */
#define WDT_CLKSRCSEL_Val     0x00000002

#define CLKOUT_SETUP          0

#define CCLKCFG_Val           0x00000003
#define PCLKSEL0_Val          0x00000000
#define PCLKSEL1_Val          0x00000000
#define PCONP_Val             0x042887DE

/*
//-------- <<< end of configuration section >>> ------------------------------
*/

/*----------------------------------------------------------------------------
  DEFINES
 *----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------
  Define clocks
 *----------------------------------------------------------------------------*/
#define XTAL        (12000000UL)        /* Oscillator frequency               */
#define OSC_CLK     (      XTAL)        /* Main oscillator frequency          */
#define IRC_OSC     ( 4000000UL)        /* Internal RC oscillator frequency   */
#define WDT_OSC     (  250000UL)        /* WDT oscillator frequency           */

/*----------------------------------------------------------------------------
  Clock Variable definitions
 *----------------------------------------------------------------------------*/
uint32_t SystemCoreClock = IRC_OSC; /*!< System Clock Frequency (Core Clock)  */
uint32_t ClockSource = IRC_OSC;
uint32_t SystemAHBFrequency = IRC_OSC;

/**
 * Misc. clock generation modules
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void WDT_CLK_Setup ( void )
{
  /* Watchdog configuration. */
  /* Freq = 0.5Mhz, div_sel is 0, divided by 2. WDT_OSC should be 250khz */
  SYSCON->WDTOSCCTRL = (0x1<<5)|0x00;
  SYSCON->WDTCLKSEL = WDT_CLKSRCSEL_Val;  /* Select clock */
  SYSCON->WDTCLKUEN = 0x01;               /* Update clock */
  SYSCON->WDTCLKUEN = 0x00;               /* Toggle update register once */
  SYSCON->WDTCLKUEN = 0x01;
  while ( !(SYSCON->WDTCLKUEN & 0x01) );  /* Wait until updated */
  SYSCON->WDTCLKDIV = 1;            /* Divided by 1 */
  SYSCON->PDRUNCFG &= ~(0x1<<6);    /* Let WDT clock run */
  return;
}

int WDT_clk_get(void)
{
  return 250000; // Watch out: this is related to the above function
}

void Main_PLL_Setup ( void )
{
  uint32_t regVal;

  ClockSource = OSC_CLK;
  SYSCON->SYSPLLCLKSEL = MAIN_CLKSRCSEL_Val;   /* Select system OSC */
  SYSCON->SYSPLLCLKUEN = 0x01;                 /* Update clock source */
  SYSCON->SYSPLLCLKUEN = 0x00;                 /* toggle Update register once */
  SYSCON->SYSPLLCLKUEN = 0x01;
  while ( !(SYSCON->SYSPLLCLKUEN & 0x01) ); /* Wait until updated */

  regVal = SYSCON->SYSPLLCTRL;
  regVal &= ~0x1FF;
  SYSCON->SYSPLLCTRL = (regVal | (MAIN_PLL_P_Val<<5) | MAIN_PLL_M_Val);

  /* Enable main system PLL, main system PLL bit 7 in PDRUNCFG. */
  SYSCON->PDRUNCFG &= ~(0x1<<7);
  while ( !(SYSCON->SYSPLLSTAT & 0x01) );	/* Wait until it's locked */

  SYSCON->MAINCLKSEL = 0x03;		/* Select PLL clock output */
  SYSCON->MAINCLKUEN = 0x01;		/* Update MCLK clock source */
  SYSCON->MAINCLKUEN = 0x00;		/* Toggle update register once */
  SYSCON->MAINCLKUEN = 0x01;
  while ( !(SYSCON->MAINCLKUEN & 0x01) );	/* Wait until updated */

  SYSCON->SYSAHBCLKDIV = SYS_AHB_DIV_Val;	/* SYS AHB clock, typical is 1 or 2 or 4 */

#if CLKOUT_SETUP
  /* debug PLL after configuration. */
  SYSCON->CLKOUTCLKSEL = 0x03;		/* Select Main clock */
  SYSCON->CLKOUTCLKUEN = 0x01;		/* Update clock */
  SYSCON->CLKOUTCLKUEN = 0x00;		/* Toggle update register once */
  SYSCON->CLKOUTCLKUEN = 0x01;
  while ( !(SYSCON->CLKOUTCLKUEN & 0x01) );		/* Wait until updated */
  SYSCON->CLKOUTCLKDIV = 1;			/* Divided by 1 */
#endif

#if MAIN_PLL_SETUP
  SystemCoreClock = ClockSource * (MAIN_PLL_M_Val+1);
#else
  SystemCoreClock = ClockSource;
#endif
  SystemAHBFrequency = (uint32_t)(SystemCoreClock/SYS_AHB_DIV_Val);
  return;
}


/**
 * Initialize the system
 *
 * @param  none
 * @return none
 *
 * @brief  Setup the microcontroller system.
 *         Initialize the System and update the SystemFrequency variable.
 */
void SystemInit (void)
{
  uint32_t i;

#ifdef __DEBUG_RAM
  SYSCON->SYSMEMREMAP = 0x1;			/* remap to internal RAM */
#else
#ifdef __DEBUG_FLASH
  SYSCON->SYSMEMREMAP = 0x2;			/* remap to internal flash */
#endif
#endif

#if (CLOCK_SETUP)                       /* Clock Setup                        */
  /* bit 0 default is crystal bypass,
  bit1 0=0~20Mhz crystal input, 1=15~50Mhz crystal input. */
  SYSCON->SYSOSCCTRL = 0x00;

  /* main system OSC run is cleared, bit 5 in PDRUNCFG register */
  SYSCON->PDRUNCFG &= ~(0x1<<5);
  /* Wait 200us for OSC to be stablized, no status
  indication, dummy wait. */
  for ( i = 0; i < 0x100; i++ );

#if (MAIN_PLL_SETUP)
  Main_PLL_Setup();
#endif

#if (WDT_CLK_SETUP)
  WDT_CLK_Setup();
#endif

#endif	/* endif CLOCK_SETUP */

  /* System clock to the IOCON needs to be enabled or
  most of the I/O related peripherals won't work. */
  SYSCON->SYSAHBCLKCTRL |= (1<<16);
  return;
}
