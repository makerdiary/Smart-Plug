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

#include "mbus_crc.h"

/******************************************************************************
 * type definitions
 *****************************************************************************/

#define POLYNOMIAL 0x3d65


/******************************************************************************
 * global variables
 *****************************************************************************/


/******************************************************************************
 * exported functons
 *****************************************************************************/

#if 0
short calc_crc(unsigned char * ptr_data, int len)
{
    int           i           = 0;
    short         crc         = 0;   /* reset LFSR */
    unsigned char j           = 0;
    unsigned char messagebyte = 0;
    unsigned char messagebit  = 0;
    unsigned char lfsrout     = 0;
  
    for (i = 0;  i < len;  i++)
    {
  	    messagebyte = *ptr_data++;
  	    for (j=0; j<8; j++)
        {
		    /* determine bit from byte, starting with MSB */
		    messagebit  = ((messagebyte & 0x80) ? 1 : 0);
		    messagebyte = messagebyte << 1;
		
		    /* determine output of LFSR */
		    lfsrout = ((crc & 0x8000) ? 1 : 0);
		
		    /* shift LFSR */
		    crc = crc << 1;
		
		    /* if messagebit XOR lfsrout == 1, add polynomial to LFSR */
		    if (messagebit ^ lfsrout)
            {
			    crc ^= POLYNOMIAL;
            }
        }
    }
  
    /* Complement output */
    crc ^= 0xFFFF;
    return crc;
}
#else
/*!
	\brief				CRC-16 function for 0x3D65 polynom (MSBF/normal)
	
	\param[in]		message			The input bytes
	\param[in]		msgsize			The number of input bytes
	\param[in]		init				The initial CRC value
	\param[in]		xor					The final XOR value
	
	\return				The CRC value.
*/
unsigned short crc16_0x3D65_normal(unsigned char *message, unsigned int msgsize, unsigned short init, unsigned short xor) {
	
	unsigned short c;
	unsigned char d, s, t;
	
	unsigned int i;
	
	// init
	c = init;
	// compute CRC
	for (i = 0; i < msgsize; i++) {
		d = message[i];
		
		s = (c >> 8) ^ d;
		t = (s >> 5) ^ (s >> 4) ^ (s >> 3) ^ s;
		c = (c << 8) ^
		     t       ^
		    (t << 2) ^
		    (t << 5) ^
		    (t << 6) ^
		    (t << 8) ^
		    (t << 10) ^
		    (t << 11) ^
		    (t << 12) ^
		    (t << 13);
	}
	// final xor
	c = c ^ xor;
	return c;
}

/*!
	\brief				ISO/IEC 60870-5-2 (MBUS) CRC-16 function.
	
	\param[in]		message			The input bytes
	\param[in]		msgsize			The number of input bytes
	
	\return				The CRC value.
*/
short calc_crc/*crc16_mbus*/(unsigned char *message, int msgsize) {
	return (short)crc16_0x3D65_normal(message, (unsigned int)msgsize, 0x0000, 0xFFFF);
}
#endif

void create_mbus_header(char *buf, char data_len, char c_field, int own_id, char dev_ver, char dev_type)
{
    short man_id = (32*32*('N'-'@')) + (32*('X'-'@')) + ('P'-'@');
    short crc;
    
    buf[0]  = (10 - 1) + data_len;           // L-field: header bytes (exclusive L-field and CRC)
    buf[1]  = c_field;                       // C-field: "SEND/NO REPLY"
    buf[2]  = (man_id >>  0) & 0xFF;         // M-field: Manufacturer LSB
    buf[3]  = (man_id >>  8) & 0xFF;         // M-field: Manufacturer MSB
    buf[4]  = (own_id >>  0) & 0xFF;         // A-field: Identification-nr
    buf[5]  = (own_id >>  8) & 0xFF;         // A-field: Identification-nr
    buf[6]  = (own_id >> 16) & 0xFF;         // A-field: Identification-nr
    buf[7]  = (own_id >> 24) & 0xFF;         // A-field: Identification-nr
    buf[8]  = dev_ver;                       // A-field: Version number
    buf[9]  = dev_type;                      // A-field: Device type ('Electricity' or 'Other')
    crc = calc_crc((unsigned char *)buf,10); // CRC over first block (10 bytes)
    buf[10] = (crc >> 8) & 0xFF;             // CRC-field: MSB
    buf[11] = (crc >> 0) & 0xFF;             // CRC-field: LSB
}

/* End of file */
