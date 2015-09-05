/*****************************************************************************
 *   uart.c:  UART API file for NXP EMxxx
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.21  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/

#include <stdio.h>
#include <string.h>

#include "bsp.h"
#include "rtk.h"
#include "uart.h"

#define IER_RBR		0x01
#define IER_THRE	0x02
#define IER_RLS		0x04

#define IIR_PEND	0x01
#define IIR_RLS		0x03
#define IIR_RDA		0x02
#define IIR_CTI		0x06
#define IIR_THRE	0x01

#define LSR_RDR		0x01
#define LSR_OE		0x02
#define LSR_PE		0x04
#define LSR_FE		0x08
#define LSR_BI		0x10
#define LSR_THRE	0x20
#define LSR_TEMT	0x40
#define LSR_RXFE	0x80

#define UART_RX_BUFSIZE   100

volatile uint16_t UART_status = 0;
volatile uint16_t UART_RxBufCnt;
volatile uint16_t UART_RxBufReadPos;
volatile uint16_t UART_RxBufWritePos;
volatile uint8_t  UART_RxBuffer[UART_RX_BUFSIZE] = {0};
uint8_t           UART_RxStopChar = 0;
uint16_t          UART_RxOverflowCnt = 0;
rtk_sem_t         UART_TxReadySem;
rtk_sem_t         UART_TxBusySem;
uint8_t *         UART_TxPdata = NULL;
uint16_t          UART_TxCount = 0;

typedef struct _baudrate_settings_
{
  uint8_t DLL;
  uint8_t DLM;
  uint8_t FDR;
} baudrate_settings_t;

static const baudrate_settings_t setting[RATE_230400_BAUD + 1] =
{
  {0xc4, 0x09, 0x10}, //RATE_300_BAUD
  {0xe2, 0x04, 0x10}, //RATE_600_BAUD
  {0x71, 0x02, 0x10}, //RATE_1200_BAUD
  {0xd0, 0x00, 0x21}, //RATE_2400_BAUD
  {0x68, 0x00, 0x21}, //RATE_4800_BAUD
  {0x34, 0x00, 0x21}, //RATE_9600_BAUD
  {0x22, 0x00, 0xf8}, //RATE_14400_BAUD
  {0x1a, 0x00, 0x21}, //RATE_19200_BAUD
  {0x0d, 0x00, 0x21}, //RATE_38400_BAUD
  {0x08, 0x00, 0x85}, //RATE_57600_BAUD
  {0x04, 0x00, 0x85}, //RATE_115200_BAUD
  {0x03, 0x00, 0xC1}  //RATE_230400_BAUD, actual 230769 baud
};

static void StoreInRxBuffer(uint8_t RxData)
{
  if (UART_RxBufCnt < UART_RX_BUFSIZE)
  {
    UART_RxBufCnt++;
    UART_RxBuffer[UART_RxBufWritePos++] = RxData;
    if (UART_RxBufWritePos == UART_RX_BUFSIZE)
    {
      UART_RxBufWritePos = 0;
    }
  }
  else
  {
    UART_RxOverflowCnt++;                          /* buffer overflow */
  }
}

/*****************************************************************************
** Function name:		UART_IRQHandler
**
** Descriptions:		UART interrupt handler
**
** parameters:			None
** Returned value:		None
**
*****************************************************************************/
void UART_IRQHandler(void)
{
  uint8_t IIRValue = (UART->IIR >> 1) & 0x07;                /* only keep the IRQ ID bits from IIR register */
  uint8_t RxData;

  if (IIRValue == IIR_RLS)		                           /* Receive Line Status */
  {
    uint8_t LSRValue = UART->LSR;                                  /* Read LSR will clear the interrupt */
    if (LSRValue & (LSR_OE|LSR_PE|LSR_FE|LSR_RXFE|LSR_BI)) /* There are errors or break interrupt */
    {
      UART_status = LSRValue;
      RxData = UART->RBR;		                           /* Dummy read on RBR to clear interrupt */
      return;                                            /* bail out */
    }
    if (LSRValue & LSR_RDR)	                               /* Receive Data Ready */			
    {
      RxData = UART->RBR;                                /* read RBR will clear the interrupt */
      StoreInRxBuffer(RxData);
    }
  }
  else if (IIRValue == IIR_RDA)                              /* Receive Data Available */
  {
    RxData = UART->RBR;
    StoreInRxBuffer(RxData);
  }
  else if (IIRValue == IIR_CTI)                              /* Character timeout indicator */
  {
    UART_status |= 0x100;                                  /* Bit 9 as the CTI error */
  }
  else if (IIRValue == IIR_THRE)                             /* THRE, transmit holding register empty */
  {
    if (UART_TxCount)
    {
      UART->THR = *UART_TxPdata++;
      UART_TxCount--;
    }
    else
    {
      UART->IER &= ~IER_THRE;                           /* Disable THRE interrupt */
      rtk_sem_release_isr(UART_TxReadySem);
    }
  }
}

