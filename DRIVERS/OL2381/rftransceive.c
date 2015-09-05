// -----------------------------------------------------------------------------
//
// rftransceive.c
//
// -----------------------------------------------------------------------------
// Routines for RF data reception/transmission based on OL2381 (RF chip).
// SFR Communication is done via SSP device.
//
// -----------------------------------------------------------------------------
//  (c) NXP B.V. 2010. All rights reserved.
//  Disclaimer
//  1.   The NXP Software/Source Code is provided to Licensee "AS IS"
//  without any warranties of any kind. NXP makes no warranties to Licensee
//  and shall not indemnify Licensee or hold it harmless for any reason
//  related to the NXP Software/Source Code or otherwise be liable to the
//  NXP customer. The NXP customer acknowledges and agrees that the NXP
//  Software/Source Code is provided AS-IS and accepts all risks of
//  utilizing the NXP Software under the conditions set forth according to
//  this disclaimer.
//
//  2.   NXP EXPRESSLY DISCLAIMS ALL WARRANTIES, EXPRESS OR IMPLIED,
//  INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF
//  MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE, AND NON-INFRINGEMENT
//  OF INTELLECTUAL PROPERTY RIGHTS. NXP SHALL HAVE NO LIABILITY TO THE NXP
//  CUSTOMER, OR ITS SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY FOR
//  ANY DAMAGES, INCLUDING WITHOUT LIMITATION, DAMAGES RESULTING OR ALLEGDED
//  TO HAVE RESULTED FROM ANY DEFECT, ERROR OR OMMISSION IN THE NXP
//  SOFTWARE/SOURCE CODE, THIRD PARTY APPLICATION SOFTWARE AND/OR
//  DOCUMENTATION, OR AS A RESULT OF ANY INFRINGEMENT OF ANY INTELLECTUAL
//  PROPERTY RIGHT OF ANY THIRD PARTY. IN NO EVENT SHALL NXP BE LIABLE FOR
//  ANY INCIDENTAL, INDIRECT, SPECIAL, EXEMPLARY, PUNITIVE, OR CONSEQUENTIAL
//  DAMAGES (INCLUDING LOST PROFITS) SUFFERED BY NXP CUSTOMER OR ITS
//  SUBSIDIARIES, AFFILIATES, OR ANY OTHER THIRD PARTY ARISING OUT OF OR
//  RELATED TO THE NXP SOFTWARE/SOURCE CODE EVEN IF NXP HAS BEEN ADVISED OF
//  THE POSSIBILITY OF SUCH DAMAGES.
// -----------------------------------------------------------------------------

#include <stdlib.h>
#include <string.h>

#include "rtk.h"
#include "pcb.h"
#include "gpio.h"
#include "rf_cfg.h"
#include "OL2381_defs.h"
#include "rftransceive.h"
#include "conversions.h"

#if RF_TRANSCEIVE_ENABLE

#define SSP_INT_FLAG_OVERRUN      0x01
#define SSP_INT_FLAG_TIMEOUT      0x02
#define SSP_INT_FLAG_RXHALFFUL    0x04
#define SSP_INT_FLAG_TXHALFEMPTY  0x08

#define RF_IDLE         0
#define RF_TX_SETUP     1
#define RF_TX_BUSY      2
#define RF_RX_SETUP     3
#define RF_RX_BUSY      4
#define RF_RX_STOP_REQ  5

// defines for SEN pin of OL2381
#define ENABLE_RF_CHIP()            gpio_set( OL2381_SEN_PORTPIN )
#define DISABLE_RF_CHIP()           gpio_clr( OL2381_SEN_PORTPIN )

#define SPIPRESCALE              2
#define SPICLOCK_MAX             (4*1000*1000)     // 4 MHz (max for OL2381)
#define SSP_BASIC_OPERATION      0x0080   // SPI Frame format, CPOL = 0, CPHA = 1

/*----------------------------------------------------------------------------*/
/* Variables declaration                                                      */
/*----------------------------------------------------------------------------*/

static volatile int RfStatus;

static int        RfDriverInited;
// Semaphore stuff
static rtk_sem_t  sem_spi_tx_done;
static rtk_sem_t  sem_rf_rx_tx_done;
static rtk_sem_t  sem_start_recv;
static rtk_sem_t  sem_exclusive_use;
// for SPI communication with RF chip the following globals
static char       SpiTxRfData;        // Boolean flag to indicate RF data transmission by SPI IRQ handler
static int        SpiTxByteCount;
static char     * SpiTxBufPointer;
static int        SpiRxByteCount;
static char     * SpiRxBufPointer;

static int        RfTransmittersWaiting;
static char       RfReceiverWaiting;  // Boolean flag to indicate Receive thread is waiting on Transmit thread

// for RF TX the following globals
static char       RfTxInvertData;     // Boolean flag to indicate all RF data to be inverted
static int        RfTxRunInFrameCount;
static short      RfTxRunInFrameValue;
static int        RfTxPreambleFrameCount;
static short      RfTxPreambleFrameValue[4];

// for RF RX the following globals
static int        RfRxSignalByteCount;
static int        RfRxByteCount;
static int        RfRxWaitSignal;

static char       RfEncoding;
static char       RfPreScaler;
static short      RfMainScaler;

static int            rf_buf_read_index;
static volatile char  rf_receive_buf[RF_MAX_RXTX_MESSAGE_LENGTH];

/*----------------------------------------------------------------------------*/
/* Local functions                                                            */
/*----------------------------------------------------------------------------*/

static void init_global_variables(void)
{
    int i;

    RfStatus               = RF_IDLE;   // Put this driver in the RF idle mode
    RfDriverInited         = RF_FALSE;
    sem_spi_tx_done   = rtk_sem_create( 0, "rf.spi" );
    sem_rf_rx_tx_done = rtk_sem_create( 0, "rf.rtx" );
    sem_start_recv    = rtk_sem_create( 0, "rf.rcv" );
    sem_exclusive_use = rtk_sem_create( 1, "rf.bsy" );

    SpiTxRfData            = RF_FALSE;
    SpiTxByteCount         = 0;
    SpiTxBufPointer        = NULL;
    SpiRxByteCount         = 0;
    SpiRxBufPointer        = NULL;

    RfTransmittersWaiting  = 0;
    RfReceiverWaiting      = 0;

    RfTxInvertData         = RF_FALSE;
    RfTxRunInFrameCount    = 0;
    RfTxRunInFrameValue    = 0;
	RfTxPreambleFrameCount = 0;
    for (i=0; i<4; i++)
    {
        RfTxPreambleFrameValue[i] = 0;
    }

	RfRxSignalByteCount    = 0;
	RfRxByteCount          = 0;
    RfRxWaitSignal         = 0;
	
    RfEncoding             = RF_ENCODING_3_OUT_OF_6;
    RfPreScaler            = 1;
    RfMainScaler           = 99;

    rf_buf_read_index = 0;
    for (i=0; i<sizeof(rf_receive_buf); i++)
    {
        rf_receive_buf[i] = 0;
    }
}

/*----------------------------------------------------------------------------*/
/* Name       : GPIO_IRQHandler                                               */
/* Description: SSP device is used for communication with OL2381.             */
/*              If RxFIFO is at least half full, read fifo until it's empty.  */
/*              If TxFIFO is at least half empty, write fifo until it's full. */
/*              This will maximize the use of both FIFOs and performance.     */
/* Parameters : none                                                          */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

void GPIO_IRQHandler(void)
{
	if (gpio_intgetflag( OL2381_P11_INT_PORTPIN ))    // check if P11/INT has generated a falling edge interrupt
	{
	    // disable (rising edge) IRQ from OL2381 pin P11/INT
	    gpio_intdisable( OL2381_P11_INT_PORTPIN );
        //??
	    gpio_intclear( OL2381_P11_INT_PORTPIN );
	}
}

