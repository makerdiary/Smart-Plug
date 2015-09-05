/**
*  @file simple_api.h
*
*  @brief  public methods for simple_api.c
*
* $Rev: 547 $
* $Author: dsmith $
* $Date: 2010-05-27 17:11:30 -0700 (Thu, 27 May 2010) $
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
#ifndef SIMPLE_API_H
#define SIMPLE_API_H

#include "application_configuration.h"

void sapiRegisterApplication(struct applicationConfiguration ac);
void sapiRegisterGenericApplication();
void sapiStartApplication();
void setJoiningPermissions(unsigned int destination, unsigned char timeout);
void sendData(unsigned int destinationShortAddress, unsigned int clusterId, unsigned char* data, unsigned char dataLength);

#endif