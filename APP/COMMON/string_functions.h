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


#ifndef STRING_FUNCTIONS_H_INCLUDED
#define STRING_FUNCTIONS_H_INCLUDED

#define WHITESPACE(x) while (*x == ' ') {x++;}

/** Convert two unsigned chars to an unsigned int, LSB first*/
#define CONVERT_TO_INT(lsb,msb) ((lsb) + 0x0100*(msb))   // ((lsb) + (((unsigned int) (msb)) << 8))

/** Get the Least Significant Byte (LSB) of an unsigned int*/
#define LSB(num) ((num) & 0xFF)

/** Get the Most Significant Byte (MSB) of an unsigned int*/
#define MSB(num) ((num) >> 8)

#define BYTES_TO_LONG(byteArray) (( ((unsigned long)byteArray[0] << 24) + ((unsigned long)byteArray[1] << 16) + ((unsigned long)byteArray[2] << 8) + ((unsigned long)byteArray[3] ) ) );

extern int ftos(char *s, float fval, int frac_size);

extern void initializeBuffer(unsigned char *buf, unsigned char len);

extern void printBinary(unsigned char n);

extern void printHexBytes(unsigned char *toPrint, unsigned char numBytes);

extern char isAsciiPrintableCharacter(unsigned char c);

extern int getTimeFormat(char *result, unsigned int sec);
#endif

/* end of string_functions.h */
