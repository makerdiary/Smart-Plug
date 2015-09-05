/**
* @file znp_interface.c
*
* @brief Interface to the Zigbee Network Processor (e.g. CC2530ZNP)
* 
* This file contains library methods that implement the CC2530ZNP interface specification as well
* as error checking and return values.
*
* Requires one physical interface file to be included, znp_interface_spi.h
*
* @note To output SPI transmit information, define ZNP_INTERFACE_VERBOSE. 
* It is recommended to define this on a per-project basis. 
* In IAR, this can be done in Project Options : C/C++ compiler : Preprocessor
* In the defined symbols box, add:
* ZNP_INTERFACE_VERBOSE
*
* @see http://processors.wiki.ti.com/index.php/Tutorial_on_the_Examples and http://e2e.ti.com/support/low_power_rf/default.aspx
*
* $Rev: 834 $
* $Author: dsmith $
* $Date: 2010-10-14 16:12:18 -0700 (Thu, 14 Oct 2010) $
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
#include "znp_interface.h"
#include <string.h>                     //for memcpy
#include "znp_hal.h"

#include "znp_interface_spi.h"

extern unsigned char znpBuf[100];

signed int znpResult = ZNP_SUCCESS;

/** Initializes the hardware interface to the ZNP (SPI). This method must be called before any other method in the library. 
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the version structure, or a pointer to indeterminate data if error.
@see Interface Specification for order of fields
@todo modify this if using UART
*/
unsigned char* znpInit()
{
#ifdef ZNP_INTERFACE_VERBOSE 
    printf("Initializing ZNP SPI\r\n");
#endif     
    halSpiInitZnp();
    return (znpReset());
}

/** Resets the ZNP using hardware and retrieves the SYS_RESET_IND message. 
This method is used to restart the ZNP's internal state machine and apply changes to startup options, zigbee device type, etc.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the version structure, or a pointer to indeterminate data if error.
@see Interface Specification for order of fields
*/
unsigned char* znpReset()
{
    RADIO_OFF();
    DEBUG_OFF();
    delayMs(10);
    RADIO_ON(); 
    DEBUG_ON();
    delayMs(10);
    znpResult = spiPoll();              //Note: this will be different if UART
    return (znpBuf+SRSP_PAYLOAD_START);  //the beginning of the reset indication string
}

/** Resets the ZNP using soft reset command. 
This method is used to restart the ZNP's internal state machine and apply changes to startup options, zigbee device type, etc.
This command is currently only used for the RF tester example
@see Interface Specification for order of fields
*/
#define SYS_RESET_PAYLOAD_LEN 0
void znpSoftReset()
{
    znpBuf[0] = SYS_RESET_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_RESET_REQ);
    znpBuf[2] = LSB(SYS_RESET_REQ);    
    znpResult = sendMessage(); 
}
#define SYS_VERSION_PAYLOAD_LEN 0
/** Retrieves the ZNP's version information using SYS_VERSION command.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the version structure, or a pointer to indeterminate data if error.
@see Interface Specification for order of fields
*/
unsigned char* getVersion()
{
    znpBuf[0] = SYS_VERSION_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_VERSION);
    znpBuf[2] = LSB(SYS_VERSION);    
    znpResult = sendMessage();  
    return (znpBuf+SRSP_PAYLOAD_START);  //the beginning of the version string    
}

#define SYS_RANDOM_PAYLOAD_LEN 0
/** Retrieves a random number from the ZNP using SYS_RANDOM command.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return the random number, or indeterminate data if error.
*/
unsigned int getRandom()
{
    znpBuf[0] = SYS_RANDOM_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_RANDOM);
    znpBuf[2] = LSB(SYS_RANDOM);  
    znpResult = sendMessage(); 
    return (CONVERT_TO_INT(znpBuf[SRSP_PAYLOAD_START], znpBuf[SRSP_PAYLOAD_START+1]));
}

