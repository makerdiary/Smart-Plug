/*******************************************************************************
*   i2c.c:  I2C C file for NXP LPC11xx/LPC13xx/LPC17xx/EM7xx Family MCUs
*
*   Copyright(C) 2011, NXP Semiconductor
*   All rights reserved.
*
*******************************************************************************/

/*******************************************************************************
*                        INCLUDE FILES
*******************************************************************************/

#include "string.h"
#include "bsp.h"
#include "rtk.h"
#include "i2c.h"

#ifdef SUPPORT_I2C_BUS

/*******************************************************************************
*                        LOCAL DEFINES
*******************************************************************************/

/* Bit definitions for I2CONSET Register */
#define I2CONSET_AA     0x00000004
#define I2CONSET_SI     0x00000008
#define I2CONSET_STO    0x00000010
#define I2CONSET_STA    0x00000020
#define I2CONSET_I2EN   0x00000040

/* Bit definitions for I2CONCLR Register */
#define I2CONCLR_AAC    0x00000004
#define I2CONCLR_SIC    0x00000008
#define I2CONCLR_STAC   0x00000020
#define I2CONCLR_I2ENC  0x00000040

/*
* I2C bitfreq = PCLK_I2C / ( I2SCLL_SCLL + I2SCLH_SCLH )
*/
#define I2SCLL_SCLL     0x0030  /* I2C SCL Duty Cycle Low Reg */
#define I2SCLH_SCLH     0x0030  /* I2C SCL Duty Cycle High Reg */

/********************/
/* I2C STATUS CODES */
/********************/

/* ---- MASTER MODE STATES ---- */
#define I2STAT_M_START                    0x08
#define I2STAT_M_REPEATED_START           0x10
#define I2STAT_M_ARB_LOST                 0x38

/* ---- MASTER TRANSMITTER STATES ---- */
#define I2STAT_M_TX_SLA_W_ACK             0x18
#define I2STAT_M_TX_SLA_W_NOACK           0x20
#define I2STAT_M_TX_DAT_ACK               0x28
#define I2STAT_M_TX_DAT_NOACK             0x30

/* ---- MASTER RECEIVE STATES ---- */
#define I2STAT_M_RX_SLA_R_ACK             0x40
#define I2STAT_M_RX_SLA_R_NOACK           0x48
#define I2STAT_M_RX_DAT_ACK               0x50
#define I2STAT_M_RX_DAT_NOACK             0x58

/* ---- SLAVE RECEIVE STATES ---- */
#define I2STAT_S_RX_SLA_W_ACK             0x60
#define I2STAT_S_RX_SLA_W_ACK_ARB_LOST    0x68
#define I2STAT_S_RX_GENCALL_ACK           0x70
#define I2STAT_S_RX_GENCALL_ACK_ARB_LOST  0x78
#define I2STAT_S_RX_DAT_ACK               0x80
#define I2STAT_S_RX_SLA_W_NOACK           0x88
#define I2STAT_S_RX_GENCALL_DAT_ACK       0x90
#define I2STAT_S_RX_GENCALL_DAT_NOACK     0x98
#define I2STAT_S_RX_STOP                  0xA0

/* ---- SLAVE TRANSMITTER STATES ---- */
#define I2STAT_S_TX_SLA_R_ACK             0xA8
#define I2STAT_S_TX_SLA_R_ACK_ARB_LOST    0xB0
#define I2STAT_S_TX_DAT_ACK               0xB8
#define I2STAT_S_TX_DAT_NOACK             0xC0
#define I2STAT_S_TX_DAT_ACK_FINISH        0xC8

/* ---- MISCELLANEOUS STATES ---- */
#define I2STAT_BUS_ERROR                  0x00
#define I2STAT_NO_INFO                    0xF8

// I2C SW driver slave states
#define I2C_SLAVE_IDLE            0
#define I2C_SLAVE_RECEIVE_BUSY    1
#define I2C_SLAVE_TRANSMIT_BUSY   2

#define REGNUM_NOT_USED     0
#define TRANSMIT_REGNUM     1
#define REGNUM_TRANSMITTED  2
#define RECEIVE_REGNUM      3
#define REGNUM_RECEIVED     4

