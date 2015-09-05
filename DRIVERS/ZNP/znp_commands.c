/**
* @file znp_commands.c
*
* @brief Miscellaneous utility for printing out the name of the ZNP command
*
* These are listed MSB first, as the bytes come "down the wire" instead of LSB first like everything else.
*
* $Rev: 663 $
* $Author: dsmith $
* $Date: 2010-07-13 20:11:15 -0700 (Tue, 13 Jul 2010) $
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
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

#include "znp_commands.h"

/** Converts the command name to a human readable string
@param command the commandId
@return the name of the command
*/
char* getCommandName(unsigned int command)
{
    switch (command)
    {
    //SYS Interface        
    case ZB_WRITE_CONFIGURATION:      return("ZB_WRITE_CONFIGURATION");
    case ZB_READ_CONFIGURATION:       return("ZB_READ_CONFIGURATION");
    case SYS_GPIO:                    return("SYS_GPIO");
    case SYS_TEST_RF:                 return("SYS_TEST_RF");
    case SYS_VERSION:                 return("SYS_VERSION");
    case SYS_OSAL_NV_READ:            return("SYS_OSAL_NV_READ");
    case SYS_OSAL_NV_WRITE:           return("SYS_OSAL_NV_WRITE");
    case SYS_RESET_IND:               return("SYS_RESET_IND");
    
    //Simple API commands
    case ZB_APP_REGISTER_REQUEST:     return("ZB_APP_REGISTER_REQUEST");      
    case ZB_APP_START_REQUEST:        return("ZB_APP_START_REQUEST");
    case ZB_SEND_DATA_REQUEST:        return("ZB_SEND_DATA_REQUEST");
    case ZB_SEND_DATA_CONFIRM:        return("ZB_SEND_DATA_CONFIRM");
    case ZB_GET_DEVICE_INFO:          return("ZB_GET_DEVICE_INFO");
    case ZB_FIND_DEVICE_REQUEST:      return("ZB_FIND_DEVICE_REQUEST");
    case ZB_FIND_DEVICE_CONFIRM:      return("ZB_FIND_DEVICE_CONFIRM");
    case ZB_PERMIT_JOINING_REQUEST:   return("ZB_PERMIT_JOINING_REQUEST");
    case ZB_START_CONFIRM:            return("ZB_START_CONFIRM");
    case ZB_RECEIVE_DATA_INDICATION:  return("ZB_RECEIVE_DATA_INDICATION");
    
    //AF/ZDO commands:    
    case AF_REGISTER:                 return("AF_REGISTER");
    case AF_DATA_REQUEST:             return("AF_DATA_REQUEST");
    case AF_DATA_CONFIRM:             return("AF_DATA_CONFIRM");
    case AF_INCOMING_MSG:             return("AF_INCOMING_MSG");
    case ZDO_STARTUP_FROM_APP:        return("ZDO_STARTUP_FROM_APP");
    case ZDO_IEEE_ADDR_REQ:           return("ZDO_IEEE_ADDR_REQ");
    case ZDO_IEEE_ADDR_RSP:           return("ZDO_IEEE_ADDR_RSP");
    case ZDO_NWK_ADDR_REQ:            return("ZDO_NWK_ADDR_REQ");
    case ZDO_NWK_ADDR_RSP:            return("ZDO_NWK_ADDR_RSP");
    case ZDO_END_DEVICE_ANNCE_IND:    return("ZDO_END_DEVICE_ANNCE_IND");
    
    default:                          return("UNKNOWN");
    }
}
