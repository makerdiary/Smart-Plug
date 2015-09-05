/*****************************************************************************
 *   uart.h:  Header file for NXP EMxxx
 *
 *   Copyright(C) 2008, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *   2008.08.21  ver 1.00    Prelimnary version, first Release
 *
******************************************************************************/
#ifndef __UART_H
#define __UART_H

typedef enum _BAUD_RATE_
{
  RATE_300_BAUD,
  RATE_600_BAUD,
  RATE_1200_BAUD,
  RATE_2400_BAUD,
  RATE_4800_BAUD,
  RATE_9600_BAUD,
  RATE_14400_BAUD,
  RATE_19200_BAUD,
  RATE_38400_BAUD,
  RATE_57600_BAUD,
  RATE_115200_BAUD,
  RATE_230400_BAUD,
  RATE_460800_BAUD    // Warning: UART PCLK must be higher than 12MHz for this to work
} UART_BAUD_RATE;

typedef enum _DATA_BITS_
{
  BITS_5,
  BITS_6,
  BITS_7,
  BITS_8
} UART_DATA_BITS;

typedef enum _STOP_BITS_
{
  BITS_1,
  BITS_2
} UART_STOP_BITS;
;

typedef enum _PARITY_BITS_
{
  ODD_PARITY,
  EVEN_PARITY,
  NO_PARITY
} UART_PARITY_BITS;
;

typedef struct _uart_config_t_
{
  UART_BAUD_RATE    baudrate;
  UART_DATA_BITS    databits;
  UART_STOP_BITS    stopbits;
  UART_PARITY_BITS  paritybits;
} uart_config_t;

void    Ser_Init(uart_config_t config);
void    Ser_get_exclusive_write_access(void);
void    Set_release_exclusive_write_access(void);
void    Ser_Write(uint8_t *pdata,  uint16_t count);
void    Ser_WrStr(uint8_t *tx_str);
int32_t Ser_Read(uint8_t *pdata,  uint16_t count);
int32_t Ser_RdStr(uint8_t *rx_str, uint16_t count, uint8_t stop_char);

uint16_t Ser_Read_timeout(uint8_t *rx_str, uint16_t count, uint8_t stop_char, uint32_t timeout);

int putchar(int ch);  //Note: Changed to match stdio.h putchar declaration
#endif /* end __UART_H */
/*****************************************************************************
**                            End Of File
******************************************************************************/