/*******************************************************************************
*                       GLOBAL VARIABLES
*******************************************************************************/


static uint8_t       I2C_Mode = 0;
static uint8_t       I2C_UseRegNum;
static uint8_t       I2C_RegNum;
static uint8_t       I2C_TRxCount;
#ifdef SUPPORT_I2C_MASTER_MODE
static uint8_t       I2C_DeviceAddress;
static uint8_t       I2C_BufCount;
static uint8_t     * I2C_BufPtr;
static i2c_error_t   I2C_Error;
#endif
static rtk_sem_t     I2C_SemWait;
static rtk_sem_t     I2C_SemBusy;

#ifdef SUPPORT_I2C_SLAVE_MODE
static volatile uint32_t I2C_SlaveState = I2C_SLAVE_IDLE;

static uint8_t (*pcb_SlaveRecv)(uint8_t reg, uint8_t *pbuf, uint8_t count);
static uint8_t (*pcb_SlaveXmit)(uint8_t reg);
#endif  // SUPPORT_I2C_SLAVE_MODE


/*******************************************************************************
* Name        : I2C_Init()
* Description : Initialize I2C controller
* Argument(s) : mode      - either I2CMASTER or I2CSLAVE
*               address   - only when mode is I2CSLAVE, specifies our own I2C slave address
*               pFuncRecv - only when mode is I2CSLAVE, callback func store I2C Recv data
*               pFuncXmit - only when mode is I2CSLAVE, callback func get I2C Xmit data
* Return(s)   : I2C_OK when init succes
*******************************************************************************/

i2c_error_t I2C_Init(uint32_t mode, uint8_t address, void *pFuncRecv, void *pFuncXmit)
{
    SYSCON->PRESETCTRL    |= (1 << 1);
    SYSCON->SYSAHBCLKCTRL |= (1 << 5);             /* Enable I2C clock */

    if ((mode != I2CMASTER) && (mode != I2CSLAVE)) return I2C_ERROR_INIT_FAILED;
    I2C_Mode = mode;
    I2C_UseRegNum = REGNUM_NOT_USED;
    NVIC_SetPriority (I2C_IRQn, I2CINT_PRIORITY);
    I2C_SemWait = rtk_sem_create( 0, "I2Cwait" );
    I2C_SemBusy = rtk_sem_create( 1, "I2Cbusy" );
    NVIC_DisableIRQ(I2C_IRQn);
    /*--- Clear all I2CON flags ---*/
    I2C->I2CONCLR = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
#ifdef SUPPORT_I2C_MASTER_MODE
    I2C_Error = I2C_OK;
#endif
    /*--- Program the SCL registers to define the I2C clock ---*/
    I2C->I2SCLL = I2SCLL_SCLL;
    I2C->I2SCLH = I2SCLH_SCLH;
    I2C->I2ADR0  = 0;
    I2C->I2ADR1  = 0;
    I2C->I2ADR2  = 0;
    I2C->I2ADR3  = 0;
    I2C->I2MASK0 = 0;
    I2C->I2MASK1 = 0;
    I2C->I2MASK2 = 0;
    I2C->I2MASK3 = 0;
    I2C->MMCTRL  = 0;
#ifdef SUPPORT_I2C_SLAVE_MODE
    if (mode == I2CSLAVE) {
        I2C_SlaveState = I2C_SLAVE_IDLE;
        /*--- Program our own I2C slave address ---*/
        I2C->I2ADR0    = (address & 0xFE);  /* Gencall bit is disabled! */
        pcb_SlaveRecv  = (uint8_t(*)(uint8_t,uint8_t*,uint8_t))pFuncRecv;
        pcb_SlaveXmit  = (uint8_t(*)(uint8_t))pFuncXmit;
        NVIC_EnableIRQ(I2C_IRQn);
        /*--- Enable the I2C device as slave ---*/
        I2C->I2CONSET  = I2CONSET_I2EN | I2CONSET_AA;
    }
#endif
    return I2C_OK;
}

/*******************************************************************************
* Name        : I2C_Read()
* Description : Read 'count' bytes into 'buf' from I2C slave identified by 'address'
* Argument(s) : address - I2C slave address
*               buf     - buffer into which the bytes will be stored
*               count   - number of bytes to read
* Return(s)   : I2C_Error
*******************************************************************************/

