/**************************************************************************//**
 * @file     LPC13xx.h
 * @brief    CMSIS Cortex-M3 Core Peripheral Access Layer Header File for
 *           NXP LPC13xx Device Series
 * @version  V1.01
 * @date     19. October 2009
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


#ifndef __LPC13xx_H__
#define __LPC13xx_H__

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

#ifdef IAR_ARM_CM3
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
#define NonMaskableInt_IRQn     -14      /*!< 2 Non Maskable Interrupt                         */
#define MemoryManagement_IRQn   -12      /*!< 4 Cortex-M3 Memory Management Interrupt          */
#define BusFault_IRQn           -11      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
#define UsageFault_IRQn         -10      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
#define SVCall_IRQn             -5       /*!< 11 Cortex-M3 SV Call Interrupt                   */
#define DebugMonitor_IRQn       -4       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
#define PendSV_IRQn             -2       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
#define SysTick_IRQn            -1       /*!< 15 Cortex-M3 System Tick Interrupt               */

/******  LPC13xx Specific Interrupt Numbers *******************************************************/
#define WAKEUP0_IRQn            0        /*!< All I/O pins can be used as wakeup source.       */
#define WAKEUP1_IRQn            1        /*!< There are 40 pins in total for LPC17xx           */
#define WAKEUP2_IRQn            2
#define WAKEUP3_IRQn            3
#define WAKEUP4_IRQn            4
#define WAKEUP5_IRQn            5
#define WAKEUP6_IRQn            6
#define WAKEUP7_IRQn            7
#define WAKEUP8_IRQn            8
#define WAKEUP9_IRQn            9
#define WAKEUP10_IRQn           10
#define WAKEUP11_IRQn           11
#define WAKEUP12_IRQn           12
#define WAKEUP13_IRQn           13
#define WAKEUP14_IRQn           14
#define WAKEUP15_IRQn           15
#define WAKEUP16_IRQn           16
#define WAKEUP17_IRQn           17
#define WAKEUP18_IRQn           18
#define WAKEUP19_IRQn           19
#define WAKEUP20_IRQn           20
#define WAKEUP21_IRQn           21
#define WAKEUP22_IRQn           22
#define WAKEUP23_IRQn           23
#define WAKEUP24_IRQn           24
#define WAKEUP25_IRQn           25
#define WAKEUP26_IRQn           26
#define WAKEUP27_IRQn           27
#define WAKEUP28_IRQn           28
#define WAKEUP29_IRQn           29
#define WAKEUP30_IRQn           30
#define WAKEUP31_IRQn           31
#define WAKEUP32_IRQn           32
#define WAKEUP33_IRQn           33
#define WAKEUP34_IRQn           34
#define WAKEUP35_IRQn           35
#define WAKEUP36_IRQn           36
#define WAKEUP37_IRQn           37
#define WAKEUP38_IRQn           38
#define WAKEUP39_IRQn           39
#define I2C_IRQn                40       /*!< I2C Interrupt                                    */
#define TIMER_16_0_IRQn         41       /*!< 16-bit Timer0 Interrupt                          */
#define TIMER_16_1_IRQn         42       /*!< 16-bit Timer1 Interrupt                          */
#define TIMER_32_0_IRQn         43       /*!< 32-bit Timer0 Interrupt                          */
#define TIMER_32_1_IRQn         44       /*!< 32-bit Timer1 Interrupt                          */
#define SSP_IRQn                45       /*!< SSP Interrupt                                    */
#define UART_IRQn               46       /*!< UART Interrupt                                   */
#define USB_IRQn                47       /*!< USB Regular Interrupt                            */
#define USB_FIQn                48       /*!< USB Fast Interrupt                               */
#define ADC_IRQn                49       /*!< A/D Converter Interrupt                          */
#define WDT_IRQn                50       /*!< Watchdog timer Interrupt                         */
#define BOD_IRQn                51       /*!< Brown Out Detect(BOD) Interrupt                  */
#define EINT3_IRQn              53       /*!< External Interrupt 3 Interrupt                   */
#define EINT2_IRQn              54       /*!< External Interrupt 2 Interrupt                   */
#define EINT1_IRQn              55       /*!< External Interrupt 1 Interrupt                   */
#define EINT0_IRQn              56       /*!< External Interrupt 0 Interrupt                   */