#define STACK_TX_POWER 0
#define SYS_STACK_TUNE_PAYLOAD_LEN 2
/** Sets the RF Output power.
@param Tx Output Power, in dB. e.g. -10 = -10dB, 3 = +3dB etc.
*/
void setTransmitPower(signed char txPowerDb)
{
    znpBuf[0] = SYS_STACK_TUNE_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_STACK_TUNE);
    znpBuf[2] = LSB(SYS_STACK_TUNE);  
    
    znpBuf[3] = STACK_TX_POWER;
    znpBuf[4] = txPowerDb;
    
    znpResult = sendMessage(); 
}

#define ZB_GET_DEVICE_INFO_PAYLOAD_LEN 1
/** Retrieves the specified Device Information Property (DIP) from the ZNP.
@param dip the device information property to retrieve
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the DIP payload, or a pointer to indeterminate data if error.
*/
unsigned char* getDeviceInformationProperty(unsigned char dip)
{
    if (dip > MAX_DEVICE_INFORMATION_PROPERTY)
    {
        znpResult = -1;  //times like this you wish you were using a language that supported exceptions
        return 0;
    }
    znpBuf[0] = ZB_GET_DEVICE_INFO_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZB_GET_DEVICE_INFO);
    znpBuf[2] = LSB(ZB_GET_DEVICE_INFO);
    znpBuf[3] = dip;
    znpResult = sendMessage();     
    return (znpBuf+SRSP_PAYLOAD_START+1);
}

/** Retrieves the Media Access Controller (MAC) Address (aka Long Address, aka IEEE Address) 
The MAC Address is an eight byte globally unique serial number for this IC.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the MAC address (LSB first), or a pointer to indeterminate data if error.
*/
unsigned char* getMacAddress()
{
    return (getDeviceInformationProperty(DIP_MAC_ADDRESS));
}


/** Retrieves the device state - indicates whether it is on the network or not.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return the device state: DEV_HOLD, DEV_NWK_DISC, DEV_ROUTER etc. or an indeterminate value if error.
*/
unsigned char getDeviceState()
{
    getDeviceInformationProperty(DIP_STATE);
    return znpBuf[SRSP_HEADER_SIZE+1];
}

/** Retrieves the name of the device state in a human-readable format. From API Specification.
@param state the value for the ZDO state
@return the name of the ZDO state
*/
char* getDeviceStateName(unsigned char state)
{
    switch (state)
    {
    case DEV_HOLD:              return("DEV_HOLD");
    case DEV_INIT:              return("DEV_INIT");
    case DEV_NWK_DISC:          return("DEV_NWK_DISC");    
    case DEV_NWK_JOINING:       return("DEV_NWK_JOINING");
    case DEV_NWK_REJOIN:        return("DEV_NWK_REJOIN");
    case DEV_END_DEVICE_UNAUTH: return("DEV_END_DEVICE_UNAUTH");    
    case DEV_END_DEVICE:        return("DEV_END_DEVICE");
    case DEV_ROUTER:            return("DEV_ROUTER");    
    case DEV_COORD_STARTING:    return("DEV_COORD_STARTING");
    case DEV_ZB_COORD:          return("DEV_ZB_COORD");
    case DEV_NWK_ORPHAN:        return("DEV_NWK_ORPHAN");       
    default:                    return("Unknown State");
    }
}


