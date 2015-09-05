/**
* @file znp_commands.h
*
* @brief Simple definitions of ZNP commands
*
* These are listed MSB first, as the bytes come "down the wire" instead of LSB first like everything else.
* @note you may want to print this out and keep it handy, as it is very useful when debugging.
* @note this is not a comprehensive list of all ZNP commands, only what is currently used by the
* library or seen when running the examples.
*
* $Rev: 663 $
* $Author: dsmith $
* $Date: 2010-07-13 20:11:15 -0700 (Tue, 13 Jul 2010) $
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

#ifndef COMMANDS_H
#define COMMANDS_H

//SYS Interface
#define ZB_WRITE_CONFIGURATION          0x2605
#define ZB_READ_CONFIGURATION           0x2604
#define SYS_GPIO                        0x210E
#define SYS_TEST_RF                     0x4140
#define SYS_VERSION                     0x2102
#define SYS_RANDOM                      0x210C
#define SYS_OSAL_NV_READ                0x2108
#define SYS_OSAL_NV_WRITE               0x2109
#define SYS_RESET_IND                   0x4180
#define SYS_STACK_TUNE                  0x210F
#define SYS_RESET_REQ                   0x4100

//Simple API commands
#define ZB_APP_REGISTER_REQUEST         0x260A
#define ZB_APP_START_REQUEST            0x2600
#define ZB_SEND_DATA_REQUEST            0x2603
#define ZB_SEND_DATA_CONFIRM            0x4683
#define ZB_GET_DEVICE_INFO              0x2606
#define ZB_FIND_DEVICE_REQUEST          0x2607
#define ZB_FIND_DEVICE_CONFIRM          0x4685
#define ZB_PERMIT_JOINING_REQUEST       0x2608
#define ZB_START_CONFIRM                0x4680 //will receive this asynchronously
#define ZB_RECEIVE_DATA_INDICATION      0x4687 //will receive this asynchronously

//AFZDO commands:
#define AF_REGISTER                     0x2400
#define AF_DATA_REQUEST                 0x2401
#define AF_DATA_CONFIRM                 0x4480
#define AF_INCOMING_MSG                 0x4481 //will receive this asynchronously
#define ZDO_STARTUP_FROM_APP            0x2540
#define ZDO_IEEE_ADDR_REQ               0x2501
#define ZDO_IEEE_ADDR_RSP               0x4581
#define ZDO_NWK_ADDR_REQ                0x2500
#define ZDO_NWK_ADDR_RSP                0x4580

#ifdef MDB1
#define ZDO_STATE_CHANGE_IND            0x45C1  //Old version of ZNP - new versions use below
#define ZDO_END_DEVICE_ANNCE_IND        0x4593 //will receive this asynchronously
#else
#define ZDO_STATE_CHANGE_IND            0x45C0 //will receive this asynchronously
#define ZDO_END_DEVICE_ANNCE_IND        0x45C1 //will receive this asynchronously
#endif

//UTIL commands:

#define UTIL_ADDRMGR_NWK_ADDR_LOOKUP    0x2741

//Received in SRSP message
#define SRSP_STATUS_SUCCESS             0x00
#define ZNP_SUCCESS                     0x00

//expected Response message types
#define NO_RESPONSE_EXPECTED            0x00


char* getCommandName(unsigned int command);


#endif
