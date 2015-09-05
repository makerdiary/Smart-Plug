using System;
using System.Collections.Generic;
using System.Collections;
using System.Diagnostics;
using System.Linq;
using System.Text;
using System.Threading.Tasks;
using System.IO.Ports;
using System.Timers;

namespace ZNPLib
{

    public class ZNP
    {
        public ZNP()
        {
            // TODO: Add constructor logic here
        }
#region All Constants

        //Radio Device Information Property 
        public const int DIP_STATE                         = 0x00;
        public const int DIP_MAC_ADDRESS                   = 0x01;
        public const int DIP_SHORT_ADDRESS                 = 0x02;
        public const int DIP_PARENT_SHORT_ADDRESS          = 0x03;
        public const int DIP_PARENT_MAC_ADDRESS            = 0x04;
        public const int DIP_CHANNEL                       = 0x05;
        public const int DIP_PANID                         = 0x06;
        public const int DIP_EXTENDED_PANID                = 0x07;
        public const int MAX_DEVICE_INFORMATION_PROPERTY   = 0x07;

        public const int DIP_MAC_ADDRESS_LENGTH            = 8;
        public const int DIP_SHORT_ADDRESS_LENGTH          = 2;
        public const int DIP_PARENT_SHORT_ADDRESS_LENGTH   = 2;
        public const int DIP_PARENT_MAC_ADDRESS_LENGTH     = 8;
        public const int DIP_CHANNEL_LENGTH                = 1;
        public const int DIP_PANID_LENGTH                  = 2;
        public const int DIP_EXTENDED_PANID_LENGTH         = 8;

        public const int ZB_WRITE_CONFIGURATION_LEN        = 2;  //excluding payload length
        public const int ZB_READ_CONFIGURATION_PAYLOAD_LEN = 1;

        public const int SRSP_DIP_VALUE_FIELD              = (SRSP_HEADER_SIZE + 1); //index in znpBuf[] of the start of the Device Information Property field. LSB is first.

        //WRITE CONFIGURATION OPTIONS 
        public const int ZCD_NV_STARTUP_OPTION             = 0x03;
        public const int ZCD_NV_STARTUP_OPTION_LEN         = 1;
        public const int ZCD_NV_LOGICAL_TYPE               = 0x87;
        public const int ZCD_NV_LOGICAL_TYPE_LEN           = 1;
        public const int ZCD_NV_ZDO_DIRECT_CB              = 0x8F;
        public const int ZCD_NV_ZDO_DIRECT_CB_LEN          = 1;
        public const int ZCD_NV_POLL_RATE                  = 0x25;
        public const int ZCD_NV_POLL_RATE_LEN              = 2;

        //For Security:
        public const int ZCD_NV_PRECFGKEY                  = 0x62;
        public const int ZCD_NV_PRECFGKEY_LEN              = 16;
        public const int ZCD_NV_PRECFGKEYS_ENABLE          = 0x63;
        public const int ZCD_NV_PRECFGKEYS_ENABLE_LEN      = 1;
        public const int ZCD_NV_SECURITY_MODE              = 0x64;
        public const int ZCD_NV_SECURITY_MODE_LEN          = 1;

        //NETWORK SPECIFIC CONFIGURATION PARAMETERS 
        public const int ZCD_NV_PANID                      = 0x83;
        public const int ZCD_NV_PANID_LEN                  = 2;
        public const int ZCD_NV_CHANLIST                   = 0x84;
        public const int ZCD_NV_CHANLIST_LEN               = 4;

        //STARTUP OPTIONS 
        public const int STARTOPT_CLEAR_CONFIG             = 0x01;
        public const int STARTOPT_CLEAR_STATE              = 0x02;
        public const int STARTOPT_AUTO                     = 0x04;

        //used in ZB_SEND_DATA_REQUEST & AF_DATA_REQUEST
        public const int MAC_ACK                           = 0x00;    //Require Acknowledgement from next device on route
        public const int AF_APS_ACK                        = 0x10;    //Require Acknowledgement from final destination (if using AFZDO)
        public const int SAPI_APS_ACK                      = 0x01;    //Require Acknowledgement from final destination (if using Simple API)
        public const int DEFAULT_RADIUS                    = 0x0F;    //Maximum number of hops to get to destination
        public const int MAXIMUM_PAYLOAD_LENGTH            = 66;      //84 bytes without security, 66 bytes with security
        public const int ALL_DEVICES                       = 0xFFFF;
        public const int ALL_ROUTERS_AND_COORDINATORS      = 0xFFFC;


        //Security Options
        public const int SECURITY_MODE_OFF                 = 0;
        public const int SECURITY_MODE_PRECONFIGURED_KEYS  = 1;
        public const int SECURITY_MODE_COORD_DIST_KEYS     = 2;

        //Callbacks
        public const int CALLBACKS_DISABLED                = 0;
        public const int CALLBACKS_ENABLED                 = 1;

        //used in PERMIT_JOINING_REQUEST methods
        public const int NO_TIMEOUT                        = 0xFF;

        //used in setting PAN ID
        public const int ANY_PAN                           = 0xFFFF;
        public const int MAX_PANID                         = 0xFFF7;
        public bool IS_VALID_PANID(int id) { return (id <= MAX_PANID); }


        //ZDO States - returned from get Device Information Property(DIP_STATE)
        public const int DEV_HOLD                          = 0;
        public const int DEV_INIT                          = 1;
        public const int DEV_NWK_DISC                      = 2;
        public const int DEV_NWK_JOINING                   = 3;
        public const int DEV_NWK_REJOIN                    = 4;
        public const int DEV_END_DEVICE_UNAUTH             = 5;
        public const int DEV_END_DEVICE                    = 6;
        public const int DEV_ROUTER                        = 7;
        public const int DEV_COORD_STARTING                = 8;
        public const int DEV_ZB_COORD                      = 9;
        public const int DEV_NWK_ORPHAN                    = 10;

        //used in setting CHAN_LIST
        //see ZDO_MGMT_NWK_UPDATE_REQ
        public const uint ANY_CHANNEL                      = 0x07FFF800;  //Channel 11-26 bitmask
        public const uint MIN_CHANNEL                      = 0x00000800;
        public const uint MAX_CHANNEL                      = ANY_CHANNEL;
        public bool IS_VALID_CHANNEL(int channel) { return ((channel >= 11) && (channel <= 26)); }

        //GPIO Pin read/write
        //Operations:
        public const int GPIO_SET_DIRECTION                = 0x00;
        public const int GPIO_SET_INPUT_MODE               = 0x01;
        public const int GPIO_SET                          = 0x02;
        public const int GPIO_CLEAR                        = 0x03;
        public const int GPIO_TOGGLE                       = 0x04;
        public const int GPIO_READ                         = 0x05;

        public const int ALL_GPIO_PINS                     = 0x0F;  //GPIO 0-3

        //options for GPIO_SET_INPUT_MODE
        public const int GPIO_INPUT_MODE_ALL_PULL_DOWNS    = 0xF0;
        public const int GPIO_INPUT_MODE_ALL_PULL_UPS      = 0x00;
        public const int GPIO_INPUT_MODE_ALL_TRI_STATE     = 0x0F;
        public const int GPIO_DIRECTION_ALL_INPUTS         = 0x00;

        public const int MIN_NV_ITEM                       = 1;
        public const int MAX_NV_ITEM                       = 7;
        //2 Bytes each:
        public const int NV_ITEM_1                         = 0x0F01;
        public const int NV_ITEM_2                         = 0x0F02;
        public const int NV_ITEM_3                         = 0x0F03;
        public const int NV_ITEM_4                         = 0x0F04;
        //16 Bytes each:
        public const int NV_ITEM_5                         = 0x0F05;
        public const int NV_ITEM_6                         = 0x0F06;

        // Define 2 bytes for RF test parameters
        public const int ZNP_NV_RF_TEST_PARMS              = 0x0F07;


        public const int SYS_RESET_PAYLOAD_LEN             = 0;
        public const int SYS_VERSION_PAYLOAD_LEN           = 0;
        public const int SYS_RANDOM_PAYLOAD_LEN            = 0;
        public const int STACK_TX_POWER                    = 0;
        public const int SYS_STACK_TUNE_PAYLOAD_LEN        = 2;
        public const int SYS_GPIO_PAYLOAD_LEN              = 2;
        public const int SYS_OSAL_NV_WRITE_PAYLOAD_LEN     = 4;
        public const int SYS_OSAL_NV_READ_PAYLOAD_LEN      = 3;  
        public const int ZB_GET_DEVICE_INFO_PAYLOAD_LEN    = 0x01;

        public const int ZNP_SUCCESS                       = 0x00;
        public const int ZNP_FAIL                          = -1;

        //Received in SRSP message
        public const int SRSP_STATUS_SUCCESS               = 0x00;
        public const int SRSP_TIMEOUT                      = -11;
        public const int SRSP_FRAMEERROR                   = -12;
        public const int SRSP_CMDERROR                     = -13;
        public const int SRSP_UNKNOWERROR                  = -14;

        //SRSP timeout in ms
        public const int SRSP_TIMEOUT_MS                   = 100;

        public const int SRSP_HEADER_SIZE                  = 3;
        public const int SRSP_BUFFER_SIZE                  = 20;

        public const int SRSP_PAYLOAD_START                = 3;
        public const int SRSP_LENGTH_FIELD                 = 0;
        public const int SRSP_CMD_LSB_FIELD                = 2;
        public const int SRSP_CMD_MSB_FIELD                = 1;

        //Logical Types
        public const int COORDINATOR                       = 0x00;
        public const int ROUTER                            = 0x01;
        public const int END_DEVICE                        = 0x02;


        //configuration parameter from the ZNP.
        public const int LENGTH_OF_LARGEST_CONFIG_PARAMETER = 17; //ZCD_NV_USERDESC is largest
        public const int STATUS_FIELD                       = SRSP_PAYLOAD_START;
        public const int CONFIG_ID_FIELD                    = SRSP_PAYLOAD_START + 1;
        public const int LENGTH_FIELD                       = SRSP_PAYLOAD_START + 2;
        public const int START_OF_VALUE_FIELD               = SRSP_PAYLOAD_START + 3;

        //expected Response message types
        public const int NO_RESPONSE_EXPECTED             = 0x00;

        public const int ZNP_NV_RF_TEST_HEADER_LEN        = 4;  //length of RF test parameters header
        public const int ZNP_NV_RF_TEST_PARMS_LEN         = 4;  //length of RF test parameters 


        //simple api
        public const int ZB_APP_REGISTER_REQUEST_PAYLOAD_LEN   = 9;
        public const int ZB_START_CONFIRM_TIMEOUT              = 5;
        public const int ZB_APP_START_REQUEST_PAYLOAD_LEN      = 0;
        public const int ZB_PERMIT_JOINING_REQUEST_PAYLOAD_LEN = 3;
        public const int ZB_SEND_DATA_CONFIRM_TIMEOUT          = 5;
        public const int ZB_SEND_DATA_REQUEST_PAYLOAD_LEN      = 8;

        // af api
        public const int AF_REGISTER_PAYLOAD_LEN               = 9;
        public const int ZDO_STARTUP_FROM_APP_PAYLOAD_LEN      = 1;
        public const int NO_START_DELAY                        = 0;
        public const int AF_DATA_CONFIRM_TIMEOUT               = 2;
        public const int AF_DATA_REQUEST_PAYLOAD_LEN           = 10;
        public const int ZDO_IEEE_ADDR_REQ_PAYLOAD_LEN         = 4;
        public const int SINGLE_DEVICE_RESPONSE                = 0;
        public const int INCLUDE_ASSOCIATED_DEVICES            = 1;
        public const int ZDO_NWK_ADDR_REQ_PAYLOAD_LEN          = 10;

        // UTIL
        public const int UTIL_ADDRMGR_NWK_ADDR_LOOKUP_PAYLOAD_LEN = 2;