#define IRQn_Type  int32_t

#else

typedef enum IRQn
{
/******  Cortex-M3 Processor Exceptions Numbers ***************************************************/
  NonMaskableInt_IRQn           = -14,      /*!< 2 Non Maskable Interrupt                         */
  MemoryManagement_IRQn         = -12,      /*!< 4 Cortex-M3 Memory Management Interrupt          */
  BusFault_IRQn                 = -11,      /*!< 5 Cortex-M3 Bus Fault Interrupt                  */
  UsageFault_IRQn               = -10,      /*!< 6 Cortex-M3 Usage Fault Interrupt                */
  SVCall_IRQn                   = -5,       /*!< 11 Cortex-M3 SV Call Interrupt                   */
  DebugMonitor_IRQn             = -4,       /*!< 12 Cortex-M3 Debug Monitor Interrupt             */
  PendSV_IRQn                   = -2,       /*!< 14 Cortex-M3 Pend SV Interrupt                   */
  SysTick_IRQn                  = -1,       /*!< 15 Cortex-M3 System Tick Interrupt               */

/******  LPC13xx Specific Interrupt Numbers *******************************************************/
  WAKEUP0_IRQn                  = 0,        /*!< All I/O pins can be used as wakeup source.       */
  WAKEUP1_IRQn                  = 1,        /*!< There are 40 pins in total for LPC17xx           */
  WAKEUP2_IRQn                  = 2,
  WAKEUP3_IRQn                  = 3,
  WAKEUP4_IRQn                  = 4,
  WAKEUP5_IRQn                  = 5,
  WAKEUP6_IRQn                  = 6,
  WAKEUP7_IRQn                  = 7,
  WAKEUP8_IRQn                  = 8,
  WAKEUP9_IRQn                  = 9,
  WAKEUP10_IRQn                 = 10,
  WAKEUP11_IRQn                 = 11,
  WAKEUP12_IRQn                 = 12,
  WAKEUP13_IRQn                 = 13,
  WAKEUP14_IRQn                 = 14,
  WAKEUP15_IRQn                 = 15,
  WAKEUP16_IRQn                 = 16,
  WAKEUP17_IRQn                 = 17,
  WAKEUP18_IRQn                 = 18,
  WAKEUP19_IRQn                 = 19,
  WAKEUP20_IRQn                 = 20,
  WAKEUP21_IRQn                 = 21,
  WAKEUP22_IRQn                 = 22,
  WAKEUP23_IRQn                 = 23,
  WAKEUP24_IRQn                 = 24,
  WAKEUP25_IRQn                 = 25,
  WAKEUP26_IRQn                 = 26,
  WAKEUP27_IRQn                 = 27,
  WAKEUP28_IRQn                 = 28,
  WAKEUP29_IRQn                 = 29,
  WAKEUP30_IRQn                 = 30,
  WAKEUP31_IRQn                 = 31,
  WAKEUP32_IRQn                 = 32,
  WAKEUP33_IRQn                 = 33,
  WAKEUP34_IRQn                 = 34,
  WAKEUP35_IRQn                 = 35,
  WAKEUP36_IRQn                 = 36,
  WAKEUP37_IRQn                 = 37,
  WAKEUP38_IRQn                 = 38,
  WAKEUP39_IRQn                 = 39,
  I2C_IRQn                      = 40,       /*!< I2C Interrupt                                    */
  TIMER_16_0_IRQn               = 41,       /*!< 16-bit Timer0 Interrupt                          */
  TIMER_16_1_IRQn               = 42,       /*!< 16-bit Timer1 Interrupt                          */
  TIMER_32_0_IRQn               = 43,       /*!< 32-bit Timer0 Interrupt                          */
  TIMER_32_1_IRQn               = 44,       /*!< 32-bit Timer1 Interrupt                          */
  SSP_IRQn                      = 45,       /*!< SSP Interrupt                                    */
  UART_IRQn                     = 46,       /*!< UART Interrupt                                   */
  USB_IRQn                      = 47,       /*!< USB Regular Interrupt                            */
  USB_FIQn                      = 48,       /*!< USB Fast Interrupt                               */
  ADC_IRQn                      = 49,       /*!< A/D Converter Interrupt                          */
  WDT_IRQn                      = 50,       /*!< Watchdog timer Interrupt                         */
  BOD_IRQn                      = 51,       /*!< Brown Out Detect(BOD) Interrupt                  */
  EINT3_IRQn                    = 53,       /*!< External Interrupt 3 Interrupt                   */
  EINT2_IRQn                    = 54,       /*!< External Interrupt 2 Interrupt                   */
  EINT1_IRQn                    = 55,       /*!< External Interrupt 1 Interrupt                   */
  EINT0_IRQn                    = 56,       /*!< External Interrupt 0 Interrupt                   */
} IRQn_Type;