/** Displays the radio's device Information Properties. Device Information Properties include: 
- Device State indicates whether the ZNP is on a network or not, and what type of device it is. 
This is a handy thing to check if things aren't operating correctly. 
If the device is starting as a coordinator, you'll see states of 01, 08, 08, then 09 once it has fully started.
- MAC Address (aka IEEE Address) is a globally unique serial number for this IC.
- Device Short Address is a network address assigned by the coordinator, similar to an IP Address in DHCP. 
The Coordinator always has a Short Address of 0.
- Parent MAC Address is the IEEE Address of this device's "parent", i.e. which device was used to join the network. 
For a router, once joined this parent MAC address is irrelevant. This DIP will NOT be updated if the network reforms.
For an end-device then this parent MAC address will always specify which router the end-device is joined to.
- Channel is which frequency channel the device is operating on.
- PAN ID (Personal Area Network Identifier) of the network is a unique number shared for all devices on the same network.
- Extended PAN ID of the network is the coordinator's MAC Address.

If device is not connected to a network then the Short Address fields will be 0xFEFF, 
the Parent MAC Address and channel will be 0, and the Extended PAN ID will be this device's MAC Address.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void getDeviceInformation()
{
    printf("Device Information Properties (MSB first)\r\n");
    getDeviceInformationProperty(DIP_STATE);
    if (znpResult != 0) return;
    printf("    Device State:               %s (%u)\r\n", getDeviceStateName(znpBuf[SRSP_DIP_VALUE_FIELD]), (znpBuf[SRSP_DIP_VALUE_FIELD])); 
    
    getDeviceInformationProperty(DIP_MAC_ADDRESS);
    printf("    MAC Address:                ");
    if (znpResult != 0) return;
    for (int i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", znpBuf[i]);
    printf("\r\n");
    
    getDeviceInformationProperty(DIP_SHORT_ADDRESS);
    if (znpResult != 0) return;
    printf("    Short Address:              %04X\r\n", CONVERT_TO_INT(znpBuf[SRSP_DIP_VALUE_FIELD] , znpBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    getDeviceInformationProperty(DIP_PARENT_SHORT_ADDRESS);
    if (znpResult != 0) return;
    printf("    Parent Short Address:       %04X\r\n", CONVERT_TO_INT(znpBuf[SRSP_DIP_VALUE_FIELD] , znpBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    getDeviceInformationProperty(DIP_PARENT_MAC_ADDRESS);
    if (znpResult != 0) return;
    printf("    Parent MAC Address:         ");
    for (int i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", znpBuf[i]);
    printf("\r\n");
    
    getDeviceInformationProperty(DIP_CHANNEL);
    if (znpResult != 0) return;
    printf("    Device Channel:             %u\r\n", znpBuf[SRSP_DIP_VALUE_FIELD]);
    
    getDeviceInformationProperty(DIP_PANID);
    if (znpResult != 0) return;
    printf("    PAN ID:                     %04X\r\n", CONVERT_TO_INT(znpBuf[SRSP_DIP_VALUE_FIELD], znpBuf[SRSP_DIP_VALUE_FIELD+1]));
    
    getDeviceInformationProperty(DIP_EXTENDED_PANID);
    if (znpResult != 0) return;
    printf("    Extended PAN ID:            ");
    for (int i = SRSP_DIP_VALUE_FIELD+7; i>=SRSP_DIP_VALUE_FIELD; i--)
        printf("%02X ", znpBuf[i]);
    printf("\r\n");
}

#define SYS_GPIO_PAYLOAD_LEN 2
/** Performs the specified General Purpose Input/Output (GPIO) operation.
@param operation which GPIO operation to do: GPIO_SET_DIRECTION, GPIO_SET_INPUT_MODE, GPIO_SET, GPIO_CLEAR, GPIO_TOGGLE, GPIO_READ
@param value the value to write
@return the value returned by the ZNP if operation = GPIO_READ; indeterminate value otherwise
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
unsigned char sysGpio(unsigned char operation, unsigned char value)
{
    if (operation > 5)
    {
        znpResult = -1;
        return 0;   
    }
    if ((value > 0x0F) && (operation != GPIO_SET_INPUT_MODE))
    {
        znpResult = -2;
        return 0;
    }
    znpBuf[0] = SYS_GPIO_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_GPIO);
    znpBuf[2] = LSB(SYS_GPIO);
    
    znpBuf[3] = operation;
    znpBuf[4] = value;
    
    znpResult = sendMessage();     
    return znpBuf[SRSP_PAYLOAD_START];
}

/** Retrieves the name of the device type in a human-readable format, e.g. Coordinator, Router, or End Device. 
@param deviceType the type of device
@return the name of the deviceType
*/
char* getDeviceTypeName(unsigned char deviceType)
{
    switch (deviceType)
    {
    case COORDINATOR:               return("Coordinator");
    case ROUTER:                    return("Router");
    case END_DEVICE:                return("End Device");          
    default:                        return("Unknown");
    }
}