/**********************************************************************************************************
 *                                                   Ser_Init()
 *
 * Description : Initialize a serial port for communication.
 *
 * Argument(s) : none.
 *
 * Return(s)   : none.
 **********************************************************************************************************/

void Ser_Init(uart_config_t config)
{
  uint8_t fcr_val;
  /* Enable UART clock */
  SYSCON->SYSAHBCLKCTRL |= (1<<12);
  SYSCON->UARTCLKDIV = SystemCoreClock / 12000000; // UART clk is 12MHz

  UART_TxReadySem = rtk_sem_create(0, "TxReadySem");
  UART_TxBusySem = rtk_sem_create(1, "TxBusySem");

  UART->LCR = 0x80;                                     /* Enable access to Divisor Latches (DLAB=1). */
  UART->DLL = setting[config.baudrate].DLL;                               /* Load divisor LSB. */
  UART->DLM = setting[config.baudrate].DLM;							                  /* Load divisor MSB. */
  UART->FDR = setting[config.baudrate].FDR;
  fcr_val = config.databits;
  fcr_val += (config.stopbits << 2);
  if (config.paritybits != NO_PARITY)
  {
    fcr_val += 0x08 + (config.paritybits << 4);
  }
  UART->LCR = fcr_val;		                                  /* DLAB=0. */
  UART->FCR = 0x07;		                                  /* Enable and reset TX and RX FIFO. */
  UART_RxBufCnt      = 0;
  UART_RxBufReadPos  = 0;
  UART_RxBufWritePos = 0;

  /* Enable the UART Interrupt */
  NVIC_SetPriority (UART_IRQn, UARTINT_PRIORITY);  // Set the vector priority
  NVIC_EnableIRQ(UART_IRQn);

  UART->IER = IER_RLS | IER_RBR;                  /* Enable UART0 RX interrupt */
}

/**********************************************************************************************************
 *                                                Ser_Write()
 *
 * Description : Write data to a serial port.
 *
 * Argument(s) : pdata       Pointer to data to write
 *             : count       Number of bytes (characters) to write
 *
 * Return(s)   : none.
 **********************************************************************************************************/

void Ser_get_exclusive_write_access(void)
{
    rtk_sem_acquire(UART_TxBusySem);         /* Wait until Uart is free */
}

void Set_release_exclusive_write_access(void)
{
    rtk_sem_release(UART_TxBusySem);
}

void Ser_Write(uint8_t *pdata, uint16_t count)
{
  uint8_t TxData = *pdata++;

  if (count > 0)
  {
    count--;                       /* sent the first char out here, the rest is done by ISR */
    UART_TxPdata = pdata;
    UART_TxCount = count;
    UART->THR    = TxData;                                 /* sent first character */
    UART->IER   |= IER_THRE;                               /* Enable THRE interrupt */
    rtk_sem_acquire(UART_TxReadySem);        /* Wait until TX complete */
  }
}

/**********************************************************************************************************
 *                                                Ser_WrStr()
 *
 * Description : Write a character string to a serial port.
 *
 * Argument(s) : tx_str      A character string.
 *
 * Return(s)   : none.
 **********************************************************************************************************/

void Ser_WrStr(uint8_t *tx_str)
{
  Ser_Write(tx_str, strlen((char *)tx_str));
}

/**********************************************************************************************************
 *                                                Ser_Read()
 *
 * Description : Read data from a serial port.
 *
 * Argument(s) : pdata       Pointer to buffer where read data must be stored
 *             : count       Max number of bytes (characters) to read
 *
 * Returns     : The number of bytes (characters) read.
 **********************************************************************************************************/

