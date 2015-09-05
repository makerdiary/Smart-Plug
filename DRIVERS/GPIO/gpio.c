/*****************************************************************************
 *   gpio.c:  GPIO C file for NXP LPC11xx/LPC13xx/EM7xx Family Microprocessors
 *
 *   Copyright(C) 2010, NXP Semiconductor
 *   All rights reserved.
 *
 *****************************************************************************/

#include "bsp.h"
#include "rtk.h"
#include "pcb.h"
#include "gpio.h"

void gpio_init(void)
{
    /* Enable AHB clock to the GPIO domain. */
    SYSCON->SYSAHBCLKCTRL |= (1<<6);
}

void gpio_alloc(int portpin, pcb_mode_t mode, int dir)
{
    pcb_func_t func = PCB_FUNC_0;

    switch (portpin)
    {
    case PCB_PORTPIN(PORT0,0):
    case PCB_PORTPIN(PORT0,10):
    case PCB_PORTPIN(PORT0,11):
    case PCB_PORTPIN(PORT1,0):
    case PCB_PORTPIN(PORT1,1):
    case PCB_PORTPIN(PORT1,2):
    case PCB_PORTPIN(PORT1,3):
        func = PCB_FUNC_1;
        break;
    default:
        func = PCB_FUNC_0;
        break;
    }
    // Claim the pin for PIO func, with selected mode
    pcb_alloc(portpin, func, mode);
    gpio_chdir(portpin, dir);
}

void gpio_free(int portpin)
{
    pcb_free(portpin);
}

void gpio_chdir(int portpin, int dir)
{
    int           port = PCB_PORTPIN2PORT(portpin);
    int           pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int  mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    if (dir == GPIO_OUTPUT) {
        pGpioPort->DIR |=  mask;
    } else {
        pGpioPort->DIR &= ~mask;
    }
    rtk_int_enable();
}

void gpio_set(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    pGpioPort->DATA |= mask;
    rtk_int_enable();
}

void gpio_clr(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    pGpioPort->DATA &= ~mask;
    rtk_int_enable();
}

void gpio_blink(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();   
    pGpioPort->DATA ^= mask;
    rtk_int_enable();
}


int gpio_getval(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    int          val  = 0;

    switch (port)
    {
    case 0: val = GPIO0->DATA; break;
    case 1: val = GPIO1->DATA; break;
    case 2: val = GPIO2->DATA; break;
    case 3: val = GPIO3->DATA; break;
    default:
        break;
    }
    return ( val & mask );
}

// A GPIO pin can generate an interrupt on a rising or falling edge of the HW
// The chosen SW implementation does not allow to program on both edges.
// This is just a choice to ease the implementation (TODO ?)

// Combines status of rising and falling ints since we cannot have both at the same time
int gpio_intgetflag(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    int          val  = 0;

    switch (port)
    {
    case 0: val = GPIO0->MIS; break;
    case 1: val = GPIO1->MIS; break;
    case 2: val = GPIO2->MIS; break;
    case 3: val = GPIO3->MIS; break;
    default:
        break;
    }
    return ( ( val & mask ) != 0 );
}

void gpio_intclear(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    pGpioPort->IC |= mask;
    rtk_int_enable();
}

// Disables rising as well as falling edge interrupts
void gpio_intdisable(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    pGpioPort->IE &= ~mask;
    rtk_int_enable();
}

// Restriction: cannot have both a falling and rising edge interrupt on one pin
void gpio_intenable(int portpin, int rising)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);
    GPIO_TypeDef *pGpioPort = 0;

    switch (port)
    {
    case 0: pGpioPort = GPIO0; break;
    case 1: pGpioPort = GPIO1; break;
    case 2: pGpioPort = GPIO2; break;
    case 3: pGpioPort = GPIO3; break;
    default:
        break;
    }
    rtk_int_disable();
    pGpioPort->IS  &= ~mask;  // edge sensitive
    pGpioPort->IBE &= ~mask;  // single edge
    if (rising)
    {
        pGpioPort->IEV |=  mask;  // rising edge
        pGpioPort->IE  |=  mask;
    }
    else
    {
        pGpioPort->IEV &= ~mask;  // falling edge
        pGpioPort->IE  |=  mask;
    }
    rtk_int_enable();
}

/******************************************************************************
**                            End Of File
******************************************************************************/