/** Sets the Zigbee Device Type for the ZNP.
@param deviceType the type of Zigbee device. Must be COORDINATOR, ROUTER, or END_DEVICE 
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setZigbeeDeviceType(unsigned char deviceType)
{
    if (deviceType > END_DEVICE) 
    {
        znpResult = -1;
        return; 
    }
#ifdef ZNP_INTERFACE_VERBOSE 
    printf("Setting Zigbee DeviceType to %s\r\n", getDeviceTypeName(deviceType));
#endif 
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_LOGICAL_TYPE_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_LOGICAL_TYPE;
    znpBuf[4] = ZCD_NV_LOGICAL_TYPE_LEN;
    znpBuf[5] = deviceType;
    
    znpResult = sendMessage();       
}

/** Configures the ZNP to only join a network with the given panId. 
If panId = ANY_PAN then the radio will join any network.
@param panId the PANID to join, or ANY_PAN to join any PAN
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setPanId(unsigned int panId)
{
#ifdef ZNP_INTERFACE_VERBOSE 
    printf("Setting Zigbee PAN ID to %04X\r\n", panId);
#endif     
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PANID_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_PANID;
    znpBuf[4] = ZCD_NV_PANID_LEN;
    znpBuf[5] = LSB(panId);
    znpBuf[6] = MSB(panId);
    
    znpResult = sendMessage();     
}

/** Configures the ZNP to only join the specified channel.
Wrapper for setChannelMask if only one channel is desired.
@param channel must be 11..26, inclusive. If set to 0 then ZNP will scan ALL CHANNELS
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setChannel(unsigned char channel)
{
    if ((!IS_VALID_CHANNEL(channel)) && (channel != 0x00))
    {
        znpResult = -10;
        return;
    }
    unsigned long channelMask = 1;
    setChannelMask((channel == 0) ? ANY_CHANNEL :(channelMask << channel));  //will set znpResult on success/fail
}

/** Configures the ZNP to only join a network according to the given channel mask. 
@param channelMask bitmap of which channels to use. If channelMask = ANY_CHANNEL then the radio will join a network on any channel. 
Default channel after a CLEAR_CONFIG is channel 11, or (MSB first) 0x00000800.
@note: if using a packet sniffer, be sure to change the channel on the packet sniffer too or else you won't see any packets.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setChannelMask(unsigned long channelMask)
{
    //printf("channelMask = %08lX\r\n", channelMask);
    
    if ((channelMask < MIN_CHANNEL) || (channelMask > MAX_CHANNEL))
    {
        znpResult = -100;
        return; 
    }
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_CHANLIST_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_CHANLIST;
    znpBuf[4] = ZCD_NV_CHANLIST_LEN;
    znpBuf[5] = LSB(channelMask);
    znpBuf[6] = (channelMask & 0xFF00) >> 8;
    znpBuf[7] = (channelMask & 0xFF0000) >> 16;
    znpBuf[8] = channelMask >> 24;
    
    znpResult = sendMessage(); 
    
#ifdef ZNP_INTERFACE_VERBOSE 
    printf("Setting to Channel List (LSB first): %02X %02X %02X %02X\r\n", znpBuf[2], znpBuf[3], znpBuf[4], znpBuf[5]);
#endif
}

/** Configures startup options on the ZNP. These will reset various parameters back to their factory defaults. 
The radio supports two types of clearing state, and both are supported:
- STARTOPT_CLEAR_CONFIG restores all settings to factory defaults. Must restart the ZNP after using this option.
- STARTOPT_CLEAR_STATE only clears network settings (PAN ID, channel, etc.)
@note all ZB_WRITE_CONFIGURATION commands take approx. 3.5mSec between SREQ & SRSP; presumably to write to flash inside the CC2530ZNP. 
@param option which options to set. Must be zero, STARTOPT_CLEAR_CONFIG, or STARTOPT_CLEAR_STATE.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setStartupOptions(unsigned char option)
{ 
    if (option > (STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE))
    {
        znpResult = -1;
        return; 
    } 
    
#ifdef ZNP_INTERFACE_VERBOSE     
    printf("Setting Startup Option to ");
    if (option & STARTOPT_CLEAR_CONFIG)
        printf("STARTOPT_CLEAR_CONFIG ");
    if (option & STARTOPT_CLEAR_STATE)
        printf("STARTOPT_CLEAR_STATE ");   
    printf("\r\n");
#endif
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_STARTUP_OPTION_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_STARTUP_OPTION;
    znpBuf[4] = ZCD_NV_STARTUP_OPTION_LEN;
    znpBuf[5] = option;
    znpResult = sendMessage();
}

/** Enable/Disabless callbacks on the ZNP. 
@param cb must be either CALLBACKS_ENABLED or CALLBACKS_DISABLED
@see section ZCD_NV_ZDO_DIRECT_CB in ZNP Interface Specification
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setCallbacks(unsigned char cb)
{ 
    if ((cb != CALLBACKS_ENABLED) && (cb != CALLBACKS_DISABLED))
    {
        znpResult = -1;
        return; 
    }
#ifdef ZNP_INTERFACE_VERBOSE     
    printf("Setting Callbacks to %s\r\n", (cb ? "ON" : "OFF"));
#endif
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_ZDO_DIRECT_CB;
    znpBuf[4] = ZCD_NV_ZDO_DIRECT_CB_LEN;
    znpBuf[5] = cb;
    znpResult = sendMessage();    
}

/** Sets the ZCD_NV_POLL_RATE, which is used by end devices and controls how often an end device
polls its associated router for new data. Default is 2000 (2 seconds). 
Higher numbers will increase latency but reduce average power consumption.
@param rate how often (in mSec) to poll; must be between 1 to 65000, inclusive.
@see section ZCD_NV_POLL_RATE in ZNP Interface Specification
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setPollRate(unsigned int pollRate)
{ 
    if ((pollRate == 0) || (pollRate > 65000))
    {
        znpResult = -1;
        return; 
    }
#ifdef ZNP_INTERFACE_VERBOSE     
    printf("Setting ZCD_NV_POLL_RATE to %u\r\n", pollRate);
#endif
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_POLL_RATE;
    znpBuf[4] = ZCD_NV_POLL_RATE_LEN;
    znpBuf[5] = LSB(pollRate);
    znpBuf[6] = MSB(pollRate);
    
    znpResult = sendMessage();    
}

/** Retrieves the name of the security mode in a human-readable format. From API Specification.
@return the name of the securityMode
*/
char* getSecurityModeName(unsigned char securityMode)
{
    switch (securityMode)
    {
    case SECURITY_MODE_OFF:                 return("OFF");
    case SECURITY_MODE_PRECONFIGURED_KEYS:  return("USE PRECONFIGURED KEY");
    case SECURITY_MODE_COORD_DIST_KEYS:     return("COORDINATOR DISTRIBUTE KEY");   
    default:                                return("Unknown");
    }
}