/*----------------------------------------------------------------------------*/

static inline void ssp_init_cr_regs(void)
{
    RF_SSPCR0  = SSP_BASIC_OPERATION;
    RF_SSPCR1  = 0;
    RF_SSPIMSC = 0;        // disable all SSP interrupts
}

/*----------------------------------------------------------------------------*/

static inline void ssp_set_framesize(char frameSize)
{
    RF_SSPCR0 &= 0xFFF0;
    RF_SSPCR0 |= (frameSize - 1);
}

/*----------------------------------------------------------------------------*/

static inline void ssp_set_cpol(void)
{
    RF_SSPCR0 |= 0x40;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_clr_cpol(void)
{
    RF_SSPCR0 &= ~0x40;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_disable(void)
{
    RF_SSPCR1 &= ~0x02;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_enable(void)
{
    RF_SSPCR1 |= 0x02;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_conf_master(void)
{
    RF_SSPCR1 &= ~0x04;    // set SSP device as master
}

/*----------------------------------------------------------------------------*/

static inline void ssp_conf_slave(void)
{
    RF_SSPCR1 |= 0x04;     // set SSP device as slave
}

/*----------------------------------------------------------------------------*/

static inline void ssp_enable_slave_output(void)
{
    RF_SSPCR1 &= ~0x08;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_disable_slave_output(void)
{
    RF_SSPCR1 |= 0x08;
}

/*----------------------------------------------------------------------------*/

static inline void ssp_disable_rxim_irq(void)
{
    RF_SSPIMSC &= ~0x04;   // disable the RXIM (Rx FIFO half full) interrupt
}

/*----------------------------------------------------------------------------*/

static inline void ssp_enable_rxim_irq(void)
{
    RF_SSPIMSC |= 0x04;    // enable the RXIM (Rx FIFO half full) interrupt
}

/*----------------------------------------------------------------------------*/

static inline void ssp_disable_txim_irq(void)
{
    RF_SSPIMSC &= ~0x08;   // disable the TXIM (Tx FIFO half empty) interrupt
}

/*----------------------------------------------------------------------------*/

static inline void ssp_enable_txim_irq(void)
{
    RF_SSPIMSC |= 0x08;    // enable the TXIM (Tx FIFO half empty) interrupt
}

/*----------------------------------------------------------------------------*/

static void ssp_rd_rxfifo(void)
{
    short sfr_data;

    // Read all data that is in RxFifo of the SSP device
    while (RF_SSPSR & 0x04)
    {
        sfr_data = RF_SSPDR;
        if (SpiRxByteCount > 0)
        {
            if (SpiRxBufPointer != 0)
            {
                *SpiRxBufPointer++ = (char)sfr_data;
            }
            SpiRxByteCount--;
        }
    }
}

/*----------------------------------------------------------------------------*/

static short ssp_empty_rxfifo(void)
{
    short dummy = 0;

    // Delete all data that is in RxFifo of the SSP device
    while (RF_SSPSR & 0x04)
    {
        dummy = RF_SSPDR;
    }
    // return dummy otherwise the compiler optimizes the while loop away
    return dummy;
}

/*----------------------------------------------------------------------------*/

void SSP_IRQHandler(void)
{
    unsigned int  reg_val = RF_SSPMIS;

    if (reg_val & SSP_INT_FLAG_RXHALFFUL)
    {
        short rxframe;
        // Rx at least half full
        // Only here because of RF receive data
        while (RF_SSPSR & 0x04)
        {
            rxframe = RF_SSPDR;
			if (RfRxByteCount < RF_MAX_RXTX_MESSAGE_LENGTH)
			{
                char rx_data;
                if (RfEncoding == RF_ENCODING_MANCHESTER_SW)
                {
                    rx_data = conv_manchester_to_byte(rxframe);
                }
                else if (RfEncoding == RF_ENCODING_3_OUT_OF_6)
                {
                    rx_data = conv_3outof6_to_byte(rxframe);
                }
                else
                {
                    // RF_ENCODING_NRZ and RF_ENCODING_MANCHESTER_HW
                    rx_data = (char)(rxframe);
                }
                rf_receive_buf[RfRxByteCount] = rx_data;
				RfRxByteCount++;
            }
        }
		if (RfRxWaitSignal)
		{
            int tmp = RfRxSignalByteCount;
            if (RfRxByteCount >= tmp)
            {
                RfRxWaitSignal = 0;
                rtk_sem_release_isr( sem_rf_rx_tx_done );
            }
		}
    }
    if (reg_val & SSP_INT_FLAG_TXHALFEMPTY)
    {
        // TxFifo at least half empty (room for 4 till 8 frames)
        // Put bytes in TxFifo till full, check TNF bit of SSP status register
        while ((RF_SSPSR & 0x02) && (SpiTxByteCount > 0))
        {
        	if (SpiTxRfData == RF_TRUE)
        	{
                // Here the RF data will be send to RF chip
                char  data_avail = 1;
                short spi_data;
                if (RfTxRunInFrameCount > 0)
                {
        		    spi_data = RfTxRunInFrameValue;
    		        RfTxRunInFrameCount -= 1;
        	    }
                else if (RfTxPreambleFrameCount > 0)
                {
                    spi_data = RfTxPreambleFrameValue[RfTxPreambleFrameCount-1];
                    RfTxPreambleFrameCount -= 1;
                }
                else if (SpiTxByteCount > 0)
                {
                    if (RfEncoding == RF_ENCODING_MANCHESTER_SW)
                    {
			            spi_data = conv_byte_to_manchester(*SpiTxBufPointer);
                    }
                    else if (RfEncoding == RF_ENCODING_3_OUT_OF_6)
                    {
			            spi_data = conv_byte_to_3outof6(*SpiTxBufPointer);
                    }
                    else
                    {
                        // RF_ENCODING_NRZ and RF_ENCODING_MANCHESTER_HW
			            spi_data = *SpiTxBufPointer;
                    }
                    SpiTxBufPointer++;
                    SpiTxByteCount--;
                }
                else
                {
                    data_avail = 0;
                }
                if (data_avail)
                {
                    if (RfTxInvertData == RF_TRUE)
                    {
                        RF_SSPDR = ~spi_data;
                    }
                    else
                    {
                        RF_SSPDR = spi_data;
                    }
                }
            }
        	else
            {
                if (SpiTxBufPointer != 0)
                {
                    // Here the RF chip SFRs will be written
                    RF_SSPDR = *SpiTxBufPointer;
                    SpiTxBufPointer++;
                }
                else
                {
                    // Here the RF chip SFRs will be read
                    RF_SSPDR = 0;        // send dummy character to read slave data
                    ssp_rd_rxfifo();     // read all data received till this moment
                }
                SpiTxByteCount--;
            }
        }
        if (SpiTxByteCount == 0)
        {
            ssp_disable_txim_irq();
            rtk_sem_release_isr( sem_spi_tx_done ); // signal transmission done
        }
    }
}

/*----------------------------------------------------------------------------*/

static void ssp_set_clk_fast(void)
{
    unsigned int cr0_scr;

    // Set the SPI bus clock freq. for fastest SFR access rate
    // For OL2381 SPI bus clock is max 4 MHz

    // SSPCPSR clock prescale register, master mode, minimum divisor is 0x02
    RF_SSPCPSR = SPIPRESCALE;

    // SSP CPSR clock prescale register (master mode), EVEN value between 2 and 254
    // SPI bus clock = PCLK / (CPSR * (SCR + 1))
    // e.g. when PCLK = 24MHz and CPSR = 2 then SCR must be 2 for a SPI clock of 4MHz
    // SPI bus clock = 24MHz / (2 * (2 + 1)) = 4 MHz
    cr0_scr = (((RF_SSP_GET_CLOCK() + SPICLOCK_MAX-1) / SPICLOCK_MAX + SPIPRESCALE-1) / SPIPRESCALE) - 1; // Expression rounds up (twice)
    RF_SSPCR0 &= 0x00FF;
    RF_SSPCR0 |= (cr0_scr << 8);
}

/*----------------------------------------------------------------------------*/
/* Name       : gen_nineth_pulse                                              */
/* Description: 9th edge is generated by pullup resistor internally defined   */
/*              on the LPC_SCK_PORTPIN (see ssp_init function)                */
/* Parameters : none                                                          */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void gen_nineth_pulse(char frameSize, char rftxmode)
{
    ssp_disable();
    ssp_set_framesize(frameSize);
    ssp_conf_slave();
    if (rftxmode == RF_TRUE)
    {
        // only for TX
        if (RfTxInvertData == RF_TRUE)
        {
            RF_SSPDR = ~RfTxRunInFrameValue;
        }
        else
        {
            RF_SSPDR = RfTxRunInFrameValue;
        }
        RfTxRunInFrameCount -= 1;
        // Change status to busy
        RfStatus = RF_TX_BUSY;
    }
    else
    {
        // only for RX
        ssp_disable_slave_output();
        if (RfEncoding != RF_ENCODING_MANCHESTER_HW)
        {
            ssp_set_cpol();
        }
        ssp_enable_rxim_irq();
        // Change status to busy
        RfStatus = RF_RX_BUSY;
    }
    ssp_enable();
}

/*----------------------------------------------------------------------------*/

static void ssp_init(void)
{
    // Init the SSP device which is the interface to the RF chip

    RF_SSP_ENABLE_DEV();
    RF_SSP_ENABLE_SCK();

    ssp_init_cr_regs();

	// Note: EN pin of RF chip is high active, which is in conflict with the SSEL line of the SSP device
	//       in the MCU. So the EN pin will be driven by a GPIO (PIO) pin instead of the SSEL line.
		
	// Setup GPIO func to control EN pin of RF chip
    gpio_alloc( OL2381_SEN_PORTPIN, PCB_MODE_PULLDOWN, GPIO_OUTPUT );

	// Setup SSP SCK func to drive SPI clock pin of RF chip
    pcb_alloc( LPC_SCK_PORTPIN, LPC_SCK_FUNC, PCB_MODE_PULLUP );

    // Setup SSP MISO func to connect to SDO pin of RF chip
    pcb_alloc( LPC_MISO_PORTPIN, LPC_MISO_FUNC, PCB_MODE_PULLDOWN );

    // Setup SSP MOSI func to connect to SDIO pin of RF chip
    pcb_alloc( LPC_MOSI_PORTPIN, LPC_MOSI_FUNC, PCB_MODE_PULLDOWN );

    // Setup the SSEL line
    pcb_alloc( LPC_SSEL_PORTPIN, LPC_SSEL_FUNC, PCB_MODE_PULLDOWN );

    ssp_set_clk_fast();
    ssp_set_framesize(8);
    ssp_conf_master();
    ssp_enable();
    ssp_empty_rxfifo();                       // clear the RxFIFO

    RF_SSP_INT_INSTALL();
    RF_SSP_INT_ENABLE();
}

/*----------------------------------------------------------------------------*/
/* Name       : ssp_wr_bytes                                                  */
/* Description: Write data via SSP interface                                  */
/* Parameters :                                                               */
/*    buffer   - pointer to data in RAM to be written via SSP                 */
/*    numBytes - number of bytes to be written                                */
/*    rfdata   - Boolean to indicate RF data transmission by SPI IRQ handler  */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void ssp_wr_bytes(char * buffer, int numBytes, char rfdata)
{
    if (numBytes > 0)
    {
        SpiTxRfData     = rfdata;
        SpiTxByteCount  = numBytes;    // number of bytes to send
        SpiRxByteCount  = 0;           // do not read useless bytes because of full duplex setup
        SpiTxBufPointer = buffer;      // real data to be send
        SpiRxBufPointer = 0;
        // Only enable the TXIM (Tx FIFO half empty) interrupt, to transmit real data
        ssp_enable_txim_irq();
        // wait for transmit done signal
        rtk_sem_acquire( sem_spi_tx_done );
        // wait till SSP Tx FIFO empty
        while ((RF_SSPSR & 0x01) == 0);
        if (rfdata == RF_TRUE)
        {
            // put 1 dummy frame in the TX fifo
            RF_SSPDR = 0;
            // now again wait till TX fifo gets empty
            while ((RF_SSPSR & 0x01) == 0);
            // now the real buffer data has been shifted out by the SSP block
        }
        ssp_empty_rxfifo();            // clear the RxFIFO
    }
}

/*----------------------------------------------------------------------------*/
/* Name       : ssp_rd_bytes_as_master                                        */
/* Description: Read data via SSP interface.                                  */
/*              Master must generate clock, thus send dummy data.             */
/* Parameters :                                                               */
/*    buffer   - pointer to RAM where data from SSP shall be written to       */
/*    numBytes - number of bytes to be read                                   */
/* Returns    : read data bytes via buffer                                    */
/*----------------------------------------------------------------------------*/

static void ssp_rd_bytes_as_master(char * buffer, int numBytes)
{
    if (numBytes > 0)
    {
    	pcb_free( LPC_MOSI_PORTPIN );
    	gpio_alloc( LPC_MOSI_PORTPIN, PCB_MODE_PULLNONE, GPIO_INPUT );
        SpiTxByteCount  = numBytes;    // for each byte to receive send a dummy byte, to generate CLK
        SpiRxByteCount  = numBytes;    // number of bytes to receive
        SpiTxBufPointer = 0;           // dummy bytes are send, so no real data
        SpiRxBufPointer = buffer;
        // Only enable the TXIM (Tx FIFO half empty) interrupt, to transmit dummy data
        ssp_enable_txim_irq();
        // wait for transmit done signal
        rtk_sem_acquire( sem_spi_tx_done );
        // wait till SSP Tx FIFO empty
        while ((RF_SSPSR & 0x01) == 0);
        ssp_rd_rxfifo();            // read all remaining data from RxFifo
        gpio_free( LPC_MOSI_PORTPIN );
        pcb_alloc( LPC_MOSI_PORTPIN, LPC_MOSI_FUNC, PCB_MODE_PULLNONE );
    }
}

/*----------------------------------------------------------------------------*/
/* Name       : RFchip_wr_one_reg                                             */
/* Description: Write to an address of the RF chip.                           */
/*              Note: This function does not switch to the approriate bank !! */
/* Parameters :                                                               */
/*    addr - address in range 0x00 till 0x3F                                  */
/*    val  - value to be written in RF chip register                          */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void RFchip_wr_one_reg(char addr, char val)
{
	char cTmp[2];

    cTmp[0] = (addr & 0x3F);              // bits 7..6 = 00b means write SFR
    cTmp[1] = val;
    DISABLE_RF_CHIP();                    // signal end of communication
    ENABLE_RF_CHIP();                     // start new communication
    ssp_wr_bytes(cTmp, 2, RF_FALSE);      // write address bits + SFR data bits
}

/*----------------------------------------------------------------------------*/
/* Name       : RFchip_wr_regs                                                */
/* Description: Write bits via SSP device to the RF chip SFRs                 */
/* Parameters :                                                               */
/*    buffer       - Start address of the data in uC RAM                      */
/*    numRegs      - Number of RF chip SFRs to write                          */
/*    RegStartAddr - Start address of RF chip SFR register                    */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void RFchip_wr_regs(char * buffer, int numRegs, char RegStartAddr)
{
    char cAddr;

	// check if write to registers in bank 1
    if (RegStartAddr > 0x3F)
    {
    	RFchip_wr_one_reg(OL2381_BANKSEL_REG, 0x01);
    }

    // write data to registers
    cAddr = (RegStartAddr & 0x3F);           // bits 7..6 = 00b means write SFR
    DISABLE_RF_CHIP();                       // signal end of communication
    ENABLE_RF_CHIP();                        // start new communication
    ssp_wr_bytes(&cAddr, 1, RF_FALSE);       // write address bits
    ssp_wr_bytes(buffer, numRegs, RF_FALSE); // write SFR data bits

    if (RegStartAddr > 0x3F)
    {
    	RFchip_wr_one_reg(OL2381_BANKSEL_REG, 0x00);
    }
}

/*----------------------------------------------------------------------------*/
/* Name       : RFchip_rd_regs                                                */
/* Description: Read bits via SSP device from the RF chip SFRs                */
/* Parameters :                                                               */
/*    buffer       - Start address of the data in uC RAM                      */
/*    numRegs      - Number of RF chip SFRs to read                           */
/*    RegStartAddr - Start address of RF chip SFR register                    */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void RFchip_rd_regs(char * buffer, int numRegs, char RegStartAddr)
{
    char cAddr;

    if (RegStartAddr > 0x3F)
    {
    	RFchip_wr_one_reg(OL2381_BANKSEL_REG, 0x01);
    }

    // read data from registers
    cAddr = (RegStartAddr & 0x3F) | 0x40;    // bits 7..6 = 01b means read SFR
    DISABLE_RF_CHIP();                       // signal end of communication
    ENABLE_RF_CHIP();                        // start new communication
    ssp_wr_bytes(&cAddr, 1, RF_FALSE);       // write address bits
    ssp_rd_bytes_as_master(buffer, numRegs); // read SFR data bits
    			
    if (RegStartAddr > 0x3F)
    {
    	RFchip_wr_one_reg(OL2381_BANKSEL_REG, 0x00);
    }
}

/*----------------------------------------------------------------------------*/
/* Name       : RFchip_tx_rfdata                                              */
/* Description: Transmit data over the air                                    */
/* Parameters :                                                               */
/*    buffer       - Start address of the data to be transmitted              */
/*    size         - the number of bytes to transmit                          */
/*    options      - the options for the RF chip transmit command             */
/*    spiFrameSize - the size (width) of the spi data frame                   */
/* Returns    : none                                                          */
/*----------------------------------------------------------------------------*/

static void RFchip_tx_rfdata(char * buffer, int size, char options, char spiFrameSize)
{
    char cAddr = (options & 0x3F) | 0xC0;    // bits 7..6 = 11b means transmit command

    // make sure that the correct number of run-in bytes and the preamble bytes are send
    DISABLE_RF_CHIP();                       // signal end of communication
    ENABLE_RF_CHIP();                        // start new communication
    ssp_wr_bytes(&cAddr, 1, RF_FALSE);       // send TRANSMIT command
    gen_nineth_pulse(spiFrameSize, RF_TRUE);
	ssp_wr_bytes(buffer, size, RF_TRUE);     // send RF data
    DISABLE_RF_CHIP();                       // signal end of communication
    ssp_set_framesize(8);
    ssp_conf_master();
}

static void RFchip_rx_rfdata(char options, char spiFrameSize)
{
    char cAddr = (options & 0x3F) | 0x80;    // bits 7..6 = 10b means receive command
    
    DISABLE_RF_CHIP();                       // signal end of communication
    ENABLE_RF_CHIP();                        // start new communication
    ssp_wr_bytes(&cAddr, 1, RF_FALSE);       // send RECEIVE command
    gen_nineth_pulse(spiFrameSize, RF_FALSE);
}

/*----------------------------------------------------------------------------*/

static int test_spi_communication(void)
{
    int  i;
    char cTmp1;
    char cTmp2;

	// setup the general purpose ports (P10-P14) of the OL2381
    RFchip_wr_regs((char*)&sfr_buffer_bank0[OL2381_PORTCON0_REG], 3, OL2381_PORTCON0_REG);
    for (i=0; i<256; i++)
    {
    	cTmp1 = i;
        RFchip_wr_one_reg(OL2381_FC3H_REG, cTmp1);
        RFchip_rd_regs(&cTmp2, 1, OL2381_FC3H_REG);
        if (cTmp1 != cTmp2)
        {
            return RF_COMMUNICATION_ERROR;
        }
    }
    return RF_SUCCESS;
}

/*----------------------------------------------------------------------------*/

static void RFchip_reset_device(void)
{
    // Reset OL2381 to power-on reset state, all SFR's are reset.
    // Set bit 0 (RESET) of PWRMODE.
    RFchip_wr_one_reg(OL2381_PWRMODE_REG, 0x01);
    DISABLE_RF_CHIP();
}

/*----------------------------------------------------------------------------*/

static void RFchip_hybernate(void)
{
#ifdef RF_LOW_POWER_MODE
    // Put OL2381 in low current mode (power down), all SFR's keep their values.
    // Set bit 1 (PD) of PWRMODE.
    RFchip_wr_one_reg(OL2381_PWRMODE_REG, 0x02);
#endif
    DISABLE_RF_CHIP();
}

/*----------------------------------------------------------------------------*/
/* Name       : RFchip_init                                                   */
/* Description: init RF chip via SSP                                          */
/* Parameters :                                                               */
/* Returns    : err_stat, RF_SUCCESS, error                                   */
/*----------------------------------------------------------------------------*/

static int RFchip_init(void)
{
    int  err_stat = RF_SUCCESS;
    char cTmp;

    // reset the OL2381 (this will also select bank 0)
    RFchip_reset_device();

    err_stat = test_spi_communication();
    if (err_stat == RF_SUCCESS)
    {
        // write 64 SFR bytes in bank 0 to init the OL2381
        RFchip_wr_regs((char*)sfr_buffer_bank0, sizeof(sfr_buffer_bank0), OL2381_BASE_ADDR);
	    // write 17 SFR bytes in bank 1 to init the OL2381
        RFchip_wr_regs((char*)sfr_buffer_bank1, sizeof(sfr_buffer_bank1), OL2381_EXTRXSTATUS_REG);
        // take away (clear) the POR interrupt
        RFchip_rd_regs(&cTmp, 1, OL2381_IFLAG_REG);
    }
    return err_stat;
}

/*----------------------------------------------------------------------------*/

static void RFchip_set_chiprate(unsigned char chiprate)
{
    char  presc  = 0;
    short mainsc = 0;

    // chip rate = (XTAL / 2^PRESC) * ((2^11 + MAINSC) / 2^12) * (1 / 128)
    //    or
    // (2^11 + MAINSC) / 2^PRESC = (chip rate * 128 * 2^12) / XTAL
    //    where XTAL is 16 MHz

    switch (chiprate)
    {
    case RF_CHIPRATE_4800:
        //    (2^11 + MAINSC) / 2^PRESC = (4.800 kHz * 128 * 2^12) / 16000 kHz = 157.2864
        //    (2^11 + MAINSC) = 157.2864 * 2^PRESC
        // when PRESC=4, then
        //    MAINSC = (157.2864 * 16) - 2^11 = (157.2864 * 16) - 2048 = 468.5824 = 469
        // when we put this back in the original formula we get the chip rate
        //    chip rate = (16000 kHz / 16) * ((2048 + 469) / 4096) * (1 / 128) = 4.801 kHz
        presc  = 4;
        mainsc = 469;
        break;
    case RF_CHIPRATE_32768:
        //    (2^11 + MAINSC) / 2^PRESC = (32.768 kHz * 128 * 2^12) / 16000 kHz = 1073.741824
        //    (2^11 + MAINSC) = 1073.741824 * 2^PRESC
        // when PRESC=1, then
        //    MAINSC = (1073.741824 * 2) - 2^11 = (1073.741824 * 2) - 2048 = 99.483648 = 99
        // when we put this back in the original formula we get the chip rate
        //    chip rate = (16000 kHz / 2) * ((2048 + 99) / 4096) * (1 / 128) = 32.761 kHz
        presc  = 1;
        mainsc = 99;
        break;
    case RF_CHIPRATE_100000:
        //    (2^11 + MAINSC) / 2^PRESC = (100.000 kHz * 128 * 2^12) / 16000 kHz = 3276.8
        //    (2^11 + MAINSC) = 3276.8 * 2^PRESC
        // when PRESC=0, then
        //    MAINSC = (3276.8 * 1) - 2^11 = (3276.8 * 1) - 2048 = 1228.8 = 1229
        // when we put this back in the original formula we get the chip rate
        //    chip rate = (16000 kHz / 1) * ((2048 + 1229) / 4096) * (1 / 128) = 100.006 kHz
        presc  = 0;
        mainsc = 1229;
        break;
    default:
        // use settings for RF_CHIPRATE_32768
        presc  = 1;
        mainsc = 99;
        break;
    }
    if ((RfPreScaler != presc) || (RfMainScaler != mainsc))
    {
        RfMainScaler = mainsc;
        RfPreScaler  = presc;
        sfr_buffer_bank0[OL2381_TIMING0_REG]  = (RfMainScaler & 0xFF);
        sfr_buffer_bank0[OL2381_TIMING1_REG] &= 0xC0;
        sfr_buffer_bank0[OL2381_TIMING1_REG] |= ((RfMainScaler >> 8) & 0x07);
        sfr_buffer_bank0[OL2381_TIMING1_REG] |= ((RfPreScaler & 0x07) << 3);
        RFchip_wr_regs((char*)&sfr_buffer_bank0[OL2381_TIMING0_REG], 2, OL2381_TIMING0_REG);
    }
}

/*----------------------------------------------------------------------------*/

static int RFchip_tx_setup(rf_tx_setup_t * tx_setup, char * pSpiFrameSize)
{
    int  i;
    int  run_in_chip_cnt    = tx_setup->run_in_chip_cnt;
    int  preamble_chip_cnt  = tx_setup->preamble_chip_cnt;
    int  remaining_chip_cnt;
    int  preamble_value     = tx_setup->preamble_value;
    int  update_txcon       = 0;
    char new_output_power   = tx_setup->rf_output_power;
    char spiFrameSize;
    char fdev = 0;

    // make sure that the correct number of run-in chips are send
    if (tx_setup->rf_encoding == RF_ENCODING_NRZ)
    {
        // 8 bits data transfer, each run-in bit will be 1 chip
        spiFrameSize = 8;
        RfTxRunInFrameValue = 0x5555;
    }
    else if (tx_setup->rf_encoding == RF_ENCODING_MANCHESTER_HW)
    {
        // 8 bits data transfer, each run-in bit will be 2 chips because of HW manchester
        run_in_chip_cnt = run_in_chip_cnt / 2;
        spiFrameSize = 8;
        RfTxRunInFrameValue = 0xFFFF;
    }
    else if (tx_setup->rf_encoding == RF_ENCODING_MANCHESTER_SW)
    {
        // 16 bits data transfer, each run-in bit will be 1 chip
        spiFrameSize = 16;
        RfTxRunInFrameValue = 0x5555;
    }
    else if (tx_setup->rf_encoding == RF_ENCODING_3_OUT_OF_6)
    {
        // 12 bits data transfer, each run-in bit will be 1 chip
        spiFrameSize = 12;
        RfTxRunInFrameValue = 0x5555;
    }
    else
    {
        return RF_ENCODING_UNKNOWN;
    }
    *pSpiFrameSize = spiFrameSize;
    RfEncoding = tx_setup->rf_encoding;
    RfTxRunInFrameCount = (run_in_chip_cnt + (spiFrameSize - 1)) / spiFrameSize;
    if (RfTxRunInFrameCount < 1)
    {
        return RF_RUN_IN_CHIP_CNT_TOO_SMALL;
    }
    if ((RfEncoding == RF_ENCODING_MANCHESTER_HW) && (preamble_chip_cnt > 16))
    {
        // For HW Manchester coding allow max 16 preamble bits,
        // because this will result in max 32 chips
        return RF_PREAMBLE_CHIP_CNT_INVALID;
    }
    // max 32 preamble chips are allowed
    if ((preamble_chip_cnt > 0) && (preamble_chip_cnt <= 32))
    {
        RfTxPreambleFrameCount = (preamble_chip_cnt + (spiFrameSize - 1)) / spiFrameSize;
        if (preamble_chip_cnt < 32) preamble_value &= ((1 << preamble_chip_cnt) - 1);
        remaining_chip_cnt = preamble_chip_cnt;
        for (i=0; i<RfTxPreambleFrameCount; i++)
        {
            if (remaining_chip_cnt >= spiFrameSize)
            {
                RfTxPreambleFrameValue[i] = preamble_value & ((1 << spiFrameSize) - 1);
                preamble_value = (preamble_value >> spiFrameSize);
                remaining_chip_cnt -= spiFrameSize;
            }
            else
            {
                RfTxPreambleFrameValue[i]  = (RfTxRunInFrameValue << remaining_chip_cnt);
                RfTxPreambleFrameValue[i] |= (preamble_value & ((1 << remaining_chip_cnt) - 1));
                preamble_value = 0;
                remaining_chip_cnt = 0;
            }
        }
    }
    else
    {
        return RF_PREAMBLE_CHIP_CNT_INVALID;
    }
    if (RfEncoding == RF_ENCODING_MANCHESTER_HW)
    {
        // check if TXCLKSEL bit is not set
        if ((sfr_buffer_bank0[OL2381_TXCON_REG] & 0x20) == 0)
        {
    	    // set the TXCLKSEL bit in TXCON register
    	    sfr_buffer_bank0[OL2381_TXCON_REG] |= 0x20;  // Use the chip clock as CLKtx
            update_txcon = 1;
        }
    }
    else
    {
        // check if TXCLKSEL bit is set
        if ((sfr_buffer_bank0[OL2381_TXCON_REG] & 0x20) == 0x20)
        {
    	    // clear the TXCLKSEL bit in TXCON register
    	    sfr_buffer_bank0[OL2381_TXCON_REG] &= ~0x20;  // Use the bit clock as CLKtx
            update_txcon = 1;
        }
    }
    if (new_output_power < RF_OUTPUT_POWER_MIN)
    {
        new_output_power = RF_OUTPUT_POWER_MIN;
    }
    else if (new_output_power > RF_OUTPUT_POWER_MAX)
    {
        new_output_power = RF_OUTPUT_POWER_MAX;
    }
    RfTxInvertData = tx_setup->rf_low_freq_is_zero;
    if (tx_setup->rf_modulation == RF_MODULATION_ASK)
    {
        // check if INV_TX_DATA bit is not set
        if ((sfr_buffer_bank0[OL2381_TXCON_REG] & 0x40) == 0)
        {
    	    // set the INV_TX_DATA bit in TXCON register (for ASK mode)
    	    sfr_buffer_bank0[OL2381_TXCON_REG] |= 0x40;
            update_txcon = 1;
        }
        // use ACON0
        if ((sfr_buffer_bank0[OL2381_ACON0_REG] & 0x1F) != new_output_power)
        {
            sfr_buffer_bank0[OL2381_ACON0_REG] &= 0xE0;
            sfr_buffer_bank0[OL2381_ACON0_REG] |= new_output_power;
            RFchip_wr_one_reg(OL2381_ACON0_REG, sfr_buffer_bank0[OL2381_ACON0_REG]);
        }
    }
    else
    {
        // check if INV_TX_DATA bit is set
        if ((sfr_buffer_bank0[OL2381_TXCON_REG] & 0x40) == 0x40)
        {
    	    // clear the INV_TX_DATA bit in TXCON register (for FSK mode)
    	    sfr_buffer_bank0[OL2381_TXCON_REG] &= ~0x40;
            update_txcon = 1;
        }
        // use ACON1
        if ((sfr_buffer_bank0[OL2381_ACON1_REG] & 0x1F) != new_output_power)
        {
            sfr_buffer_bank0[OL2381_ACON1_REG] &= 0xE0;
            sfr_buffer_bank0[OL2381_ACON1_REG] |= new_output_power;
            RFchip_wr_one_reg(OL2381_ACON1_REG, sfr_buffer_bank0[OL2381_ACON1_REG]);
        }
    }
    if (update_txcon)
    {
        RFchip_wr_one_reg(OL2381_TXCON_REG, sfr_buffer_bank0[OL2381_TXCON_REG]);
    }

    RFchip_set_chiprate(tx_setup->rf_chiprate);
    //
    // FSK deviation freq = (+/-) Fref * (FDEV / 65536)
    //    or
    // FDEV = (FSKdev * 65536) / Fref
    //    where Fref is 16 MHz (or 16000kHz)
    //
    if (tx_setup->rf_chiprate == RF_CHIPRATE_100000)
    {
        // FDEV = (35 kHz * 65536) / 16000 kHz = 143.36 = 144 = 18 * 8 = 18 * 2^3
        // so FDEV_EXP = 3 and FDEV_MANT = 18 or 0x12 or 10010b
        fdev = 0x72;
    }
    else
    {
        // FDEV = (50 kHz * 65536) / 16000 kHz = 204.8 = 204 = 26 * 8 = 26 * 2^3
        // so FDEV_EXP = 3 and FDEV_MANT = 26 or 0x1A or 11010b
        fdev = 0x7A;
    }
    if (sfr_buffer_bank0[OL2381_FDEV_REG] != fdev)
    {
        sfr_buffer_bank0[OL2381_FDEV_REG] = fdev;
        RFchip_wr_one_reg(OL2381_FDEV_REG, sfr_buffer_bank0[OL2381_FDEV_REG]);
    }
    return RF_SUCCESS;
}

/*----------------------------------------------------------------------------*/

static int RFchip_rx_setup(rf_rx_setup_t * rx_setup, char * pSpiFrameSize)
{
    int  err_stat = RF_SUCCESS;
    int  i = 0;
    int  update_rxbw = 0;
    int  update_rxcon = 0;
    int  update_preamble = 0;
    char spiFrameSize;
    char cfbw = 0;
    char bbfc = 0;
    int  preamble_chip_cnt = 0;
    char preamble_bytes[4];
static int prev_preamble_chip_cnt = 0;
static int prev_preamble_value    = 0;

    if (rx_setup->rf_encoding == RF_ENCODING_NRZ)
    {
        spiFrameSize = 8;
    }
    else if (rx_setup->rf_encoding == RF_ENCODING_MANCHESTER_HW)
    {
        spiFrameSize = 8;
    }
    else if (rx_setup->rf_encoding == RF_ENCODING_MANCHESTER_SW)
    {
        spiFrameSize = 16;
    }
    else if (rx_setup->rf_encoding == RF_ENCODING_3_OUT_OF_6)
    {
        spiFrameSize = 12;
    }
    else
    {
        return RF_ENCODING_UNKNOWN;
    }
    *pSpiFrameSize = spiFrameSize;
    RfEncoding = rx_setup->rf_encoding;
    if ((RfEncoding == RF_ENCODING_MANCHESTER_HW) && (rx_setup->preamble_chip_cnt > 16))
    {
      	// For HW Manchester coding allow max 16 preamble bits,
       	// because this will result in max 32 chips
        return RF_PREAMBLE_CHIP_CNT_INVALID;
    }
    // max 32 preamble chips are allowed
    if ((rx_setup->preamble_chip_cnt > 0) && (rx_setup->preamble_chip_cnt <= 32))
    {
        preamble_chip_cnt = rx_setup->preamble_chip_cnt;
        for (i=0; i<4; i++)
        {
        	preamble_bytes[i] = (char)(((rx_setup->preamble_value) >> (i * 8)) & 0x000000FF);
        }
    }
    else
    {
        return RF_PREAMBLE_CHIP_CNT_INVALID;
    }
    if (prev_preamble_chip_cnt != preamble_chip_cnt)
    {
    	prev_preamble_chip_cnt = preamble_chip_cnt;
    	update_preamble = 1;
    }
    if (prev_preamble_value != rx_setup->preamble_value)
    {
    	prev_preamble_value = rx_setup->preamble_value;
        update_preamble = 1;
    }

    if (rx_setup->rf_modulation == RF_MODULATION_ASK)
    {
        // check if DEMOD_ASK bit is not set
        if ((sfr_buffer_bank0[OL2381_RXBW_REG] & 0x80) == 0)
        {
    	    // set DEMOD_ASK bit in RXBW register (set ASK mode)
    	    sfr_buffer_bank0[OL2381_RXBW_REG] |= 0x80;
            // setup other SFR registers for ASK mode
    	    sfr_buffer_bank0[OL2381_UMODAMPTH_REG] = OL2381_ASK_UMODAMPTH_VAL;
    	    sfr_buffer_bank0[OL2381_LMODAMPTH_REG] = OL2381_ASK_LMODAMPTH_VAL;
    	    sfr_buffer_bank0[OL2381_EMODAMPTH_REG] = OL2381_ASK_EMODAMPTH_VAL;
            update_rxbw = 1;
        }
    }
    else
    {
        // check if DEMOD_ASK bit is set
        if ((sfr_buffer_bank0[OL2381_RXBW_REG] & 0x80) == 0x80)
        {
    	    // clear DEMOD_ASK bit in RXBW register (set FSK mode)
    	    sfr_buffer_bank0[OL2381_RXBW_REG] &= ~0x80;
            // setup other SFR registers for FSK mode
    	    sfr_buffer_bank0[OL2381_UMODAMPTH_REG] = OL2381_FSK_UMODAMPTH_VAL;
    	    sfr_buffer_bank0[OL2381_LMODAMPTH_REG] = OL2381_FSK_LMODAMPTH_VAL;
    	    sfr_buffer_bank0[OL2381_EMODAMPTH_REG] = OL2381_FSK_EMODAMPTH_VAL;
            update_rxbw = 1;
        }
    }
    if (update_rxbw)
    {
        RFchip_wr_one_reg(OL2381_RXBW_REG, sfr_buffer_bank0[OL2381_RXBW_REG]);
        RFchip_wr_regs((char*)&sfr_buffer_bank0[OL2381_UMODAMPTH_REG], 3, OL2381_UMODAMPTH_REG);
    }

    sfr_buffer_bank0[OL2381_PREACON_REG] &= 0xE0;
    if (RfEncoding == RF_ENCODING_MANCHESTER_HW)
    {
        short manchester_val = 0;
        
        // check if RX_DATA_TRANSP bit is set
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x01) == 0x01)
        {
            // clear the RX_DATA_TRANSP bit in RXCON register
    	    sfr_buffer_bank0[OL2381_RXCON_REG] &= ~0x01;
            update_rxcon = 1;
        }
        
        // check if RX_MANCHESTER bit is not set
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x04) == 0)
        {
            // set the RX_MANCHESTER bit in RXCON register (set MANCHESTER decoding)
    	    sfr_buffer_bank0[OL2381_RXCON_REG] |= 0x04;
            update_rxcon = 1;
        	update_preamble = 1;
        }
    	
        // For HW Manchester coding X preamble bits result in 2*X chips
        sfr_buffer_bank0[OL2381_PREACON_REG] |= ((preamble_chip_cnt * 2) & 0x1F);
        manchester_val = conv_byte_to_manchester(~preamble_bytes[0]);
    	sfr_buffer_bank0[OL2381_PREA0_REG] = (manchester_val >> 0) & 0xFF;
        sfr_buffer_bank0[OL2381_PREA1_REG] = (manchester_val >> 8) & 0xFF;
        manchester_val = conv_byte_to_manchester(~preamble_bytes[1]);
        sfr_buffer_bank0[OL2381_PREA2_REG] = (manchester_val >> 0) & 0xFF;
        sfr_buffer_bank0[OL2381_PREA3_REG] = (manchester_val >> 8) & 0xFF;
    }
    else
    {
        // check if RX_DATA_TRANSP bit is not set
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x01) == 0)
        {
            // set the RX_DATA_TRANSP bit in RXCON register
    	    sfr_buffer_bank0[OL2381_RXCON_REG] |= 0x01;
            update_rxcon = 1;
        }
        
        // check if RX_MANCHESTER bit is set
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x04) == 0x04)
        {
            // clear the RX_MANCHESTER bit in RXCON register (set NRZ decoding)
    	    sfr_buffer_bank0[OL2381_RXCON_REG] &= ~0x04;
            update_rxcon = 1;
        	update_preamble = 1;
        }
    	
        sfr_buffer_bank0[OL2381_PREACON_REG] |= (preamble_chip_cnt & 0x1F);
        sfr_buffer_bank0[OL2381_PREA0_REG] = preamble_bytes[0];
        sfr_buffer_bank0[OL2381_PREA1_REG] = preamble_bytes[1];
        sfr_buffer_bank0[OL2381_PREA2_REG] = preamble_bytes[2];
        sfr_buffer_bank0[OL2381_PREA3_REG] = preamble_bytes[3];
    }
    if (rx_setup->rf_low_freq_is_zero == RF_TRUE)
    {
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x20) == 0)
        {
   	        sfr_buffer_bank0[OL2381_RXCON_REG] |=  0x20;
            update_rxcon = 1;
        }
    }
    else
    {
        if ((sfr_buffer_bank0[OL2381_RXCON_REG] & 0x20) == 0x20)
        {
   	        sfr_buffer_bank0[OL2381_RXCON_REG] &= ~0x20;
            update_rxcon = 1;
        }
    }
    if (update_rxcon)
    {
        RFchip_wr_one_reg(OL2381_RXCON_REG, sfr_buffer_bank0[OL2381_RXCON_REG]);
    }
    if (update_preamble)
    {
        RFchip_wr_regs((char*)&sfr_buffer_bank0[OL2381_PREACON_REG], 5, OL2381_PREACON_REG);
    }
    RFchip_set_chiprate(rx_setup->rf_chiprate);
    if (rx_setup->rf_chiprate == RF_CHIPRATE_100000)
    {
        cfbw = 0;    // set receive channel bandwidth to 300kHz
        bbfc = 1;    // set baseband digital filter cut-off freq. to 57kHz
    }
    else
    {
        cfbw = 1;    // set receive channel bandwidth to 200kHz
        bbfc = 0;    // set baseband digital filter cut-off freq. to 115kHz
    }
    if ((sfr_buffer_bank0[OL2381_RXBW_REG] & 0x70) != (cfbw << 4))
    {
        sfr_buffer_bank0[OL2381_RXBW_REG] &= 0x8F;
        sfr_buffer_bank0[OL2381_RXBW_REG] |= (cfbw << 4);
        RFchip_wr_one_reg(OL2381_RXBW_REG, sfr_buffer_bank0[OL2381_RXBW_REG]);
    }
    if ((sfr_buffer_bank0[OL2381_RXBBCON_REG] & 0x0F) != bbfc)
    {
        sfr_buffer_bank0[OL2381_RXBBCON_REG] &= 0xF0;
        sfr_buffer_bank0[OL2381_RXBBCON_REG] |= bbfc;
        RFchip_wr_one_reg(OL2381_RXBBCON_REG, sfr_buffer_bank0[OL2381_RXBBCON_REG]);
    }
    return err_stat;
}

