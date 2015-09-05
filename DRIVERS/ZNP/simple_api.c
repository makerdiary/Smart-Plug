/**
* @file simple_api.c
*
* @brief Methods that implement the CC2530ZNP Simple Application Programming Interface (Simple API).
* 
* This Simple API is an easy-to-use interface to configure, send, and receive Zigbee data.
* This file acts as an interface between the user's application and the ZNP physical interface, either SPI or UART.
* Refer to Interface Specification for more information.
*
* @note For more information, define SIMPLE_API_VERBOSE. It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* SIMPLE_API_VERBOSE
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 589 $
* $Author: dsmith $
* $Date: 2010-06-11 15:32:36 -0700 (Fri, 11 Jun 2010) $
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

#include "simple_api.h"
#include "znp_interface.h"
#include "application_configuration.h"
#include "znp_hal.h"
#include <string.h> //for memcpy()

#include "znp_interface_spi.h"

extern unsigned char znpBuf[100];

/** Incremented for each SEND_DATA_REQUEST, wraps around to 0. */
unsigned char sequenceNumber = 0;

extern signed int znpResult;                    //from znp_interface

/** Configures the ZNP for our application.
Sets which endpoint, profileId, etc. we're using as well as binding information.
@note Simple API can only register one application.
@param the applicationConfiguration to register.
@pre ZNP was initialized and ZCD_NV_LOGICAL_TYPE has been set (either COORDINATOR/ROUTER/END_DEVICE).
@post can now use sapiStartApplication()
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@see applicationConfiguration
*/
void sapiRegisterApplication(struct applicationConfiguration ac)
{
#ifdef SIMPLE_API_VERBOSE
    printf("Register Application Configuration with Simple API\r\n");
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
    unsigned char bufferIndex;
    
    //znpBuf[0] (length) will be set later
    znpBuf[1] = MSB(ZB_APP_REGISTER_REQUEST);
    znpBuf[2] = LSB(ZB_APP_REGISTER_REQUEST); 
    
    znpBuf[3] = ac.endPoint;
    znpBuf[4] = LSB(ac.profileId);
    znpBuf[5] = MSB(ac.profileId);
    znpBuf[6] = LSB(ac.deviceId);
    znpBuf[7] = MSB(ac.deviceId);
    znpBuf[8] = ac.deviceVersion;
    znpBuf[9] = 0x00;                     //unused, set to zero
    znpBuf[10] = ac.numberOfBindingInputClusters;
    bufferIndex = 11;
    for (int i = 0; i<ac.numberOfBindingInputClusters; i++)
    {
        znpBuf[bufferIndex++] = LSB(ac.bindingInputClusters[i]);
        znpBuf[bufferIndex++] = MSB(ac.bindingInputClusters[i]);
    }
    znpBuf[bufferIndex++] = ac.numberOfBindingOutputClusters;
    for (int i = 0; i<ac.numberOfBindingOutputClusters; i++)
    {
        znpBuf[bufferIndex++] = LSB(ac.bindingOutputClusters[i]);
        znpBuf[bufferIndex++] = MSB(ac.bindingOutputClusters[i]);
    }
    znpBuf[0] = bufferIndex;
    znpResult = sendMessage(); 
}

#define ZB_APP_REGISTER_REQUEST_PAYLOAD_LEN 9
/** Configures the ZNP for a "generic" application: one endpoint, no binding or fancy stuff. 
Sets which endpoint, profileId, etc. we're using as well as binding information
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@see sapiRegisterApplication()
*/
void sapiRegisterGenericApplication()
{
    znpBuf[0] = ZB_APP_REGISTER_REQUEST_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZB_APP_REGISTER_REQUEST);
    znpBuf[2] = LSB(ZB_APP_REGISTER_REQUEST); 
    
    znpBuf[3] = DEFAULT_ENDPOINT;
    znpBuf[4] = LSB(DEFAULT_PROFILE_ID);
    znpBuf[5] = MSB(DEFAULT_PROFILE_ID);
    znpBuf[6] = LSB(ZNP_DEVICE_ID);
    znpBuf[7] = MSB(ZNP_DEVICE_ID);
    znpBuf[8] = DEVICE_VERSION;
    znpBuf[9] = 0; //unused
    znpBuf[10] = 0; //number of binding input clusters
    znpBuf[11] = 0; //number of binding output clusters
    znpResult = sendMessage();
}