/** Configures the ZNP for the specified security mode.
If securityMode = SECURITY_MODE_OFF then only ZCD_NV_SECURITY_MODE is written. 
Otherwise ZCD_NV_SECURITY_MODE and ZCD_NV_PRECFGKEYS_ENABLE are written.
@note if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
@param securityMode SECURITY_MODE_OFF or SECURITY_MODE_PRECONFIGURED_KEYS or SECURITY_MODE_COORD_DIST_KEYS
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setSecurityMode(unsigned char securityMode)
{
    if (securityMode > SECURITY_MODE_COORD_DIST_KEYS)
    {
        znpResult = -1;
        return; 
    }
#ifdef ZNP_INTERFACE_VERBOSE     
    printf("Setting Security = %s\r\n", getSecurityModeName(securityMode));      
#endif
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_SECURITY_MODE_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
    
    znpBuf[3] = ZCD_NV_SECURITY_MODE;
    znpBuf[4] = ZCD_NV_SECURITY_MODE_LEN;
    znpBuf[5] = (securityMode > 0); 
    
    znpResult = sendMessage();
    if (znpResult != 0)
        return;
    
    if (securityMode != SECURITY_MODE_OFF)   //if turning security off, don't need to set pre-configured keys on/off
    {
        //2. Configure pre-configured keys on/off:
        znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEYS_ENABLE_LEN;
        znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
        znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);
        
        znpBuf[3] = ZCD_NV_PRECFGKEYS_ENABLE;
        znpBuf[4] = ZCD_NV_PRECFGKEYS_ENABLE_LEN;
        znpBuf[5] = (securityMode == SECURITY_MODE_PRECONFIGURED_KEYS); 
        
        znpResult = sendMessage();
    }
}

/** Loads the specified key into the ZNP. Does not change security mode, need to use setSecurityMode() too.
@note if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
@param key preConfiguredKey a 16B key to use
@pre setSecurityMode() called
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void setSecurityKey(unsigned char* key)
{
#ifdef ZNP_INTERFACE_VERBOSE     
    printf("Setting Security Key: ");
    printHexBytes(key, ZCD_NV_PRECFGKEY_LEN);
#endif
    znpBuf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEY_LEN;
    znpBuf[1] = MSB(ZB_WRITE_CONFIGURATION);
    znpBuf[2] = LSB(ZB_WRITE_CONFIGURATION);    
    
    znpBuf[3] = ZCD_NV_PRECFGKEY;
    znpBuf[4] = ZCD_NV_PRECFGKEY_LEN;
    
    for (int i=5; i<(ZCD_NV_PRECFGKEY_LEN+5); i++)
        znpBuf[i] = key[i];            
    znpResult = sendMessage();
}

/** Utility method to display stored network configuration parameters. These are the configuration
parameters stored in NV memory and are used to initialize the ZNP.
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void getNetworkConfigurationParameters()
{
    printf("ZNP Configuration Parameters\r\n");
    unsigned char* param;
    
    param = getConfigurationParameter(ZCD_NV_PANID);
    if (znpResult != 0) return;
    printf("    ZCD_NV_PANID                %04X\r\n", (CONVERT_TO_INT(param[0], param[1])));
    
    param = getConfigurationParameter(ZCD_NV_CHANLIST);
    if (znpResult != 0) return;
    printf("    ZCD_NV_CHANLIST             %02X %02X %02X %02X\r\n", param[0], param[1], param[2], param[3]);
    
    param = getConfigurationParameter(ZCD_NV_SECURITY_MODE);
    if (znpResult != 0) return;
    printf("    ZCD_NV_SECURITY_MODE        %02X\r\n", param[0]);
    
    param = getConfigurationParameter(ZCD_NV_PRECFGKEYS_ENABLE);
    if (znpResult != 0) return;
    printf("    ZCD_NV_PRECFGKEYS_ENABLE    %02X\r\n", param[0]);    
    
    param = getConfigurationParameter(ZCD_NV_PRECFGKEY);
    if (znpResult != 0) return;
    printf("    ZCD_NV_PRECFGKEY            ");    
    printHexBytes(param, ZCD_NV_PRECFGKEY_LEN);
    
}

#define ZB_READ_CONFIGURATION_PAYLOAD_LEN 1
/** Retrieves the given configuration parameter from the ZNP. 
@param configId the configuration parameter to retrieve. See Interface Spec for list of valid config parameters. Not checked for validity.
@return pointer to beginning of configuration parameter
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@note does not verify configId is a valid configId
@note does not verify length returned in the SRSP matches what the length should be for that configId.
*/
unsigned char* getConfigurationParameter(unsigned char configId)
{
#define LENGTH_OF_LARGEST_CONFIG_PARAMETER 17  //ZCD_NV_USERDESC is largest
    
    znpBuf[0] = ZB_READ_CONFIGURATION_PAYLOAD_LEN;
    znpBuf[1] = MSB(ZB_READ_CONFIGURATION);
    znpBuf[2] = LSB(ZB_READ_CONFIGURATION);  
    
    znpBuf[3] = configId;
    
    znpResult = sendMessage();
    
#define STATUS_FIELD            SRSP_PAYLOAD_START
#define CONFIG_ID_FIELD         SRSP_PAYLOAD_START + 1
#define LENGTH_FIELD            SRSP_PAYLOAD_START + 2
#define START_OF_VALUE_FIELD    SRSP_PAYLOAD_START + 3
    
    //when using SPI, znpBuf[3] holds status, [4] holds configId, [5] holds length
    if ((znpBuf[STATUS_FIELD] != SRSP_STATUS_SUCCESS) ||                  //if status was success
        (znpBuf[CONFIG_ID_FIELD] != configId) ||                          //if the requested configId was what we requested
            (znpBuf[LENGTH_FIELD] > LENGTH_OF_LARGEST_CONFIG_PARAMETER) ||    //if the length is valid
                (znpBuf[LENGTH_FIELD] == 0))
    {
        znpResult = -1;              //error
        return 0;
    } else {
        return (znpBuf + START_OF_VALUE_FIELD);
    }
}

