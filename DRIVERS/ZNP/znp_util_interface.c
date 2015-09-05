/**
* @file znp_util_interface.c
*
* @brief UTIL Interface to the Zigbee Network Processor (e.g. CC2530ZNP)
* 
* Refer to interface spec for more information
*
* Requires one physical interface file to be included, znp_interface_spi.h
*
* @note To output SPI transmit information, define ZNP_UTIL_INTERFACE_VERBOSE.
* It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* ZNP_UTIL_INTERFACE_VERBOSE
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 665 $
* $Author: dsmith $
* $Date: 2010-07-13 20:13:50 -0700 (Tue, 13 Jul 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS?WITHOUT WARRANTY 
* OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, 
* TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS INSTRUMENTS 
* OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, 
* BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY 
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/
#include "znp_util_interface.h"
//#include <string.h>                     //for memcpy
#include "znp_hal.h"
#include "znp_interface_spi.h"


extern unsigned char znpBuf[100];

#define UTIL_ADDRMGR_NWK_ADDR_LOOKUP_PAYLOAD_LEN 2
/** Uses the UTIL interface to get the long address of a device based on its short address.
The device to lookup must either be a parent or child of the ZNP, or have established a binding or application link key.
@note on a trivial network (one coord, one router) this takes approx. 0.4833mSec to complete.
@param shortAddress the shortAddress to lookup
@return the long address returned by the ZNP.
*/
unsigned char* znpGetMacForShort(unsigned int shortAddress)
{
    znpBuf[0] = UTIL_ADDRMGR_NWK_ADDR_LOOKUP_PAYLOAD_LEN;
    znpBuf[1] = MSB(UTIL_ADDRMGR_NWK_ADDR_LOOKUP);
    znpBuf[2] = LSB(UTIL_ADDRMGR_NWK_ADDR_LOOKUP);
    
    znpBuf[3] = LSB(shortAddress);
    znpBuf[4] = MSB(shortAddress);
    
    //znpResult = sendMessage();
    sendMessage();
    return (znpBuf+SRSP_PAYLOAD_START);
}
