/******************************************************************************/
/*  Copyright (c) 2010 NXP B.V.  All rights are reserved.                     */
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


/*******************************************************************************
 * project include files
 ******************************************************************************/

#include "conversions.h"


/*******************************************************************************
 * type definitions
 ******************************************************************************/


/*******************************************************************************
 * global variables
 ******************************************************************************/

static const unsigned char table_manchester[16] =
/*    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F  */
  {0xAA,0xA9,0xA6,0xA5,0x9A,0x99,0x96,0x95,0x6A,0x69,0x66,0x65,0x5A,0x59,0x56,0x55};
static const unsigned char table_3outof6[16] =
/*    0,   1,   2,   3,   4,   5,   6,   7,   8,   9,   A,   B,   C,   D,   E,   F  */
  {0x16,0x0D,0x0E,0x0B,0x1C,0x19,0x1A,0x13,0x2C,0x25,0x26,0x23,0x34,0x31,0x32,0x29};


/*******************************************************************************
 * global functions
 ******************************************************************************/

/*----------------------------------------------------------------------------*/
/* Name       : conv_byte_to_manchester                                       */
/* Description: Convert a data byte into a manchester encoded value (16 bits) */
/* Parameters :                                                               */
/*    val - data byte to convert                                              */
/* Returns    : the manchester value for the passed data byte                 */
/*----------------------------------------------------------------------------*/

short conv_byte_to_manchester(char val)
{
	short retval;
    retval  = (((short)table_manchester[(val >> 0) & 0x0F]) << 0);
    retval |= (((short)table_manchester[(val >> 4) & 0x0F]) << 8);
	return retval;
}

/*----------------------------------------------------------------------------*/
/* Name       : conv_manchester_to_byte                                       */
/* Description: Convert a manchester encoded value (16 bits) into a data byte */
/* Parameters :                                                               */
/*    val - manchester encoded value to convert                               */
/* Returns    : the converted data byte                                       */
/*----------------------------------------------------------------------------*/

char conv_manchester_to_byte(short val)
{
    char retval = 0;
    char lsbval = ((val >> 0) & 0xFF);
    char msbval = ((val >> 8) & 0xFF);
	int  i = 0;
    
	for (i=0; i<16; i++)
	{
        if (table_manchester[i] == lsbval)
        {
            retval |= (i << 0);
        }
        if (table_manchester[i] == msbval)
        {
            retval |= (i << 4);
        }
    }
    return retval;
}

/*----------------------------------------------------------------------------*/
/* Name       : conv_byte_to_3outof6                                          */
/* Description: Convert a data byte into a "3 out of 6" value (12 bits)       */
/* Parameters :                                                               */
/*    val - data byte to convert                                              */
/* Returns    : the "3 out of 6" value for the passed data byte               */
/*----------------------------------------------------------------------------*/

short conv_byte_to_3outof6(char val)
{
    short retval;
    retval  = (((short)table_3outof6[(val >> 0) & 0x0F]) << 0);
    retval |= (((short)table_3outof6[(val >> 4) & 0x0F]) << 6);
    return retval;
}

/*----------------------------------------------------------------------------*/
/* Name       : conv_3outof6_to_byte                                          */
/* Description: Convert a "3 out of 6" value (12 bits) into a data byte       */
/* Parameters :                                                               */
/*    val - the 12 bits "3 out of 6" value to convert                         */
/* Returns    : the converted data byte                                       */
/*----------------------------------------------------------------------------*/

char conv_3outof6_to_byte(short val)
{
    char retval = 0;
    char lsbval = ((val >> 0) & 0x3F);
    char msbval = ((val >> 6) & 0x3F);
	int  i = 0;

	for (i=0; i<16; i++)
	{
        if (table_3outof6[i] == lsbval)
        {
            retval |= (i << 0);
        }
        if (table_3outof6[i] == msbval)
        {
            retval |= (i << 4);
        }
    }
    return retval;
}

/* End of file */
