/**
* @file af_zdo.c
*
* @brief Methods that implement the CC2530ZNP Application Framework / Zigbee Device Objects (AF/ZDO) interface.
* 
* The AF/ZDO interface is a more powerful version of the Simple API and allows you to configure, send, and receive Zigbee data.
* This file acts as an interface between the user's application and the ZNP physical interface.
* RFIC interface could be either SPI or UART.
* Refer to Interface Specification for more information.
*
* @note For more information, define AF_ZDO_VERBOSE. It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* AF_ZDO_VERBOSE
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 601 $
* $Author: dsmith $
* $Date: 2010-06-16 13:09:21 -0700 (Wed, 16 Jun 2010) $
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

#include "af_zdo.h"
#include "znp_interface.h"
#include "znp_hal.h"
#include <string.h>                 //for memcpy()
#include "application_configuration.h"

#include "znp_interface_spi.h"

extern unsigned char znpBuf[100];

/** Incremented for each AF_DATA_REQUEST, wraps around to 0. */
unsigned char transactionSequenceNumber = 0;

extern signed int znpResult;                    //from znp_interface

/** Configures the ZNP for our application.
Sets which profileId, etc. we're using as well as binding information FOR EACH ENDPOINT
Using the AF/ZDO interface, you can have more than one endpoint. Recommended not to exceed 10 endpoints.
@param ac the applicationConfiguration to register.
@pre znp was initialized and ZCD_NV_LOGICAL_TYPE has been set (COORDINATOR/ROUTER/END_DEVICE).
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@post can now use afStartApplication()
@note this method does not check whether the endpoint being registered has already been registered - it is up to the application to manage that.
@see applicationConfiguration
*/
void afRegisterApplication(struct applicationConfiguration ac)
{
#ifdef AF_ZDO_VERBOSE
    printf("Register Application Configuration with AF/ZDO\r\n");
#endif
    if (ac.endPoint == 0)
    {
        znpResult = -1;
        return;
    }
    if ((ac.numberOfBindingInputClusters > MAX_BINDING_CLUSTERS) || (ac.numberOfBindingOutputClusters > MAX_BINDING_CLUSTERS))
    {
        znpResult = -2;
        return;
    }
    
    //printApplicationConfiguration(ac);
    
    //znpBuf[0] (length) will be set later
    znpBuf[1] = MSB(AF_REGISTER);
    znpBuf[2] = LSB(AF_REGISTER);  
    
    unsigned char bufferIndex;
    znpBuf[3] = ac.endPoint;
    znpBuf[4] = ac.profileId & 0xFF;      //LSB
    znpBuf[5] = ac.profileId >> 8;        //MSB
    znpBuf[6] = ac.deviceId & 0xFF;       //LSB
    znpBuf[7] = ac.deviceId >> 8;         //MSB
    znpBuf[8] = ac.deviceVersion;
    znpBuf[9] = ac.latencyRequested;
    znpBuf[10] = ac.numberOfBindingInputClusters;
    
    bufferIndex = 11;
    for (int i = 0; i<ac.numberOfBindingInputClusters; i++)
    {
        znpBuf[bufferIndex++] = ac.bindingInputClusters[i] & 0xFF;
        znpBuf[bufferIndex++] = ac.bindingInputClusters[i] >> 8;
    }
    znpBuf[bufferIndex++] = ac.numberOfBindingOutputClusters;
    for (int i = 0; i<ac.numberOfBindingOutputClusters; i++)
    {
        znpBuf[bufferIndex++] = ac.bindingOutputClusters[i] & 0xFF;
        znpBuf[bufferIndex++] = ac.bindingOutputClusters[i] >> 8;
    }
        znpBuf[0] = bufferIndex;
        znpResult = sendMessage();   
}

#define AF_REGISTER_PAYLOAD_LEN 9
/** Configures the ZNP for a "generic" application: one endpoint, no binding or fancy stuff. 
Sets which endpoint, profileId, etc. we're using as well as binding information
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@see afRegisterApplication()
*/
void afRegisterGenericApplication()
{
    znpBuf[0] = AF_REGISTER_PAYLOAD_LEN;
    znpBuf[1] = MSB(AF_REGISTER);
    znpBuf[2] = LSB(AF_REGISTER);      
    
    znpBuf[3] = DEFAULT_ENDPOINT;
    znpBuf[4] = LSB(DEFAULT_PROFILE_ID);
    znpBuf[5] = MSB(DEFAULT_PROFILE_ID);
    znpBuf[6] = LSB(ZNP_DEVICE_ID);
    znpBuf[7] = MSB(ZNP_DEVICE_ID);
    znpBuf[8] = DEVICE_VERSION;
    znpBuf[9] = LATENCY_NORMAL;
    znpBuf[10] = 0; //number of binding input clusters
    znpBuf[11] = 0; //number of binding output clusters
    znpResult = sendMessage();
}