        //RF TEST MODES
        public const int RF_TEST_RECEIVE                 = 1;
        public const int RF_TEST_UNMODULATED             = 2;
        public const int RF_TEST_MODULATED               = 3;
        public const int RF_TEST_NONE                    = 0xFF;  //NOTE: this is for the application only, don't send this to the ZNP

        //From Table 1 in cc2530 datasheet, p 21
        public const int RF_OUTPUT_POWER_PLUS_4_5_DBM    = 0xF5;  //+4.5dBm
        public const int RF_OUTPUT_POWER_PLUS_2_5_DBM    = 0xE5;  //+2.5dBm
        public const int RF_OUTPUT_POWER_PLUS_1_0_DBM    = 0xD5;  //+1.0dBm
        public const int RF_OUTPUT_POWER_MINUS_0_5_DBM   = 0xC5;  //-0.5dBm
        public const int RF_OUTPUT_POWER_MINUS_1_5_DBM   = 0xB5;  //-1.5dBm
        public const int RF_OUTPUT_POWER_MINUS_3_0_DBM   = 0xA5;  //-3.0dBm
        public const int RF_OUTPUT_POWER_MINUS_10_0_DBM  = 0x65;  //-10.0dBm

        //used for RF Test Modes:
        public const int RF_TEST_CHANNEL_MIN             = 11;
        public const int RF_TEST_CHANNEL_MAX             = 26;


#endregion

        
        
        public byte[] znpBuf= new byte[100];     // 用于存储异步响应帧
        public bool znpBufReceived = false;       // znpBuf接收到数据

        /** Incremented for each SEND_DATA_REQUEST, wraps around to 0. */
        private static byte sequenceNumber = 0;
        /** Incremented for each AF_DATA_REQUEST, wraps around to 0. */
        private static byte transactionSequenceNumber = 0;

        private static bool ReceiveSRSPTimeOut = false;   // 接收同步响应超时
        private static Timer ReceiveSRSPTimeOutTimer = new Timer(SRSP_TIMEOUT_MS); //同步响应超时为100ms

        private static bool WaitForMsgTimeOut = false;   // 等待特定信息超时
        private static Timer WaitForMsgTimeOutTimer = new Timer(1000); //初始化1s

        public int znpResult = ZNP_SUCCESS;


        private static SerialPort znpCom = new SerialPort();


        /// <summary>
        /// Initialize the ZNP interface
        /// </summary>
        /// <returns>the version information of ZNP</returns>
        public bool znpInit()
        {
            ReceiveSRSPTimeOutTimer = new Timer(SRSP_TIMEOUT_MS); //同步响应超时为100ms
            WaitForMsgTimeOutTimer = new Timer(1000); //初始化1s
            znpCom = new SerialPort();

            string[] PortNameList = SerialPort.GetPortNames();
            
            try
            {
                if (znpCom.IsOpen)
                {
                    znpCom.Close();
                }

                /* 115200-8-N-1 */
                znpCom.BaudRate = 115200;
                znpCom.DataBits = 8;
                znpCom.Parity = Parity.None;
                znpCom.StopBits = StopBits.One;

                znpCom.ReceivedBytesThreshold = 1; //收到1个字节发生中断
                znpCom.DataReceived += znpCom_DataReceived;


                ReceiveSRSPTimeOutTimer.Elapsed += new ElapsedEventHandler(ReceiveSRSPTimeOutTimer_Elapsed); //接收同步响应超时
                ReceiveSRSPTimeOutTimer.Enabled = true;
                ReceiveSRSPTimeOutTimer.AutoReset = true;
                ReceiveSRSPTimeOutTimer.Stop();

                WaitForMsgTimeOutTimer.Elapsed += new ElapsedEventHandler(WaitForMsgTimeOutTimer_Elapsed);   //等待特定信息超时
                WaitForMsgTimeOutTimer.Enabled = true;
                WaitForMsgTimeOutTimer.Stop();

                for (int i = 0; i < PortNameList.Length; i++)
                {
                    znpCom.PortName = PortNameList[i];

                    try
                    {
                        if (!znpCom.IsOpen)
                        {
                            znpCom.Open();
                        }

                        getVersion();
                        if (znpResult == ZNP_SUCCESS)
                        {
                            return true;
                        }
                        else
                        {
                            znpCom.Close();
                        }
                    }
                    catch
                    {

                    }
                }

                return false;
            }
            catch
            {
                return false;
            }

        }


        public string znpComName()
        {
            return znpCom.PortName;
        }


