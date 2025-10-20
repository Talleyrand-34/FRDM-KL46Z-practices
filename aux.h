#ifndef AUX_H
#define AUX_H

// Initializes the green LED (PTD5)
void led_green_init(void);

// Toggles the green LED state
void led_green_toggle(void);

// Initializes the red LED (PTE29)
void led_red_init(void);

// Toggles the red LED state
void led_red_toggle(void);

// Initializes the SW1 button (PTC3)
void sw1_init(void);

// Initializes the SW2 button (PTC12)
void sw2_init(void);

// Checks if the SW1 button is pressed
int is_pressed_sw1(void);

// Checks if the SW2 button is pressed
int is_pressed_sw2(void);

// Initializes the environment (LEDs and buttons)
void init_environment(void);

// Turns off both LEDs
void leds_off(void);

#endif // AUX_H
