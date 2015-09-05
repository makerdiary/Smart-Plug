/**
* @file application_configuration.c
*
* @brief Contains Zigbee Application Configuration methods. 
*
* This is used by both Simple API and AFZDO to configure the ZNP for the particular application.
* Simple API can register one applicationConfiguration; AFZDO can register multiple applicationConfigurations.
* Refer to Interface Specification for more information.
* 
* @see sapiRegisterApplication()
* @see afRegisterApplication()
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
#include "znp_hal.h"
#include "application_configuration.h"  


/** Creates an application configuration for a Coordinator.
Modify the values in this for your application.
@return the applicationConfiguration for a router
@see applicationConfiguration */
struct applicationConfiguration getApplicationConfigurationForCoordinator()
{
    printf("Getting Application Configuration For Coordinator\r\n");       
    struct applicationConfiguration ac;
    ac.endPoint =                       DEFAULT_ENDPOINT;       //must be same for all devices on the network
    ac.profileId =                      DEFAULT_PROFILE_ID;     //must be same for all devices on the network
    ac.deviceId =                       ZNP_DEVICE_ID;              //doesn't matter
    ac.deviceVersion =                  DEVICE_VERSION;         //doesn't matter
    ac.latencyRequested =               LATENCY_NORMAL;
    ac.numberOfBindingInputClusters =   1; 
    ac.bindingInputClusters[0] =        0x0001;    
    ac.numberOfBindingOutputClusters =  0;   
    return ac;
}

/** Creates an application configuration for a Router. 
Modify the values in this for your application.
@return the applicationConfiguration for a router
@see applicationConfiguration */
struct applicationConfiguration getApplicationConfigurationForRouter()
{
    printf("Getting Application Configuration For Router\r\n");    
    struct applicationConfiguration ac;
    ac.endPoint =                       DEFAULT_ENDPOINT;       //must be same for all devices on the network
    ac.profileId =                      DEFAULT_PROFILE_ID;     //must be same for all devices on the network
    ac.deviceId =                       ZNP_DEVICE_ID;              //doesn't matter
    ac.deviceVersion =                  DEVICE_VERSION;         //doesn't matter
    ac.latencyRequested =               LATENCY_NORMAL;
    ac.numberOfBindingInputClusters =   0;   
    ac.numberOfBindingOutputClusters =  1; //1 
    ac.bindingOutputClusters[0] =       0x0001;
    return ac;
}

/** Displays the applicationConfiguration in a human readable format
@param ac the applicationConfiguration to print
@see applicationConfiguration */
void printApplicationConfiguration(struct applicationConfiguration ac)
{
    printf("Application Configuration:\r\n");    
    printf("    endpoint = 0x%02X\r\n", ac.endPoint);
    printf("    profileId = 0x%04X\r\n", ac.profileId);
    printf("    deviceId = 0x%04X\r\n", ac.deviceId);
    printf("    deviceVersion = 0x%04X\r\n", ac.deviceVersion);
    printf("    latencyRequested = "); 
    switch (ac.latencyRequested)
    {
    case LATENCY_NORMAL: printf("NORMAL"); break;
    case LATENCY_FAST_BEACONS: printf("FAST_BEACONS"); break;
    case LATENCY_SLOW_BEACONS: printf("SLOW_BEACONS"); break;
    }
    printf("\r\n");
    printf("    Number of Binding Input Clusters = %u ", ac.numberOfBindingInputClusters);
    if (ac.numberOfBindingInputClusters > 0)
    {
        printf("{");
        for (int i=0; i< ac.numberOfBindingInputClusters; i++)
            printf("0x%02X ", ac.bindingInputClusters[i]);
        printf("}");
    }
    printf("\r\n");
    printf("    Number of Binding Output Clusters = %u ", ac.numberOfBindingOutputClusters);
    if (ac.numberOfBindingOutputClusters > 0)
    {
        printf("{");
        for (int i=0; i< ac.numberOfBindingOutputClusters; i++)
            printf("0x%02X ", ac.bindingOutputClusters[i]);
        printf("}");
    }
        printf("\r\n");
}
