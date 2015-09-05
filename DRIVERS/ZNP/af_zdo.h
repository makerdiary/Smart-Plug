/**
*  @file af_zdo.h
*
*  @brief  public methods for af_zdo.c
*
* $Rev: 622 $
* $Author: dsmith $
* $Date: 2010-07-02 10:55:33 -0700 (Fri, 02 Jul 2010) $
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
#ifndef AF_ZDO_H
#define AF_ZDO_H

#include "application_configuration.h"

void afRegisterApplication(struct applicationConfiguration ac);
void afRegisterGenericApplication();
void zdoStartApplication();
//signed int setJoiningPermissions(unsigned int destination, unsigned char timeout);
void afSendData(unsigned char destinationEndpoint, unsigned char sourceEndpoint, 
                      unsigned int destinationShortAddress, unsigned int clusterId, 
                      unsigned char* data, unsigned char dataLength);
void zdoRequestIeeeAddress(unsigned int shortAddress, unsigned char requestType, unsigned char startIndex);
unsigned char* zdoNetworkAddressRequest(unsigned char* ieeeAddress, unsigned char requestType, unsigned char startIndex);
signed int printAfIncomingMsgHeader(unsigned char* srsp);
void printAfIncomingMsgHeaderNames();
void printZdoNetworkAddressResponse(unsigned char* rsp);

#define SINGLE_DEVICE_RESPONSE          0
#define INCLUDE_ASSOCIATED_DEVICES      1

#define AF_INCOMING_MESSAGE_CLUSTER_LSB_FIELD       (SRSP_PAYLOAD_START+2)
#define AF_INCOMING_MESSAGE_CLUSTER_MSB_FIELD       (SRSP_PAYLOAD_START+3)
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS_LSB_FIELD (SRSP_PAYLOAD_START+4)
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS_MSB_FIELD (SRSP_PAYLOAD_START+5)
#define AF_INCOMING_MESSAGE_SHORT_ADDRESS()         (CONVERT_TO_INT(znpBuf[AF_INCOMING_MESSAGE_SHORT_ADDRESS_LSB_FIELD], znpBuf[AF_INCOMING_MESSAGE_SHORT_ADDRESS_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_CLUSTER()               (CONVERT_TO_INT(znpBuf[AF_INCOMING_MESSAGE_CLUSTER_LSB_FIELD], znpBuf[AF_INCOMING_MESSAGE_CLUSTER_MSB_FIELD]))
#define AF_INCOMING_MESSAGE_PAYLOAD_LEN_FIELD       (SRSP_PAYLOAD_START+16)
#define AF_INCOMING_MESSAGE_PAYLOAD_START_FIELD     (SRSP_PAYLOAD_START+17)

#define IS_AF_INCOMING_MESSAGE()   (CONVERT_TO_INT(znpBuf[SRSP_CMD_LSB_FIELD], znpBuf[SRSP_CMD_MSB_FIELD]) == AF_INCOMING_MSG)

//#define AF_DATA_REQUEST_HEADER_SIZE     10

#endif
