/******************************************************************************
 * @file     EMxxx.h
 * @brief    CMSIS Cortex-M0 Core Peripheral Access Layer Header File for
 *           NXP EMxxx Device Series
 * @version  V1.00
 * @date     27. May 2011
 *
 ******************************************************************************/


#ifndef __EMxxx_H__
#define __EMxxx_H__

#ifdef __cplusplus
 extern "C" {
#endif 



/******************************************************************************/
/*                Processor and Core Peripherals                              */
/******************************************************************************/

/*
 * ==========================================================================
 * ---------- Interrupt Number Definition -----------------------------------
 * ==========================================================================
 */

typedef enum IRQn
{
/******  Cortex-M0 Processor Exceptions Numbers *********************************/
  NonMaskableInt_IRQn   = -14,  /*!< 2 Non Maskable Interrupt                   */
  HardFault_IRQn        = -13,  /*!< 3 Cortex-M0 Hard Fault Interrupt           */
  SVCall_IRQn           = -5,   /*!< 11 Cortex-M0 SV Call Interrupt             */
  PendSV_IRQn           = -2,   /*!< 14 Cortex-M0 Pend SV Interrupt             */
  SysTick_IRQn          = -1,   /*!< 15 Cortex-M0 System Tick Interrupt         */

/******  EMxxx Specific Interrupt Numbers ***************************************/
  WAKEUP0_IRQn          = 0,    /*!< All I/O pins can be used as wakeup source. */
  WAKEUP1_IRQn          = 1,    /*!< There are 13 pins in total for EMxxx       */
  WAKEUP2_IRQn          = 2,
  WAKEUP3_IRQn          = 3,
  WAKEUP4_IRQn          = 4,
  WAKEUP5_IRQn          = 5,
  WAKEUP6_IRQn          = 6,
  WAKEUP7_IRQn          = 7,
  WAKEUP8_IRQn          = 8,
  WAKEUP9_IRQn          = 9,
  WAKEUP10_IRQn         = 10,
  WAKEUP11_IRQn         = 11,
  WAKEUP12_IRQn         = 12,
  I2C_IRQn              = 15,   /*!< I2C Interrupt                              */
  TIMER_16_0_IRQn       = 16,   /*!< 16-bit Timer0 Interrupt                    */
  METROLOGY_IRQn        = 17,   /*!< Metrology Interrupt                        */
  TIMER_32_0_IRQn       = 18,   /*!< 32-bit Timer0 Interrupt                    */
  TIMER_32_1_IRQn       = 19,   /*!< 32-bit Timer1 Interrupt                    */
  SSP_IRQn              = 20,   /*!< SSP Interrupt                              */
  UART_IRQn             = 21,   /*!< UART Interrupt                             */
  WDT_IRQn              = 25,   /*!< Watchdog timer Interrupt                   */
  BOD_IRQn              = 26,   /*!< Brown Out Detect(BOD) Interrupt            */
  EINT3_IRQn            = 28,   /*!< External Interrupt 3 Interrupt             */
  EINT2_IRQn            = 29,   /*!< External Interrupt 2 Interrupt             */
  EINT1_IRQn            = 30,   /*!< External Interrupt 1 Interrupt             */
  EINT0_IRQn            = 31,   /*!< External Interrupt 0 Interrupt             */
} IRQn_Type;


/*
 * ==========================================================================
 * ----------- Processor and Core Peripheral Section ------------------------
 * ==========================================================================
 */

/* Configuration of the Cortex-M0 Processor and Core Peripherals */
#define __MPU_PRESENT             0         /*!< MPU present or not                               */
#define __NVIC_PRIO_BITS          2         /*!< Number of Bits used for Priority Levels          */
#define __Vendor_SysTickConfig    0         /*!< Set to 1 if different SysTick Config is used     */



#include "core_cm0.h"                       /* Cortex-M0 processor and core peripherals           */
#include "system_EM7xx.h"                   /* System Header                                      */


/******************************************************************************/
/*                Device Specific Peripheral Registers structures             */
/******************************************************************************/

#if defined ( __CC_ARM   )
#pragma anon_unions
#endif

/*------------- System Control (SYSCON) --------------------------------------*/
typedef struct
{
  __IO uint32_t SYSMEMREMAP;            /*!< Offset: 0x000 System memory remap (R/W) */
  __IO uint32_t PRESETCTRL;             /*!< Offset: 0x004 Peripheral reset control (R/W) */
  __IO uint32_t SYSPLLCTRL;             /*!< Offset: 0x008 System PLL control (R/W) */
  __IO uint32_t SYSPLLSTAT;             /*!< Offset: 0x00C System PLL status (R/ ) */
       uint32_t RESERVED0[4];

  __IO uint32_t SYSOSCCTRL;             /*!< Offset: 0x020 System oscillator control (R/W) */
  __IO uint32_t WDTOSCCTRL;             /*!< Offset: 0x024 Watchdog oscillator control (R/W) */
  __IO uint32_t IRCCTRL;                /*!< Offset: 0x028 IRC control (R/W) */			
       uint32_t RESERVED1[1];
  __IO uint32_t SYSRESSTAT;             /*!< Offset: 0x030 System reset status Register (R/ ) */
       uint32_t RESERVED2[3];
  __IO uint32_t SYSPLLCLKSEL;           /*!< Offset: 0x040 System PLL clock source select (R/W) */	
  __IO uint32_t SYSPLLCLKUEN;           /*!< Offset: 0x044 System PLL clock source update enable (R/W) */
       uint32_t RESERVED3[10];

  __IO uint32_t MAINCLKSEL;             /*!< Offset: 0x070 Main clock source select (R/W) */
  __IO uint32_t MAINCLKUEN;             /*!< Offset: 0x074 Main clock source update enable (R/W) */
  __IO uint32_t SYSAHBCLKDIV;           /*!< Offset: 0x078 System AHB clock divider (R/W) */
       uint32_t RESERVED4[1];

  __IO uint32_t SYSAHBCLKCTRL;          /*!< Offset: 0x080 System AHB clock control (R/W) */
       uint32_t RESERVED5[4];
  __IO uint32_t SSPCLKDIV;              /*!< Offset: 0x094 SSP0 clock divider (R/W) */   
  __IO uint32_t UARTCLKDIV;             /*!< Offset: 0x098 UART clock divider (R/W) */
       uint32_t RESERVED6[5];

  __IO uint32_t SYSTICKCLKDIV;          /*!< Offset: 0x0B0 SYSTICK clock divider (R/W) */          
       uint32_t RESERVED7[7];

  __IO uint32_t WDTCLKSEL;              /*!< Offset: 0x0D0 WDT clock source select (R/W) */
  __IO uint32_t WDTCLKUEN;              /*!< Offset: 0x0D4 WDT clock source update enable (R/W) */
  __IO uint32_t WDTCLKDIV;              /*!< Offset: 0x0D8 WDT clock divider (R/W) */
       uint32_t RESERVED8[1];
  __IO uint32_t CLKOUTCLKSEL;           /*!< Offset: 0x0E0 CLKOUT clock source select (R/W) */
  __IO uint32_t CLKOUTCLKUEN;           /*!< Offset: 0x0E4 CLKOUT clock source update enable (R/W) */
  __IO uint32_t CLKOUTCLKDIV;           /*!< Offset: 0x0E8 CLKOUT clock divider (R/W) */   
       uint32_t RESERVED9[5];

  __IO uint32_t PIOPORCAP0;             /*!< Offset: 0x100 POR captured PIO status 0 (R/ ) */  
  __IO uint32_t PIOPORCAP1;             /*!< Offset: 0x104 POR captured PIO status 1 (R/ ) */  
       uint32_t RESERVED10[18];

  __IO uint32_t BODCTRL;                /*!< Offset: 0x150 BOD control (R/W) */
       uint32_t RESERVED11[1];
  __IO uint32_t SYSTCKCAL;              /*!< Offset: 0x158 System tick counter calibration (R/W) */
       uint32_t RESERVED12;
  __IO uint32_t MAINREGVOUT0CFG;        /*!< Offset: 0x160 Main Regulator Voltage 0 Configuration */ 
  __IO uint32_t MAINREGVOUT1CFG;        /*!< Offset: 0x164 Main Regulator Voltage 1 Configuration */
       uint32_t RESERVED13[38];

  __IO uint32_t STARTAPRP0;             /*!< Offset: 0x200 Start logic edge control Register 0 (R/W) */ 
  __IO uint32_t STARTERP0;              /*!< Offset: 0x204 Start logic signal enable Register 0 (R/W) */ 
  __IO uint32_t STARTRSRP0CLR;          /*!< Offset: 0x208 Start logic reset Register 0  ( /W) */
  __IO uint32_t STARTSRP0;              /*!< Offset: 0x20C Start logic status Register 0 (R/W) */
       uint32_t RESERVED14[8];

  __IO uint32_t PDSLEEPCFG;             /*!< Offset: 0x230 Power-down states in Deep-sleep mode (R/W) */
  __IO uint32_t PDAWAKECFG;             /*!< Offset: 0x234 Power-down states after wake-up (R/W) */ 
  __IO uint32_t PDRUNCFG;               /*!< Offset: 0x238 Power-down configuration Register (R/W) */
       uint32_t RESERVED15[101];
  __O  uint32_t VOUTCFGPROT;            /*!< Offset: 0x3D0 Voltage Output Configuration Protection Register (W) */
       uint32_t RESERVED16[8];
  __I  uint32_t DEVICE_ID;              /*!< Offset: 0x3F4 Device ID (R/ ) */
} SYSCON_TypeDef;


/*------------- Pin Connect Block (IOCONFIG) --------------------------------*/
typedef struct
{
  __IO uint32_t PIO2_6;                 /*!< Offset: 0x000 I/O configuration for pin PIO2_6 (R/W) */
       uint32_t RESERVED0;
  __IO uint32_t PIO2_0;                 /*!< Offset: 0x008 I/O configuration for pin PIO2_0/DTR/SSEL1 (R/W) */
  __IO uint32_t RESET_PIO0_0;           /*!< Offset: 0x00C I/O configuration for pin RESET/PIO0_0  (R/W) */
  __IO uint32_t PIO0_1;                 /*!< Offset: 0x010 I/O configuration for pin PIO0_1/CLKOUT/CT32B0_MAT2 (R/W) */
  __IO uint32_t PIO1_8;                 /*!< Offset: 0x014 I/O configuration for pin PIO1_8 (R/W) */
  __IO uint32_t RESERVED1;
  __IO uint32_t PIO0_2;                 /*!< Offset: 0x01C I/O configuration for pin PIO0_2/SSEL0/CT16B0_CAP0 (R/W) */

  __IO uint32_t PIO2_7;                 /*!< Offset: 0x020 I/O configuration for pin PIO2_7 (R/W) */
  __IO uint32_t PIO2_8;                 /*!< Offset: 0x024 I/O configuration for pin PIO2_8 (R/W) */
  __IO uint32_t PIO2_1;                 /*!< Offset: 0x028 I/O configuration for pin PIO2_1/nDSR/SCK1 (R/W) */
  __IO uint32_t PIO0_3;                 /*!< Offset: 0x02C I/O configuration for pin PIO0_3 (R/W) */
  __IO uint32_t PIO0_4;                 /*!< Offset: 0x030 I/O configuration for pin PIO0_4/SCL (R/W) */
  __IO uint32_t PIO0_5;                 /*!< Offset: 0x034 I/O configuration for pin PIO0_5/SDA (R/W) */
  __IO uint32_t PIO1_9;                 /*!< Offset: 0x038 I/O configuration for pin PIO1_9 (R/W) */
  __IO uint32_t PIO3_4;                 /*!< Offset: 0x03C I/O configuration for pin PIO3_4 (R/W) */

  __IO uint32_t PIO2_4;                 /*!< Offset: 0x040 I/O configuration for pin PIO2_4 (R/W) */
  __IO uint32_t PIO2_5;                 /*!< Offset: 0x044 I/O configuration for pin PIO2_5 (R/W) */
  __IO uint32_t PIO3_5;                 /*!< Offset: 0x048 I/O configuration for pin PIO3_5 (R/W) */
  __IO uint32_t PIO0_6;                 /*!< Offset: 0x04C I/O configuration for pin PIO0_6/SCK0 (R/W) */
  __IO uint32_t PIO0_7;                 /*!< Offset: 0x050 I/O configuration for pin PIO0_7/nCTS (R/W) */
  __IO uint32_t PIO2_9;                 /*!< Offset: 0x054 I/O configuration for pin PIO2_9 (R/W) */
  __IO uint32_t PIO2_10;                /*!< Offset: 0x058 I/O configuration for pin PIO2_10 (R/W) */
  __IO uint32_t PIO2_2;                 /*!< Offset: 0x05C I/O configuration for pin PIO2_2/DCD/MISO1 (R/W) */

  __IO uint32_t PIO0_8;                 /*!< Offset: 0x060 I/O configuration for pin PIO0_8/MISO0/CT16B0_MAT0 (R/W) */
  __IO uint32_t PIO0_9;                 /*!< Offset: 0x064 I/O configuration for pin PIO0_9/MOSI0/CT16B0_MAT1 (R/W) */
  __IO uint32_t SWCLK_PIO0_10;          /*!< Offset: 0x068 I/O configuration for pin SWCLK/PIO0_10/SCK0/CT16B0_MAT2 (R/W) */
  __IO uint32_t PIO1_10;                /*!< Offset: 0x06C I/O configuration for pin PIO1_10 (R/W) */
  __IO uint32_t PIO2_11;                /*!< Offset: 0x070 I/O configuration for pin PIO2_11/SCK0 (R/W) */
  __IO uint32_t R_PIO0_11;              /*!< Offset: 0x074 I/O configuration for pin TDI/PIO0_11/CT32B0_MAT3 (R/W) */
  __IO uint32_t R_PIO1_0;               /*!< Offset: 0x078 I/O configuration for pin TMS/PIO1_0/CT32B1_CAP0 (R/W) */
  __IO uint32_t R_PIO1_1;               /*!< Offset: 0x07C I/O configuration for pin TDO/PIO1_1/CT32B1_MAT0 (R/W) */

  __IO uint32_t R_PIO1_2;               /*!< Offset: 0x080 I/O configuration for pin nTRST/PIO1_2/CT32B1_MAT1 (R/W) */
  __IO uint32_t PIO3_0;                 /*!< Offset: 0x084 I/O configuration for pin PIO3_0/nDTR (R/W) */
  __IO uint32_t PIO3_1;                 /*!< Offset: 0x088 I/O configuration for pin PIO3_1/nDSR (R/W) */
  __IO uint32_t PIO2_3;                 /*!< Offset: 0x08C I/O configuration for pin PIO2_3/RI/MOSI1 (R/W) */
  __IO uint32_t SWDIO_PIO1_3;           /*!< Offset: 0x090 I/O configuration for pin SWDIO/PIO1_3/CT32B1_MAT2 (R/W) */
  __IO uint32_t PIO1_4;                 /*!< Offset: 0x094 I/O configuration for pin PIO1_4/CT32B1_MAT3 (R/W) */
  __IO uint32_t PIO1_11;                /*!< Offset: 0x098 I/O configuration for pin PIO1_11 (R/W) */
  __IO uint32_t PIO3_2;                 /*!< Offset: 0x09C I/O configuration for pin PIO3_2/nDCD (R/W) */

  __IO uint32_t PIO1_5;                 /*!< Offset: 0x0A0 I/O configuration for pin PIO1_5/nRTS/CT32B0_CAP0 (R/W) */
  __IO uint32_t PIO1_6;                 /*!< Offset: 0x0A4 I/O configuration for pin PIO1_6/RXD/CT32B0_MAT0 (R/W) */
  __IO uint32_t PIO1_7;                 /*!< Offset: 0x0A8 I/O configuration for pin PIO1_7/TXD/CT32B0_MAT1 (R/W) */
  __IO uint32_t PIO3_3;                 /*!< Offset: 0x0AC I/O configuration for pin PIO3_3/nRI (R/W) */
  __IO uint32_t SCK_LOC;                /*!< Offset: 0x0B0 SCK pin location select Register (R/W) */
  __IO uint32_t DSR_LOC;                /*!< Offset: 0x0B4 DSR pin location select Register (R/W) */
  __IO uint32_t DCD_LOC;                /*!< Offset: 0x0B8 DCD pin location select Register (R/W) */
  __IO uint32_t RI_LOC;                 /*!< Offset: 0x0BC RI pin location Register (R/W) */
} IOCONFIG_TypeDef;


/*------------- Power Management Unit (PMU) --------------------------*/
typedef struct
{
  __IO uint32_t PCON;                   /*!< Offset: 0x000 Power control Register (R/W) */
  __IO uint32_t GPREG0;                 /*!< Offset: 0x004 General purpose Register 0 (R/W) */
  __IO uint32_t GPREG1;                 /*!< Offset: 0x008 General purpose Register 1 (R/W) */
  __IO uint32_t GPREG2;                 /*!< Offset: 0x00C General purpose Register 2 (R/W) */
  __IO uint32_t GPREG3;                 /*!< Offset: 0x010 General purpose Register 3 (R/W) */
  __IO uint32_t GPREG4;                 /*!< Offset: 0x014 General purpose Register 4 (R/W) */
} PMU_TypeDef;


/*------------- General Purpose Input/Output (GPIO) --------------------------*/
typedef struct
{
  union {
    __IO uint32_t MASKED_ACCESS[4096];  /*!< Offset: 0x0000 to 0x3FFC Port data Register for pins PIOn_0 to PIOn_11 (R/W) */
    struct {
         uint32_t RESERVED0[4095];
    __IO uint32_t DATA;                 /*!< Offset: 0x3FFC Port data Register (R/W) */
    };
  };
       uint32_t RESERVED1[4096];
  __IO uint32_t DIR;                    /*!< Offset: 0x8000 Data direction Register (R/W) */
  __IO uint32_t IS;                     /*!< Offset: 0x8004 Interrupt sense Register (R/W) */
  __IO uint32_t IBE;                    /*!< Offset: 0x8008 Interrupt both edges Register (R/W) */
  __IO uint32_t IEV;                    /*!< Offset: 0x800C Interrupt event Register  (R/W) */
  __IO uint32_t IE;                     /*!< Offset: 0x8010 Interrupt mask Register (R/W) */
  __IO uint32_t RIS;                    /*!< Offset: 0x8014 Raw interrupt status Register (R/ ) */
  __IO uint32_t MIS;                    /*!< Offset: 0x8018 Masked interrupt status Register (R/ ) */
  __IO uint32_t IC;                     /*!< Offset: 0x801C Interrupt clear Register (R/W) */
} GPIO_TypeDef;


/*------------- Timer (TIM) --------------------------------------------------*/
typedef struct
{
  __IO uint32_t IR;                     /*!< Offset: 0x000 Interrupt Register (R/W) */
  __IO uint32_t TCR;                    /*!< Offset: 0x004 Timer Control Register (R/W) */
  __IO uint32_t TC;                     /*!< Offset: 0x008 Timer Counter Register (R/W) */
  __IO uint32_t PR;                     /*!< Offset: 0x00C Prescale Register (R/W) */
  __IO uint32_t PC;                     /*!< Offset: 0x010 Prescale Counter Register (R/W) */
  __IO uint32_t MCR;                    /*!< Offset: 0x014 Match Control Register (R/W) */
  __IO uint32_t MR0;                    /*!< Offset: 0x018 Match Register 0 (R/W) */
  __IO uint32_t MR1;                    /*!< Offset: 0x01C Match Register 1 (R/W) */
  __IO uint32_t MR2;                    /*!< Offset: 0x020 Match Register 2 (R/W) */
  __IO uint32_t MR3;                    /*!< Offset: 0x024 Match Register 3 (R/W) */
  __IO uint32_t CCR;                    /*!< Offset: 0x028 Capture Control Register (R/W) */
  __I  uint32_t CR0;                    /*!< Offset: 0x02C Capture Register 0 (R/ ) */
       uint32_t RESERVED1[3];
  __IO uint32_t EMR;                    /*!< Offset: 0x03C External Match Register (R/W) */
       uint32_t RESERVED2[12];
  __IO uint32_t CTCR;                   /*!< Offset: 0x070 Count Control Register (R/W) */
  __IO uint32_t PWMC;                   /*!< Offset: 0x074 PWM Control Register (R/W) */
} TIM_TypeDef;


/*------------- Universal Asynchronous Receiver Transmitter (UART) -----------*/
typedef struct
{
  union {
  __I  uint32_t  RBR;                   /*!< Offset: 0x000 Receiver Buffer  Register (R/ ) */
  __O  uint32_t  THR;                   /*!< Offset: 0x000 Transmit Holding Register ( /W) */
  __IO uint32_t  DLL;                   /*!< Offset: 0x000 Divisor Latch LSB (R/W) */
  };
  union {
  __IO uint32_t  DLM;                   /*!< Offset: 0x004 Divisor Latch MSB (R/W) */
  __IO uint32_t  IER;                   /*!< Offset: 0x000 Interrupt Enable Register (R/W) */
  };
  union {
  __I  uint32_t  IIR;                   /*!< Offset: 0x008 Interrupt ID Register (R/ ) */
  __O  uint32_t  FCR;                   /*!< Offset: 0x008 FIFO Control Register ( /W) */
  };
  __IO uint32_t  LCR;                   /*!< Offset: 0x00C Line Control Register (R/W) */
  __IO uint32_t  MCR;                   /*!< Offset: 0x010 Modem control Register (R/W) */
  __I  uint32_t  LSR;                   /*!< Offset: 0x014 Line Status Register (R/ ) */
  __I  uint32_t  MSR;                   /*!< Offset: 0x018 Modem status Register (R/ ) */
  __IO uint32_t  SCR;                   /*!< Offset: 0x01C Scratch Pad Register (R/W) */
  __IO uint32_t  ACR;                   /*!< Offset: 0x020 Auto-baud Control Register (R/W) */
       uint32_t  RESERVED0;
  __IO uint32_t  FDR;                   /*!< Offset: 0x028 Fractional Divider Register (R/W) */
       uint32_t  RESERVED1;
  __IO uint32_t  TER;                   /*!< Offset: 0x030 Transmit Enable Register (R/W) */
       uint32_t  RESERVED2[6];
  __IO uint32_t  RS485CTRL;             /*!< Offset: 0x04C RS-485/EIA-485 Control Register (R/W) */
  __IO uint32_t  ADRMATCH;              /*!< Offset: 0x050 RS-485/EIA-485 address match Register (R/W) */
  __IO uint32_t  RS485DLY;              /*!< Offset: 0x054 RS-485/EIA-485 direction control delay Register (R/W) */
} UART_TypeDef;


/*------------- Synchronous Serial Communication (SSP) -----------------------*/
typedef struct
{
  __IO uint32_t CR0;                    /*!< Offset: 0x000 Control Register 0 (R/W) */
  __IO uint32_t CR1;                    /*!< Offset: 0x004 Control Register 1 (R/W) */
  __IO uint32_t DR;                     /*!< Offset: 0x008 Data Register (R/W) */
  __I  uint32_t SR;                     /*!< Offset: 0x00C Status Registe (R/ ) */
  __IO uint32_t CPSR;                   /*!< Offset: 0x010 Clock Prescale Register (R/W) */
  __IO uint32_t IMSC;                   /*!< Offset: 0x014 Interrupt Mask Set and Clear Register (R/W) */
  __IO uint32_t RIS;                    /*!< Offset: 0x018 Raw Interrupt Status Register (R/W) */
  __IO uint32_t MIS;                    /*!< Offset: 0x01C Masked Interrupt Status Register (R/W) */
  __IO uint32_t ICR;                    /*!< Offset: 0x020 SSPICR Interrupt Clear Register (R/W) */
} SSP_TypeDef;


/*------------- Inter-Integrated Circuit (I2C) -------------------------------*/
typedef struct
{
  __IO uint32_t I2CONSET;                 /*!< Offset: 0x000 I2C Control Set Register (R/W) */
  __I  uint32_t I2STAT;                   /*!< Offset: 0x004 I2C Status Register (R/ ) */
  __IO uint32_t I2DAT;                    /*!< Offset: 0x008 I2C Data Register (R/W) */
  __IO uint32_t I2ADR0;                   /*!< Offset: 0x00C I2C Slave Address Register 0 (R/W) */
  __IO uint32_t I2SCLH;                   /*!< Offset: 0x010 SCH Duty Cycle Register High Half Word (R/W) */
  __IO uint32_t I2SCLL;                   /*!< Offset: 0x014 SCL Duty Cycle Register Low Half Word (R/W) */
  __O  uint32_t I2CONCLR;                 /*!< Offset: 0x018 I2C Control Clear Register ( /W) */
  __IO uint32_t MMCTRL;                   /*!< Offset: 0x01C Monitor mode control register (R/W) */
  __IO uint32_t I2ADR1;                   /*!< Offset: 0x020 I2C Slave Address Register 1 (R/W) */
  __IO uint32_t I2ADR2;                   /*!< Offset: 0x024 I2C Slave Address Register 2 (R/W) */
  __IO uint32_t I2ADR3;                   /*!< Offset: 0x028 I2C Slave Address Register 3 (R/W) */
  __I  uint32_t I2DATA_BUFFER;            /*!< Offset: 0x02C Data buffer register ( /W) */
  __IO uint32_t I2MASK0;                  /*!< Offset: 0x030 I2C Slave address mask register 0 (R/W) */
  __IO uint32_t I2MASK1;                  /*!< Offset: 0x034 I2C Slave address mask register 1 (R/W) */
  __IO uint32_t I2MASK2;                  /*!< Offset: 0x038 I2C Slave address mask register 2 (R/W) */
  __IO uint32_t I2MASK3;                  /*!< Offset: 0x03C I2C Slave address mask register 3 (R/W) */
} I2C_TypeDef;


/*------------- Watchdog Timer (WDT) -----------------------------------------*/
typedef struct
{
  __IO uint32_t MOD;                    /*!< Offset: 0x000 Watchdog mode register (R/W) */
  __IO uint32_t TC;                     /*!< Offset: 0x004 Watchdog timer constant register (R/W) */
  __O  uint32_t FEED;                   /*!< Offset: 0x008 Watchdog feed sequence register ( /W) */
  __I  uint32_t TV;                     /*!< Offset: 0x00C Watchdog timer value register (R/ ) */
} WDT_TypeDef;

#if defined ( __CC_ARM   )
#pragma no_anon_unions
#endif


/******************************************************************************/
/*                         Peripheral memory map                              */
/******************************************************************************/
/* Base addresses                                                             */
#define FLASH_BASE            (0x00000000UL)
#define RAM_BASE              (0x10000000UL)
#define APB0_BASE             (0x40000000UL)
#define AHB_BASE              (0x50000000UL)

/* APB0 peripherals                                                          */
#define I2C_BASE              (APB0_BASE + 0x00000)
#define WDT_BASE              (APB0_BASE + 0x04000)
#define UART_BASE             (APB0_BASE + 0x08000)
#define CT16B0_BASE           (APB0_BASE + 0x0C000)
#define CT32B0_BASE           (APB0_BASE + 0x14000)
#define CT32B1_BASE           (APB0_BASE + 0x18000)
#define PMU_BASE              (APB0_BASE + 0x38000)
#define SSP_BASE              (APB0_BASE + 0x40000)
#define IOCONFIG_BASE         (APB0_BASE + 0x44000)
#define SYSCON_BASE           (APB0_BASE + 0x48000)

/* AHB peripherals */	
#define GPIO_BASE             (AHB_BASE  + 0x00000)
#define GPIO0_BASE            (AHB_BASE  + 0x00000)
#define GPIO1_BASE            (AHB_BASE  + 0x10000)
#define GPIO2_BASE            (AHB_BASE  + 0x20000)
#define GPIO3_BASE            (AHB_BASE  + 0x30000)

/******************************************************************************/
/*                         Peripheral declaration                             */
/******************************************************************************/
#define I2C                   ((      I2C_TypeDef *)       I2C_BASE)
#define WDT                   ((      WDT_TypeDef *)       WDT_BASE)
#define UART                  ((     UART_TypeDef *)      UART_BASE)
#define TIM16_0               ((      TIM_TypeDef *)    CT16B0_BASE)
#define TIM32_0               ((      TIM_TypeDef *)    CT32B0_BASE)
#define TIM32_1               ((      TIM_TypeDef *)    CT32B1_BASE)
#define PMU                   ((      PMU_TypeDef *)       PMU_BASE)
#define SSP                   ((      SSP_TypeDef *)       SSP_BASE)
#define IOCONFIG              (( IOCONFIG_TypeDef *)  IOCONFIG_BASE)
#define SYSCON                ((   SYSCON_TypeDef *)    SYSCON_BASE)
#define GPIO0                 ((     GPIO_TypeDef *)     GPIO0_BASE)
#define GPIO1                 ((     GPIO_TypeDef *)     GPIO1_BASE)
#define GPIO2                 ((     GPIO_TypeDef *)     GPIO2_BASE)
#define GPIO3                 ((     GPIO_TypeDef *)     GPIO3_BASE)

#ifdef __cplusplus
}
#endif

#endif  // __EMxxx_H__
