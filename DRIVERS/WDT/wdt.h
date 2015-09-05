/*****************************************************************************
 *   wdt.h:  Header file for NXP EMxxx
 *
 *   Copyright(C) 2009, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *
 ******************************************************************************/

#ifndef __WDT_H 
#define __WDT_H

#define WDEN     0x00000001
#define WDRESET  0x00000002
#define WDTOF    0x00000004
#define WDINT    0x00000008

void         WDT_Init			(void);
void         WDT_Feed			(void);

#endif /* end __WDT_H */
