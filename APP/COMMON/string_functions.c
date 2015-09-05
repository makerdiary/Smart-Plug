/*****************************************************************************/
/*  Copyright (c) 2010 NXP B.V.  All rights are reserved.                    */
/*  Reproduction in whole or in part is prohibited without the prior         */
/*  written consent of the copyright owner.                                  */
/*                                                                           */
/*  This software and any compilation or derivative thereof is, and          */
/*  shall remain the proprietary information of NXP and is                   */
/*  highly confidential in nature. Any and all use hereof is restricted      */
/*  and is subject to the terms and conditions set forth in the              */
/*  software license agreement concluded with NXP B.V.                       */
/*                                                                           */
/*  Under no circumstances is this software or any derivative thereof        */
/*  to be combined with any Open Source Software, exposed to, or in any      */
/*  way licensed under any Open License Terms without the express prior      */
/*  written permission of the copyright owner.                               */
/*                                                                           */
/*  For the purpose of the above, the term Open Source Software means        */
/*  any software that is licensed under Open License Terms. Open             */
/*  License Terms means terms in any license that require as a               */
/*  condition of use, modification and/or distribution of a work             */
/*                                                                           */
/*  1. the making available of source code or other materials                */
/*     preferred for modification, or                                        */
/*                                                                           */
/*  2. the granting of permission for creating derivative                    */
/*     works, or                                                             */
/*                                                                           */
/*  3. the reproduction of certain notices or license terms                  */
/*     in derivative works or accompanying documentation, or                 */
/*                                                                           */
/*  4. the granting of a royalty-free license to any party                   */
/*     under Intellectual Property Rights                                    */
/*                                                                           */
/*  regarding the work and/or any work that contains, is combined with,      */
/*  requires or otherwise is based on the work.                              */
/*                                                                           */
/*  This software is provided for ease of recompilation only.                */
/*  Modification and reverse engineering of this software are strictly       */
/*  prohibited.                                                              */
/*                                                                           */
/*****************************************************************************/

/******************************************************************************
 * standard include files
 *****************************************************************************/


/******************************************************************************
 * project include files
 *****************************************************************************/

#include "dprint_macros.h"
#include "string_functions.h"


// function: ftos - float to string
// description: decimal representation of a number is
//                 <iii>.<fff>
//              where <iii> is the integer or integral part of the decimal number,
//              and <fff> is the fractional part of the decimal number.
// parameters: char *result   - string to put the value in
//             fval       (float)  - the float to convert
//             frac_size  (int)     - fraction size(max=4), or the number of digits behind the period (.)
// returns:    *result     : 
//             return value: length of characters written (without trailing '\n')
//
// note: this function is needed because some libc printf implementaitions do not correctly support %f.

int ftos(char *result, float fval, int frac_size)
{
    int  mul = 1;
    int  len = 0;
    int  i;
    int  ival;
    int  fraction;

    result[0] = '\0';
    if (frac_size > 4) frac_size = 4;
    for (i = 0; i < frac_size; i++)
    {
        mul *= 10;
    }
    if (fval < (float)0.0)
    {
        result[0] = '-';
        result[1] = '\0';
        len++;
        fval = (float)0.0 - fval;    /* make fval positive */
    }
    fval    += ((float)0.5 / mul);   /* do rounding of smallest digit */
    ival     = (int)fval;
    fraction = (int)((fval - (float)ival) * mul);
    len     += DO_SPRINTF( &result[len], "%d", ival);
    switch (frac_size)
    {
    case  1: len += DO_SPRINTF(&result[len], ".%1d",  fraction); break;
    case  2: len += DO_SPRINTF(&result[len], ".%02d", fraction); break;
    case  3: len += DO_SPRINTF(&result[len], ".%03d", fraction); break;
    case  4: len += DO_SPRINTF(&result[len], ".%04d", fraction); break;
    default: break;
    }
    return len;
}

int getTimeFormat(char *result, unsigned int sec)
{
    int len = 0;
    int day,hour,minute,second,tmp;
    day    = sec / 86400;
    hour   = (tmp = sec - day * 86400) / 3600;
    minute = (tmp = tmp - hour * 3600) / 60;
    second = tmp - minute * 60;
    
    len = DO_SPRINTF(result, "%04d %02d:%02d:%02d", day, hour, minute, second);
    
    return len;
}


// function: initializeBuffer
// description: Fills the messagePartBuffer with DEADBEEF to aid in debugging
// parameters: unsigned char *buf  - the buffer to be filled
//             len (unsigned char) - the len of the buffer
// returns:    *s          : 
//             return value: void
//
// note: 

void initializeBuffer(unsigned char *buf, unsigned char len)
{
    for (int i=0; i<len; i+=4)
    {
        buf[i]   = 0xDE;
        buf[i+1] = 0xAD;
        buf[i+2] = 0xBE;
        buf[i+3] = 0xEF;
    }
}


// function: printHexBytes
// description: print bytes in hex form
// parameters: unsigned char *toPrint  - the string to be printed
//             numBytes  (unsigned char) - the number of Bytes
// returns:    *s          : 
//             return value: void
//
// note: 

void printHexBytes(unsigned char *toPrint, unsigned char numBytes)
{   
    for (int i=numBytes-1; i>=0; --i)
        printf("%02X ", toPrint[i]);  
    printf("\r\n");
}



// function: printBinary
// description: print a number in its binary form
// parameters: unsigned char n  - the number
// returns: 
//             return value: void
//
// note:

void printBinary(unsigned char n)
{
    unsigned int i, j = 0;
    unsigned char buf[sizeof(n)*8 + 1];
    i = 1<<(sizeof(n)*8-1);
    
    while (i > 0)
    {
        buf[j] = (n & i)? '1' : '0';
        i >>= 1;
        ++j;
    }
    buf[j] = '\0';
    DPRINT_NOARG(buf);
}


// function: isAsciiPrintableCharacter
// description: returns true if is printable: ASCII values 0x20 (space) through 0x7E (~)
// parameters: unsigned char c  - the char
// returns: 
//             return value (boolean): true or false
//
// note:

char isAsciiPrintableCharacter(unsigned char c)
{
    return ((c >= 0x20) && (c <= 0x7E));
}


/* End of file */
