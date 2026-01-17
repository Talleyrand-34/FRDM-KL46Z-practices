#ifndef MMA8451_H
#define MMA8451_H

#include "fsl_i2c.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define MMA8451_I2C_ADDRESS (0x1D)

// Register Addresses
#define MMA8451_STATUS 0x00
#define MMA8451_OUT_X_MSB 0x01
#define MMA8451_OUT_X_LSB 0x02
#define MMA8451_OUT_Y_MSB 0x03
#define MMA8451_OUT_Y_LSB 0x04
#define MMA8451_OUT_Z_MSB 0x05
#define MMA8451_OUT_Z_LSB 0x06
#define MMA8451_WHO_AM_I 0x0D
#define MMA8451_XYZ_DATA_CFG 0x0E
#define MMA8451_CTRL_REG1 0x2A

typedef struct _accel_data
{
    int16_t x;
    int16_t y;
    int16_t z;
} accel_data_t;

/*******************************************************************************
 * API
 ******************************************************************************/
status_t MMA8451_Init(I2C_Type *base);
status_t MMA8451_ReadData(I2C_Type *base, accel_data_t *data);

#endif // MMA8451_H