/*----------------------------------------------------------------------------*/
/* Exported functions                                                         */
/*----------------------------------------------------------------------------*/

/*----------------------------------------------------------------------------*/
/* Name       : rf_init                                                       */
/* Description: Enables and configures UHF transceiver                        */
/* Parameters :                                                               */
/*   cold_init - when true, also the OL2381 will be inited                    */
/* Returns    : RF_SUCCESS or RF_COMMUNICATION_ERROR                          */
/*----------------------------------------------------------------------------*/

int rf_init(int cold_init)
{
    int err_stat = RF_SUCCESS;

    init_global_variables();
    ssp_init();

    // disable (falling edge) IRQ from OL2381 pin P11/INT
    gpio_intdisable( OL2381_P11_INT_PORTPIN );
	// Setup GPIO func to connect to P11/INT pin of OL2381
    gpio_alloc( OL2381_P11_INT_PORTPIN, PCB_MODE_PULLNONE, GPIO_INPUT );

    // Initialize the interrupt vector for the EINTx pio pins
    RF_EINT_X_INT_INSTALL();
    RF_EINT_X_INT_ENABLE();

    if (cold_init == RF_INIT_COLD)
    {
        err_stat = RFchip_init();
	}
	// clear any possibly pending interrupt from P11/INT
	gpio_intclear( OL2381_P11_INT_PORTPIN );
    // enable (falling edge) IRQ from OL2381 pin P11/INT
    gpio_intenable( OL2381_P11_INT_PORTPIN, GPIO_FALLING );

    if (err_stat == RF_SUCCESS) RfDriverInited = RF_TRUE;
    return err_stat;
}

