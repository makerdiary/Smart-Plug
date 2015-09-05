/******************************************************************************/
/*  Copyright (c) 2011 NXP B.V.  All rights are reserved.                     */
/*  Reproduction in whole or in part is prohibited without the prior          */
/*  written consent of the copyright owner.                                   */
/*                                                                            */
/*  This software and any compilation or derivative thereof is, and           */
/*  shall remain the proprietary information of NXP and is                    */
/*  highly confidential in nature. Any and all use hereof is restricted       */
/*  and is subject to the terms and conditions set forth in the               */
/*  software license agreement concluded with NXP B.V.                        */
/*                                                                            */
/*  Under no circumstances is this software or any derivative thereof         */
/*  to be combined with any Open Source Software, exposed to, or in any       */
/*  way licensed under any Open License Terms without the express prior       */
/*  written permission of the copyright owner.                                */
/*                                                                            */
/*  For the purpose of the above, the term Open Source Software means         */
/*  any software that is licensed under Open License Terms. Open              */
/*  License Terms means terms in any license that require as a                */
/*  condition of use, modification and/or distribution of a work              */
/*                                                                            */
/*  1. the making available of source code or other materials                 */
/*     preferred for modification, or                                         */
/*                                                                            */
/*  2. the granting of permission for creating derivative                     */
/*     works, or                                                              */
/*                                                                            */
/*  3. the reproduction of certain notices or license terms                   */
/*     in derivative works or accompanying documentation, or                  */
/*                                                                            */
/*  4. the granting of a royalty-free license to any party                    */
/*     under Intellectual Property Rights                                     */
/*                                                                            */
/*  regarding the work and/or any work that contains, is combined with,       */
/*  requires or otherwise is based on the work.                               */
/*                                                                            */
/*  This software is provided for ease of recompilation only.                 */
/*  Modification and reverse engineering of this software are strictly        */
/*  prohibited.                                                               */
/*                                                                            */
/******************************************************************************/

/*******************************************************************************
 * standard include files
 ******************************************************************************/

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

/*******************************************************************************
 * project include files                                   
 ******************************************************************************/

#include "bsp.h"
#include "i2c.h"
#include "lcd.h"
#include "rtk.h"
#include "pcb.h"
#include "gpio.h"

#ifdef SUPPORT_LOCAL_LCD

/*******************************************************************************
 * type definitions                                   
 ******************************************************************************/

#define I2C_ADR_PCF2119        0x76              // LCD

#define M0L_RESET_LCD          PCB_PORTPIN(PORT1,5)
#define LCD_CONTROL            0x00
#define LCD_DATA               0x40

#define LCD_FUNC_TYPE          0x30    // 8-bits, 1 line x 32 chars, 1:18 mux
#define LCD_FUNC_STANDARD      0x00
#define LCD_FUNC_EXTENDED      0x01

#define LCD_STANDARD_CLEAR     0x01    // Clear display
#define LCD_STANDARD_HOME      0x02    // Return home
#define LCD_STANDARD_ENTRY     0x06    // DDRAM/CGRAM increments by 1, cursor moves to right, no shift
#define LCD_STANDARD_DISPLAY   0x0C    // Display control: display on, cursor off, no blink

#define LCD_EXTENDED_VA        0x80    // Vlcd-A: characters
#define LCD_EXTENDED_VB        0xC0    // Vlcd-B: icons

#define LCD_DEFAULT_CONTRAST   40

/*******************************************************************************
 * global variables
 ******************************************************************************/

static uint8_t   contrast;

/*******************************************************************************
 * Init
 ******************************************************************************/

void lcd_init(void)
{
    unsigned char lcd_control[] = {
        LCD_CONTROL,                            // Control byte, basic instructions
        LCD_FUNC_TYPE | LCD_FUNC_EXTENDED,      // Type, go to extended set
        LCD_EXTENDED_VA | LCD_DEFAULT_CONTRAST, // Vlcd-A: characters contrast
        LCD_EXTENDED_VB,                        // Vlcd-B: icons off
        LCD_FUNC_TYPE | LCD_FUNC_STANDARD,      // Type, go to standard set
        LCD_STANDARD_DISPLAY,                   // Display control
        LCD_STANDARD_ENTRY,                     // Entry mode set
        LCD_STANDARD_HOME,                      // Return home
        LCD_STANDARD_CLEAR                      // Clear display
    };

    rtk_tsk_sleep(100); /* RC circuit has to become stable, so wait 100 msec */
#ifdef M0L_RESET_LCD
    /* reset the LCD display module (type BATRON - BTHQ21605V-COG-FSRE-I2C) */
    gpio_alloc( M0L_RESET_LCD, PCB_MODE_PULLNONE, GPIO_OUTPUT );
    gpio_set(M0L_RESET_LCD);
    rtk_tsk_sleep(100);
    gpio_clr(M0L_RESET_LCD);
    rtk_tsk_sleep(100);
#endif
    I2C_Write(I2C_ADR_PCF2119, lcd_control, sizeof(lcd_control));
    rtk_tsk_sleep(10);  // wait 10 msec
    contrast = LCD_DEFAULT_CONTRAST;
}

/*******************************************************************************
 * Contrast Functions
 ******************************************************************************/

static void lcd_contrast_set( void )
{
    unsigned char lcd_control[] = {
        LCD_CONTROL,                          // Control byte, basic instructions
        LCD_FUNC_TYPE | LCD_FUNC_EXTENDED,    // Type, go to extended set
        LCD_EXTENDED_VA | contrast            // Vlcd-A: characters contrast
    };

    I2C_Write(I2C_ADR_PCF2119, lcd_control, sizeof(lcd_control));
}

void lcd_contrast_up( void )
{
    if ( contrast < 50 )
    {
      contrast += 2;
      lcd_contrast_set();
    }
}

void lcd_contrast_dn( void )
{
    if ( contrast > 30 )
    {
      contrast -= 2;
      lcd_contrast_set();
    }
}

/*******************************************************************************
 * Text Function
 ******************************************************************************/

void lcd_update(char *pnew_lcd_text)
{
    int  i;
    int  txt_len = strlen(pnew_lcd_text);
    unsigned char lcd_control_data[(LCD_NR_OF_LINES * LCD_LINE_LENGTH) + 1];
    
    lcd_control_data[0] = LCD_CONTROL;          // Control byte, basic instructions
    lcd_control_data[1] = LCD_FUNC_TYPE | LCD_FUNC_STANDARD;
    lcd_control_data[2] = LCD_STANDARD_HOME;    // Return home
    I2C_Write(I2C_ADR_PCF2119, lcd_control_data, 3);

    // Select data register
    lcd_control_data[0] = LCD_DATA;             // Set for data input
    for (i=0; i<(LCD_NR_OF_LINES * LCD_LINE_LENGTH); i++)
    {
        if (i < txt_len)
        {
            lcd_control_data[1+i] = pnew_lcd_text[i] + 0x80;
        }
        else
        {
            lcd_control_data[1+i] = ' ' + 0x80;
        }
    }
    I2C_Write(I2C_ADR_PCF2119, lcd_control_data, (LCD_NR_OF_LINES * LCD_LINE_LENGTH) + 1);
}

#endif  // #ifdef SUPPORT_LOCAL_LCD

/* End of file */
