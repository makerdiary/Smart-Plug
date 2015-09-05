/**
*  @file znp_interface.h
*
*  @brief  public methods for znp_interface.c
*
* $Rev: 619 $
* $Author: dsmith $
* $Date: 2010-07-02 10:54:13 -0700 (Fri, 02 Jul 2010) $
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
#ifndef ZNP_INTERFACE_H
#define ZNP_INTERFACE_H

#include "znp_commands.h"

//Miscellaneous
unsigned char* znpInit();
unsigned char* znpReset();
//void znpSoftReset();
char* getResetReason(unsigned char reason);
unsigned char* getVersion();
unsigned int getRandom();
void printZnpBuf();

//signed int waitForMessage(unsigned int messageType, unsigned int timeoutMs, unsigned int intervalMs);
signed int waitForMessage(unsigned int messageType, unsigned char timeoutSecs);

//ZNP Configuration
void setZigbeeDeviceType(unsigned char deviceType);
void setPanId(unsigned int panId);
void setChannel(unsigned char channel);
void setChannelMask(unsigned long channelMask);
void setStartupOptions(unsigned char option);
void setCallbacks(unsigned char cb);
void setPollRate(unsigned int pollRate);
void setSecurityMode(unsigned char securityMode);
void setSecurityKey(unsigned char* key);
unsigned char* getConfigurationParameter(unsigned char configId);
void getNetworkConfigurationParameters();

//Device Information Properties
unsigned char* getDeviceInformationProperty(unsigned char dip);
unsigned char* getMacAddress();
unsigned char getDeviceState();
char* getDeviceStateName(unsigned char state);
void getDeviceInformation();

//NV Memory Items
unsigned char getNvItemSize(unsigned char nvItem);
void writeNvItem(unsigned char nvItem, unsigned char* data);
unsigned char* readNvItem(unsigned char nvItem);

// RF test parameters in NV
signed int WriteRfTestParam(unsigned char mode, unsigned char channel, unsigned char power);

//GPIO pin control
unsigned char sysGpio(unsigned char operation, unsigned char value);

#define ZNP_MESSAGE_COMMAND_ID()         (CONVERT_TO_INT(znpBuf[SRSP_CMD_LSB_FIELD], znpBuf[SRSP_CMD_MSB_FIELD]))




//LOGICAL TYPES 
#define COORDINATOR                     0x00
#define ROUTER                          0x01
#define END_DEVICE                      0x02

//Radio Device Information Property 
#define DIP_STATE                       0x00
#define DIP_MAC_ADDRESS                 0x01
#define DIP_SHORT_ADDRESS               0x02
#define DIP_PARENT_SHORT_ADDRESS        0x03
#define DIP_PARENT_MAC_ADDRESS          0x04
#define DIP_CHANNEL                     0x05
#define DIP_PANID                       0x06
#define DIP_EXTENDED_PANID              0x07
#define MAX_DEVICE_INFORMATION_PROPERTY 0x07

#define DIP_MAC_ADDRESS_LENGTH          8
#define DIP_SHORT_ADDRESS_LENGTH        2
#define DIP_PARENT_SHORT_ADDRESS_LENGTH 2
#define DIP_PARENT_MAC_ADDRESS_LENGTH   8
#define DIP_CHANNEL_LENGTH              1
#define DIP_PANID_LENGTH                2
#define DIP_EXTENDED_PANID_LENGTH       8

#define ZB_WRITE_CONFIGURATION_LEN 2  //excluding payload length

#define SRSP_DIP_VALUE_FIELD (SRSP_HEADER_SIZE+1) //index in znpBuf[] of the start of the Device Information Property field. LSB is first.

//WRITE CONFIGURATION OPTIONS 
#define ZCD_NV_STARTUP_OPTION           0x03
#define ZCD_NV_STARTUP_OPTION_LEN       1
#define ZCD_NV_LOGICAL_TYPE             0x87
#define ZCD_NV_LOGICAL_TYPE_LEN         1
#define ZCD_NV_ZDO_DIRECT_CB            0x8F
#define ZCD_NV_ZDO_DIRECT_CB_LEN        1
#define ZCD_NV_POLL_RATE                0x25
#define ZCD_NV_POLL_RATE_LEN            2

//For Security:
#define ZCD_NV_PRECFGKEY                0x62
#define ZCD_NV_PRECFGKEY_LEN            16
#define ZCD_NV_PRECFGKEYS_ENABLE        0x63
#define ZCD_NV_PRECFGKEYS_ENABLE_LEN    1
#define ZCD_NV_SECURITY_MODE            0x64
#define ZCD_NV_SECURITY_MODE_LEN        1

//NETWORK SPECIFIC CONFIGURATION PARAMETERS 
#define ZCD_NV_PANID                    0x83
#define ZCD_NV_PANID_LEN                2
#define ZCD_NV_CHANLIST                 0x84
#define ZCD_NV_CHANLIST_LEN             4

//STARTUP OPTIONS 
#define STARTOPT_CLEAR_CONFIG           0x01
#define STARTOPT_CLEAR_STATE            0x02
#define STARTOPT_AUTO                   0x04

//used in ZB_SEND_DATA_REQUEST & AF_DATA_REQUEST
#define MAC_ACK                         0x00    //Require Acknowledgement from next device on route
#define AF_APS_ACK                      0x10    //Require Acknowledgement from final destination (if using AFZDO)
#define SAPI_APS_ACK                    0x01    //Require Acknowledgement from final destination (if using Simple API)
#define DEFAULT_RADIUS                  0x0F    //Maximum number of hops to get to destination
#define MAXIMUM_PAYLOAD_LENGTH          66      //84 bytes without security, 66 bytes with security
#define ALL_DEVICES                     0xFFFF
#define ALL_ROUTERS_AND_COORDINATORS    0xFFFC

//NETWORK SPECIFIC CONFIGURATION PARAMETERS 
#define ZCD_NV_CHANLIST                 0x84
#define ZCD_NV_CHANLIST_LEN             4     

//Security Options
#define SECURITY_MODE_OFF                0
#define SECURITY_MODE_PRECONFIGURED_KEYS 1
#define SECURITY_MODE_COORD_DIST_KEYS    2

//Callbacks
#define CALLBACKS_DISABLED              0
#define CALLBACKS_ENABLED               1

//used in PERMIT_JOINING_REQUEST methods
#define NO_TIMEOUT                      0xFF

//used in setting PAN ID
#define ANY_PAN                         0xFFFF
#define MAX_PANID                       0xFFF7
#define IS_VALID_PANID(id)       (id<=MAX_PANID)

//
//ZDO States - returned from get Device Information Property(DIP_STATE)
//
#define DEV_HOLD                        0
#define DEV_INIT                        1
#define DEV_NWK_DISC                    2
#define DEV_NWK_JOINING                 3
#define DEV_NWK_REJOIN                  4
#define DEV_END_DEVICE_UNAUTH           5
#define DEV_END_DEVICE                  6
#define DEV_ROUTER                      7
#define DEV_COORD_STARTING              8
#define DEV_ZB_COORD                    9
#define DEV_NWK_ORPHAN                  10

//used in setting CHAN_LIST
//see ZDO_MGMT_NWK_UPDATE_REQ
#define ANY_CHANNEL                     0x07FFF800  //Channel 11-26 bitmask
#define MIN_CHANNEL                     0x00000800
#define MAX_CHANNEL                     ANY_CHANNEL
#define IS_VALID_CHANNEL(channel)       ((channel>=11) && (channel<=26))

//
//      GPIO Pin read/write
//
//Operations:
#define GPIO_SET_DIRECTION      0x00
#define GPIO_SET_INPUT_MODE     0x01
#define GPIO_SET                0x02
#define GPIO_CLEAR              0x03
#define GPIO_TOGGLE             0x04
#define GPIO_READ               0x05

#define ALL_GPIO_PINS           0x0F  //GPIO 0-3

//options for GPIO_SET_INPUT_MODE
#define GPIO_INPUT_MODE_ALL_PULL_DOWNS  0xF0
#define GPIO_INPUT_MODE_ALL_PULL_UPS    0x00
#define GPIO_INPUT_MODE_ALL_TRI_STATE   0x0F
#define GPIO_DIRECTION_ALL_INPUTS       0x00

#define MIN_NV_ITEM                     1
#define MAX_NV_ITEM                     7
//2 Bytes each:
#define NV_ITEM_1                       0x0F01
#define NV_ITEM_2                       0x0F02
#define NV_ITEM_3                       0x0F03
#define NV_ITEM_4                       0x0F04
//16 Bytes each:
#define NV_ITEM_5                       0x0F05
#define NV_ITEM_6                       0x0F06

// Define 2 bytes for RF test parameters
#define ZNP_NV_RF_TEST_PARMS            0x0F07


#endif
