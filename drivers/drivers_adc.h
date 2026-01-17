#ifndef DRIVERS_ADC_H
#define DRIVERS_ADC_H

#include <stdint.h>

/**
 * @brief Initializes the ADC module and the pin connected to the light sensor.
 */
void ADC_InitSensor(void);

/**
 * @brief Performs a conversion and reads the value from the light sensor.
 * @return The digital value from the ADC (e.g., 0-4095 for 12-bit).
 */
uint16_t ADC_ReadSensor(void);

#endif // DRIVERS_ADC_H