#define ZDO_STARTUP_FROM_APP_PAYLOAD_LEN 1
#define NO_START_DELAY 0
/** Starts the Zigbee stack in the ZNP using the settings from a previous afRegisterApplication().
After this start request process completes, the device is ready to send, receive, and route network traffic.
@note On a coordinator in a trivial test setup, it takes approximately 300mSec between sending 
START_REQUEST and receiving START_REQUEST_SRSP and then another 200-1000mSec from when we receive 
START_REQUEST_SRSP to when we receive START_CONFIRM. Set START_CONFIRM_TIMEOUT based on size of your network.
@note ZDO_STARTUP_FROM_APP field StartDelay not used
@pre afRegisterApplication() was a success.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@post We will see Device Status change to DEV_ROUTER, DEV_ZB_COORD, or DEV_END_DEVICE correspondingly if everything was ok.
*/
void zdoStartApplication()
{
#ifdef AF_ZDO_VERBOSE    
    printf("Start Application with AF/ZDO...");
#endif    
    znpBuf[0] = ZDO_STARTUP_FROM_APP_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZDO_STARTUP_FROM_APP);
    znpBuf[2] = LSB(ZDO_STARTUP_FROM_APP);      
    
    znpBuf[3] = NO_START_DELAY;
    znpResult = sendMessage();
}