#endif

/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M3 Processor and Core Peripherals */
#define __MPU_PRESENT             1         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          3         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */


#include "core_cm3.h"                       /* Cortex-M3 processor and core peripherals           */
#include "system_LPC13xx.h"                 /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SYSCON) --------------------------------------*/
typedef struct
{
  __IO uint32_t SYSMEMREMAP;     /* Sys mem. Remap, Offset 0x0   */
  __IO uint32_t PRESETCTRL;
  __IO uint32_t SYSPLLCTRL;      /* Sys PLL control              */
  __IO uint32_t SYSPLLSTAT;
  __IO uint32_t USBPLLCTRL;      /* USB PLL control, offset 0x10 */
  __IO uint32_t USBPLLSTAT;
       uint32_t RESERVED0[2];

  __IO uint32_t SYSOSCCTRL;      /* Offset 0x20 */
  __IO uint32_t WDTOSCCTRL;
  __IO uint32_t IRCCTRL;
       uint32_t RESERVED1[1];
  __IO uint32_t SYSRESSTAT;      /* Offset 0x30 */
       uint32_t RESERVED2[3];
  __IO uint32_t SYSPLLCLKSEL;    /* Offset 0x40 */	
  __IO uint32_t SYSPLLCLKUEN;
  __IO uint32_t USBPLLCLKSEL;
  __IO uint32_t USBPLLCLKUEN;
       uint32_t RESERVED3[8];

  __IO uint32_t MAINCLKSEL;      /* Offset 0x70 */
  __IO uint32_t MAINCLKUEN;
  __IO uint32_t SYSAHBCLKDIV;
       uint32_t RESERVED4[1];

  __IO uint32_t SYSAHBCLKCTRL;   /* Offset 0x80 */
       uint32_t RESERVED5[4];
  __IO uint32_t SSPCLKDIV;
  __IO uint32_t UARTCLKDIV;
       uint32_t RESERVED6[4];
  __IO uint32_t TRACECLKDIV;

  __IO uint32_t SYSTICKCLKDIV;   /* Offset 0xB0 */
       uint32_t RESERVED7[3];

  __IO uint32_t USBCLKSEL;       /* Offset 0xC0 */
  __IO uint32_t USBCLKUEN;
  __IO uint32_t USBCLKDIV;
       uint32_t RESERVED8[1];
  __IO uint32_t WDTCLKSEL;       /* Offset 0xD0 */
  __IO uint32_t WDTCLKUEN;
  __IO uint32_t WDTCLKDIV;
       uint32_t RESERVED9[1];
  __IO uint32_t CLKOUTCLKSEL;    /* Offset 0xE0 */
  __IO uint32_t CLKOUTUEN;
  __IO uint32_t CLKOUTDIV;
       uint32_t RESERVED10[5];

  __IO uint32_t PIOPORCAP0;      /* Offset 0x100 */
  __IO uint32_t PIOPORCAP1;
       uint32_t RESERVED11[18];

  __IO uint32_t BODCTRL;         /* Offset 0x150 */
       uint32_t RESERVED12[1];
  __IO uint32_t SYSTCKCAL;
       uint32_t RESERVED13[41];

  __IO uint32_t STARTAPRP0;      /* Offset 0x200 */
  __IO uint32_t STARTERP0;
  __IO uint32_t STARTRSRP0CLR;
  __IO uint32_t STARTSRP0;
  __IO uint32_t STARTAPRP1;
  __IO uint32_t STARTERP1;
  __IO uint32_t STARTRSRP1CLR;
  __IO uint32_t STARTSRP1;
       uint32_t RESERVED14[4];

  __IO uint32_t PDSLEEPCFG;      /* Offset 0x230 */
  __IO uint32_t PDAWAKECFG;
  __IO uint32_t PDRUNCFG;
       uint32_t RESERVED15[110];
  __I  uint32_t DEVICE_ID;
} SYSCON_TypeDef;