/** Utility method for displaying nice reset reason from a SYS_RESET_IND message.
@param reason the reset reason
@return a string containing the name of the reset reason.
*/
char* getResetReason(unsigned char reason)
{
    switch (reason)
    {
    case 0:     return "Power-up";
    case 1:     return "External";
    case 2:     return "Watch-dog";
    default:    return "Unknown";
    }
}

#define WAIT_FOR_MESSAGE_TIMEOUT
/** Waits for the specified type of message. Silently discards any other messages received.
Received message will be in znpBuf[]. If the specified type of message isn't received then returns error code.
@return 0 if success, -21 if timeout, -31 if received a message but it wasn't the type we're expecting.
*/
signed int waitForMessage(unsigned int messageType, unsigned char timeoutSecs)
{
    unsigned long counter = 0;
    unsigned long timeout = (timeoutSecs / 2) * XTAL; //since loop takes more than 1 clock cycle
    
    while (SRDY_IS_HIGH() && (counter != timeout))
        counter++;   
    if (counter == timeout)
        return -21;
    spiPoll();
    if ((znpBuf[SRSP_LENGTH_FIELD] > 0) && (CONVERT_TO_INT(znpBuf[2], znpBuf[1]) == messageType))
    {
#ifdef ZNP_INTERFACE_VERBOSE
        printf("Received expected message %04X\r\n", messageType);
#endif
        return 0;
    } else {                    //unexpected message received
        //#ifdef ZNP_INTERFACE_VERBOSE
        printf("waitForMessage Unexpected Message: L%u, messageType = %04X\r\n", 
               znpBuf[SRSP_LENGTH_FIELD] , (CONVERT_TO_INT(znpBuf[2], znpBuf[1])));
        //#endif
        return -31;
    }
    
}

