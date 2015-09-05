/*****************************************************************************
 *   gpio.h:  Header file for NXP LPC1xxx Family Microprocessors
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.09.01  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __GPIO_H
#define __GPIO_H

#include "pcb.h"

#define GPIO_INPUT  0
#define GPIO_OUTPUT 1

#define GPIO_RISING  1
#define GPIO_FALLING 0

void gpio_init      ( void );

void gpio_alloc     ( int portpin, pcb_mode_t mode, int dir );
void gpio_chdir     ( int portpin, int dir );
void gpio_free      ( int portpin );

void gpio_setval    ( int portpin, int val );
void gpio_set       ( int portpin );
void gpio_clr       ( int portpin );
void gpio_blink     ( int portpin );
int  gpio_getval    ( int portpin );

int  gpio_intgetflag( int portpin );
void gpio_intclear  ( int portpin );
void gpio_intdisable( int portpin );
void gpio_intenable ( int portpin, int rising );

#endif /* end __GPIO_H */

/* End Of File */
