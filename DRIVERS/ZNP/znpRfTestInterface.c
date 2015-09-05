/**
* @file znpRfTestInterface.c
*
* @brief RF Test Interface for ZNP
* 
* Use this when doing RF testing with the ZNP.
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 586 $
* $Author: dsmith $
* $Date: 2010-06-11 10:17:22 -0700 (Fri, 11 Jun 2010) $
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
#include "znpRfTestInterface.h"
#include "znp_hal.h"

#define ZNP_SPI 1
#ifdef ZNP_SPI
#include "znp_interface_spi.h"
#endif

extern signed int writeRfTestParam(unsigned char mode, unsigned char channel, unsigned char power);

/** Configures the ZNP to output a test signal, modulated or unmodulated. The radio must be re-initialized after this method is used.
@note this command does NOT respond with an SRSP.
@param mode whether to output modulated or unmodulated signal. Must be RF_TEST_MODULATED or RF_TEST_UNMODULATED or RF_TEST_RECEIVE. Note: RF_TEST_RECEIVE does nothing.
@param channel which 802.15.4 channel to output test signal on. Must be between 11 - 26.
@pre ZNP was initialized
@post ZNP is outputting the specified type of RF test signal on the specified channel
@return 0 if success, -1 if invalid channel, -2 if invalid test mode, else error code from sendMessage()
*/
signed int rfTest(unsigned char mode, unsigned char channel)
{
    if ((channel<RF_TEST_CHANNEL_MIN) || (channel>RF_TEST_CHANNEL_MAX))
    {
        printf("error - channel must be between 11-26\r\n");
        return -1;
    }
    if ((mode != RF_TEST_MODULATED) && (mode != RF_TEST_UNMODULATED) && (mode != RF_TEST_RECEIVE))
    {
        printf("error - unknown mode\r\n");
        return -2;
    }
    unsigned int frequency = 2405 + 5*(channel-11);  //CC253x family datasheet, equation 19-1
    if (mode == RF_TEST_MODULATED) printf("RF Test Mode Modulated");
    if (mode == RF_TEST_UNMODULATED) printf("RF Test Mode UnModulated");
    if (mode == RF_TEST_RECEIVE) printf("RF Test Mode Receive");
    printf("(mode %u) on channel %u (frequency = %u Hz)\r\n", mode, channel, frequency);
   return writeRfTestParam(mode, channel, RF_OUTPUT_POWER_PLUS_4_5_DBM);
}