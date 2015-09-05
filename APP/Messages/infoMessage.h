/**
*
* @file infoMessage.h
*
* @brief Public methods for infoMessage.c
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
#include "header.h"

#ifndef INFO_MESSAGE_H
#define INFO_MESSAGE_H

/**
* INFO MESSAGE
* FROM: Device
* TO: Server
* RESPONSE: none
*
* Contains device information, like manufacturer, model, etc.
*/
#define MAX_PARAMETERS_IN_INFO_MESSAGE 17

#define INFO_MESSAGE_CLUSTER   0x0007

#define INFO_MESSAGE_VERSION 0x02
#define INFO_MESSAGE_FLAGS_NONE 0x00
#define DEVICETYPE_SMITH_ELECTRONCS_ROUTER_DEMO 0x01 
#define DEVICESUBTYPE_SMITH_ELECTRONCS_ROUTER_DEMO 0x01
#define CAUSE_SCHEDULED 0x01
#define CAUSE_MOTION    0x02
#define CAUSE_STARTUP   0x03


struct infoMessage   //FROM device
{
  struct header* header; //12B
  unsigned char deviceType; 
  unsigned char deviceSubType;  
  unsigned char cause;  //TBD
  unsigned char numParameters;
  unsigned int parameters[MAX_PARAMETERS_IN_INFO_MESSAGE];
};
void printInfoMessage(struct infoMessage* im);
void serializeInfoMessage(struct infoMessage* im, unsigned char* destinationPtr);
struct infoMessage deserializeInfoMessage(unsigned char* source);
unsigned int getSizeOfInfoMessage(struct infoMessage* im);

#endif