/*----------------------------------------------------------------------------*/
/* Name       : rf_write                                                      */
/* Description: Sends the A - F configuration bits to the OL2381,             */
/*              switches to transmit mode and transmits the app data          */
/* Parameters :                                                               */
/*    buf      - Pointer to the data bytes to transmit                        */
/*    size     - Total number of data bytes to transmit                       */
/*    tx_setup - Setup structure for transmitter                              */
/* Returns    : RF_SUCCESS, RF_CALLING_ERROR, or other error                  */
/*----------------------------------------------------------------------------*/

int rf_write(char * buf, int size, rf_tx_setup_t * tx_setup)
{
    //                RF frequency    (A)sync   PA       ME/NRZ  ACON0/1
    //               A-bit    B-bit    C-bit    D-bit    E-bit    F-bit
    char options  = (0<<5) | (0<<4) | (1<<3) | (1<<2) | (0<<1) | (0<<0); // use FC0
    int  signal_sem = 0;
    char spiFrameSize;
    int  err_stat = RF_SUCCESS;

    if ((RfDriverInited != RF_TRUE)||
        (size <= 0)                ||
        (size > RF_MAX_RXTX_MESSAGE_LENGTH)) return RF_CALLING_ERROR;

    rtk_int_disable();
    RfTransmittersWaiting++;
    if (RfStatus == RF_RX_BUSY)
    {
        // Force the receiver to stop only when nothing received yet
        if (RfRxWaitSignal && (RfRxByteCount == 0))
        {
        	RfRxWaitSignal = 0;
            RfStatus = RF_RX_STOP_REQ;
            signal_sem = 1;
        }
    }
    rtk_int_enable();
    if (signal_sem)
    {
        rtk_sem_release( sem_rf_rx_tx_done );
    	signal_sem = 0;
    }

	rtk_sem_acquire( sem_exclusive_use ); // wait till transceiver ready

    err_stat = RFchip_tx_setup(tx_setup, &spiFrameSize);
    if (err_stat == RF_SUCCESS)
    {
        if (tx_setup->rf_modulation == RF_MODULATION_FSK)
        {
            options |= 0x01;  // Use ACON1 for FSK
        }
        if (tx_setup->rf_encoding == RF_ENCODING_MANCHESTER_HW)
        {
            options |= 0x02;  // Manchester encoding will be done by RF chip
        }
        if (tx_setup->rf_chiprate == RF_CHIPRATE_100000)
        {
            options |= 0x10;  // use FC1
        }
        else if (tx_setup->rf_chiprate == RF_CHIPRATE_4800)
        {
            options |= 0x20;  // use FC2
        }
        // Put this driver in the RF transmit mode
        RfStatus = RF_TX_SETUP;
        RFchip_tx_rfdata(buf, size, options, spiFrameSize);
        RfStatus = RF_IDLE;
        RFchip_hybernate();
    }

    rtk_sem_release( sem_exclusive_use );

    rtk_int_disable();
    RfTransmittersWaiting--;
    if ((RfTransmittersWaiting == 0) && (RfReceiverWaiting))
    {
    	RfReceiverWaiting = 0;
    	signal_sem = 1;
    }
    rtk_int_enable();
    if (signal_sem)
    {
        rtk_sem_release( sem_start_recv );
        signal_sem = 0;
    }
    return err_stat;
}

