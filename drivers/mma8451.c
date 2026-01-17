#include "mma8451.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

static status_t MMA8451_WriteRegister(I2C_Type *base, uint8_t reg, uint8_t value)
{
    i2c_master_transfer_t transfer;
    memset(&transfer, 0, sizeof(transfer));

    transfer.slaveAddress = MMA8451_I2C_ADDRESS;
    transfer.direction = kI2C_Write;
    transfer.subaddress = reg;
    transfer.subaddressSize = 1;
    transfer.data = &value;
    transfer.dataSize = 1;
    transfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &transfer);
}

static status_t MMA8451_ReadRegisters(I2C_Type *base, uint8_t reg, uint8_t *buffer, uint32_t size)
{
    i2c_master_transfer_t transfer;
    memset(&transfer, 0, sizeof(transfer));

    transfer.slaveAddress = MMA8451_I2C_ADDRESS;
    transfer.direction = kI2C_Read;
    transfer.subaddress = reg;
    transfer.subaddressSize = 1;
    transfer.data = buffer;
    transfer.dataSize = size;
    transfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &transfer);
}

status_t MMA8451_Init(I2C_Type *base)
{
    status_t status;

    // Put sensor into standby mode
    status = MMA8451_WriteRegister(base, MMA8451_CTRL_REG1, 0x00);
    if (status != kStatus_Success)
    {
        return status;
    }

    // Set full-scale range to +/-4g
    status = MMA8451_WriteRegister(base, MMA8451_XYZ_DATA_CFG, 0x01);
    if (status != kStatus_Success)
    {
        return status;
    }

    // Set to active mode, 200Hz data rate, low noise
    status = MMA8451_WriteRegister(base, MMA8451_CTRL_REG1, 0x0D);

    return status;
}

status_t MMA8451_ReadData(I2C_Type *base, accel_data_t *data)
{
    status_t status;
    uint8_t buffer[6];

    status = MMA8451_ReadRegisters(base, MMA8451_OUT_X_MSB, buffer, sizeof(buffer));
    if (status != kStatus_Success)
    {
        return status;
    }

    data->x = (int16_t)(((buffer[0] << 8) | buffer[1])) >> 2;
    data->y = (int16_t)(((buffer[2] << 8) | buffer[3])) >> 2;
    data->z = (int16_t)(((buffer[4] << 8) | buffer[5])) >> 2;

    return kStatus_Success;
}