#ifdef SUPPORT_I2C_MASTER_MODE
i2c_error_t I2C_Read(uint8_t address, uint8_t *buf, uint8_t count)
{
    if (I2C_Mode != I2CMASTER) return I2C_ERROR_WRONG_MODE;
    rtk_sem_acquire( I2C_SemBusy );        /* Wait until I2C is free */

    /*--- Clear all I2CON flags ---*/
    I2C->I2CONCLR     = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
    I2C_Error = I2C_OK;
    /*--- Enable the I2C device as master ---*/
    I2C->I2CONSET     = I2CONSET_I2EN;
    I2C_DeviceAddress = (address | 0x01);      /* Set R/W bit to indicate READ */
    I2C_UseRegNum     = REGNUM_NOT_USED;
    I2C_BufPtr        = buf;
    I2C_BufCount      = count;
    /*--- Initiate a start ---*/
    I2C->I2CONSET     = I2CONSET_STA;
    NVIC_EnableIRQ(I2C_IRQn);
    rtk_sem_acquire( I2C_SemWait );        /* Wait until complete */

    rtk_sem_release( I2C_SemBusy );        /* Free I2C */
    return I2C_Error;
}
#endif

/*******************************************************************************
* Name        : I2C_ReadRegs()
* Description : Read 'count' bytes into 'buf' from I2C slave identified by 'address' starting from 'register'
* Argument(s) : address - I2C slave address
*               reg     - register to start read from
*               buf     - buffer into which the bytes will be stored
*               count   - number of bytes to read
* Return(s)   : I2C_Error
*******************************************************************************/

#ifdef SUPPORT_I2C_MASTER_MODE
i2c_error_t I2C_ReadRegs(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t count)
{
    if (I2C_Mode != I2CMASTER) return I2C_ERROR_WRONG_MODE;
    rtk_sem_acquire( I2C_SemBusy );        /* Wait until I2C is free */

    /*--- Clear all I2CON flags ---*/
    I2C->I2CONCLR     = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
    I2C_Error = I2C_OK;
    /*--- Enable the I2C device as master ---*/
    I2C->I2CONSET     = I2CONSET_I2EN;
    I2C_DeviceAddress = (address & 0xFE);      /* Clear R/W bit to indicate WRITE */
    I2C_RegNum        = reg;
    I2C_UseRegNum     = TRANSMIT_REGNUM;
    I2C_BufPtr        = NULL;
    I2C_BufCount      = 0;
    /*--- Initiate a start ---*/
    I2C->I2CONSET     = I2CONSET_STA;
    NVIC_EnableIRQ(I2C_IRQn);
    rtk_sem_acquire( I2C_SemWait );        /* Wait until complete */

    if (I2C_Error == I2C_OK) {
        /*--- Clear all I2CON flags ---*/
        I2C->I2CONCLR     = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
        /*--- Enable the I2C device as master ---*/
        I2C->I2CONSET     = I2CONSET_I2EN;
        I2C_DeviceAddress = (address | 0x01);      /* Set R/W bit to indicate READ */
        I2C_UseRegNum     = REGNUM_NOT_USED;
        I2C_BufPtr        = buf;
        I2C_BufCount      = count;
        /*--- Initiate a start ---*/
        I2C->I2CONSET     = I2CONSET_STA;
        NVIC_EnableIRQ(I2C_IRQn);
        rtk_sem_acquire( I2C_SemWait );        /* Wait until complete */
    }

    rtk_sem_release( I2C_SemBusy );        /* Free I2C */
    return I2C_Error;
}
#endif

/*******************************************************************************
* Name        : I2C_Write()
* Description : Write 'count' bytes from 'buf' to I2C slave identified by 'address'
* Argument(s) : address - I2C slave address
*               buf     - buffer which holds the data to be written
*               count   - number of bytes to write
* Return(s)   : I2C_Error
*******************************************************************************/

