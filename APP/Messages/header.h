/**
*
* @file header.h
*
* @brief public methods for header.c
*
* $Rev: 598 $
* $Author: dsmith $
* $Date: 2010-06-14 16:08:43 -0700 (Mon, 14 Jun 2010) $
*
* YOU ACKNOWLEDGE AND AGREE THAT THE SOFTWARE AND DOCUMENTATION ARE PROVIDED “AS IS” WITHOUT WARRANTY 
* OF ANY KIND, EITHER EXPRESS OR IMPLIED, INCLUDING WITHOUT LIMITATION, ANY WARRANTY OF MERCHANTABILITY, 
* TITLE, NON-INFRINGEMENT AND FITNESS FOR A PARTICULAR PURPOSE. IN NO EVENT SHALL TEXAS INSTRUMENTS 
* OR ITS LICENSORS BE LIABLE OR OBLIGATED UNDER CONTRACT, NEGLIGENCE, STRICT LIABILITY, CONTRIBUTION, 
* BREACH OF WARRANTY, OR OTHER LEGAL EQUITABLE THEORY ANY DIRECT OR INDIRECT DAMAGES OR EXPENSES
* INCLUDING BUT NOT LIMITED TO ANY INCIDENTAL, SPECIAL, INDIRECT, PUNITIVE OR CONSEQUENTIAL DAMAGES, 
* LOST PROFITS OR LOST DATA, COST OF PROCUREMENT OF SUBSTITUTE GOODS, TECHNOLOGY, SERVICES, OR ANY 
* CLAIMS BY THIRD PARTIES (INCLUDING BUT NOT LIMITED TO ANY DEFENSE THEREOF), OR OTHER SIMILAR COSTS.
*/
#ifndef HEADER_H
#define HEADER_H

/**
* Common Message Header
*/
struct header
{
    /** Application level sequence number, used to track response from the server etc. LSB first*/
    unsigned int sequence;
    
    /** Protocol version. If message protocol changes then use a different number for version and if/then on receiver accordingly */
    unsigned char version;  
    
    /** Flags to indicate various stuff; dependent on application. 
    One might be for a pending firmware update. 
    For battery powered devices one might indicate low batt. */
    unsigned char flags;
    
    /** MAC Address of sender (for messages from device to server) or of destination (for messages from server to device). 
    Mainly used for messages going through a gateway (e.g. Zigbee to Ethernet) so that the server receiving the message knows who sent the message.
    While this can be obtained by querying the network, it's faster to put it in the message */
    unsigned char mac[8];
};

void printHeader(struct header* h);
void serializeHeader(struct header* h, unsigned char* destination);
unsigned int getSizeOfHeader(struct header* h);
struct header deserializeHeader( unsigned char* sourcePtr);

#endif