#define AF_DATA_CONFIRM_TIMEOUT 2
#define AF_DATA_REQUEST_PAYLOAD_LEN 10
/** Sends a message to another device over the Zigbee network using the AF command AF_DATA_REQUEST.
@param  destinationEndpoint which endpoint to send this to.
@param  sourceEndpoint which endpoint this message originated from on our device
@param  destinationShortAddress the short address of the destination, or ALL_DEVICES or ALL_ROUTERS_AND_COORDINATORS to broadcast the message.
@param  clusterId which cluster this message is for. User definable. Zigbee supports up to 2^16 clusters. 
A cluster is typically a particular command, e.g. "turn on lights" or "get temperature". 
If using a predefined Zigbee Alliance Application Profile then this cluster will follow the Zigbee Cluster Library.
@param  data is the data to send.
@param  dataLength is how many bytes of data to send. Must be less than MAXIMUM_PAYLOAD_LENGTH.
@note On a coordinator in a trivial test setup, it takes approximately 10mSec from sending 
AF_DATA_REQUEST to when we receive AF_DATA_CONFIRM.
@note   <code>transactionSequenceNumber</code> is an optional user-definable reference number to match AF_DATA_REQUEST messages with AF_DATA_CONFIRM messages. 
@note   The ZNP will automatically require an ACK from the next device on the route when sending data. 
To require an ACK from the final destination, change MAC_ACK to APS_ACK at the expense of increased network traffic.
@note   The <code>radius</code> is the maximum number of hops that this packet can travel through before it will be dropped.
Should be set to the maximum number of hops expected in the network.
@note   adjust AF_DATA_CONFIRM_INTERVAL_MS and AF_DATA_CONFIRM_TIMEOUT_MS based on network size, number of hops, etc.
@pre    the application was started successfully
@pre    there is another device on the network with short address of <code>destinationShortAddress</code> 
and that device has successfully started its application.
@post   srsp will contain a AF_DATA_REQUEST_SRSP. We will receive a AF_DATA_REQUEST_SRSP regardless of whether the message was successfully sent or not.
@post   we will receive a AF_DATA_CONFIRM if the message was successfully sent. 
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void afSendData(unsigned char destinationEndpoint, unsigned char sourceEndpoint, unsigned int destinationShortAddress, unsigned int clusterId, unsigned char* data, unsigned char dataLength)
{
    if (dataLength > MAXIMUM_PAYLOAD_LENGTH)
    {
        znpResult = -1;
        return;
    }
#ifdef AF_ZDO_VERBOSE     
    printf("Sending %u bytes to endpoint %u from endpoint %u with cluster %u (%04X) at Short Address %u (0x%04X)\r\n", 
           dataLength, destinationEndpoint, sourceEndpoint, clusterId, clusterId, destinationShortAddress, destinationShortAddress);
#endif  
    
    znpBuf[0] = AF_DATA_REQUEST_PAYLOAD_LEN + dataLength;
    znpBuf[1] = MSB(AF_DATA_REQUEST);
    znpBuf[2] = LSB(AF_DATA_REQUEST);      
    
    znpBuf[3] = LSB(destinationShortAddress); 
    znpBuf[4] = MSB(destinationShortAddress);
    znpBuf[5] = destinationEndpoint;
    znpBuf[6] = sourceEndpoint;
    znpBuf[7] = LSB(clusterId); 
    znpBuf[8] = MSB(clusterId); 
    znpBuf[9] = transactionSequenceNumber++;
    znpBuf[10] = MAC_ACK; //Could also use AF_APS_ACK;
    znpBuf[11] = DEFAULT_RADIUS;
    znpBuf[12] = dataLength; 
    
    memcpy(znpBuf+AF_DATA_REQUEST_PAYLOAD_LEN+3, data, dataLength);
    znpResult = sendMessage();
    if (znpResult != 0)
        return;
    
    //Now, wait for message, and verify that it's a AF_DATA_CONFIRM, else timeout.
    //NOTE: Do not print anything out here, or else you might miss the AF_DATA_CONFIRM!
    znpResult = waitForMessage(AF_DATA_CONFIRM, AF_DATA_CONFIRM_TIMEOUT);
    if (znpResult != 0)
        return;
    //note: znpBuf[4] = endpoint, znpBuf[5] = transaction sequence number
    if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
    {
        znpResult = -2;
        return;
    }
}

#define ZDO_IEEE_ADDR_REQ_PAYLOAD_LEN 4
/** Requests a device's MAC Address (64-bit IEEE Address) given a short address.
@param shortAddress the short address to locate
@param requestType must be SINGLE_DEVICE_RESPONSE or INCLUDE_ASSOCIATED_DEVICES. 
If SINGLE_DEVICE_RESPONSE is selected, then only information about the requested device will be returned. 
If INCLUDE_ASSOCIATED_DEVICES is selected, then the short addresses of the selected device's children will be returned too.
@param startIndex If INCLUDE_ASSOCIATED_DEVICES was selected, then there may be too many children to 
fit in one ZDO_IEEE_ADDR_RSP message. So, use startIndex to get the next set of children's short addresses.
@post An ZDO_IEEE_ADDR_RSP message will be received, with one or more entries.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void zdoRequestIeeeAddress(unsigned int shortAddress, unsigned char requestType, unsigned char startIndex)
{
    if ((requestType != SINGLE_DEVICE_RESPONSE) && (requestType != INCLUDE_ASSOCIATED_DEVICES))
    {
        znpResult = -1;
        return;
    }
#ifdef AF_ZDO_VERBOSE     
    printf("Requesting IEEE Address for short address %04X, requestType %s, startIndex %u\r\n", 
           shortAddress, (requestType == 0) ? "Single" : "Extended", startIndex);
#endif 
    
    znpBuf[0] = ZDO_IEEE_ADDR_REQ_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZDO_IEEE_ADDR_REQ);
    znpBuf[2] = LSB(ZDO_IEEE_ADDR_REQ);      
    
    znpBuf[3] = LSB(shortAddress);
    znpBuf[4] = MSB(shortAddress);
    znpBuf[5] = requestType;
    znpBuf[6] = startIndex;
    
    znpResult = sendMessage();
    if (znpResult != 0)     
        return;
    
    znpResult = waitForMessage(ZDO_IEEE_ADDR_RSP, 5);
    if (znpResult != 0)
        return;
    if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
    {
        znpResult = -2;
        return;
    }    
}

#define ZDO_NWK_ADDR_REQ_PAYLOAD_LEN 10
/** Requests a device's Short Address for a given long address.
@param ieeeAddress the long address to locate
@param requestType must be SINGLE_DEVICE_RESPONSE or INCLUDE_ASSOCIATED_DEVICES. 
If SINGLE_DEVICE_RESPONSE is selected, then only information about the requested device will be returned. 
If INCLUDE_ASSOCIATED_DEVICES is selected, then the short addresses of the selected device's children will be returned too.
@param startIndex If INCLUDE_ASSOCIATED_DEVICES was selected, then there may be 
too many children to fit in one ZDO_NWK_ADDR_RSP message. 
So, use startIndex to get the next set of children's short addresses.
@note DOES NOT WORK FOR SLEEPING END DEVICES
@post An ZDO_NWK_ADDR_RSP message will be received, with one or more entries.
@return a pointer to the beginning of the payload, or a pointer to indeterminate data if error.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
unsigned char* zdoNetworkAddressRequest(unsigned char* ieeeAddress, unsigned char requestType, unsigned char startIndex)
{
    if ((requestType != SINGLE_DEVICE_RESPONSE) && (requestType != INCLUDE_ASSOCIATED_DEVICES)) 
    {
        znpResult = -1;
        return 0;
    }
#ifdef AF_ZDO_VERBOSE     
    printf("Requesting Network Address for long address ");
    printHexBytes(ieeeAddress, 8);
    printf("requestType %s, startIndex %u\r\n", (requestType == 0) ? "Single" : "Extended", startIndex);
#endif
    
    znpBuf[0] = ZDO_NWK_ADDR_REQ_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZDO_NWK_ADDR_REQ);
    znpBuf[2] = LSB(ZDO_NWK_ADDR_REQ);      

    memcpy(znpBuf+3, ieeeAddress, 8);
    znpBuf[11] = requestType;
    znpBuf[12] = startIndex;

    znpResult = sendMessage();

    if (znpResult != 0)     
        return 0;
    znpResult = waitForMessage(ZDO_NWK_ADDR_RSP, 5);
    if (znpResult != 0)
        return 0;
    if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
    {
        znpResult = -2;
        return 0;
    }
    return (znpBuf + SRSP_PAYLOAD_START);
}

/** Displays the header information in an AF_INCOMING_MSG.
@param srsp a pointer to the buffer containing the message
@return 0 if success, -1 if not a AF_INCOMING_MSG.
*/
signed int printAfIncomingMsgHeader(unsigned char* srsp)
{
    if (CONVERT_TO_INT(srsp[SRSP_CMD_LSB_FIELD], srsp[SRSP_CMD_MSB_FIELD]) == AF_INCOMING_MSG)
    {
        printf("#%02u: Grp%04x Clus%04x, SrcAd%04x, SrcEnd%02x DestEnd%02x Bc%02x Lqi%02x Sec%02x Len%02u", 
               srsp[SRSP_HEADER_SIZE+15],
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE], srsp[SRSP_HEADER_SIZE+1]), 
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE+2], srsp[SRSP_HEADER_SIZE+3]),
               CONVERT_TO_INT(srsp[SRSP_HEADER_SIZE+4], srsp[SRSP_HEADER_SIZE+5]), 
               srsp[SRSP_HEADER_SIZE+6], srsp[SRSP_HEADER_SIZE+7], srsp[SRSP_HEADER_SIZE+8], 
               srsp[SRSP_HEADER_SIZE+9], srsp[SRSP_HEADER_SIZE+10], srsp[SRSP_HEADER_SIZE+16]);
        return 0;
    } else {
        return -1;
    }
}

/** Displays the names that go with the above*/
void printAfIncomingMsgHeaderNames()
{
        printf("Fields are: Group, Cluster, Source Address, Source Endpoint, \
Destination Endpoint, wasBroadcast, Link Quality Indicator, SecurityUse, Payload Length\r\n");    
}

/** Prints out the returned value of zdoNetworkAddressRequest() */
void printZdoNetworkAddressResponse(unsigned char* rsp)
{
    printf("Status = %u; MAC:", rsp[0]);
    for (int i=1; i<9; i++)
        printf("%02X ", rsp[i]);
    printf("NwkAddr:%04X; StartIdx:%u, numAssocDev:%u\r\n", CONVERT_TO_INT(rsp[9], rsp[10]), rsp[11],rsp[12]);
    for (int j=0; j<rsp[12]; j++)
        printf("%04X ", CONVERT_TO_INT(rsp[(j+13)], rsp[(j+14)]));
    printf("\r\n");
}