#ifdef SUPPORT_I2C_MASTER_MODE
i2c_error_t I2C_Write(uint8_t address, uint8_t *buf, uint8_t count)
{
    if (I2C_Mode != I2CMASTER) return I2C_ERROR_WRONG_MODE;
    rtk_sem_acquire( I2C_SemBusy );        /* Wait until I2C is free */

    /*--- Clear all I2CON flags ---*/
    I2C->I2CONCLR     = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
    I2C_Error = I2C_OK;
    /*--- Enable the I2C device as master ---*/
    I2C->I2CONSET     = I2CONSET_I2EN;
    I2C_DeviceAddress = (address & 0xFE);  /* Clear R/W bit to indicate WRITE */
    I2C_UseRegNum     = REGNUM_NOT_USED;
    I2C_BufPtr        = buf;
    I2C_BufCount      = count;
    /*--- Initiate a start ---*/
    I2C->I2CONSET     = I2CONSET_STA;
    NVIC_EnableIRQ(I2C_IRQn);
    rtk_sem_acquire( I2C_SemWait );    /* Wait until complete */

    rtk_sem_release( I2C_SemBusy );        /* Free I2C */
    return I2C_Error;
}
#endif

/*******************************************************************************
* Name        : I2C_WriteRegs()
* Description : Write value 'val' to register 'reg' of I2C slave identified by 'address'
* Argument(s) : address - I2C slave address
*               reg     - register to be written to
*               buf     - buffer which holds the data to be written
*               count   - number of bytes to write
* Return(s)   : I2C_OK or I2C_Error
*******************************************************************************/

i2c_error_t I2C_WriteRegs(uint8_t address, uint8_t reg, uint8_t *buf, uint8_t count)
{
    i2c_error_t RetVal;

    rtk_sem_acquire( I2C_SemBusy );        /* Wait until I2C is free */

#ifdef SUPPORT_I2C_MASTER_MODE
    if (I2C_Mode == I2CMASTER) {
        /*--- Clear all I2CON flags ---*/
        I2C->I2CONCLR     = I2CONCLR_I2ENC | I2CONCLR_STAC | I2CONCLR_SIC | I2CONCLR_AAC;
        I2C_Error = I2C_OK;
        /*--- Enable the I2C device as master ---*/
        I2C->I2CONSET     = I2CONSET_I2EN;
        I2C_DeviceAddress = (address & 0xFE);  /* Clear R/W bit to indicate WRITE */
        I2C_RegNum        = reg;
        I2C_UseRegNum     = TRANSMIT_REGNUM;
        I2C_BufPtr        = buf;
        I2C_BufCount      = count;
        /*--- Initiate a start ---*/
        I2C->I2CONSET     = I2CONSET_STA;
        NVIC_EnableIRQ(I2C_IRQn);
        rtk_sem_acquire( I2C_SemWait );    /* Wait until complete */
        RetVal = I2C_Error;
    }
#endif
#ifdef SUPPORT_I2C_SLAVE_MODE
    if (I2C_Mode == I2CSLAVE) {
        int data_copied = 0;
        while (!data_copied) {
            NVIC_DisableIRQ(I2C_IRQn);
            if (I2C_SlaveState == I2C_SLAVE_IDLE) {
                pcb_SlaveRecv(reg, buf, count);
                NVIC_EnableIRQ(I2C_IRQn);
                data_copied = 1;
                RetVal = I2C_OK;
            } else {
                NVIC_EnableIRQ(I2C_IRQn);
                rtk_sem_acquire( I2C_SemWait );
            }
        }
    }
#endif

    rtk_sem_release( I2C_SemBusy );        /* Free I2C */
    return RetVal;
}

/*******************************************************************************
* Name        : I2C_IRQHandler()
* Description : I2C interrupt handler, deals with master and slave mode.
* Argument(s) : none
* Return(s)   : none
*******************************************************************************/

