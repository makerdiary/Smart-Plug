/**
* @file znp_interface_spi.c
*
* @brief Interface to the Zigbee Network Processor (e.g. CC2530ZNP) over the Serial Peripheral Interface (SPI) port.
*
* @note To output SPI transmit information, define ZNP_INTERFACE_SPI_VERBOSE.
* It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* ZNP_INTERFACE_SPI_VERBOSE
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 590 $
* $Author: dsmith $
* $Date: 2010-06-11 17:04:51 -0700 (Fri, 11 Jun 2010) $
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
#include "znp_hal.h"
#include "znp_interface_spi.h"


/** This buffer will hold the transmitted messages and received SRSP Payload after sendMessage() was called. */
unsigned char znpBuf[100];

/**
Sends a CC2530ZNP Synchronous Request (SREQ) message and retrieves the response. 
A SREQ is a message to the ZNP that is immediately followed by a Synchronous Response (SRSP) message  from the ZNP. 
As opposed to an Asynchronous Request (AREQ) message, which does not have a SRSP.
This is a private method that gets wrapped by sendMessage().
@pre CC2530ZNP has been initialized
@pre znpBuf contains a properly formatted message
@post received data is written to znpBuf
@return 0 if success, -1 if timeout waiting for SRDY to go low after chip select; -2 if timeout waiting for SRSP.
@TODO: timeout on SRDY_IS_HIGH() and SRDY_IS_LOW(), or separate this method and return control to the application
*/
signed int sendSreq()
{
    SPI_SS_SET();   
    while (SRDY_IS_HIGH());                    //wait until SRDY goes low
    spiWrite(znpBuf, (*znpBuf + 3));              // *bytes (first byte) is length after the first 3 bytes, all frames have at least the first 3 bytes
    *znpBuf = 0; *(znpBuf+1) = 0; *(znpBuf+2) = 0; //poll message is 0,0,0
    //SPI_SS_CLEAR();    //NOTE: MRDY must remain asserted here, but can de-assert SS if the two signals are separate
    while (SRDY_IS_LOW());                    //wait for data
    //SPI_SS_SET();      //NOTE: if SS & MRDY are separate signals then can re-assert SS here.
    spiWrite(znpBuf, 3);
    if (*znpBuf > 0)                             // *bytes (first byte) contains number of bytes to receive
        spiWrite(znpBuf+3, *znpBuf);              //write-to-read: read data into buffer    
    SPI_SS_CLEAR();
    return 0;  
}

/**
Polls the ZNP for data. This is used to receive data from the ZNP, for example when a message has arrived. 
This will be initiated by detecting SRDY going low. 
@pre CC2530ZNP has been initialized.
@pre SRDY has gone low
@pre CC2530ZNP has been initialized
@post received data is written to znpBuf
@return result of sendSreq()
@note this method not required to be implemented when using UART interface.
*/
signed int spiPoll()
{
    *znpBuf = 0; *(znpBuf+1) = 0; *(znpBuf+2) = 0;  //poll message is 0,0,0 
    return(sendSreq());
}


/** Public method to send messages to the ZNP. This will send one message and then receive the 
Synchronous Response (SRSP) message from the ZNP to indicate the command was received.
@pre znpBuf contains a properly formatted message
@pre ZNP has been initialized
@post buffer znpBuf contains the response (if any) from the ZNP.
@return 0 if the SRSP received was the expected SRSP, -11 if it was NOT the expected SRSP, 
-12 if sendSreq timeout, or -13 if too many bytes received. 
*/
signed int sendMessage()
{
#ifdef ZNP_INTERFACE_SPI_VERBOSE    
    printf("Tx: ");
    printHexBytes(znpBuf, znpBuf[0] + 3);
#endif    
    
    unsigned char expectedSrspCmdMsb = znpBuf[1] + 0x40;
    unsigned char expectedSrspCmdLsb = znpBuf[2];

    signed int result = sendSreq(); //send message, buffer now holds received data
    
    if (result != 0)                            //ERROR - spiSreq() timeout
    {
        #ifdef ZNP_INTERFACE_SPI_VERBOSE    
        printf("ERROR - sreq() timeout %i\r\n", result);
        #endif 
        return -12;
    }
    if (znpBuf[0] > SRSP_BUFFER_SIZE)           //Error - SRSP length exceeds buffer size
    {
        #ifdef ZNP_INTERFACE_SPI_VERBOSE    
        printf("ERROR - SRSP too long %u > %u\r\n", znpBuf[0], SRSP_BUFFER_SIZE);
        #endif         
        return -13;
    }

    //The correct SRSP will always be 0x4000 + cmd, or simpler 0x4000 | cmd
    //For example, if the SREQ is 0x2605 then the corresponding SRSP is 0x6605
    if ((znpBuf[1] == expectedSrspCmdMsb) && (znpBuf[2] == expectedSrspCmdLsb))    //verify the correct SRSP was received
        return 0;
    else
    {
        #ifdef ZNP_INTERFACE_SPI_VERBOSE    
        printf("ERROR - Wrong SRSP - received %02X-%02X, expected %02X-%02X\r\n", znpBuf[1], znpBuf[2],expectedSrspCmdMsb,expectedSrspCmdLsb);
        #endif          
        return -11;   //Wrong SRSP received
        
    }
}

