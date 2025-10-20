
#include "MKL46Z4.h"
#include "aux.h"

// LED_GREEN = PTD5
void led_green_init()
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK; // Enable clock for Port D
    PORTD->PCR[5] = PORT_PCR_MUX(1);    // Configure PTD5 as GPIO
    GPIOD->PDDR |= (1 << 5);            // Set PTD5 as output
    GPIOD->PSOR |= (1 << 5);            // Turn off initially
}

void led_green_toggle()
{
    GPIOD->PTOR |= (1 << 5);            // Toggle green LED
}

// LED_RED = PTE29
void led_red_init()
{
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK; // Enable clock for Port E
    PORTE->PCR[29] = PORT_PCR_MUX(1);   // Configure PTE29 as GPIO
    GPIOE->PDDR |= (1 << 29);           // Set PTE29 as output
    GPIOE->PSOR |= (1 << 29);           // Turn off initially
}

void led_red_toggle()
{
    GPIOE->PTOR |= (1 << 29);           // Toggle red LED
}

void sw1_init() {
    SIM->SCGC5 |= (1 << 11);            // Enable clock for Port C
    PORTC->PCR[3] &= ~0x700;            // Clear MUX bits
    PORTC->PCR[3] |= (1 << 8);          // Set MUX to 1 (GPIO)

    PORTC->PCR[3] |= (1 << 1);          // 1 = Pull-up / 0 = Pull-down
    GPIOC->PDDR &= ~(1 << 3);           // Configure PTC3 as input
}

void sw2_init() {
    SIM->SCGC5 |= (1 << 11);            // Enable clock for Port C
    PORTC->PCR[12] &= ~0x700;           // Clear MUX bits
    PORTC->PCR[12] |= (1 << 8);         // Set MUX to 1 (GPIO)

    PORTC->PCR[12] |= (1 << 1);         // 1 = Pull-up / 0 = Pull-down
    GPIOC->PDDR &= ~(1 << 12);          // Configure PTC12 as input
}

int is_pressed_sw1() {
    return !(GPIOC->PDIR & (1 << 3));   // Returns 1 if button is pressed (active low)
}

int is_pressed_sw2() {
    return !(GPIOC->PDIR & (1 << 12));  // Returns 1 if button is pressed (active low)
}

void init_environment() {
    led_green_init();
    led_red_init();
    sw1_init();
    sw2_init();
}

void leds_off() {
    GPIOD->PSOR |= (1 << 5);            // Turn off green LED
    GPIOE->PSOR |= (1 << 29);           // Turn off red LED
}
