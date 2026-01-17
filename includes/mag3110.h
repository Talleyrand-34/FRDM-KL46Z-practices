#ifndef MAG3110_H
#define MAG3110_H

#include "fsl_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MAG3110_I2C_ADDRESS (0x0E)

// Register Addresses
#define MAG3110_DR_STATUS 0x00
#define MAG3110_OUT_X_MSB 0x01
#define MAG3110_OUT_X_LSB 0x02
#define MAG3110_OUT_Y_MSB 0x03
#define MAG3110_OUT_Y_LSB 0x04
#define MAG3110_OUT_Z_MSB 0x05
#define MAG3110_OUT_Z_LSB 0x06
#define MAG3110_WHO_AM_I 0x07
#define MAG3110_SYSMOD 0x08
#define MAG3110_OFF_X_MSB 0x09
#define MAG3110_OFF_X_LSB 0x0A
#define MAG3110_OFF_Y_MSB 0x0B
#define MAG3110_OFF_Y_LSB 0x0C
#define MAG3110_OFF_Z_MSB 0x0D
#define MAG3110_OFF_Z_LSB 0x0E
#define MAG3110_DIE_TEMP 0x0F
#define MAG3110_CTRL_REG1 0x10
#define MAG3110_CTRL_REG2 0x11

// Control Register 1 Bits
#define MAG_CTRL_REG1_AC_MASK 0x01
#define MAG_CTRL_REG1_TM_MASK 0x02
#define MAG_CTRL_REG1_FR_MASK 0x04
#define MAG_CTRL_REG1_OS_MASK 0x18

// Control Register 2 Bits
#define MAG_CTRL_REG2_AUTO_MRST_EN_MASK 0x80
#define MAG_CTRL_REG2_RAW_MASK 0x20
#define MAG_CTRL_REG2_MAG_RST_MASK 0x10

typedef struct _mag_data
{
    int16_t x;
    int16_t y;
    int16_t z;
} mag_data_t;


/*******************************************************************************
 * API
 ******************************************************************************/
status_t MAG3110_Init(I2C_Type *base);
status_t MAG3110_ReadData(I2C_Type *base, mag_data_t *data);


#endif // MAG3110_H