int32_t Ser_Read(uint8_t *pdata, uint16_t count)
{
  int32_t bytes_read = 0;
  uint8_t stop_char_found = 0;
  uint8_t RxData;

  while ((bytes_read < count) && !stop_char_found)
  {
    while (UART_RxBufCnt && (bytes_read < count) && !stop_char_found)
    {
      UART->IER &= ~(IER_RLS | IER_RBR);     /* disable UART RX interrupt */
      UART_RxBufCnt--;
      RxData = UART_RxBuffer[UART_RxBufReadPos++];
      UART->IER |= (IER_RLS | IER_RBR);      /* enable UART RX interrupt */
      if (UART_RxBufReadPos == UART_RX_BUFSIZE)
      {
        UART_RxBufReadPos = 0;
      }
      if ((UART_RxStopChar != 0) && (RxData == UART_RxStopChar))
      {
        stop_char_found = 1;
      }
      *pdata++ = RxData;
      bytes_read++;
    }
    if ((bytes_read < count) && !stop_char_found)
    {
      rtk_tsk_sleep(10);
    }
  }
  return bytes_read;
}

/**********************************************************************************************************
 *                                                Ser_RdStr()
 *
 * Description : Read a character string from a serial port.
 *               End of string is detected on reception of stop character.
 *
 * Argument(s) : rx_str      Pointer to buffer where read data must be stored
 *             : count       Max number of bytes (characters) to read,
 *                           including the terminating end of string '\0' character
 *             : stop_char   Stop reading data when stop character is received.
 *
 * Returns     : The number of bytes (characters) read, including the end of string '\0' character.
 **********************************************************************************************************/
int32_t Ser_RdStr(uint8_t *rx_str, uint16_t count, uint8_t stop_char)
{
  int32_t bytes_read = 0;

  count--;                        /* reserve room for the string terminator '\0' */
  UART_RxStopChar = stop_char;
  bytes_read = Ser_Read(rx_str, count);
  UART_RxStopChar = 0;
  rx_str[bytes_read++] = '\0';    /* append the string terminator '\0' */
  return bytes_read;
}


uint16_t Ser_Read_timeout(uint8_t *rx_str, uint16_t count, uint8_t stop_char, uint32_t timeout)
{
    uint16_t bytes_read = 0;
    uint8_t  stop_char_found = 0;
    uint8_t  timed_out = 0;
    uint8_t  RxData;

    if (count == 0) return 0;
    while ((bytes_read < count) && !stop_char_found && !timed_out)
    {
        while (UART_RxBufCnt && (bytes_read < count) && !stop_char_found)
        {
            UART->IER &= ~(IER_RLS | IER_RBR);     /* disable UART RX interrupt */
            UART_RxBufCnt--;
            RxData = UART_RxBuffer[UART_RxBufReadPos++];
            UART->IER |= (IER_RLS | IER_RBR);      /* enable UART RX interrupt */
            if (UART_RxBufReadPos == UART_RX_BUFSIZE)
            {
                UART_RxBufReadPos = 0;
            }
            if ((stop_char != 0) && (RxData == stop_char))
            {
                stop_char_found = 1;
            }
            *rx_str++ = RxData;
            bytes_read++;
        }
        if (timeout == 0)
        {
            timed_out = 1;
        }
        else
        {
            if ((bytes_read < count) && !stop_char_found)
            {
                // this timeout mechanism isn't optimal, but it works for us
                rtk_tsk_sleep(2);
                if (timeout >= 2)
                    timeout -= 2;
                else
                    timed_out = 1;
            }
        }
    }
    return bytes_read;
}

/**********************************************************************************************************
 *                                                Ser_Write()
 *
 * Description : Write data to a serial port.
 *
 * Argument(s) : pdata       Pointer to data to write
 *             : count       Number of bytes (characters) to write
 *
 * Return(s)   : none.
 **********************************************************************************************************/

/**********************************************************************************************************
 *                                                putchar()
 *
 * Description : Write one byte to a serial port.
 *
 * Argument(s) : ch          byte to write
 *
 * Return(s)   : the byte had written.
 * Note        : Changed to match stdio.h putchar declaration
 **********************************************************************************************************/
int putchar(int ch)
{
    Ser_Write((uint8_t *)(&ch),1);
    return(ch);
}
/* EOF */