/*------------- Pin Connect Block (IOCON) --------------------------------*/
typedef struct
{
  __IO uint32_t PIO2_6;
       uint32_t RESERVED0[1];
  __IO uint32_t PIO2_0;
  __IO uint32_t RESET_PIO0_0;
  __IO uint32_t PIO0_1;
  __IO uint32_t PIO1_8;
       uint32_t RESERVED1[1];
  __IO uint32_t PIO0_2;

  __IO uint32_t PIO2_7;
  __IO uint32_t PIO2_8;
  __IO uint32_t PIO2_1;
  __IO uint32_t PIO0_3;
  __IO uint32_t PIO0_4;
  __IO uint32_t PIO0_5;
  __IO uint32_t PIO1_9;
  __IO uint32_t PIO3_4;

  __IO uint32_t PIO2_4;
  __IO uint32_t PIO2_5;
  __IO uint32_t PIO3_5;
  __IO uint32_t PIO0_6;
  __IO uint32_t PIO0_7;
  __IO uint32_t PIO2_9;
  __IO uint32_t PIO2_10;
  __IO uint32_t PIO2_2;

  __IO uint32_t PIO0_8;
  __IO uint32_t PIO0_9;
  __IO uint32_t PIO0_10_JTAG_TCK;
  __IO uint32_t PIO1_10;
  __IO uint32_t PIO2_11;
  __IO uint32_t PIO0_11_JTAG_TDI;
  __IO uint32_t PIO1_0_JTAG_TMS;
  __IO uint32_t PIO1_1_JTAG_TDO;

  __IO uint32_t PIO1_2_JTAG_nTRST;
  __IO uint32_t PIO3_0;
  __IO uint32_t PIO3_1;
  __IO uint32_t PIO2_3;
  __IO uint32_t PIO1_3_ARM_SWDIO;
  __IO uint32_t PIO1_4;
  __IO uint32_t PIO1_11;
  __IO uint32_t PIO3_2;

  __IO uint32_t PIO1_5;
  __IO uint32_t PIO1_6;
  __IO uint32_t PIO1_7;
  __IO uint32_t PIO3_3;
  __IO uint32_t SCK_LOC;   /* For HB1 only, new feature */
} IOCONFIG_TypeDef;


/*------------- Power Management Unit (PMU) --------------------------*/
typedef struct
{
  __IO uint32_t PCON;
  __IO uint32_t GPREG0;
  __IO uint32_t GPREG1;
  __IO uint32_t GPREG2;
  __IO uint32_t GPREG3;
  __IO uint32_t GPREG4;
} PMU_TypeDef;