#define ZB_START_CONFIRM_TIMEOUT 5
#define ZB_APP_START_REQUEST_PAYLOAD_LEN 0
/** Starts the Zigbee stack in the ZNP using the settings from a previous sapiRegisterApplication().
After this start request process completes, the device is ready to send, receive, and route network traffic.
@note On a coordinator in a trivial test setup, it takes approximately 300mSec between 
START_REQUEST and receiving START_REQUEST_SRSP and then another 200-1000mSec from when we receive 
START_REQUEST_SRSP to when we receive START_CONFIRM. 
@note Set START_CONFIRM_TIMEOUT based on size of your network.
@pre sapiRegisterApplication() was a success.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@note we will receive a START_CONFIRM if everything was ok. 
Should always receive a start confirm for a coordinator. 
Will only receive a start confirm on a router if we could join a zigbee network.
*/
void sapiStartApplication()
{
#ifdef SIMPLE_API_VERBOSE    
    printf("Start Application with Simple API...");
#endif    
    znpBuf[0] = ZB_APP_START_REQUEST_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZB_APP_START_REQUEST);
    znpBuf[2] = LSB(ZB_APP_START_REQUEST);     
    
    znpResult = sendMessage();  
    if (znpResult != 0) 
        return;
    
    //Now, wait for message, and verify that it's a ZB_START_CONFIRM, else timeout.
    //NOTE: Do not print anything out here, or else you might miss the ZB_START_CONFIRM!
    znpResult = -1;    
    while (znpResult != 0)
        znpResult = waitForMessage(ZB_START_CONFIRM, ZB_START_CONFIRM_TIMEOUT);
    
    if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
    {
        znpResult = -2;
        return;
    }    
}