void I2C_IRQHandler(void)
{
    uint8_t StatValue = I2C->I2STAT;
#ifdef SUPPORT_I2C_SLAVE_MODE
    uint8_t XmitData;
#endif

    switch (StatValue & 0xF8)
    {
#ifdef SUPPORT_I2C_MASTER_MODE
    case I2STAT_M_START:
    case I2STAT_M_REPEATED_START:
        I2C_TRxCount = 0;
        if (I2C_UseRegNum == REGNUM_TRANSMITTED) {
            I2C_UseRegNum = TRANSMIT_REGNUM;  /* reset flag again */
        }
        I2C->I2DAT    = I2C_DeviceAddress;
        I2C->I2CONSET = I2CONSET_AA;
        I2C->I2CONCLR = I2CONCLR_STAC | I2CONCLR_SIC;
        break;

    case I2STAT_M_ARB_LOST:
        I2C_Error = I2C_ERROR_ARB_LOST;
        I2C->I2CONCLR = I2CONCLR_SIC; /* no repeated start implemented! */
        NVIC_DisableIRQ(I2C_IRQn);
        rtk_sem_release_isr( I2C_SemWait );
        break;

    /* Master transmitter states */
    case I2STAT_M_TX_SLA_W_ACK:
    case I2STAT_M_TX_DAT_ACK:
        if (I2C_UseRegNum == TRANSMIT_REGNUM) {
            I2C_UseRegNum = REGNUM_TRANSMITTED;
            I2C->I2DAT    = I2C_RegNum;
            I2C->I2CONSET = I2CONSET_AA;
            I2C->I2CONCLR = I2CONCLR_SIC;
        } else if (I2C_TRxCount < I2C_BufCount) {
            I2C->I2DAT    = *(I2C_BufPtr + I2C_TRxCount);
            I2C_TRxCount++;
            I2C->I2CONSET = I2CONSET_AA;
            I2C->I2CONCLR = I2CONCLR_SIC;
        } else {
            I2C->I2CONSET = I2CONSET_STO | I2CONSET_AA;
            I2C->I2CONCLR = I2CONCLR_SIC;
            NVIC_DisableIRQ(I2C_IRQn);
            rtk_sem_release_isr( I2C_SemWait );
        }
        break;

    case I2STAT_M_TX_SLA_W_NOACK:
    case I2STAT_M_TX_DAT_NOACK:
    case I2STAT_M_RX_SLA_R_NOACK:
        I2C_Error     = I2C_ERROR_NOACK;
        I2C->I2CONSET = I2CONSET_STO | I2CONSET_AA;
        I2C->I2CONCLR = I2CONCLR_SIC;
        NVIC_DisableIRQ(I2C_IRQn);
        rtk_sem_release_isr( I2C_SemWait );
        break;

    /* Master receive states */
    case I2STAT_M_RX_SLA_R_ACK:
        if ((I2C_TRxCount + 1) <= I2C_BufCount) {
            I2C->I2CONSET = I2CONSET_AA;
            I2C->I2CONCLR = I2CONCLR_SIC;
        } else {
            I2C->I2CONCLR = I2CONCLR_SIC | I2CONCLR_AAC;
        }
        break;

    case I2STAT_M_RX_DAT_ACK:
        *(I2C_BufPtr + I2C_TRxCount) = I2C->I2DAT;
        I2C_TRxCount++;
        if ((I2C_TRxCount + 1) <= I2C_BufCount) {
            I2C->I2CONSET = I2CONSET_AA;
            I2C->I2CONCLR = I2CONCLR_SIC;
        } else {
            I2C->I2CONCLR = I2CONCLR_SIC | I2CONCLR_AAC;
        }
        break;

    case I2STAT_M_RX_DAT_NOACK:
        *(I2C_BufPtr + I2C_TRxCount) = I2C->I2DAT;
        I2C_TRxCount++;
        I2C->I2CONSET = I2CONSET_STO | I2CONSET_AA;
        I2C->I2CONCLR = I2CONCLR_SIC;
        NVIC_DisableIRQ(I2C_IRQn);
        rtk_sem_release_isr( I2C_SemWait );
        break;
#endif
        
#ifdef SUPPORT_I2C_SLAVE_MODE
    /* Slave receive states */
    case I2STAT_S_RX_SLA_W_ACK:
    case I2STAT_S_RX_SLA_W_ACK_ARB_LOST:   /* no repeated start implemented! */
    case I2STAT_S_RX_GENCALL_ACK:
    case I2STAT_S_RX_GENCALL_ACK_ARB_LOST: /* no repeated start implemented! */
        I2C_TRxCount = 0;
        I2C_UseRegNum  = RECEIVE_REGNUM;
	    I2C_SlaveState = I2C_SLAVE_RECEIVE_BUSY;
	    I2C->I2CONSET  = I2CONSET_AA;
	    I2C->I2CONCLR  = I2CONCLR_SIC;
        break;
	
    case I2STAT_S_RX_DAT_ACK:
    case I2STAT_S_RX_GENCALL_DAT_ACK:
        if (I2C_UseRegNum == RECEIVE_REGNUM) {
            I2C_UseRegNum = REGNUM_RECEIVED;
            I2C_RegNum    = I2C->I2DAT;
        } else {
            uint8_t RecvData = I2C->I2DAT;
	        if (pcb_SlaveRecv) pcb_SlaveRecv(I2C_RegNum + I2C_TRxCount, &RecvData, 1);
            I2C_TRxCount++;
        }
        I2C->I2CONSET = I2CONSET_AA;
	    I2C->I2CONCLR = I2CONCLR_SIC;
    	break;
		
    case I2STAT_S_RX_SLA_W_NOACK:
    case I2STAT_S_RX_GENCALL_DAT_NOACK:
    case I2STAT_S_RX_STOP: /* Stop condition or repeated start has been received */
	    I2C->I2CONSET  = I2CONSET_AA;
	    I2C->I2CONCLR  = I2CONCLR_SIC;
	    I2C_SlaveState = I2C_SLAVE_IDLE;
        rtk_sem_release_isr( I2C_SemWait );
        break;

    /* Slave transmitter states */
    case I2STAT_S_TX_SLA_R_ACK:
    case I2STAT_S_TX_SLA_R_ACK_ARB_LOST:  /* no repeated start implemented! */
        I2C_TRxCount = 0;
	    I2C_SlaveState = I2C_SLAVE_TRANSMIT_BUSY;
        /* --- intentional fall through --- */
    case I2STAT_S_TX_DAT_ACK:
    case I2STAT_S_TX_DAT_ACK_FINISH:
        if (pcb_SlaveXmit) XmitData = pcb_SlaveXmit(I2C_RegNum + I2C_TRxCount);
        I2C->I2DAT = XmitData;  /* write data back to i2c master */
        I2C->I2CONSET = I2CONSET_AA;
	    I2C->I2CONCLR = I2CONCLR_SIC;
        I2C_TRxCount++;
        break;

    case I2STAT_S_TX_DAT_NOACK:
//	    I2C->I2CONCLR  = I2CONCLR_AAC;
	    I2C->I2CONCLR  = I2CONCLR_SIC;
	    I2C_SlaveState = I2C_SLAVE_IDLE;
        rtk_sem_release_isr( I2C_SemWait );
        I2C->I2CONSET  = I2CONSET_I2EN | I2CONSET_SI;
        break;
#endif

    /* miscellaneous states */
#if 0
    case I2STAT_BUS_ERROR:
        I2C->I2CONSET = I2CONSET_STO | I2CONSET_AA;
        I2C->I2CONCLR = I2CONCLR_SIC;
#ifdef SUPPORT_I2C_MASTER_MODE
        if (I2C_Mode == I2CMASTER) {
            I2C_Error = I2C_ERROR_BUS_ERROR;
            NVIC_DisableIRQ(I2C_IRQn);
        }
#endif
#ifdef SUPPORT_I2C_SLAVE_MODE
        if (I2C_Mode == I2CSLAVE) {
            I2C_SlaveState = I2C_SLAVE_IDLE;
        }
#endif
        rtk_sem_release_isr( I2C_SemWait );
        break;
#endif
    case I2STAT_NO_INFO:
    default:
#ifdef SUPPORT_I2C_MASTER_MODE
        I2C_Error = I2C_ERROR_UNKNOWN_STATE;
#endif
        I2C->I2CONCLR = I2CONCLR_SIC;
#ifdef SUPPORT_I2C_SLAVE_MODE
        if (I2C_Mode == I2CSLAVE) {
	        I2C->I2CONSET = I2CONSET_I2EN | I2CONSET_SI;
        }
#endif
        break;
    }
}
#endif

/* EOF */