/*
*               NON-VOLATILE (NV) MEMORY ITEMS
*/

/** Retrieves the size of the selected Non-Volatile (NV) memory item.
NV Items 1 through 4 are 2 bytes each; 5 & 6 are 16 bytes each.
@param nvItem which Non-Volatile memory item. Must be 1-6 inclusive.
@return size of the given nvItem
*/
unsigned char getNvItemSize(unsigned char nvItem)
{
    return (nvItem < 5) ? 2 : 16;  //items 1-4 have size=2; items 5,6 have size=16
}

#define SYS_OSAL_NV_WRITE_PAYLOAD_LEN 4  //excludes length of NV item
/** Writes the specified Non-Volatile (NV) memory item to the ZNP. 
The contents of the selected nvItem will be overwritten from memory starting at data.
@pre ZNP was initialized.
@param nvItem which nvItem to write, 1 through 6 inclusive
@param data the data to write
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
void writeNvItem(unsigned char nvItem, unsigned char* data)
{
    if ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM))
    {
        znpResult = -1;
        return;
    }   
    unsigned char nvItemSize = getNvItemSize(nvItem);
#ifdef ZNP_INTERFACE_VERBOSE
    printf("Writing NV Item %u (length %u) with data: ", nvItem, nvItemSize);
    printHexBytes(data, nvItemSize);
#endif      
    znpBuf[0] = SYS_OSAL_NV_WRITE_PAYLOAD_LEN + nvItemSize;
    znpBuf[1] = MSB(SYS_OSAL_NV_WRITE);
    znpBuf[2] = LSB(SYS_OSAL_NV_WRITE);  
    
    znpBuf[3] = nvItem;         //item number, 1-6
    znpBuf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
    znpBuf[5] = 0;              //offset from beginning of the NV item, not used
    znpBuf[6] = nvItemSize;     //length
    
    memcpy(znpBuf+7, data, nvItemSize);
    znpResult = sendMessage();    
}

#define SYS_OSAL_NV_READ_PAYLOAD_LEN 3  
/** Retrieves the specified Non-Volatile (NV) memory item from the ZNP. 
@pre ZNP was initialized.
@param nvItem which nvItem to write, 1 through 6 inclusive
@post znpResult contains the error code, or ZNP_SUCCESS if success.
@return a pointer to the beginning of the NV item retrieved, or a pointer to indeterminate data if error.
@note does not verify the length of the returned nvItem was what we expected
*/
unsigned char* readNvItem(unsigned char nvItem)
{
    if ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM))
    {
        znpResult = -1;
        return 0;
    }
    
    znpBuf[0] = SYS_OSAL_NV_READ_PAYLOAD_LEN;
    znpBuf[1] = MSB(SYS_OSAL_NV_READ);
    znpBuf[2] = LSB(SYS_OSAL_NV_READ);  
    
    znpBuf[3] = nvItem;         //item number, 1-6
    znpBuf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
    znpBuf[5] = 0;              //offset from beginning of the NV item, not used
    
    znpResult = sendMessage();   
    
    if (znpResult != 0)
        return 0;
    if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS)  
    {
        znpResult = -2;
        return 0;
    }
    else
    {
        return (znpBuf + SRSP_PAYLOAD_START + 2);
    }
}