/*----------------------------------------------------------------------------*/
/* Name       : rf_start_read                                                 */
/* Description: Start the receiver                                            */
/* Parameters :                                                               */
/*    rx_setup - Setup structure for receiver                                 */
/* Returns    : RF_SUCCESS or RF_CALLING_ERROR                                */
/*----------------------------------------------------------------------------*/

int rf_start_read(rf_rx_setup_t * rx_setup)
{
    //              RF frequency      RX sub command    gain selection
    //              A-bit    B-bit    C-bit    D-bit    E-bit    F-bit
    char options = (0<<5) | (0<<4) | (1<<3) | (0<<2) | (1<<1) | (0<<0); // use FC0, PRDA sub command
    char spiFrameSize;
    int  err_stat = RF_SUCCESS;

    if ((RfDriverInited != RF_TRUE) ||
        (RfStatus == RF_RX_BUSY)    ||
        (RfStatus == RF_RX_STOP_REQ)) return RF_CALLING_ERROR;

    rtk_int_disable();
    if (RfTransmittersWaiting > 0)
    {
        // Give all waiting transmitter(s) the chance to send,
        // since normally the system is continously in receive mode.
    	RfReceiverWaiting = 1;
        rtk_int_enable();
    	rtk_sem_acquire( sem_start_recv );
    }
    else
    {
        rtk_int_enable();
    }

	rtk_sem_acquire( sem_exclusive_use ); // wait till transceiver ready

    // Put this driver in the RF receive mode
    RfStatus = RF_RX_SETUP;
    err_stat = RFchip_rx_setup(rx_setup, &spiFrameSize);
    if (err_stat == RF_SUCCESS)
    {
        rf_buf_read_index = 0;
        if (rx_setup->rf_rx_gain == RF_RX_GAIN_HIGH)
        {
            options |= 0x01;  // use high gain
        }
        if (rx_setup->rf_chiprate == RF_CHIPRATE_100000)
        {
            options |= 0x10;  // use FC1
        }
        else if (rx_setup->rf_chiprate == RF_CHIPRATE_4800)
        {
            options |= 0x20;  // use FC2
        }
        // make the parameters available for the IRQ handler that takes care for the RF RX
	    RfRxByteCount = 0;
        RFchip_rx_rfdata(options, spiFrameSize);
    }
    else
    {
        RfStatus = RF_IDLE;      // Take this driver out of RF receive mode
        rtk_sem_release( sem_exclusive_use );
    }
    return err_stat;
}

