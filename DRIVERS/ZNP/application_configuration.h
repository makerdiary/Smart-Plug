/**
* @file application_configuration.h
* 
*  @brief  public methods for application_configuration.c
*
* $Rev: 547 $
* $Author: dsmith $
* $Date: 2010-05-27 17:11:30 -0700 (Thu, 27 May 2010) $
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

#ifndef APPLICATION_CONFIGURATION_H
#define APPLICATION_CONFIGURATION_H

//default values used when creating applicationConfigurations in Simple API or AFZDO
#define DEFAULT_ENDPOINT        0xD7 
#define DEFAULT_PROFILE_ID      0xD7D7
#define ZNP_DEVICE_ID           0x4567
#define DEVICE_VERSION          0x89

//Values for latencyRequested field of struct applicationConfiguration. Not used in Simple API.
#define LATENCY_NORMAL          0
#define LATENCY_FAST_BEACONS    1
#define LATENCY_SLOW_BEACONS    2


/** This is a set of application configuration parameters. These parameters affect the way our particular Zigbee Application will behave. */
struct applicationConfiguration
{
  /** The Zigbee Endpoint. Simple API only supports one endpoint. Must use the same value of endpoint for all devices in the network.*/
  unsigned char endPoint;
  
  /** The Application Profile ID as assigned by the Zigbee Association. Must use the same application profile for all devices in the network. */
  unsigned int profileId;
  
  /** A user-defined device ID field. Not used in Simple API, but when using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command. */
  unsigned int deviceId;
  
  /** A user-defined device ID field. Not used in Simple API, but when using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command. */ 
  unsigned char deviceVersion;
    
  /** A very simple Quality of Service (QoS) setting. Must be LATENCY_NORMAL, LATENCY_FAST_BEACONS, or LATENCY_SLOW_BEACONS. Not used in Simple API.*/
  unsigned char latencyRequested;
  
#define MAX_BINDING_CLUSTERS 32  /**< Maximum number of clusters available for binding */
  
  /** Number of Input Clusters for Binding. If not using binding then set to zero.*/
  unsigned char numberOfBindingInputClusters;

  /** List of Input Clusters for Binding. If not using binding then this does not apply. To allow another device to bind to this device, must use ZB_ALLOW_BIND on this device and must use ZB_BIND_DEVICE on the other device. */  
  unsigned int bindingInputClusters[MAX_BINDING_CLUSTERS];
  
  /** Number of Output Clusters for Binding. If not using binding then set to zero.*/  
  unsigned char numberOfBindingOutputClusters;

  /** List of Output Clusters for Binding. If not using binding then this does not apply. To bind to another device, that device must use ZB_ALLOW_BIND and this device must use ZB_BIND_DEVICE to create a binding. */    
  unsigned int bindingOutputClusters[MAX_BINDING_CLUSTERS];
};


struct applicationConfiguration getApplicationConfigurationForCoordinator();
struct applicationConfiguration getApplicationConfigurationForRouter();
void printApplicationConfiguration(struct applicationConfiguration ac);

#endif