#define ZNP_NV_RF_TEST_HEADER_LEN 4  //length of RF test parameters header
#define ZNP_NV_RF_TEST_PARMS_LEN 4  //length of RF test parameters 
/** Writes the specified Non-Volatile (NV) memory RF test parameters to the ZNP. 
// The settings in the test parameter location takes effect after a reset. The 
// only way to exit test mode is a HW reset.
@param mode which mode to use, modulated, unmodulated or Rx
@param channel test tone
@post znpResult contains the error code, or ZNP_SUCCESS if success.
*/
signed int writeRfTestParam(unsigned char mode, unsigned char channel, unsigned char power)
{
    znpBuf[0] = ZNP_NV_RF_TEST_HEADER_LEN + ZNP_NV_RF_TEST_PARMS_LEN;
    znpBuf[1] = MSB(SYS_OSAL_NV_WRITE);
    znpBuf[2] = LSB(SYS_OSAL_NV_WRITE);  
    
    znpBuf[3] = LSB(ZNP_NV_RF_TEST_PARMS); 
    znpBuf[4] = MSB(ZNP_NV_RF_TEST_PARMS); 
    znpBuf[5] = 0;                            //offset from beginning of the NV item, not used
    znpBuf[6] = ZNP_NV_RF_TEST_PARMS_LEN;     //length
    znpBuf[7] = mode;
    znpBuf[8] = channel;
    znpBuf[9] = power;
    znpBuf[10] = 0x07;                         // Test tone, see CC2530 UG for explaination
    znpResult = sendMessage();    

    znpSoftReset();

    return znpResult;
}

/** Displays the contents of the Znp Buffer, znpBuf.
Useful for debugging ZNP communications.
*/
void printZnpBuf()
{
    printf("zb: ");
    for (int i=0; i<znpBuf[0]+3; i++)
        printf("%02X ", znpBuf[i]);
    printf("\r\n");
}