#define ZB_PERMIT_JOINING_REQUEST_PAYLOAD_LEN 3
/** Sets the Joining Permissions for this device or other devices. 
By default, after a setStartupOptions(CLEAR_CONFIG), joining is set to allow all devices, indefinitely.
@param destination which short address to set joining permissions for, or ALL_ROUTERS_AND_COORDINATORS 
to set joining permissions for all. 
@param timeout how long in seconds to allow permissions, or if set to NO_TIMEOUT then joining will be on indefinitely.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setJoiningPermissions(unsigned int destination, unsigned char timeout)
{
#ifdef SIMPLE_API_VERBOSE 
    printf("Allowing Joining for destination ");
    if (destination == ALL_ROUTERS_AND_COORDINATORS) 
        printf("ALL_ROUTERS_AND_COORDINATORS");
    else
        printf("0x%04X", destination);
    if (timeout == NO_TIMEOUT) 
        printf(" forever\r\n");
    else
        printf(" for %u seconds\r\n", timeout);
#endif
    
    znpBuf[0] = ZB_PERMIT_JOINING_REQUEST_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZB_PERMIT_JOINING_REQUEST);
    znpBuf[2] = LSB(ZB_PERMIT_JOINING_REQUEST);   
    
    znpBuf[3] = LSB(destination);
    znpBuf[4] = MSB(destination);
    znpBuf[5] = timeout;
    znpResult = sendMessage();   
}

#define ZB_SEND_DATA_CONFIRM_TIMEOUT    5
#define ZB_SEND_DATA_REQUEST_PAYLOAD_LEN 8
/** Sends a message to another device over the Zigbee network.
@note On a coordinator in a trivial test setup, it takes approximately 10mSec from sending 
ZB_SEND_DATA_REQUEST to when we receive ZB_SEND_DATA_CONFIRM.
@note   <code>handle</code> is an optional user-definable reference number to match SEND_DATA_CONFIRM messages 
with the SEND_DATA_REQUEST message. This can be used to determine which SEND_DATA_REQUEST generated an error.
@note   The ZNP will automatically require an ACK from the next device on the route when sending data. 
To require an ACK from the final destination, change MAC_ACK to APS_ACK. The downside is that this increases network traffic.
@note   The <code>radius</code> is the maximum number of hops that this packet can travel through before it will be dropped. 
Should be set to the maximum number of hops expected in the network.
@note   adjust SEND_DATA_CONFIRM_INTERVAL_MS and SEND_DATA_CONFIRM_TIMEOUT_MS based on network size, number of hops, etc.
@param  destinationShortAddress the short address of the destination, or ALL_DEVICES or ALL_ROUTERS_AND_COORDINATORS to broadcast the message.
@param  clusterId which cluster this message is for. User definable. Zigbee supports up to 2^16 clusters. 
A cluster is typically a particular command, e.g. "turn on lights" or "get temperature". 
If using a predefined Zigbee Alliance Application Profile then this cluster will follow the Zigbee Cluster Library.
@param  data is the data to send.
@param  dataLength is how many bytes of data to send. Must be less than MAXIMUM_PAYLOAD_LENGTH.
@pre    sapiStartApplication() was a success, START_CONFIRM received without any errors.
@pre    there is another device on the network with short address of <code>destinationShortAddress</code> 
and that device has successfully started its application using sapiStartApplication().
@post   srsp will contain a ZB_SEND_DATA_REQUEST_SRSP. We will receive a ZB_SEND_DATA_REQUEST_SRSP regardless of whether the message was successfully sent or not.
@post   we will receive a ZB_SEND_DATA_CONFIRM if the message was successfully sent. 
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void sendData(unsigned int destinationShortAddress, unsigned int clusterId, unsigned char* data, unsigned char dataLength)   
{
#ifdef SIMPLE_API_VERBOSE     
    printf("Sending %u bytes to cluster %u (%04X) at Short Address %u (0x%04X)\r\n", dataLength, clusterId, clusterId, destinationShortAddress, destinationShortAddress);
#endif    
    if (dataLength > MAXIMUM_PAYLOAD_LENGTH)
    {
        znpResult = -1;
        return;
    }
    
    znpBuf[0] = ZB_SEND_DATA_REQUEST_PAYLOAD_LEN + dataLength;
    znpBuf[1] = MSB(ZB_SEND_DATA_REQUEST);
    znpBuf[2] = LSB(ZB_SEND_DATA_REQUEST);  
    
    znpBuf[3] = LSB(destinationShortAddress); 
    znpBuf[4] = MSB(destinationShortAddress); 
    znpBuf[5] = LSB(clusterId); 
    znpBuf[6] = MSB(clusterId); 
    znpBuf[7] = sequenceNumber++;  //handle aka Transaction Sequence Number
    znpBuf[8] = MAC_ACK;  
    znpBuf[9] = DEFAULT_RADIUS;
    znpBuf[10] = dataLength;  
    memcpy(znpBuf+ZB_SEND_DATA_REQUEST_PAYLOAD_LEN+3, data, dataLength);
    
    znpResult = sendMessage();
    if (znpResult != 0)
        return;    
    
    //Now, wait for message, and verify that it's a ZB_SEND_DATA_CONFIRM, else timeout.
    //NOTE: Do not print anything out here, or else you might miss the ZB_SEND_DATA_CONFIRM!
    
    znpResult = waitForMessage(ZB_SEND_DATA_CONFIRM, ZB_SEND_DATA_CONFIRM_TIMEOUT);
    if (znpResult != 0)
        return;

    //NOTE: znpBuf[3] contains the transaction sequence number; we ignore
    if (znpBuf[SRSP_PAYLOAD_START+1] != SRSP_STATUS_SUCCESS) //verify status is succes
        znpResult = -3;
}