/*----------------------------------------------------------------------------*/
/* Name       : rf_read                                                       */
/* Description: read received data from the RF receive buffer                 */
/* Parameters :                                                               */
/*    buf      - Buffer pointer to read data into                             */
/*    size     - Total number of data bytes to read                           */
/*    timeout  - Time that the caller wants to wait for data (in msec)        */
/* Returns    : RF_CALLING_ERROR, RF_RX_ABORT, RF_RX_TIMEOUT or               */
/*              number of read bytes                                          */
/*----------------------------------------------------------------------------*/

int rf_read(char * buf, int size, unsigned int timeout)
{
    int err_stat = RF_SUCCESS;

    if ((RfDriverInited != RF_TRUE) ||
        (RfStatus != RF_RX_BUSY)    ||
        (size <= 0)                 ||
        ((rf_buf_read_index + size) > RF_MAX_RXTX_MESSAGE_LENGTH))
    {
        return RF_CALLING_ERROR;
    }

    if ((RfTransmittersWaiting > 0) && (RfRxByteCount == 0))
    {
        RfStatus = RF_RX_STOP_REQ;
        err_stat = RF_RX_ABORT;
    }
    else
    {
        if ((rf_buf_read_index + size) <= RfRxByteCount)
        {
            // The data that has to be read is already in the receive buffer.
            memcpy(buf, (void const *)&rf_receive_buf[rf_buf_read_index], size);
            rf_buf_read_index += size;
            err_stat = size;
        }
        else
        {
            RfRxSignalByteCount = rf_buf_read_index + size;
            RfRxWaitSignal = 1;
            if (timeout == 0)
                rtk_sem_acquire( sem_rf_rx_tx_done );
            else
                rtk_sem_acquire_timeout( sem_rf_rx_tx_done, timeout );
            RfRxWaitSignal = 0;

            if (RfStatus == RF_RX_STOP_REQ)
            {
                err_stat = RF_RX_ABORT;
            }
            else
            {
                err_stat = RfRxByteCount - rf_buf_read_index;
                if (err_stat == 0)
                {
                    err_stat = RF_RX_TIMEOUT;
                }
                else
                {
                    if (err_stat > size)
                    {
                        err_stat = size;
                    }
                    memcpy(buf, (void const *)&rf_receive_buf[rf_buf_read_index], err_stat);
                    rf_buf_read_index += err_stat;
                }
            }
        }
    }
    return err_stat;
}

