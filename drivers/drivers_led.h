#ifndef DRIVERS_LED_H
#define DRIVERS_LED_H

/**
 * @brief Initializes the LED pins and the PWM timer module.
 */
void LED_Init(void);

/**
 * @brief Sets the brightness of the Green LED.
 * @param brightness A value from 0.0 (off) to 1.0 (maximum brightness).
 */
void LED_SetGreen(float brightness);

/**
 * @brief Sets the brightness of the Red LED.
 * @param brightness A value from 0.0 (off) to 1.0 (maximum brightness).
 */
void LED_SetRed(float brightness);


#endif // DRIVERS_LED_H
