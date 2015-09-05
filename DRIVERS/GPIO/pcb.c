/*****************************************************************************
 *   pcb.c:  C file for NXP LPC11xx/LPC13xx/EM7xx Family Microprocessors
 *
 *   Copyright(C) 2010, NXP Semiconductor
 *   All rights reserved.
 *
 *****************************************************************************/

#include "bsp.h"
#include "rtk.h"
#include "pcb.h"

#define PCB_PIO_BASE 0x40044000
#define PCB_ADDR_TO_INDEX(x)   ( ( (x) - PCB_PIO_BASE ) >> 2 )
#define PCB_INDEX_TO_ADDR(x)   ( ( (x) << 2 ) + PCB_PIO_BASE )

#define PCB_PORT_PIN_TO_ADDR(port,pin)   \
  (volatile unsigned long*)( PCB_INDEX_TO_ADDR( pcb_addr_table[((port) * 12 ) + (pin)] ) )


const uint8_t pcb_addr_table[48] = {
	/* PIO0_0*/ PCB_ADDR_TO_INDEX( 0x4004400C ) ,
	/* PIO0_1*/ PCB_ADDR_TO_INDEX( 0x40044010 ) ,
	/* PIO0_2*/ PCB_ADDR_TO_INDEX( 0x4004401C ) ,
	/* PIO0_3*/ PCB_ADDR_TO_INDEX( 0x4004402C ) ,
	/* PIO0_4*/ PCB_ADDR_TO_INDEX( 0x40044030 ) ,
	/* PIO0_5*/ PCB_ADDR_TO_INDEX( 0x40044034 ) ,
	/* PIO0_6*/ PCB_ADDR_TO_INDEX( 0x4004404C ) ,
	/* PIO0_7*/ PCB_ADDR_TO_INDEX( 0x40044050 ) ,
	/* PIO0_8*/ PCB_ADDR_TO_INDEX( 0x40044060 ) ,
	/* PIO0_9*/ PCB_ADDR_TO_INDEX( 0x40044064 ) ,
	/* PIO0_10*/PCB_ADDR_TO_INDEX( 0x40044068 ) ,
	/* PIO0_11*/PCB_ADDR_TO_INDEX( 0x40044074 ) ,

	/* PIO1_0*/ PCB_ADDR_TO_INDEX( 0x40044078 ) ,
	/* PIO1_1*/ PCB_ADDR_TO_INDEX( 0x4004407C ) ,
	/* PIO1_2*/ PCB_ADDR_TO_INDEX( 0x40044080 ) ,
	/* PIO1_3*/ PCB_ADDR_TO_INDEX( 0x40044090 ) ,
	/* PIO1_4*/ PCB_ADDR_TO_INDEX( 0x40044094 ) ,
	/* PIO1_5*/ PCB_ADDR_TO_INDEX( 0x400440A0 ) ,
	/* PIO1_6*/ PCB_ADDR_TO_INDEX( 0x400440A4 ) ,
	/* PIO1_7*/ PCB_ADDR_TO_INDEX( 0x400440A8 ) ,
	/* PIO1_8*/ PCB_ADDR_TO_INDEX( 0x40044014 ) ,
	/* PIO1_9*/ PCB_ADDR_TO_INDEX( 0x40044038 ) ,
	/* PIO1_10*/PCB_ADDR_TO_INDEX( 0x4004406C ) ,
	/* PIO1_11*/PCB_ADDR_TO_INDEX( 0x40044098 ) ,

	/* PIO2_0*/ PCB_ADDR_TO_INDEX( 0x40044008 ) ,
	/* PIO2_1*/ PCB_ADDR_TO_INDEX( 0x40044028 ) ,
	/* PIO2_2*/ PCB_ADDR_TO_INDEX( 0x4004405C ) ,
	/* PIO2_3*/ PCB_ADDR_TO_INDEX( 0x4004408C ) ,
	/* PIO2_4*/ PCB_ADDR_TO_INDEX( 0x40044040 ) ,
	/* PIO2_5*/ PCB_ADDR_TO_INDEX( 0x40044044 ) ,
	/* PIO2_6*/ PCB_ADDR_TO_INDEX( 0x40044000 ) ,
	/* PIO2_7*/ PCB_ADDR_TO_INDEX( 0x40044020 ) ,
	/* PIO2_8*/ PCB_ADDR_TO_INDEX( 0x40044024 ) ,
	/* PIO2_9*/ PCB_ADDR_TO_INDEX( 0x40044054 ) ,
	/* PIO2_10*/PCB_ADDR_TO_INDEX( 0x40044058 ) ,
	/* PIO2_11*/PCB_ADDR_TO_INDEX( 0x40044070 ) ,

	/* PIO3_0*/ PCB_ADDR_TO_INDEX( 0x40044084 ) ,
	/* PIO3_1*/ PCB_ADDR_TO_INDEX( 0x40044088 ) ,
	/* PIO3_2*/ PCB_ADDR_TO_INDEX( 0x4004409C ) ,
	/* PIO3_3*/ PCB_ADDR_TO_INDEX( 0x400440AC ) ,
	/* PIO3_4*/ PCB_ADDR_TO_INDEX( 0x4004403C ) ,
	/* PIO3_5*/ PCB_ADDR_TO_INDEX( 0x40044048 ) ,
	/* 6 */ 0x00,
	/* 7 */ 0x00,
	/* 8 */ 0x00,
	/* 9 */ 0x00,
	/* 10*/ 0x00,
	/* 11*/ 0x00,
	};

static int pcb_claimed[PCB_NUMPORTS];

// This functions registers all portpins as not being claimed.
// This function is NOT task-safe.

void pcb_init(void)
{
    int i = 0;

    // Set to "no" claims
    for (i=0; i<PCB_NUMPORTS; i++)
    {
        pcb_claimed[i]= 0;
    }
}

// This function configures the function and electrical mode of a pin of a port.
// Use PCB_PORTPIN (from pcb.h) to form a pin of a port.
// use PCB_FUNC_x (from pcb.h) for mode (these vary from pin to pin, so describtive names are absent).
// Use PCB_MODE_xxx (from pcb.h) to select an electrical mode.
// Note that this function also registers the portpin as claimed.
// A new claim will fail (return -1) unless pcb_free is called first
// This function is task-safe.

void pcb_alloc(int portpin, pcb_func_t func, pcb_mode_t mode)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);

    if ((pcb_claimed[port] & mask) == mask)
    {
        //?? todo: assert ?
    }
    else
    {
      volatile unsigned long * pIoConfigReg = PCB_PORT_PIN_TO_ADDR( port, pin );
      rtk_int_disable();
      *pIoConfigReg = (mode << 3) | (func & 0x07);
      pcb_claimed[port] |= mask;
      rtk_int_enable();
    }
}

// This function frees a portpin claimed via pcb_alloc.
// This function is task-safe.

void pcb_free(int portpin)
{
    int          port = PCB_PORTPIN2PORT(portpin);
    int          pin  = PCB_PORTPIN2PIN(portpin);
    unsigned int mask = (1 << pin);

    rtk_int_disable();
    pcb_claimed[port] &= ~mask;
    rtk_int_enable();
}

/* EOF */
