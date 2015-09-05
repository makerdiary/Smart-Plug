/*****************************************************************************
 *   znp_hal.h:  Header file for NXP LPC Family Microprocessors
 *
 *   Copyright(C) 2012 E-Make
 *   All rights reserved.
 *
 *   History
 *
 ******************************************************************************/

#ifndef ZNP_HAL_H_INCLUDED
#define ZNP_HAL_H_INCLUDED

#include "dprint_macros.h"    //for print info
#include "string_functions.h"
#include "rtk.h"
#include "pcb.h"
#include "gpio.h"
#include "bsp.h"
#include "conversions.h"
extern uint32_t SystemCoreClock;

//
// MAcros for CC2530ZNP Interface
//
#define SSP_SCLK_PORTPIN  PCB_PORTPIN(PORT0,6)
#define SSP_SSEL_PORTPIN  PCB_PORTPIN(PORT0,2)
#define SSP_MISO_PORTPIN  PCB_PORTPIN(PORT0,8)
#define SSP_MOSI_PORTPIN  PCB_PORTPIN(PORT0,9)

#define ZNP_MRDY_PORTPIN  PCB_PORTPIN(PORT0,7)
#define ZNP_SRDY_PORTPIN  PCB_PORTPIN(PORT1,5)
#define ZNP_CFG0_PORTPIN  PCB_PORTPIN(PORT1,4)
#define ZNP_CFG1_PORTPIN  PCB_PORTPIN(PORT1,11)
#define ZNP_RESET_PORTPIN PCB_PORTPIN(PORT2,0)

#define SSP_ENABLE_SCK()  IOCONFIG->SCK_LOC = 0x02  //select the pin PORT0_6 for spi SCLK

#define XTAL              (LPC_CORE_CLOCKSPEED_HZ)
// Enable the SSP device in the EM773
#define SSP_ENABLE_DEV()       do \
                               { \
                                   SYSCON->PRESETCTRL    |= (0x1 <<  0); \
                                   SYSCON->SYSAHBCLKCTRL |= (0x1 << 11); \
                                   SYSCON->SSPCLKDIV      = 0x01;        \
                               } while(0)

#define SSP_FRAME_MODE    (0x07)       //SPI Mode 8-bit ,CPOL = 0, CPHA = 0

#define SSP_NORMAL_SPEED  (2000000)   //the default spi speed is 2MHz
//#define SSP_MAX_SPEED     (4000000)   //the max spi speed is 4MHz

#define SSP_GET_PCLK()    SystemCoreClock

//
#define RADIO_ON()        (gpio_set(ZNP_RESET_PORTPIN)) //ZNP Reset Line
#define RADIO_OFF()       (gpio_clr(ZNP_RESET_PORTPIN))
//ZNP SPI
#define SPI_SS_SET()      do \
                          { \
                              gpio_clr(SSP_SSEL_PORTPIN); \
                              gpio_clr(ZNP_MRDY_PORTPIN); \
                          } while(0)

#define SPI_SS_CLEAR()    do \
                          { \
                              gpio_set(SSP_SSEL_PORTPIN); \
                              gpio_set(ZNP_MRDY_PORTPIN); \
                          } while(0)

#define SRDY_IS_HIGH()    (gpio_getval(ZNP_SRDY_PORTPIN))
#define SRDY_IS_LOW()     (!gpio_getval(ZNP_SRDY_PORTPIN))

//indicate the presence of the 32kHz crystal connected to the CC2530-ZNP
#define ZNP_CFG0_SET()    (gpio_set(ZNP_CFG0_PORTPIN))
//indicate the absence of the 32kHz crystal connected to the CC2530-ZNP
#define ZNP_CFG0_CLEAR()  (gpio_clr(ZNP_CFG0_PORTPIN))

//use the SPI transport mode
#define ZNP_CFG1_SET()    (gpio_set(ZNP_CFG1_PORTPIN))
//use the UART transport mode,never used!!!
#define ZNP_CFG1_CLEAR()  (gpio_clr(ZNP_CFG1_PORTPIN))

//reversed
#define DEBUG_ON()
#define DEBUG_OFF()



//
//  METHODS REQUIRED FOR ZNP
//
void halSpiInitZnp(void);
void znp_hal_init(void);

void delayMs(unsigned int ms);
void delayUs(unsigned int us);

void spiWrite(unsigned char *bytes, unsigned char numBytes);

#endif
//eof

