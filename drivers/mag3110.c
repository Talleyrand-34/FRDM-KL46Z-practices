#include "mag3110.h"
#include "fsl_i2c.h"

/*******************************************************************************
 * Code
 ******************************************************************************/

// Local delay function
static void mag_delay(volatile uint32_t nof)
{
    while (nof--)
    {
        __NOP();
    }
}

static status_t MAG3110_WriteRegister(I2C_Type *base, uint8_t reg, uint8_t value)
{
    i2c_master_transfer_t transfer;
    memset(&transfer, 0, sizeof(transfer));

    transfer.slaveAddress = MAG3110_I2C_ADDRESS;
    transfer.direction = kI2C_Write;
    transfer.subaddress = reg;
    transfer.subaddressSize = 1;
    transfer.data = &value;
    transfer.dataSize = 1;
    transfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &transfer);
}

static status_t MAG3110_ReadRegisters(I2C_Type *base, uint8_t reg, uint8_t *buffer, uint32_t size)
{
    i2c_master_transfer_t transfer;
    memset(&transfer, 0, sizeof(transfer));

    transfer.slaveAddress = MAG3110_I2C_ADDRESS;
    transfer.direction = kI2C_Read;
    transfer.subaddress = reg;
    transfer.subaddressSize = 1;
    transfer.data = buffer;
    transfer.dataSize = size;
    transfer.flags = kI2C_TransferDefaultFlag;

    return I2C_MasterTransferBlocking(base, &transfer);
}

status_t MAG3110_Init(I2C_Type *base)
{
    status_t status;

    // Set to standby mode
    status = MAG3110_WriteRegister(base, MAG3110_CTRL_REG1, 0x00);
    if (status != kStatus_Success)
    {
        return status;
    }
    mag_delay(10000); // Delay

    // Enable automatic magnetic sensor resets
    status = MAG3110_WriteRegister(base, MAG3110_CTRL_REG2, MAG_CTRL_REG2_AUTO_MRST_EN_MASK);
     if (status != kStatus_Success)
    {
        return status;
    }
    mag_delay(10000); // Delay

    // Set to active mode, 80Hz data rate
    status = MAG3110_WriteRegister(base, MAG3110_CTRL_REG1, MAG_CTRL_REG1_AC_MASK);

    return status;
}

status_t MAG3110_ReadData(I2C_Type *base, mag_data_t *data)
{
    status_t status;
    uint8_t buffer[6];

    status = MAG3110_ReadRegisters(base, MAG3110_OUT_X_MSB, buffer, sizeof(buffer));
    if (status != kStatus_Success)
    {
        return status;
    }

    data->x = (int16_t)((buffer[0] << 8) | buffer[1]);
    data->y = (int16_t)((buffer[2] << 8) | buffer[3]);
    data->z = (int16_t)((buffer[4] << 8) | buffer[5]);

    return kStatus_Success;
}