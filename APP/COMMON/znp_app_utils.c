/**
* @file znp_app_utils.c
*
* @brief Utilities for ZNP application.
*
*
* $Rev: 1.0 $
* $Author: czl $
* $Date: 2012-11-04 $
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
#include "znp_interface.h"
#include "dprint_macros.h"
#include "string_functions.h"
#include "af_zdo.h"              //for pollingAndFindDevice()
#include "znp_commands.h"        //for pollingAndFindDevice()
#include "znp_app_utils.h"

#include "znp_interface_spi.h"

extern unsigned char znpBuf[100];

extern signed int znpResult;                  

//Change this to use a different channel; must be 11-26, inclusive, or 0 to automatically select the channel. 
//NOTE: All devices in the network must have the same channel
#define DEFAULT_CHANNEL 11

/** Poll the ZNP for any messages and display them to the console.
@pre SRDY went LOW indicating data is ready
*/
void pollAndDisplay()
{
    spiPoll();
    if (znpBuf[SRSP_LENGTH_FIELD] > 0)
    {
        printf("Rx: ");
        printHexBytes(znpBuf, (znpBuf[SRSP_LENGTH_FIELD] + SRSP_HEADER_SIZE));
        znpBuf[SRSP_LENGTH_FIELD] = 0;
    } 
}

#define WAIT_FOR_DEVICE_STATE_TIMEOUT 4000000l
/** Wait until a message is received. Exits if received message is a ZDO_STATE_CHANGE_IND
and the state matches what we want. Else loops. 
@param expectedState the deviceState we are expecting - DEV_ZB_COORD etc.
@return 0 if success, -1 if timeout
*/
signed int waitForDeviceState(unsigned char expectedState)
{
    printf("Waiting for network... ");
    unsigned char state = 0xFF;
    unsigned long timeout = WAIT_FOR_DEVICE_STATE_TIMEOUT;
    while (state != expectedState)
    {
        while ((SRDY_IS_HIGH()) && (timeout > 0))
            timeout--;
        if (timeout == 0)
            return -1;  //error
        
        spiPoll();
        if (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) == ZDO_STATE_CHANGE_IND)
        {
            state = znpBuf[SRSP_PAYLOAD_START];
            printf("%s ", getDeviceStateName(state));   
        }
    }
    printf("\r\n");
    return 0;
}

/** Attempts to start the ZNP.
@param the type of device we're starting, e.g. ROUTER
@post znpResult contains the ZNP library error code, if any.
@return ZNP_SUCCESS if successful, else an error code indicating where it failed.
@see Communications Examples for more information about each of these steps.
*/
signed int startZnp(unsigned char deviceType)
{
    printf("ZNP Startup ");
    
    znpInit(); 
    if (znpResult != ZNP_SUCCESS) 
        return -1; 
    
    setStartupOptions(STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE);
    if (znpResult != ZNP_SUCCESS) 
        return -2; 
    
    znpReset();
    if (znpResult != ZNP_SUCCESS) 
        return -3; 
    
    setZigbeeDeviceType(deviceType);
    if (znpResult != ZNP_SUCCESS) 
        return -4; 
    
    setChannel(DEFAULT_CHANNEL);
    if (znpResult != ZNP_SUCCESS) 
        return -5; 
    
    setCallbacks(CALLBACKS_ENABLED);
    if (znpResult != ZNP_SUCCESS) 
        return -6; 
    
    afRegisterGenericApplication();
    if (znpResult != ZNP_SUCCESS) 
        return -8; 
    
    zdoStartApplication();
    if (znpResult != ZNP_SUCCESS) 
        return -9; 
    
    /* Wait until this device has joined a network.
    Device State will change to DEV_ROUTER or DEV_COORD to indicate that the device has correctly joined a network. */
    unsigned char deviceState = 0;
    switch (deviceType)
    {
    case ROUTER: deviceState = DEV_ROUTER; break;
    case END_DEVICE: deviceState = DEV_END_DEVICE; break;
    case COORDINATOR: deviceState = DEV_ZB_COORD; break;
    default: printf("ERROR - UNKNOWN DEVICE TYPE\r\n"); return -10;
    }
    
    znpResult = waitForDeviceState(deviceState);
    if (znpResult != ZNP_SUCCESS) 
        return -10; 
    
    return ZNP_SUCCESS;
}