/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  union {
    __IO uint32_t MASKED_ACCESS[4096];
    struct {
         uint32_t RESERVED0[4095];
    __IO uint32_t DATA;
    };
  };
       uint32_t RESERVED1[4096];
  __IO uint32_t DIR;
  __IO uint32_t IS;
  __IO uint32_t IBE;
  __IO uint32_t IEV;
  __IO uint32_t IE;
  __IO uint32_t RIS;
  __IO uint32_t MIS;
  __IO uint32_t IC;
} GPIO_TypeDef;


/*------------- Timer (TMR) --------------------------------------------------*/
typedef struct
{
  __IO uint32_t IR;
  __IO uint32_t TCR;
  __IO uint32_t TC;
  __IO uint32_t PR;
  __IO uint32_t PC;
  __IO uint32_t MCR;
  __IO uint32_t MR0;
  __IO uint32_t MR1;
  __IO uint32_t MR2;
  __IO uint32_t MR3;
  __IO uint32_t CCR;
  __I  uint32_t CR0;
       uint32_t RESERVED1[3];
  __IO uint32_t EMR;
       uint32_t RESERVED2[12];
  __IO uint32_t CTCR;
  __IO uint32_t PWMC;
} TMR_TypeDef;

/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  __I  uint32_t  RBR;
  __O  uint32_t  THR;
  __IO uint32_t  DLL;
  };
  union {
  __IO uint32_t  DLM;
  __IO uint32_t  IER;
  };
  union {
  __I  uint32_t  IIR;
  __O  uint32_t  FCR;
  };
  __IO uint32_t  LCR;
  __IO uint32_t  MCR;
  __I  uint32_t  LSR;
  __I  uint32_t  MSR;
  __IO uint32_t  SCR;
  __IO uint32_t  ACR;
  __IO uint32_t  ICR;
  __IO uint32_t  FDR;
       uint32_t  RESERVED0;
  __IO uint32_t  TER;
       uint32_t  RESERVED1[6];
  __IO uint32_t  RS485CTRL;
  __IO uint32_t  ADRMATCH;
  __IO uint32_t  RS485DLY;
  __I  uint32_t  FIFOLVL;
} UART_TypeDef;

/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  __IO uint32_t CR0;
  __IO uint32_t CR1;
  __IO uint32_t DR;
  __I  uint32_t SR;
  __IO uint32_t CPSR;
  __IO uint32_t IMSC;
  __IO uint32_t RIS;
  __IO uint32_t MIS;
  __IO uint32_t ICR;
} SSP_TypeDef;

/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  __IO uint32_t I2CONSET;
  __I  uint32_t I2STAT;
  __IO uint32_t I2DAT;
  __IO uint32_t ADR0;
  __IO uint32_t I2SCLH;
  __IO uint32_t I2SCLL;
  __O  uint32_t I2CONCLR;
  __IO uint32_t MMCTRL;
  __IO uint32_t ADR1;
  __IO uint32_t ADR2;
  __IO uint32_t ADR3;
  __I  uint32_t DATA_BUFFER;
  __IO uint32_t MASK0;
  __IO uint32_t MASK1;
  __IO uint32_t MASK2;
  __IO uint32_t MASK3;
} I2C_TypeDef;

/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  __IO uint32_t MOD;
  __IO uint32_t TC;
  __O  uint32_t FEED;
  __I  uint32_t TV;
} WDT_TypeDef;

/*------------- Analog-to-Digital Converter (ADC) ----------------------------*/
typedef struct
{
  __IO uint32_t CR;
  __IO uint32_t GDR;
       uint32_t RESERVED0;
  __IO uint32_t INTEN;
  __I  uint32_t DR0;
  __I  uint32_t DR1;
  __I  uint32_t DR2;
  __I  uint32_t DR3;
  __I  uint32_t DR4;
  __I  uint32_t DR5;
  __I  uint32_t DR6;
  __I  uint32_t DR7;
  __I  uint32_t STAT;
} ADC_TypeDef;


