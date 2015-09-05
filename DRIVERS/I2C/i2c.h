/*****************************************************************************
 *   i2c.h:  Header file for NXP LPC Family Microprocessors
 *
 *   Copyright(C) 2011, NXP Semiconductor
 *   All rights reserved.
 *
 *   History
 *
 ******************************************************************************/

#ifndef __I2C_H
#define __I2C_H

#define I2CMASTER    0x01
#define I2CSLAVE     0x02

typedef enum
{
  I2C_OK,
  I2C_ERROR_BUS_ERROR,
  I2C_ERROR_ARB_LOST,
  I2C_ERROR_NOACK,
  I2C_ERROR_UNKNOWN_STATE,
  I2C_ERROR_INIT_FAILED,
  I2C_ERROR_WRONG_MODE
} i2c_error_t;

i2c_error_t I2C_Init     (uint32_t I2cMode, uint8_t address, void *pFuncRecv, void *pFuncXmit);
i2c_error_t I2C_Read     (uint8_t address,              uint8_t *p_buf, uint8_t count);
i2c_error_t I2C_ReadRegs (uint8_t address, uint8_t reg, uint8_t *p_buf, uint8_t count);
i2c_error_t I2C_Write    (uint8_t address,              uint8_t *p_buf, uint8_t count);
i2c_error_t I2C_WriteRegs(uint8_t address, uint8_t reg, uint8_t *p_buf, uint8_t count);

#endif /* end __I2C_H */