/*----------------------------------------------------------------------------*/
/* Name       : rf_stop_read                                                  */
/* Description: Stop the read                                                 */
/* Parameters :                                                               */
/*   rssi_level - pointer to callers variable in which rssi level is returned */
/* Returns    : RF_SUCCESS or RF_CALLING_ERROR                                */
/*----------------------------------------------------------------------------*/

int rf_stop_read(int * rssi_level)
{
    int err_stat = RF_SUCCESS;

    if (RfDriverInited != RF_TRUE) return RF_CALLING_ERROR;

    if ((RfStatus == RF_RX_BUSY) || (RfStatus == RF_RX_STOP_REQ))
    {
        ssp_disable_rxim_irq();
        DISABLE_RF_CHIP();                              // signal end of communication
        ssp_disable();
        ssp_set_framesize(8);
        ssp_conf_master();
        ssp_clr_cpol();
        ssp_enable_slave_output();
        ssp_enable();
        if (rssi_level)
        {
            char cTmp;
            RFchip_rd_regs(&cTmp, 1, OL2381_RSSILEVEL_REG);
            *rssi_level = (int)((unsigned char) cTmp);
        }
        RFchip_hybernate();
        RfStatus = RF_IDLE;      // Take this driver out of RF receive mode
        rtk_sem_release( sem_exclusive_use );
    }
    return err_stat;
}

#endif // end RF_TRANSCEIVE_ENABLE

// eof