/*------------- Universal Serial Bus (USB) -----------------------------------*/
typedef struct
{
  __I  uint32_t DevIntSt;            /* USB Device Interrupt Registers     */
  __IO uint32_t DevIntEn;
  __O  uint32_t DevIntClr;
  __O  uint32_t DevIntSet;

  __O  uint32_t CmdCode;             /* USB Device SIE Command Registers   */
  __I  uint32_t CmdData;

  __I  uint32_t RxData;              /* USB Device Transfer Registers      */
  __O  uint32_t TxData;
  __I  uint32_t RxPLen;
  __O  uint32_t TxPLen;
  __IO uint32_t Ctrl;
  __O  uint32_t DevFIQSel;
} USB_TypeDef;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define FLASH_BASE        (0x00000000UL)
#define RAM_BASE          (0x10000000UL)
#define APB0_BASE         (0x40000000UL)
#define AHB_BASE          (0x50000000UL)

/* APB0 peripherals                                                           */
#define I2C_BASE          (APB0_BASE + 0x00000)
#define WDT_BASE          (APB0_BASE + 0x04000)
#define UART_BASE         (APB0_BASE + 0x08000)
#define CT16B0_BASE       (APB0_BASE + 0x0C000)
#define CT16B1_BASE       (APB0_BASE + 0x10000)
#define CT32B0_BASE       (APB0_BASE + 0x14000)
#define CT32B1_BASE       (APB0_BASE + 0x18000)
#define ADC_BASE          (APB0_BASE + 0x1C000)
#define USB_BASE          (APB0_BASE + 0x20000)
#define PMU_BASE          (APB0_BASE + 0x38000)
#define SSP_BASE          (APB0_BASE + 0x40000)
#define IOCONFIG_BASE     (APB0_BASE + 0x44000)
#define SYSCON_BASE       (APB0_BASE + 0x48000)

/* AHB peripherals                                                            */	
#define GPIO_BASE         (AHB_BASE  + 0x00000)
#define GPIO0_BASE        (AHB_BASE  + 0x00000)
#define GPIO1_BASE        (AHB_BASE  + 0x10000)
#define GPIO2_BASE        (AHB_BASE  + 0x20000)
#define GPIO3_BASE        (AHB_BASE  + 0x30000)

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define I2C               ((I2C_TypeDef      *) I2C_BASE   )
#define WDT               ((WDT_TypeDef      *) WDT_BASE   )
#define UART              ((UART_TypeDef     *) UART_BASE  )
#define TMR16B0           ((TMR_TypeDef      *) CT16B0_BASE)
#define TMR16B1           ((TMR_TypeDef      *) CT16B1_BASE)
#define TMR32B0           ((TMR_TypeDef      *) CT32B0_BASE)
#define TMR32B1           ((TMR_TypeDef      *) CT32B1_BASE)
#define ADC               ((ADC_TypeDef      *) ADC_BASE   )
#define PMU               ((PMU_TypeDef      *) PMU_BASE   )
#define SSP               ((SSP_TypeDef      *) SSP_BASE   )
#define IOCONFIG          ((IOCONFIG_TypeDef *) IOCONFIG_BASE )
#define SYSCON            ((SYSCON_TypeDef   *) SYSCON_BASE)
#define USB               ((USB_TypeDef      *) USB_BASE   )
#define GPIO0             ((GPIO_TypeDef     *) GPIO0_BASE )
#define GPIO1             ((GPIO_TypeDef     *) GPIO1_BASE )
#define GPIO2             ((GPIO_TypeDef     *) GPIO2_BASE )
#define GPIO3             ((GPIO_TypeDef     *) GPIO3_BASE )

#endif  // __LPC13xx_H__