        /// <summary>
        /// Close ZNP Port
        /// </summary>
        /// <returns>true of false</returns>
        public bool znpCOMClose()
        {
            try
            {
                if (znpCom.IsOpen)
                    znpCom.Close();
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// Open ZNP Port
        /// </summary>
        /// <returns>true or false</returns>
        public bool znpCOMOpen()
        {
            try
            {
                if (!znpCom.IsOpen)
                    znpCom.Open();
                return true;
            }
            catch
            {
                return false;
            }
        }

        /// <summary>
        /// get the state of ZNP COM
        /// </summary>
        /// <returns>open or closed</returns>
        public bool znpCOMIsOpen()
        {
            return znpCom.IsOpen;
        }

        /// <summary>
        /// Send SREQ Message
        /// </summary>
        /// <param name="znpTxMsg">the message to send</param>
        /// <returns>contains the error code, or ZNP_SUCCESS if success.</returns>
        private int sendMessage(byte[] znpTxMsg)
        {


            byte expectedSrspCmdMsb = (byte)(znpTxMsg[1] + 0x40);
            byte expectedSrspCmdLsb = znpTxMsg[2];
            int len = znpTxMsg[0] + 3;                 //帧格式：SOF len cmd0 cmd1 Data FCS

            try
            {
                List<byte> TxBuf = new List<byte>();
                TxBuf.Add(0xFE);                       //添加 SOF

                for (int i = 0; i < len; i++)          //添加 len cmd0 cmd1 Data
                {
                    TxBuf.Add(znpTxMsg[i]);
                }

                TxBuf.Add(calcFCS(znpTxMsg, 0, len));  //添加校验FCS

                znpCom.Write(TxBuf.ToArray(), 0, len + 2);

                znpBufReceived = false;             //还未接收
                ReceiveSRSPTimeOut = false;
                ReceiveSRSPTimeOutTimer.Start();  //启动超时定时器

                while (znpBufReceived == false && ReceiveSRSPTimeOut == false);
                  
                if (ReceiveSRSPTimeOut == true) 
                    return SRSP_TIMEOUT;      //发送不成功

                if (znpBuf[0] == 0)
                    return SRSP_FRAMEERROR;       // 错误的SRSP帧

                if ((znpBuf[1] == expectedSrspCmdMsb) && (znpBuf[2] == expectedSrspCmdLsb))
                {
                    znpBufReceived = false;
                    return ZNP_SUCCESS;
                }
                    
                else
                    return SRSP_CMDERROR;        // cmd错误
            }
            catch (Exception)
            {
                return SRSP_UNKNOWERROR;         // 未知错误
            }
        }

        /// <summary>
        /// Receive SRSP timeout interrupt
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void ReceiveSRSPTimeOutTimer_Elapsed(object sender, System.Timers.ElapsedEventArgs e)
        {
            ReceiveSRSPTimeOut = true;
            ReceiveSRSPTimeOutTimer.Stop();
        }

        /// <summary>
        /// SerialPort Receive Interrupt.If received 4 bytes,then this event happens.
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        /// 
        private const byte SOP_STATE  = 0x00;
        private const byte CMD_STATE1 = 0x01;
        private const byte CMD_STATE2 = 0x02;
        private const byte LEN_STATE  = 0x03;
        private const byte DATA_STATE = 0x04;
        private const byte FCS_STATE  = 0x05;

        private byte ReceivedState = SOP_STATE;
        private byte[] CMD_Token = new byte[2];
        private byte LEN_Token;
        private byte FCS_Token;
        private byte tempDataLen;
        private byte[] RxMsg = new byte[100]; 
        private void znpCom_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            
            int n = znpCom.BytesToRead;
            byte[] RxBuf = new byte[n];

            znpCom.Read(RxBuf, 0, n);

            for (int i = 0; i < n; i++)
            {
                switch (ReceivedState)
                {
                    case SOP_STATE:
                        if (RxBuf[i] == 0xFE)
                            ReceivedState = LEN_STATE;
                        break;

                    case LEN_STATE:
                        LEN_Token = RxBuf[i];

                        tempDataLen = 0;

                        RxMsg[SRSP_LENGTH_FIELD] = LEN_Token;
                        ReceivedState = CMD_STATE1;
                        break;

                    case CMD_STATE1:
                        RxMsg[SRSP_CMD_MSB_FIELD] = RxBuf[i];
                        ReceivedState = CMD_STATE2;
                        break;

                    case CMD_STATE2:
                        RxMsg[SRSP_CMD_LSB_FIELD] = RxBuf[i];
                        if (LEN_Token > 0)
                        {
                            ReceivedState = DATA_STATE;
                        }
                        else
                        {
                            ReceivedState = FCS_STATE;
                        }
                        break;

                    case DATA_STATE:
                        /* Fill in the buffer the first byte of the data */
                        RxMsg[SRSP_PAYLOAD_START + tempDataLen++] = RxBuf[i];

                        /* If number of bytes read is equal to data length, time to move on to FCS */
                        if (tempDataLen == LEN_Token)
                        {
                            ReceivedState = FCS_STATE;
                        }
                        break;

                    case FCS_STATE:
                        FCS_Token = RxBuf[i];
                        /* Make sure it's correct */
                        if (calcFCS(RxMsg, SRSP_LENGTH_FIELD, SRSP_HEADER_SIZE + LEN_Token) == FCS_Token)
                        {
                            Array.Copy(RxMsg, znpBuf, SRSP_HEADER_SIZE + LEN_Token);
                            znpBufReceived = true;
                        }
                        ReceivedState = SOP_STATE;
                        break;
                }
            }
        }
            

        //计算FCS
        /// <summary>
        /// calculate FCS
        /// </summary>
        /// <param name="pMsg">a source array</param>
        /// <param name="offset">start position of source array</param>
        /// <param name="len">length of the array</param>
        /// <returns>the FCS</returns>
        private byte calcFCS(byte[] pMsg, int offset, int len)
        {
            byte result = 0;
            for (int i = offset; i < len; i++)
            {
                result ^= pMsg[i];
            }
            return result;
        }



        /// <summary>
        /// This method is used to restart the ZNP's internal state machine and apply changes to startup options, zigbee device type, etc.
        /// This command is currently only used for the RF tester example
        /// see Interface Specification for order of fields
        /// Resets the ZNP using soft reset command.
        /// </summary>
        public int znpSoftReset()
        {
            byte[] buf = new byte[SYS_RESET_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_RESET_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_RESET_REQ & 0xFF00) >> 8;
            buf[2] = Commands.SYS_RESET_REQ & 0xFF;
            znpResult = sendMessage(buf);

            return (waitForMessage(Commands.SYS_RESET_IND, 5));

        }

       
        /// <summary>
        /// Retrieves the ZNP's version information using SYS_VERSION command. See Interface Specification for order of fields.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <returns>a array to the beginning of the version structure, or a array to indeterminate data if error.</returns>
        public byte[] getVersion()
        {
            byte[] buf = new byte[SYS_VERSION_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_VERSION_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_VERSION & 0xFF00) >> 8;
            buf[2] = Commands.SYS_VERSION & 0xFF;

            znpResult = sendMessage(buf);

            byte[] result = new byte[znpBuf[0]];
            if (znpResult == ZNP_SUCCESS)
            {
                Array.ConstrainedCopy(znpBuf, SRSP_PAYLOAD_START, result, 0, znpBuf[0]);
            }
            return result;
        }

        /// <summary>
        /// Retrieves a random number from the ZNP using SYS_RANDOM command.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <returns>the random number, or indeterminate data if error.</returns>
        public uint getRandom()
        {
            byte[] buf = new byte[SYS_RANDOM_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_RANDOM_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_RANDOM & 0xFF00) >> 8;
            buf[2] = Commands.SYS_RANDOM & 0xFF;

            znpResult = sendMessage(buf);

            return ((uint)(znpBuf[SRSP_PAYLOAD_START] + znpBuf[SRSP_PAYLOAD_START + 1] * 0x0100));
        }

        /// <summary>
        /// Sets the RF Output power.
        /// </summary>
        /// <param name="txPowerDb">Tx Output Power, in dB. e.g. -10 = -10dB, 3 = +3dB etc.</param>
        public void setTransmitPower(sbyte txPowerDb)
        {
            byte[] buf = new byte[SYS_STACK_TUNE_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_STACK_TUNE_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_STACK_TUNE & 0xFF00) >> 8;
            buf[2] = Commands.SYS_STACK_TUNE & 0xFF;

            buf[3] = STACK_TX_POWER;
            buf[4] = (byte)txPowerDb;

            znpResult = sendMessage(buf);
        }

        /// <summary>
        /// Retrieves the specified Device Information Property (DIP) from the ZNP.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="dip">the device information property to retrieve</param>
        /// <returns>a array to the beginning of the DIP payload, or a array to indeterminate data if error.</returns>
        public byte[] getDeviceInformationProperty(byte dip)
        {
            Debug.Assert( dip <= MAX_DEVICE_INFORMATION_PROPERTY);

            byte[] buf = new byte[ZB_GET_DEVICE_INFO_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_GET_DEVICE_INFO_PAYLOAD_LEN;
            buf[1] = (Commands.ZB_GET_DEVICE_INFO & 0xFF00) >> 8;
            buf[2] = Commands.ZB_GET_DEVICE_INFO & 0xFF;
            buf[3] = dip;

            znpResult = sendMessage(buf);

            byte[] result = new byte[znpBuf[0]-1];
            if (znpResult == ZNP_SUCCESS)
            {
                Array.ConstrainedCopy(znpBuf, SRSP_PAYLOAD_START + 1, result, 0, znpBuf[0] - 1);
            }
            return result;
        }


        /// <summary>
        /// Retrieves the Media Access Controller (MAC) Address (aka Long Address, aka IEEE Address)
        /// The MAC Address is an eight byte globally unique serial number for this IC.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <returns>a array to the beginning of the MAC address (LSB first), or a array to indeterminate data if error.</returns>
        public byte[] getMacAddress()
        {
            return (getDeviceInformationProperty(DIP_MAC_ADDRESS));
        }


        /// <summary>
        /// Retrieves the device state - indicates whether it is on the network or not.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <returns>the device state: DEV_HOLD, DEV_NWK_DISC, DEV_ROUTER etc. or an indeterminate value if error.</returns>
        public byte getDeviceState()
        {
            getDeviceInformationProperty(DIP_STATE);
            return znpBuf[SRSP_HEADER_SIZE + 1];
        }


        /// <summary>
        /// Retrieves the name of the device state in a human-readable format. From API Specification.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="state">the value for the ZDO state</param>
        /// <returns>the name of the ZDO state</returns>
        public string getDeviceStateName(byte state)
        {
            switch (state)
            {
                case DEV_HOLD: 
                    return ("DEV_HOLD");
                case DEV_INIT: 
                    return ("DEV_INIT");
                case DEV_NWK_DISC: 
                    return ("DEV_NWK_DISC");
                case DEV_NWK_JOINING: 
                    return ("DEV_NWK_JOINING");
                case DEV_NWK_REJOIN: 
                    return ("DEV_NWK_REJOIN");
                case DEV_END_DEVICE_UNAUTH: 
                    return ("DEV_END_DEVICE_UNAUTH");
                case DEV_END_DEVICE: 
                    return ("DEV_END_DEVICE");
                case DEV_ROUTER: 
                    return ("DEV_ROUTER");
                case DEV_COORD_STARTING: 
                    return ("DEV_COORD_STARTING");
                case DEV_ZB_COORD: 
                    return ("DEV_ZB_COORD");
                case DEV_NWK_ORPHAN: 
                    return ("DEV_NWK_ORPHAN");
                default: 
                    return ("Unknown State");
            }
        }

        /// <summary>
        /// Displays the radio's device Information Properties. Device Information Properties include: 
        /// Device State indicates whether the ZNP is on a network or not, and what type of device it is. 
        /// This is a handy thing to check if things aren't operating correctly. 
        /// If the device is starting as a coordinator, you'll see states of 01, 08, 08, then 09 once it has fully started.
        /// - MAC Address (aka IEEE Address) is a globally unique serial number for this IC.
        /// - Device Short Address is a network address assigned by the coordinator, similar to an IP Address in DHCP. 
        /// The Coordinator always has a Short Address of 0.
        /// - Parent MAC Address is the IEEE Address of this device's "parent", i.e. which device was used to join the network. 
        /// For a router, once joined this parent MAC address is irrelevant. This DIP will NOT be updated if the network reforms.
        /// For an end-device then this parent MAC address will always specify which router the end-device is joined to.
        /// - Channel is which frequency channel the device is operating on.
        /// - PAN ID (Personal Area Network Identifier) of the network is a unique number shared for all devices on the same network.
        /// - Extended PAN ID of the network is the coordinator's MAC Address.
        /// 
        /// If device is not connected to a network then the Short Address fields will be 0xFEFF, 
        /// the Parent MAC Address and channel will be 0, and the Extended PAN ID will be this device's MAC Address.
        /// </summary>
        /// <returns>output string</returns>
        public string getDeviceInformation()
        {
            StringBuilder outputStr = new StringBuilder();
            outputStr.Append("Device Information Properties (MSB first)\r\n");
            getDeviceInformationProperty(DIP_STATE);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append(string.Format("    Device State:               {0} ({1})\r\n", getDeviceStateName(znpBuf[SRSP_DIP_VALUE_FIELD]), (znpBuf[SRSP_DIP_VALUE_FIELD])));

            getDeviceInformationProperty(DIP_MAC_ADDRESS);
            outputStr.Append("    MAC Address:                ");
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            for (int i = SRSP_DIP_VALUE_FIELD + 7; i >= SRSP_DIP_VALUE_FIELD; i--)
            {
                outputStr.Append(znpBuf[i].ToString("X2") + " ");
            }
            outputStr.Append("\r\n");

            getDeviceInformationProperty(DIP_SHORT_ADDRESS);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append(string.Format("    Short Address:              {0}\r\n", (znpBuf[SRSP_DIP_VALUE_FIELD + 1] * 0x0100 + znpBuf[SRSP_DIP_VALUE_FIELD]).ToString("X4")));

            getDeviceInformationProperty(DIP_PARENT_SHORT_ADDRESS);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append(string.Format("    Parent Short Address:       {0}\r\n", (znpBuf[SRSP_DIP_VALUE_FIELD + 1] * 0x0100 + znpBuf[SRSP_DIP_VALUE_FIELD]).ToString("X4")));

            getDeviceInformationProperty(DIP_PARENT_MAC_ADDRESS);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append("    Parent MAC Address:         ");
            for (int i = SRSP_DIP_VALUE_FIELD + 7; i >= SRSP_DIP_VALUE_FIELD; i--)
            {
                outputStr.Append(znpBuf[i].ToString("X2") + " ");
            }
            outputStr.Append("\r\n");

            getDeviceInformationProperty(DIP_CHANNEL);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append(string.Format("    Device Channel:             {0}\r\n", znpBuf[SRSP_DIP_VALUE_FIELD]));

            getDeviceInformationProperty(DIP_PANID);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append(string.Format("    PAN ID:                     {0}\r\n", (znpBuf[SRSP_DIP_VALUE_FIELD + 1] * 0x0100 + znpBuf[SRSP_DIP_VALUE_FIELD]).ToString("X4")));

            getDeviceInformationProperty(DIP_EXTENDED_PANID);
            if (znpResult != ZNP_SUCCESS) return outputStr.ToString();
            outputStr.Append("    Extended PAN ID:            ");
            for (int i = SRSP_DIP_VALUE_FIELD + 7; i >= SRSP_DIP_VALUE_FIELD; i--)
            {
                outputStr.Append(znpBuf[i].ToString("X2") + " ");
            }
            outputStr.Append("\r\n");

            return outputStr.ToString();
        }


        /// <summary>
        /// Performs the specified General Purpose Input/Output (GPIO) operation.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="operation">which GPIO operation to do: GPIO_SET_DIRECTION, GPIO_SET_INPUT_MODE, GPIO_SET, GPIO_CLEAR, GPIO_TOGGLE, GPIO_READ</param>
        /// <param name="value">the value to write</param>
        /// <returns>the value returned by the ZNP if operation = GPIO_READ; indeterminate value otherwise</returns>
        public byte sysGpio(byte operation, byte value)
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
            byte[] buf = new byte[SYS_GPIO_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_GPIO_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_GPIO & 0xFF00) >> 8;
            buf[2] = Commands.SYS_GPIO & 0xFF;

            buf[3] = operation;
            buf[4] = value;

            znpResult = sendMessage(buf);
            return znpBuf[SRSP_PAYLOAD_START];
        }


        /// <summary>
        /// Retrieves the name of the device type in a human-readable format, e.g. Coordinator, Router, or End Device.
        /// </summary>
        /// <param name="deviceType">the type of device</param>
        /// <returns>the name of the deviceType</returns>
        public string getDeviceTypeName(byte deviceType)
        {
            switch (deviceType)
            {
                case COORDINATOR: 
                    return ("Coordinator");
                case ROUTER: 
                    return ("Router");
                case END_DEVICE: 
                    return ("End Device");
                default: 
                    return ("Unknown");
            }
        }


        /// <summary>
        /// Sets the Zigbee Device Type for the ZNP.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="deviceType">the type of Zigbee device. Must be COORDINATOR, ROUTER, or END_DEVICE </param>
        public void setZigbeeDeviceType(byte deviceType)
        {
            if (deviceType > END_DEVICE)
            {
                znpResult = -1;
                return;
            }
            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_LOGICAL_TYPE_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_LOGICAL_TYPE_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_LOGICAL_TYPE;
            buf[4] = ZCD_NV_LOGICAL_TYPE_LEN;
            buf[5] = deviceType;

            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Configures the ZNP to only join a network with the given panId. 
        /// If panId = ANY_PAN then the radio will join any network.
        /// </summary>
        /// <param name="panId">the PANID to join, or ANY_PAN to join any PAN</param>
        public void setPanId(uint panId)
        {
            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PANID_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PANID_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_PANID;
            buf[4] = ZCD_NV_PANID_LEN;
            buf[5] = (byte)(panId & 0xFF);
            buf[6] = (byte)((panId & 0xFF00) >> 8);

            znpResult = sendMessage(buf);
        }

        /// <summary>
        /// Configures the ZNP to only join the specified channel.
        /// Wrapper for setChannelMask if only one channel is desired.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="channel">must be 11..26, inclusive. If set to 0 then ZNP will scan ALL CHANNELS</param>
        public void setChannel(byte channel)
        {
            if ((!IS_VALID_CHANNEL(channel)) && (channel != 0x00))
            {
                znpResult = -10;
                return;
            }
            uint channelMask = 1;
            setChannelMask((channel == 0) ? ANY_CHANNEL : (channelMask << channel)); //will set znpResult on success/fail
            
        }


        /// <summary>
        /// Configures the ZNP to only join a network according to the given channel mask. 
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="channelMask">
        /// bitmap of which channels to use. If channelMask = ANY_CHANNEL then the radio will join a network on any channel. 
        /// Default channel after a CLEAR_CONFIG is channel 11, or (MSB first) 0x00000800.
        /// Note:if using a packet sniffer, be sure to change the channel on the packet sniffer too or else you won't see any packets.
        /// </param>
        public void setChannelMask(uint channelMask)
        {
            if ((channelMask < MIN_CHANNEL) || (channelMask > MAX_CHANNEL))
            {
                znpResult = -100;
                return;
            }
            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_CHANLIST_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_CHANLIST_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_CHANLIST;
            buf[4] = ZCD_NV_CHANLIST_LEN;
            buf[5] = (byte)(channelMask & 0xFF);
            buf[6] = (byte)((channelMask & 0xFF00) >> 8);
            buf[7] = (byte)((channelMask & 0xFF0000) >> 16);
            buf[8] = (byte)(channelMask >> 24);

            znpResult = sendMessage(buf);
        }

        /// <summary>
        /// Configures startup options on the ZNP. These will reset various parameters back to their factory defaults. 
        /// The radio supports two types of clearing state, and both are supported:
        /// - STARTOPT_CLEAR_CONFIG restores all settings to factory defaults. Must restart the ZNP after using this option.
        /// - STARTOPT_CLEAR_STATE only clears network settings (PAN ID, channel, etc.)
        /// note: all ZB_WRITE_CONFIGURATION commands take approx. 3.5mSec between SREQ & SRSP; presumably to write to flash inside the CC2530ZNP.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="option">which options to set. Must be zero, STARTOPT_CLEAR_CONFIG, or STARTOPT_CLEAR_STATE.</param>
        public void setStartupOptions(byte option)
        {
            if (option > (STARTOPT_CLEAR_CONFIG + STARTOPT_CLEAR_STATE))
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_STARTUP_OPTION_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_STARTUP_OPTION_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;
            buf[3] = option;

            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Enable/Disabless callbacks on the ZNP. 
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// see section ZCD_NV_ZDO_DIRECT_CB in ZNP Interface Specification
        /// </summary>
        /// <param name="cb">must be either CALLBACKS_ENABLED or CALLBACKS_DISABLED</param>
        public void setCallbacks(byte cb)
        {
            if ((cb != CALLBACKS_ENABLED) && (cb != CALLBACKS_DISABLED))
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_ZDO_DIRECT_CB;
            buf[4] = ZCD_NV_ZDO_DIRECT_CB_LEN;
            buf[5] = cb;
            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Sets the ZCD_NV_POLL_RATE, which is used by end devices and controls how often an end device
        /// polls its associated router for new data. Default is 2000 (2 seconds). 
        /// Higher numbers will increase latency but reduce average power consumption.
        /// see section ZCD_NV_POLL_RATE in ZNP Interface Specification
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="pollRate">how often (in mSec) to poll; must be between 1 to 65000, inclusive.</param>
        public void setPollRate(uint pollRate)
        {
            if((pollRate == 0) || (pollRate > 65000))
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_ZDO_DIRECT_CB_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_POLL_RATE;
            buf[4] = ZCD_NV_POLL_RATE_LEN;
            buf[5] = (byte)(pollRate & 0xFF);
            buf[6] = (byte)((pollRate & 0xFF00)>>8);

            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Retrieves the name of the security mode in a human-readable format. From API Specification.
        /// </summary>
        /// <param name="securityMode">SECURITY_MODE_OFF or SECURITY_MODE_PRECONFIGURED_KEYS or SECURITY_MODE_COORD_DIST_KEYS</param>
        /// <returns>the name of the securityMode</returns>
        public string getSecurityModeName(byte securityMode)
        {
            switch (securityMode)
            {
                case SECURITY_MODE_OFF: 
                    return ("OFF");
                case SECURITY_MODE_PRECONFIGURED_KEYS: 
                    return ("USE PRECONFIGURED KEY");
                case SECURITY_MODE_COORD_DIST_KEYS: 
                    return ("COORDINATOR DISTRIBUTE KEY");
                default: 
                    return ("Unknown");
            }
        }

        /// <summary>
        /// Configures the ZNP for the specified security mode.
        /// If securityMode = SECURITY_MODE_OFF then only ZCD_NV_SECURITY_MODE is written. 
        /// Otherwise ZCD_NV_SECURITY_MODE and ZCD_NV_PRECFGKEYS_ENABLE are written.
        /// note: if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="securityMode">SECURITY_MODE_OFF or SECURITY_MODE_PRECONFIGURED_KEYS or SECURITY_MODE_COORD_DIST_KEYS</param>
        public void setSecurityMode(byte securityMode)
        {
            if (securityMode > SECURITY_MODE_COORD_DIST_KEYS)
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_SECURITY_MODE_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_SECURITY_MODE_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

            buf[3] = ZCD_NV_SECURITY_MODE;
            buf[4] = ZCD_NV_SECURITY_MODE_LEN;
            buf[5] = (byte)(securityMode > 0? 1:0);

            znpResult = sendMessage(buf);
            if (znpResult != ZNP_SUCCESS)
                return;

            if (securityMode != SECURITY_MODE_OFF)   //if turning security off, don't need to set pre-configured keys on/off
            {
                //2. Configure pre-configured keys on/off:
                byte[] buf1 = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEYS_ENABLE_LEN + SRSP_HEADER_SIZE];

                buf1[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEYS_ENABLE_LEN;
                buf1[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
                buf1[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;

                buf1[3] = ZCD_NV_PRECFGKEYS_ENABLE;
                buf1[4] = ZCD_NV_PRECFGKEYS_ENABLE_LEN;
                buf1[5] = (byte)(securityMode == SECURITY_MODE_PRECONFIGURED_KEYS? 1:0);

                znpResult = sendMessage(buf1);
            }
        }


        /// <summary>
        /// Loads the specified key into the ZNP. Does not change security mode, need to use setSecurityMode() too.
        /// note: if NOT using pre-configured keys then the coordinator will distribute its key to all devices.
        /// setSecurityMode() called
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="key">preConfiguredKey a 16B key to use</param>
        public void setSecurityKey(byte[] key)
        {
            byte[] buf = new byte[ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEY_LEN + SRSP_HEADER_SIZE];
            buf[0] = ZB_WRITE_CONFIGURATION_LEN + ZCD_NV_PRECFGKEY_LEN;
            buf[1] = (Commands.ZB_WRITE_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_WRITE_CONFIGURATION & 0xFF;  
    
            buf[3] = ZCD_NV_PRECFGKEY;
            buf[4] = ZCD_NV_PRECFGKEY_LEN;
    
            for (int i=5; i<(ZCD_NV_PRECFGKEY_LEN+5); i++)
                buf[i] = key[i];            
            znpResult = sendMessage(buf);
        }



        /// <summary>
        /// Retrieves the given configuration parameter from the ZNP.
        /// note: does not verify configId is a valid configId
        /// note: does not verify length returned in the SRSP matches what the length should be for that configId.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="configId">the configuration parameter to retrieve. See Interface Spec for list of valid config parameters. Not checked for validity.</param>
        /// <returns>array to beginning of configuration parameter</returns>
        public byte[] getConfigurationParameter(byte configId)
        {
            byte[] buf = new byte[ZB_READ_CONFIGURATION_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_READ_CONFIGURATION_PAYLOAD_LEN;
            buf[1] = (Commands.ZB_READ_CONFIGURATION & 0xFF00) >> 8;
            buf[2] = Commands.ZB_READ_CONFIGURATION & 0xFF;

            buf[3] = configId;

            znpResult = sendMessage(buf);

            byte[] result = new byte[znpBuf[LENGTH_FIELD]];
            //when using SPI, znpBuf[3] holds status, [4] holds configId, [5] holds length
            if ((buf[STATUS_FIELD] != SRSP_STATUS_SUCCESS) ||                  //if status was success
                (buf[CONFIG_ID_FIELD] != configId) ||                          //if the requested configId was what we requested
                (buf[LENGTH_FIELD] > LENGTH_OF_LARGEST_CONFIG_PARAMETER) ||    //if the length is valid
                (buf[LENGTH_FIELD] == 0))
            {
                znpResult = -1;              //error
            }
            else
            {
                Array.ConstrainedCopy(znpBuf, START_OF_VALUE_FIELD, result, 0, znpBuf[LENGTH_FIELD]);
            }
            return result;
        }



        /// <summary>
        /// Utility method for displaying nice reset reason from a SYS_RESET_IND message.
        /// </summary>
        /// <param name="reason">the reset reason</param>
        /// <returns>a string containing the name of the reset reason.</returns>
        public string getResetReason(byte reason)
        {
            switch (reason)
            {
    
                case 0:     return "Power-up";
                case 1:     return "External";
                case 2:     return "Watch-dog";
                default:    return "Unknown";
            }
        }


        /// <summary>
        /// Waits for the specified type of message. Silently discards any other messages received.
        /// Received message will be in znpBuf[]. If the specified type of message isn't received then returns error code.
        /// </summary>
        /// <param name="messageType"></param>
        /// <param name="timeoutSecs"></param>
        /// <returns>0 if success, -21 if timeout, -31 if received a message but it wasn't the type we're expecting.</returns>
        public int waitForMessage(uint messageType, byte timeoutSecs)
        {
            WaitForMsgTimeOut = false;
            WaitForMsgTimeOutTimer.Interval = timeoutSecs * 1000;

            WaitForMsgTimeOutTimer.Start();
            while (znpBufReceived == false && WaitForMsgTimeOut == false) ;

            if (WaitForMsgTimeOut == true)
            {
                return -21;
            }
            if ((znpBuf[SRSP_LENGTH_FIELD] > 0) && (znpBuf[2] + (znpBuf[1]<<8)) == messageType)
            {
                return 0;
            }
            else
            {
                return -31;
            }
        }
        private void WaitForMsgTimeOutTimer_Elapsed(object sender, ElapsedEventArgs e)
        {
            WaitForMsgTimeOut = true;
            WaitForMsgTimeOutTimer.Stop();
        }


        /// <summary>
        /// Retrieves the size of the selected Non-Volatile (NV) memory item.
        /// NV Items 1 through 4 are 2 bytes each; 5 & 6 are 16 bytes each.
        /// </summary>
        /// <param name="nvItem">which Non-Volatile memory item. Must be 1-6 inclusive.</param>
        /// <returns>size of the given nvItem</returns>
        public byte getNvItemSize(byte nvItem)
        {
            return (byte)((nvItem < 5) ? 2 : 16);  //items 1-4 have size=2; items 5,6 have size=16
        }


        /// <summary>
        /// Writes the specified Non-Volatile (NV) memory item to the ZNP. 
        /// The contents of the selected nvItem will be overwritten from memory starting at data.
        /// ZNP was initialized.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="nvItem">which nvItem to write, 1 through 6 inclusive</param>
        /// <param name="data">the data to write</param>
        public void writeNvItem(byte nvItem, byte[] data)
        {
            if ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM))
            {
                znpResult = -1;
                return;
            }
            byte nvItemSize = getNvItemSize(nvItem);

            byte[] buf = new byte[SYS_OSAL_NV_WRITE_PAYLOAD_LEN + nvItemSize + SRSP_HEADER_SIZE];
            buf[0] = (byte)(SYS_OSAL_NV_WRITE_PAYLOAD_LEN + nvItemSize);
            buf[1] = (Commands.SYS_OSAL_NV_WRITE & 0xFF00) >> 8;
            buf[2] = Commands.SYS_OSAL_NV_WRITE & 0xFF;
  
            buf[3] = nvItem;         //item number, 1-6
            buf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
            buf[5] = 0;              //offset from beginning of the NV item, not used
            buf[6] = nvItemSize;     //length

            Array.ConstrainedCopy(data, 0, buf, 7, nvItemSize);

            znpResult = sendMessage(buf);    
        }


        /// <summary>
        /// Retrieves the specified Non-Volatile (NV) memory item from the ZNP. 
        /// ZNP was initialized.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// note: does not verify the length of the returned nvItem was what we expected
        /// </summary>
        /// <param name="nvItem">which nvItem to write, 1 through 6 inclusive</param>
        /// <returns>a array to the beginning of the NV item retrieved, or a array to indeterminate data if error.</returns>
        public byte[] readNvItem(byte nvItem)
        {
            if ((nvItem < MIN_NV_ITEM) || (nvItem > MAX_NV_ITEM))
            {
                znpResult = -1;
                return (byte[])null;
            }
            byte[] buf = new byte[SYS_OSAL_NV_READ_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = SYS_OSAL_NV_READ_PAYLOAD_LEN;
            buf[1] = (Commands.SYS_OSAL_NV_READ & 0xFF00) >> 8;
            buf[2] = Commands.SYS_OSAL_NV_READ & 0xFF;  
    
            buf[3] = nvItem;         //item number, 1-6
            buf[4] = 0x0F;           //MSB of item number, but only items 1-6 are supported
            buf[5] = 0;              //offset from beginning of the NV item, not used
    
            znpResult = sendMessage(buf);   
    
            if (znpResult != 0)
                return (byte[])null;
            
            if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS)  
            {
                znpResult = -2;
                return (byte[])null;
            }
            else
            {
                byte[] result = new byte[SYS_OSAL_NV_READ_PAYLOAD_LEN - 2];
                Array.ConstrainedCopy(znpBuf, SRSP_PAYLOAD_START + 2, result, 0, SYS_OSAL_NV_READ_PAYLOAD_LEN - 2);

                return result;
            }
}


        /// <summary>
        /// Writes the specified Non-Volatile (NV) memory RF test parameters to the ZNP. 
        /// The settings in the test parameter location takes effect after a reset. The 
        /// only way to exit test mode is a HW reset.
        /// </summary>
        /// <param name="mode">which mode to use, modulated, unmodulated or Rx</param>
        /// <param name="channel">test tone</param>
        /// <param name="power"></param>
        /// <returns>znpResult contains the error code, or ZNP_SUCCESS if success.</returns>
        public int writeRfTestParam(byte mode, byte channel, byte power)
        {
            byte[] buf = new byte[ZNP_NV_RF_TEST_HEADER_LEN + ZNP_NV_RF_TEST_PARMS_LEN + SRSP_HEADER_SIZE];


            buf[0] = ZNP_NV_RF_TEST_HEADER_LEN + ZNP_NV_RF_TEST_PARMS_LEN;
            buf[1] = (Commands.SYS_OSAL_NV_WRITE & 0xFF00) >> 8;
            buf[2] = Commands.SYS_OSAL_NV_WRITE & 0xFF;

            buf[3] = ZNP_NV_RF_TEST_PARMS & 0xFF;
            buf[4] = (ZNP_NV_RF_TEST_PARMS & 0xFF00) >> 8; 
            buf[5] = 0;                            //offset from beginning of the NV item, not used
            buf[6] = ZNP_NV_RF_TEST_PARMS_LEN;     //length
            buf[7] = mode;
            buf[8] = channel;
            buf[9] = power;
            buf[10] = 0x07;                         // Test tone, see CC2530 UG for explaination
            znpResult = sendMessage(buf);    

            znpSoftReset();

            return znpResult;
        }

        /// <summary>
        /// Creates an application configuration for a Coordinator.
        /// Modify the values in this for your application.
        /// </summary>
        /// <returns>the applicationConfiguration for a Coordinator</returns>
        public applicationConfiguration getApplicationConfigurationForCoordinator()
        {
            applicationConfiguration ac = new applicationConfiguration();

            ac.endPoint                      = applicationConfiguration.DEFAULT_ENDPOINT;       //must be same for all devices on the network
            ac.profileId                     = applicationConfiguration.DEFAULT_PROFILE_ID;     //must be same for all devices on the network
            ac.deviceId                      = applicationConfiguration.ZNP_DEVICE_ID;          //doesn't matter
            ac.deviceVersion                 = applicationConfiguration.DEVICE_VERSION;         //doesn't matter
            ac.latencyRequested              = applicationConfiguration.LATENCY_NORMAL;
            ac.numberOfBindingInputClusters  = 1;
            ac.bindingInputClusters[0]       = 0x0001;
            ac.numberOfBindingOutputClusters = 0;

            return ac;
        }


        /// <summary>
        /// Creates an application configuration for a Router. 
        /// Modify the values in this for your application.
        /// </summary>
        /// <returns>the applicationConfiguration for a router</returns>
        public applicationConfiguration getApplicationConfigurationForRouter()
        {
            applicationConfiguration ac      = new applicationConfiguration();

            ac.endPoint                      = applicationConfiguration.DEFAULT_ENDPOINT;       //must be same for all devices on the network
            ac.profileId                     = applicationConfiguration.DEFAULT_PROFILE_ID;     //must be same for all devices on the network
            ac.deviceId                      = applicationConfiguration.ZNP_DEVICE_ID;              //doesn't matter
            ac.deviceVersion                 = applicationConfiguration.DEVICE_VERSION;         //doesn't matter
            ac.latencyRequested              = applicationConfiguration.LATENCY_NORMAL;
            ac.numberOfBindingInputClusters  = 0;
            ac.numberOfBindingOutputClusters = 1; //1 
            ac.bindingOutputClusters[0]      = 0x0001;

            return ac;
        }


        /// <summary>
        /// Displays the applicationConfiguration in a human readable format
        /// </summary>
        /// <param name="ac">the applicationConfiguration to print</param>
        /// <returns>the string to display</returns>
        public string printApplicationConfiguration(applicationConfiguration ac)
        {
            StringBuilder outputStr = new StringBuilder();

            outputStr.Append("Application Configuration:\r\n");
            outputStr.Append(string.Format("    endpoint = 0x{0}\r\n", ac.endPoint.ToString("X2")));
            outputStr.Append(string.Format("    profileId = 0x{0}\r\n", ac.profileId.ToString("X4")));
            outputStr.Append(string.Format("    deviceId = 0x{0}\r\n", ac.deviceId.ToString("X4")));
            outputStr.Append(string.Format("    deviceVersion = 0x{0}\r\n", ac.deviceVersion.ToString("X4")));
            outputStr.Append("    latencyRequested = ");

            switch (ac.latencyRequested)
            {
                case applicationConfiguration.LATENCY_NORMAL:
                    outputStr.Append("NORMAL");
                    break;
                case applicationConfiguration.LATENCY_FAST_BEACONS:
                    outputStr.Append("FAST_BEACONS");
                    break;
                case applicationConfiguration.LATENCY_SLOW_BEACONS:
                    outputStr.Append("SLOW_BEACONS");
                    break;
            }
            outputStr.Append("\r\n");
            outputStr.Append(string.Format("    Number of Binding Input Clusters = {0} ", ac.numberOfBindingInputClusters));
            if (ac.numberOfBindingInputClusters > 0)
            {
                outputStr.Append("{");
                for (int i = 0; i < ac.numberOfBindingInputClusters; i++)
                {
                    outputStr.Append(string.Format("0x{0} ", ac.bindingInputClusters[i].ToString("X2")));
                }
                outputStr.Append("}");
            }

            outputStr.Append("\r\n");
            outputStr.Append(string.Format("    Number of Binding Output Clusters = {0} ", ac.numberOfBindingOutputClusters));
            if (ac.numberOfBindingOutputClusters > 0)
            {
                outputStr.Append("{");
                for (int i = 0; i < ac.numberOfBindingOutputClusters; i++)
                {
                    outputStr.Append(string.Format("0x{0} ", ac.bindingOutputClusters[i].ToString("X2")));
                }
                outputStr.Append("}");
            }
            outputStr.Append("\r\n");

            return outputStr.ToString();
        }

        /// <summary>
        /// Configures the ZNP for our application.
        /// Sets which endpoint, profileId, etc. we're using as well as binding information.
        /// note: Simple API can only register one application.
        /// ZNP was initialized and ZCD_NV_LOGICAL_TYPE has been set (either COORDINATOR/ROUTER/END_DEVICE).
        /// can now use sapiStartApplication()
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// see applicationConfiguration
        /// </summary>
        /// <param name="ac">the applicationConfiguration to register.</param>
        public void sapiRegisterApplication(applicationConfiguration ac)
        {
            if (ac.endPoint == 0)
            {
                znpResult = -1;
                return;
            }
            if ((ac.numberOfBindingInputClusters > applicationConfiguration.MAX_BINDING_CLUSTERS) || (ac.numberOfBindingOutputClusters > applicationConfiguration.MAX_BINDING_CLUSTERS))
            {
                znpResult = -2;
                return;
            }

            byte[] buf = new byte[10 + 2 + 2 * (ac.numberOfBindingInputClusters + ac.numberOfBindingOutputClusters)];

            byte bufferIndex;

            //buf[0] (length) will be set later
            buf[1] = (Commands.ZB_APP_REGISTER_REQUEST & 0xFF00) >> 8;
            buf[2] = Commands.ZB_APP_REGISTER_REQUEST & 0xFF;

            buf[3] = ac.endPoint;
            buf[4] = (byte)(ac.profileId & 0xFF);
            buf[5] = (byte)((ac.profileId & 0xFF00) >> 8);
            buf[6] = (byte)(ac.deviceId & 0xFF);
            buf[7] = (byte)((ac.deviceId & 0xFF00) >> 8);
            buf[8] = ac.deviceVersion;
            buf[9] = 0x00;                                //unused, set to zero
            buf[10] = ac.numberOfBindingInputClusters;
            bufferIndex = 11;

            for (int i = 0; i < ac.numberOfBindingInputClusters; i++)
            {
                buf[bufferIndex++] = (byte)(ac.bindingInputClusters[i] & 0xFF);
                buf[bufferIndex++] = (byte)((ac.bindingInputClusters[i] & 0xFF00)>>8);
            }
            buf[bufferIndex++] = ac.numberOfBindingOutputClusters;
            for (int i = 0; i < ac.numberOfBindingOutputClusters; i++)
            {
                buf[bufferIndex++] = (byte)(ac.bindingOutputClusters[i] & 0xFF);
                buf[bufferIndex++] = (byte)((ac.bindingOutputClusters[i] & 0xFF00) >> 8);
            }
            buf[0] = bufferIndex;

            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Configures the ZNP for a "generic" application: one endpoint, no binding or fancy stuff. 
        /// Sets which endpoint, profileId, etc. we're using as well as binding information
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// see sapiRegisterApplication()
        /// </summary>
        public void sapiRegisterGenericApplication()
        {
            byte[] buf = new byte[ZB_APP_REGISTER_REQUEST_PAYLOAD_LEN + SRSP_HEADER_SIZE];
            buf[0] = ZB_APP_REGISTER_REQUEST_PAYLOAD_LEN;
            buf[1] = (Commands.ZB_APP_REGISTER_REQUEST & 0xFF00) >> 8;
            buf[2] = Commands.ZB_APP_REGISTER_REQUEST & 0xFF;

            buf[3] = applicationConfiguration.DEFAULT_ENDPOINT;
            buf[4] = applicationConfiguration.DEFAULT_PROFILE_ID & 0xFF;
            buf[5] = (applicationConfiguration.DEFAULT_PROFILE_ID & 0xFF00) >> 8;
            buf[6] = applicationConfiguration.ZNP_DEVICE_ID & 0xFF;
            buf[7] = (applicationConfiguration.ZNP_DEVICE_ID & 0xFF00) >> 8;
            buf[8] = applicationConfiguration.DEVICE_VERSION;
            buf[9] = 0; //unused
            buf[10] = 0; //number of binding input clusters
            buf[11] = 0; //number of binding output clusters

            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Starts the Zigbee stack in the ZNP using the settings from a previous sapiRegisterApplication().
        /// After this start request process completes, the device is ready to send, receive, and route network traffic.
        /// note: On a coordinator in a trivial test setup, it takes approximately 300mSec between 
        /// START_REQUEST and receiving START_REQUEST_SRSP and then another 200-1000mSec from when we receive 
        /// START_REQUEST_SRSP to when we receive START_CONFIRM. 
        /// note: Set START_CONFIRM_TIMEOUT based on size of your network.
        /// sapiRegisterApplication() was a success.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// note: we will receive a START_CONFIRM if everything was ok. 
        /// Should always receive a start confirm for a coordinator. 
        /// Will only receive a start confirm on a router if we could join a zigbee network.
        /// </summary>
        public void sapiStartApplication()
        {
            byte[] buf = new byte[ZB_APP_START_REQUEST_PAYLOAD_LEN + SRSP_HEADER_SIZE];


            buf[0] = ZB_APP_START_REQUEST_PAYLOAD_LEN;
            buf[1] = (Commands.ZB_APP_START_REQUEST & 0xFF00) >> 8;
            buf[2] = Commands.ZB_APP_START_REQUEST & 0xFF;   
    
            znpResult = sendMessage(buf);  
    
            if (znpResult != ZNP_SUCCESS) 
                return;
    
            //Now, wait for message, and verify that it's a ZB_START_CONFIRM, else timeout.
            //NOTE: Do not print anything out here, or else you might miss the ZB_START_CONFIRM!
    
            znpResult = -1;    
            while (znpResult != 0)
                znpResult = waitForMessage( Commands.ZB_START_CONFIRM, ZB_START_CONFIRM_TIMEOUT);
    
            if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
            {
                znpResult = -2;
                return;
            }    
        }

        /// <summary>
        /// Sets the Joining Permissions for this device or other devices. 
        /// By default, after a setStartupOptions(CLEAR_CONFIG), joining is set to allow all devices, indefinitely.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="destination">which short address to set joining permissions for, or ALL_ROUTERS_AND_COORDINATORS to set joining permissions for all. </param>
        /// <param name="timeout">how long in seconds to allow permissions, or if set to NO_TIMEOUT then joining will be on indefinitely.</param>
        public void setJoiningPermissions(uint destination, byte timeout)
        {
            byte[] buf = new byte[ZB_PERMIT_JOINING_REQUEST_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZB_PERMIT_JOINING_REQUEST_PAYLOAD_LEN;
            buf[1] = (Commands.ZB_PERMIT_JOINING_REQUEST & 0xFF00) >> 8;
            buf[2] = Commands.ZB_PERMIT_JOINING_REQUEST & 0xFF;

            buf[3] = (byte)(destination & 0xFF);
            buf[4] = (byte)((destination & 0xFF00) >> 8);
            buf[5] = timeout;
            znpResult = sendMessage(buf);   
        }


        /// <summary>
        /// Sends a message to another device over the Zigbee network.
        /// note: On a coordinator in a trivial test setup, it takes approximately 10mSec from sending 
        /// ZB_SEND_DATA_REQUEST to when we receive ZB_SEND_DATA_CONFIRM.
        /// note:handle is an optional user-definable reference number to match SEND_DATA_CONFIRM messages 
        /// with the SEND_DATA_REQUEST message. This can be used to determine which SEND_DATA_REQUEST generated an error.
        /// note:  The ZNP will automatically require an ACK from the next device on the route when sending data. 
        /// To require an ACK from the final destination, change MAC_ACK to APS_ACK. The downside is that this increases network traffic.
        /// note:   The radius is the maximum number of hops that this packet can travel through before it will be dropped. 
        /// Should be set to the maximum number of hops expected in the network.
        /// note:   adjust SEND_DATA_CONFIRM_INTERVAL_MS and SEND_DATA_CONFIRM_TIMEOUT_MS based on network size, number of hops, etc.
        /// sapiStartApplication() was a success, START_CONFIRM received without any errors.
        ///    there is another device on the network with short address of <code>destinationShortAddress</code> 
        /// and that device has successfully started its application using sapiStartApplication().
        ///   srsp will contain a ZB_SEND_DATA_REQUEST_SRSP. We will receive a ZB_SEND_DATA_REQUEST_SRSP regardless of whether the message was successfully sent or not.
        ///   we will receive a ZB_SEND_DATA_CONFIRM if the message was successfully sent. 
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="destinationShortAddress">the short address of the destination, or ALL_DEVICES or ALL_ROUTERS_AND_COORDINATORS to broadcast the message.</param>
        /// <param name="clusterId">which cluster this message is for. User definable. Zigbee supports up to 2^16 clusters. 
        /// A cluster is typically a particular command, e.g. "turn on lights" or "get temperature". 
        /// If using a predefined Zigbee Alliance Application Profile then this cluster will follow the Zigbee Cluster Library.
        /// </param>
        /// <param name="data">the data to send.</param>
        /// <param name="dataLength">how many bytes of data to send. Must be less than MAXIMUM_PAYLOAD_LENGTH.</param>
        public void sendData(uint destinationShortAddress, uint clusterId, byte[] data, byte dataLength)
        {
            if (dataLength > MAXIMUM_PAYLOAD_LENGTH)
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZB_SEND_DATA_REQUEST_PAYLOAD_LEN + dataLength];

            buf[0] = (byte)(ZB_SEND_DATA_REQUEST_PAYLOAD_LEN + dataLength);
            buf[1] = (Commands.ZB_SEND_DATA_REQUEST & 0xFF00)>>8;
            buf[2] = Commands.ZB_SEND_DATA_REQUEST & 0xFF;

            buf[3] = (byte)(destinationShortAddress & 0xFF);
            buf[4] = (byte)((destinationShortAddress & 0xFF00)>>8);
            buf[5] = (byte)(clusterId & 0xFF);
            buf[6] = (byte)((clusterId & 0xFF00) >> 8);
            buf[7] = sequenceNumber++;  //handle aka Transaction Sequence Number
            buf[8] = MAC_ACK;
            buf[9] = DEFAULT_RADIUS;
            buf[10] = dataLength;

            Array.ConstrainedCopy(data, 0, buf, ZB_SEND_DATA_REQUEST_PAYLOAD_LEN + 3, dataLength);

            znpResult = sendMessage(buf);
            if (znpResult != 0)
                return;

            //Now, wait for message, and verify that it's a ZB_SEND_DATA_CONFIRM, else timeout.
            //NOTE: Do not print anything out here, or else you might miss the ZB_SEND_DATA_CONFIRM!

            znpResult = waitForMessage(Commands.ZB_SEND_DATA_CONFIRM, ZB_SEND_DATA_CONFIRM_TIMEOUT);
            if (znpResult != 0)
                return;

            //NOTE: znpBuf[3] contains the transaction sequence number; we ignore
            if (znpBuf[SRSP_PAYLOAD_START + 1] != SRSP_STATUS_SUCCESS) //verify status is succes
                znpResult = -3;
        }


        /// <summary>
        /// Configures the ZNP for our application.
        /// Sets which profileId, etc. we're using as well as binding information FOR EACH ENDPOINT
        /// Using the AF/ZDO interface, you can have more than one endpoint. Recommended not to exceed 10 endpoints.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// can now use afStartApplication()
        /// note: this method does not check whether the endpoint being registered has already been registered - it is up to the application to manage that.
        /// see applicationConfiguration
        /// </summary>
        /// <param name="ac">the applicationConfiguration to register.</param>
        public void afRegisterApplication(applicationConfiguration ac)
        {
            if (ac.endPoint == 0)
            {
                znpResult = -1;
                return;
            }
            if ((ac.numberOfBindingInputClusters > applicationConfiguration.MAX_BINDING_CLUSTERS) || (ac.numberOfBindingOutputClusters > applicationConfiguration.MAX_BINDING_CLUSTERS))
            {
                znpResult = -2;
                return;
            }

            byte[] buf = new byte[10 + 2 + 2 * (ac.numberOfBindingInputClusters + ac.numberOfBindingOutputClusters)];

            //znpBuf[0] (length) will be set later
            buf[1] = (Commands.AF_REGISTER & 0xFF00) >> 8;
            buf[2] = Commands.AF_REGISTER & 0xFF;

            byte bufferIndex;
            buf[3] = ac.endPoint;
            buf[4] = (byte)(ac.profileId & 0xFF);
            buf[5] = (byte)((ac.profileId & 0xFF00) >> 8);
            buf[6] = (byte)(ac.deviceId & 0xFF);
            buf[7] = (byte)((ac.deviceId & 0xFF00) >> 8);
            buf[8] = ac.deviceVersion;
            buf[9] = ac.latencyRequested;
            buf[10] = ac.numberOfBindingInputClusters;

            bufferIndex = 11;
            for (int i = 0; i < ac.numberOfBindingInputClusters; i++)
            {
                buf[bufferIndex++] = (byte)(ac.bindingInputClusters[i] & 0xFF);
                buf[bufferIndex++] = (byte)((ac.bindingInputClusters[i] & 0xFF00) >> 8);
            }
            buf[bufferIndex++] = ac.numberOfBindingOutputClusters;
            for (int i = 0; i < ac.numberOfBindingOutputClusters; i++)
            {
                buf[bufferIndex++] = (byte)(ac.bindingOutputClusters[i] & 0xFF);
                buf[bufferIndex++] = (byte)((ac.bindingInputClusters[i] & 0xFF00) >> 8);
            }
            buf[0] = bufferIndex;
            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Configures the ZNP for a "generic" application: one endpoint, no binding or fancy stuff. 
        /// Sets which endpoint, profileId, etc. we're using as well as binding information
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <see cref="afRegisterApplication"/>
        public void afRegisterGenericApplication()
        {
            byte[] buf = new byte[AF_REGISTER_PAYLOAD_LEN + SRSP_HEADER_SIZE];
            buf[0] = AF_REGISTER_PAYLOAD_LEN;
            buf[1] = (Commands.AF_REGISTER & 0xFF00) >> 8;
            buf[2] = Commands.AF_REGISTER & 0xFF;

            buf[3] = applicationConfiguration.DEFAULT_ENDPOINT;
            buf[4] = applicationConfiguration.DEFAULT_PROFILE_ID & 0xFF;
            buf[5] = (applicationConfiguration.DEFAULT_PROFILE_ID & 0xFF00) >> 8;
            buf[6] = applicationConfiguration.ZNP_DEVICE_ID & 0xFF;
            buf[7] = (applicationConfiguration.ZNP_DEVICE_ID & 0xFF00) >> 8;
            buf[8] = applicationConfiguration.DEVICE_VERSION;
            buf[9] = applicationConfiguration.LATENCY_NORMAL;
            buf[10] = 0; //number of binding input clusters
            buf[11] = 0; //number of binding output clusters
            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Starts the Zigbee stack in the ZNP using the settings from a previous afRegisterApplication().
        /// After this start request process completes, the device is ready to send, receive, and route network traffic.
        /// note: On a coordinator in a trivial test setup, it takes approximately 300mSec between sending 
        /// START_REQUEST and receiving START_REQUEST_SRSP and then another 200-1000mSec from when we receive 
        /// START_REQUEST_SRSP to when we receive START_CONFIRM. Set START_CONFIRM_TIMEOUT based on size of your network.
        /// note: ZDO_STARTUP_FROM_APP field StartDelay not used
        /// pre afRegisterApplication() was a success.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// We will see Device Status change to DEV_ROUTER, DEV_ZB_COORD, or DEV_END_DEVICE correspondingly if everything was ok.
        /// </summary>
        public void zdoStartApplication()
        {
            byte[] buf = new byte[ZDO_STARTUP_FROM_APP_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZDO_STARTUP_FROM_APP_PAYLOAD_LEN;
            buf[1] = (Commands.ZDO_STARTUP_FROM_APP & 0xFF00) >> 8;
            buf[2] = Commands.ZDO_STARTUP_FROM_APP & 0xFF;

            buf[3] = NO_START_DELAY;
            znpResult = sendMessage(buf);
        }


        /// <summary>
        /// Sends a message to another device over the Zigbee network using the AF command AF_DATA_REQUEST.
        /// note: On a coordinator in a trivial test setup, it takes approximately 10mSec from sending 
        /// AF_DATA_REQUEST to when we receive AF_DATA_CONFIRM.
        /// note: transactionSequenceNumber is an optional user-definable reference number to match AF_DATA_REQUEST messages with AF_DATA_CONFIRM messages. 
        /// The ZNP will automatically require an ACK from the next device on the route when sending data. 
        /// To require an ACK from the final destination, change MAC_ACK to APS_ACK at the expense of increased network traffic.
        /// note:  The radius is the maximum number of hops that this packet can travel through before it will be dropped.
        /// Should be set to the maximum number of hops expected in the network.
        /// note:  adjust AF_DATA_CONFIRM_INTERVAL_MS and AF_DATA_CONFIRM_TIMEOUT_MS based on network size, number of hops, etc.
        /// pre    the application was started successfully
        /// pre    there is another device on the network with short address of <code>destinationShortAddress</code> 
        /// and that device has successfully started its application.
        /// srsp will contain a AF_DATA_REQUEST_SRSP. We will receive a AF_DATA_REQUEST_SRSP regardless of whether the message was successfully sent or not.
        /// we will receive a AF_DATA_CONFIRM if the message was successfully sent. 
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="destinationEndpoint">which endpoint to send this to.</param>
        /// <param name="sourceEndpoint">which endpoint this message originated from on our device</param>
        /// <param name="destinationShortAddress">the short address of the destination, or ALL_DEVICES or ALL_ROUTERS_AND_COORDINATORS to broadcast the message.</param>
        /// <param name="clusterId">which cluster this message is for. User definable. Zigbee supports up to 2^16 clusters.
        /// A cluster is typically a particular command, e.g. "turn on lights" or "get temperature". 
        /// If using a predefined Zigbee Alliance Application Profile then this cluster will follow the Zigbee Cluster Library.
        /// </param>
        /// <param name="data">the data to send</param>
        /// <param name="dataLength">how many bytes of data to send. Must be less than MAXIMUM_PAYLOAD_LENGTH.</param>
        public void afSendData(byte destinationEndpoint, byte sourceEndpoint, uint destinationShortAddress, uint clusterId, byte[] data, byte dataLength)
        {
            if (dataLength > MAXIMUM_PAYLOAD_LENGTH)
            {
                znpResult = -1;
                return;
            }
            byte[] buf = new byte[AF_DATA_REQUEST_PAYLOAD_LEN + dataLength];

            buf[0] = (byte)(AF_DATA_REQUEST_PAYLOAD_LEN + dataLength);
            buf[1] = (Commands.AF_DATA_REQUEST & 0xFF00) >> 8;
            buf[2] = Commands.AF_DATA_REQUEST & 0xFF;

            buf[3] = (byte)(destinationShortAddress & 0xFF);
            buf[4] = (byte)((destinationShortAddress & 0xFF00) >> 8);
            buf[5] = destinationEndpoint;
            buf[6] = sourceEndpoint;
            buf[7] = (byte)(clusterId & 0xFF);
            buf[8] = (byte)((clusterId & 0xFF00) >> 8);
            buf[9] = transactionSequenceNumber++;
            buf[10] = MAC_ACK; //Could also use AF_APS_ACK;
            buf[11] = DEFAULT_RADIUS;
            buf[12] = dataLength;

            Array.ConstrainedCopy(data, 0, buf, AF_DATA_REQUEST_PAYLOAD_LEN + 3, dataLength);

            znpResult = sendMessage(buf);
            if (znpResult != 0)
                return;

            //Now, wait for message, and verify that it's a AF_DATA_CONFIRM, else timeout.
            //NOTE: Do not print anything out here, or else you might miss the AF_DATA_CONFIRM!
            znpResult = waitForMessage(Commands.AF_DATA_REQUEST, AF_DATA_CONFIRM_TIMEOUT);
            if (znpResult != 0)
                return;
            //note: znpBuf[4] = endpoint, znpBuf[5] = transaction sequence number
            if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
            {
                znpResult = -2;
                return;
            }
        }


        /// <summary>
        /// Requests a device's MAC Address (64-bit IEEE Address) given a short address.
        /// An ZDO_IEEE_ADDR_RSP message will be received, with one or more entries.
        /// znpResult contains the error code, or ZNP_SUCCESS if success.
        /// </summary>
        /// <param name="shortAddress">the short address to locate</param>
        /// <param name="requestType">must be SINGLE_DEVICE_RESPONSE or INCLUDE_ASSOCIATED_DEVICES.
        /// If SINGLE_DEVICE_RESPONSE is selected, then only information about the requested device will be returned. 
        /// If INCLUDE_ASSOCIATED_DEVICES is selected, then the short addresses of the selected device's children will be returned too.
        /// </param>
        /// <param name="startIndex">If INCLUDE_ASSOCIATED_DEVICES was selected, then there may be too many children to 
        /// fit in one ZDO_IEEE_ADDR_RSP message. So, use startIndex to get the next set of children's short addresses.
        /// </param>
        public void zdoRequestIeeeAddress(uint shortAddress, byte requestType, byte startIndex)
        {
            if ((requestType != SINGLE_DEVICE_RESPONSE) && (requestType != INCLUDE_ASSOCIATED_DEVICES))
            {
                znpResult = -1;
                return;
            }

            byte[] buf = new byte[ZDO_IEEE_ADDR_REQ_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZDO_IEEE_ADDR_REQ_PAYLOAD_LEN;
            buf[1] = (Commands.ZDO_IEEE_ADDR_REQ & 0xFF00) >> 8;
            buf[2] = Commands.ZDO_IEEE_ADDR_REQ & 0xFF;

            buf[3] = (byte)(shortAddress & 0xFF);
            buf[4] = (byte)((shortAddress & 0xFF00) >> 8);
            buf[5] = requestType;
            buf[6] = startIndex;

            znpResult = sendMessage(buf);
            if (znpResult != 0)
                return;

            znpResult = waitForMessage(Commands.ZDO_IEEE_ADDR_RSP, 5);
            if (znpResult != 0)
                return;
            if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
            {
                znpResult = -2;
                return;
            }  
        }


        /// <summary>
        /// Requests a device's Short Address for a given long address.
        /// note: DOES NOT WORK FOR SLEEPING END DEVICES
        /// An ZDO_NWK_ADDR_RSP message will be received, with one or more entries.
        /// </summary>
        /// <param name="ieeeAddress">the long address to locate</param>
        /// <param name="requestType">must be SINGLE_DEVICE_RESPONSE or INCLUDE_ASSOCIATED_DEVICES.
        /// If SINGLE_DEVICE_RESPONSE is selected, then only information about the requested device will be returned. 
        /// If INCLUDE_ASSOCIATED_DEVICES is selected, then the short addresses of the selected device's children will be returned too.
        /// </param>
        /// <param name="startIndex">If INCLUDE_ASSOCIATED_DEVICES was selected, then there may be 
        /// too many children to fit in one ZDO_NWK_ADDR_RSP message. 
        /// So, use startIndex to get the next set of children's short addresses.
        /// </param>
        /// <returns>a array to the beginning of the payload, or a array to indeterminate data if error.</returns>
        public byte[] zdoNetworkAddressRequest(byte[] ieeeAddress, byte requestType, byte startIndex)
        {
            if ((requestType != SINGLE_DEVICE_RESPONSE) && (requestType != INCLUDE_ASSOCIATED_DEVICES))
            {
                znpResult = -1;
                return (byte[])null;
            }

            byte[] buf = new byte[ZDO_NWK_ADDR_REQ_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = ZDO_NWK_ADDR_REQ_PAYLOAD_LEN;
            buf[1] = (Commands.ZDO_NWK_ADDR_REQ & 0xFF00) >> 8;
            buf[2] = Commands.ZDO_NWK_ADDR_REQ & 0xFF;

            Array.ConstrainedCopy(ieeeAddress, 0, buf, 3, 8);
            buf[11] = requestType;
            buf[12] = startIndex;

            znpResult = sendMessage(buf);

            if (znpResult != 0)
                return (byte[])null;
            znpResult = waitForMessage(Commands.ZDO_NWK_ADDR_RSP, 5);
            if (znpResult != 0)
                return (byte[])null;
            if (znpBuf[SRSP_PAYLOAD_START] != SRSP_STATUS_SUCCESS) //verify status is succes
            {
                znpResult = -2;
                return (byte[])null;
            }
            byte[] result = new byte[znpBuf[0] + SRSP_HEADER_SIZE];
            Array.ConstrainedCopy(znpBuf, SRSP_PAYLOAD_START, result, 0, znpBuf[0]);
            return result;

        }


        /// <summary>
        /// Displays the header information in an AF_INCOMING_MSG.
        /// </summary>
        /// <param name="srsp">a array to the buffer containing the message</param>
        /// <returns>0 if success, -1 if not a AF_INCOMING_MSG.</returns>
        public string printAfIncomingMsgHeader(byte[] srsp)
        {
            if((srsp[SRSP_CMD_LSB_FIELD] + srsp[SRSP_CMD_MSB_FIELD] *0x0100) == Commands.AF_INCOMING_MSG)
            {
              return string.Format("#{0:D2}: Grp{1:x4} Clus{2:x4}, SrcAd{3:x4}, SrcEnd{4:x2} DestEnd{5:x2} Bc{6:x2} Lqi{7:x2} Sec{8:x2} Len{9:D2}",
                              (srsp[SRSP_HEADER_SIZE] + srsp[SRSP_HEADER_SIZE+1] *0x0100),
                              (srsp[SRSP_HEADER_SIZE+2] + srsp[SRSP_HEADER_SIZE+3] *0x0100),
                              (srsp[SRSP_HEADER_SIZE+4] + srsp[SRSP_HEADER_SIZE+5] *0x0100),
                              srsp[SRSP_HEADER_SIZE+6],srsp[SRSP_HEADER_SIZE+7],srsp[SRSP_HEADER_SIZE+8],
                              srsp[SRSP_HEADER_SIZE+9],srsp[SRSP_HEADER_SIZE+10],srsp[SRSP_HEADER_SIZE+16]);

            }
            else
            {
                return (string)null;
            }
        }

        /// <summary>
        /// Displays the names that go with the above
        /// </summary>
        /// <returns></returns>
        public string printAfIncomingMsgHeaderNames()
        {
            return("Fields are: Group, Cluster, Source Address, Source Endpoint, Destination Endpoint, wasBroadcast, Link Quality Indicator, SecurityUse, Payload Length\r\n");
        }
        
       
        /// <summary>
        /// Prints out the returned value of zdoNetworkAddressRequest()
        /// </summary>
        /// <param name="rsp"></param>
        /// <returns></returns>
        public string printZdoNetworkAddressResponse(byte[] rsp)
        {
            StringBuilder outputStr = new StringBuilder();
            outputStr.Append(string.Format("Status = {0:G}; MAC:",rsp[0]));
            for (int i=1; i<9; i++)
                outputStr.Append(string.Format("{0:X2} ", rsp[i]));
            outputStr.Append(string.Format("NwkAddr:{0:X4}; StartIdx:{1:G}, numAssocDev:{2:G}\r\n", (rsp[9] + rsp[10] * 0x0100), rsp[11], rsp[12]));

            for(int j=0;j<rsp[12];j++)
                outputStr.Append(string.Format("{0:X4} ",(rsp[j+13] + rsp[j+14]*0x0100)));
            outputStr.Append("\r\n");
            return outputStr.ToString();
        }


        /// <summary>
        /// Uses the UTIL interface to get the long address of a device based on its short address.
        /// The device to lookup must either be a parent or child of the ZNP, or have established a binding or application link key.
        /// note: on a trivial network (one coord, one router) this takes approx. 0.4833mSec to complete.
        /// </summary>
        /// <param name="shortAddress">the shortAddress to lookup</param>
        /// <returns>the long address returned by the ZNP.</returns>
        public byte[] znpGetMacForShort(uint shortAddress)
        {
            byte[] buf = new byte[UTIL_ADDRMGR_NWK_ADDR_LOOKUP_PAYLOAD_LEN + SRSP_HEADER_SIZE];

            buf[0] = UTIL_ADDRMGR_NWK_ADDR_LOOKUP_PAYLOAD_LEN;
            buf[1] = (Commands.UTIL_ADDRMGR_NWK_ADDR_LOOKUP & 0xFF00) >> 8;
            buf[2] = Commands.UTIL_ADDRMGR_NWK_ADDR_LOOKUP & 0xFF;

            buf[3] = (byte)(shortAddress & 0xFF);
            buf[4] = (byte)((shortAddress & 0xFF00) >> 8);

            znpResult = sendMessage(buf);

            byte[] result = new byte[znpBuf[0]];

            Array.ConstrainedCopy(znpBuf, SRSP_PAYLOAD_START, result, 0, znpBuf[0]);
            return result;
        }


        /// <summary>
        /// Configures the ZNP to output a test signal, modulated or unmodulated. The radio must be re-initialized after this method is used.
        /// note: this command does NOT respond with an SRSP.
        /// </summary>
        /// <param name="mode">whether to output modulated or unmodulated signal. Must be RF_TEST_MODULATED or RF_TEST_UNMODULATED or RF_TEST_RECEIVE. Note: RF_TEST_RECEIVE does nothing.</param>
        /// <param name="channel">which 802.15.4 channel to output test signal on. Must be between 11 - 26.
        /// pre ZNP was initialized
        /// ZNP is outputting the specified type of RF test signal on the specified channel
        /// </param>
        /// <param name="error"></param>
        /// <returns>0 if success, -1 if invalid channel, -2 if invalid test mode, else error code from sendMessage()</returns>
        public string rfTest(byte mode, byte channel, ref int error)
        {
            StringBuilder outputStr = new StringBuilder();
            if ((channel<RF_TEST_CHANNEL_MIN) || (channel>RF_TEST_CHANNEL_MAX))
            {
                outputStr.Append("error - channel must be between 11-26\r\n");
                error =-1;
            }
            if ((mode != RF_TEST_MODULATED) && (mode != RF_TEST_UNMODULATED) && (mode != RF_TEST_RECEIVE))
            {
                outputStr.Append("error - unknown mode\r\n");
                error = -2;
            }
    
            int frequency = 2405 + 5*(channel-11);  //CC253x family datasheet, equation 19-1
            if (mode == RF_TEST_MODULATED) outputStr.Append("RF Test Mode Modulated");
            if (mode == RF_TEST_UNMODULATED) outputStr.Append("RF Test Mode UnModulated");
            if (mode == RF_TEST_RECEIVE) outputStr.Append("RF Test Mode Receive");
            outputStr.Append(string.Format("(mode {0:G}) on channel {1:G} (frequency = {2:G} Hz)\r\n", mode, channel, frequency));

            error = writeRfTestParam(mode, channel, RF_OUTPUT_POWER_PLUS_4_5_DBM);

            return outputStr.ToString();
        }

    }


    /// <summary>
    /// This is a set of application configuration parameters. These parameters affect the way our particular Zigbee Application will behave.
    /// </summary>
    public class applicationConfiguration
    {
        /** Maximum number of clusters available for binding */
        public const int MAX_BINDING_CLUSTERS = 32;
        //default values used when creating applicationConfigurations in Simple API or AFZDO
        public const int DEFAULT_ENDPOINT     = 0xD7;
        public const int DEFAULT_PROFILE_ID   = 0xD7D7;
        public const int ZNP_DEVICE_ID        = 0x4567;
        public const int DEVICE_VERSION       = 0x89;
        //Values for latencyRequested field of struct applicationConfiguration. Not used in Simple API.
        public const int LATENCY_NORMAL       = 0;
        public const int LATENCY_FAST_BEACONS = 1;
        public const int LATENCY_SLOW_BEACONS = 2;

        
        private byte endpoint;
        private uint profileid;
        private uint deviceid;
        private byte deviceversion;
        private byte latencyrequested;
        private byte numberofbindinginputclusters;
        private uint[] bindinginputclusters = new uint[MAX_BINDING_CLUSTERS];
        private byte numberofbindingoutputclusters;
        private uint[] bindingoutputclusters = new uint[MAX_BINDING_CLUSTERS];

        /// <summary>
        /// The Zigbee Endpoint. Simple API only supports one endpoint. Must use the same value of endpoint for all devices in the network.
        /// </summary>
        public byte endPoint
        {
            get{return endpoint;}
            set{endpoint = value;}
        }

        /// <summary>
        /// The Application Profile ID as assigned by the Zigbee Association. Must use the same application profile for all devices in the network.
        /// </summary>
        public uint profileId
        {
            get { return profileid; }
            set { profileid = value; }
        }

        /// <summary>
        /// A user-defined device ID field. Not used in Simple API, but when using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command.
        /// </summary>
        public uint deviceId
        {
            get { return deviceid; }
            set { deviceid = value; }
        }

        /// <summary>
        /// A user-defined device ID field. Not used in Simple API, but when using AFZDO API a remote device can query for this using the ZDO_SIMPLE_DESC_REQ command.
        /// </summary>
        public byte deviceVersion
        {
            get { return deviceversion; }
            set { deviceversion = value; }
        }

        /// <summary>
        /// A very simple Quality of Service (QoS) setting. Must be LATENCY_NORMAL, LATENCY_FAST_BEACONS, or LATENCY_SLOW_BEACONS. Not used in Simple API.
        /// </summary>
        public byte latencyRequested
        {
            get { return latencyrequested; }
            set { latencyrequested = value; }
        }

        /// <summary>
        /// Number of Input Clusters for Binding. If not using binding then set to zero.
        /// </summary>
        public byte numberOfBindingInputClusters
        {
            get { return numberofbindinginputclusters; }
            set { numberofbindinginputclusters = value; }
        }

        /// <summary>
        /// List of Input Clusters for Binding. If not using binding then this does not apply. To allow another device to bind to this device, must use ZB_ALLOW_BIND on this device and must use ZB_BIND_DEVICE on the other device.
        /// </summary>
        public uint[] bindingInputClusters
        {
            get { return bindinginputclusters; }
            set { bindinginputclusters = value; }
        }

        /// <summary>
        /// Number of Output Clusters for Binding. If not using binding then set to zero.
        /// </summary>
        public byte numberOfBindingOutputClusters
        {
            get { return numberofbindingoutputclusters; }
            set { numberofbindingoutputclusters = value; }
        }

        /// <summary>
        /// List of Output Clusters for Binding. If not using binding then this does not apply. To bind to another device, that device must use ZB_ALLOW_BIND and this device must use ZB_BIND_DEVICE to create a binding. 
        /// </summary>
        public uint[] bindingOutputClusters
        {
            get { return bindingoutputclusters; }
            set { bindingoutputclusters = value; }
        }
    }


    /// <summary>
    /// CC2530-ZNP software command interface
    /// </summary>
    public class Commands
    {
        #region SYS Interface
        public const int ZB_WRITE_CONFIGURATION = 0x2605;
        public const int ZB_READ_CONFIGURATION  = 0x2604;
        public const int SYS_GPIO               = 0x210E;
        public const int SYS_TEST_RF            = 0x4140;
        public const int SYS_VERSION            = 0x2102;
        public const int SYS_RANDOM             = 0x210C;
        public const int SYS_OSAL_NV_READ       = 0x2108;
        public const int SYS_OSAL_NV_WRITE      = 0x2109;
        public const int SYS_RESET_IND          = 0x4180;
        public const int SYS_STACK_TUNE         = 0x210F;
        public const int SYS_RESET_REQ          = 0x4100;

        #endregion

        #region Simple API commands

        public const int ZB_APP_REGISTER_REQUEST    = 0x260A;
        public const int ZB_APP_START_REQUEST       = 0x2600;
        public const int ZB_SEND_DATA_REQUEST       = 0x2603;
        public const int ZB_SEND_DATA_CONFIRM       = 0x4683;
        public const int ZB_GET_DEVICE_INFO         = 0x2606;
        public const int ZB_FIND_DEVICE_REQUEST     = 0x2607;
        public const int ZB_FIND_DEVICE_CONFIRM     = 0x4685;
        public const int ZB_PERMIT_JOINING_REQUEST  = 0x2608;
        public const int ZB_START_CONFIRM           = 0x4680; //will receive this asynchronously
        public const int ZB_RECEIVE_DATA_INDICATION = 0x4687; //will receive this asynchronously

        #endregion

        #region AFZDO commands
        public const int AF_REGISTER              = 0x2400;
        public const int AF_DATA_REQUEST          = 0x2401;
        public const int AF_DATA_CONFIRM          = 0x4480;
        public const int AF_INCOMING_MSG          = 0x4481; //will receive this asynchronously
        public const int ZDO_STARTUP_FROM_APP     = 0x2540;
        public const int ZDO_IEEE_ADDR_REQ        = 0x2501;
        public const int ZDO_IEEE_ADDR_RSP        = 0x4581;
        public const int ZDO_NWK_ADDR_REQ         = 0x2500;
        public const int ZDO_NWK_ADDR_RSP         = 0x4580;

        public const int ZDO_STATE_CHANGE_IND     = 0x45C0; //will receive this asynchronously  Old version of ZNP = 0x45C1
        public const int ZDO_END_DEVICE_ANNCE_IND = 0x45C1; //will receive this asynchronously  Old version of ZNP = 0x4593

        #endregion

        #region UTIL commands
        public const int UTIL_ADDRMGR_NWK_ADDR_LOOKUP = 0x2741;

        #endregion

        /// <summary>
        /// get ZNP command name
        /// </summary>
        /// <param name="command">command</param>
        /// <returns>command name</returns>
        public string getCommandName(int command)
        {
            switch (command)
            {
                //SYS Interface        
                case ZB_WRITE_CONFIGURATION:     return ("ZB_WRITE_CONFIGURATION");
                case ZB_READ_CONFIGURATION:      return ("ZB_READ_CONFIGURATION");
                case SYS_GPIO:                   return ("SYS_GPIO");
                case SYS_TEST_RF:                return ("SYS_TEST_RF");
                case SYS_VERSION:                return ("SYS_VERSION");
                case SYS_OSAL_NV_READ:           return ("SYS_OSAL_NV_READ");
                case SYS_OSAL_NV_WRITE:          return ("SYS_OSAL_NV_WRITE");
                case SYS_RESET_IND:              return ("SYS_RESET_IND");

                //Simple API commands
                case ZB_APP_REGISTER_REQUEST:    return ("ZB_APP_REGISTER_REQUEST");
                case ZB_APP_START_REQUEST:       return ("ZB_APP_START_REQUEST");
                case ZB_SEND_DATA_REQUEST:       return ("ZB_SEND_DATA_REQUEST");
                case ZB_SEND_DATA_CONFIRM:       return ("ZB_SEND_DATA_CONFIRM");
                case ZB_GET_DEVICE_INFO:         return ("ZB_GET_DEVICE_INFO");
                case ZB_FIND_DEVICE_REQUEST:     return ("ZB_FIND_DEVICE_REQUEST");
                case ZB_FIND_DEVICE_CONFIRM:     return ("ZB_FIND_DEVICE_CONFIRM");
                case ZB_PERMIT_JOINING_REQUEST:  return ("ZB_PERMIT_JOINING_REQUEST");
                case ZB_START_CONFIRM:           return ("ZB_START_CONFIRM");
                case ZB_RECEIVE_DATA_INDICATION: return ("ZB_RECEIVE_DATA_INDICATION");

                //AF/ZDO commands:    
                case AF_REGISTER:                return ("AF_REGISTER");
                case AF_DATA_REQUEST:            return ("AF_DATA_REQUEST");
                case AF_DATA_CONFIRM:            return ("AF_DATA_CONFIRM");
                case AF_INCOMING_MSG:            return ("AF_INCOMING_MSG");
                case ZDO_STARTUP_FROM_APP:       return ("ZDO_STARTUP_FROM_APP");
                case ZDO_IEEE_ADDR_REQ:          return ("ZDO_IEEE_ADDR_REQ");
                case ZDO_IEEE_ADDR_RSP:          return ("ZDO_IEEE_ADDR_RSP");
                case ZDO_NWK_ADDR_REQ:           return ("ZDO_NWK_ADDR_REQ");
                case ZDO_NWK_ADDR_RSP:           return ("ZDO_NWK_ADDR_RSP");
                case ZDO_END_DEVICE_ANNCE_IND:   return ("ZDO_END_DEVICE_ANNCE_IND");

                default:                         return ("UNKNOWN");
            }
        }

    }

}
