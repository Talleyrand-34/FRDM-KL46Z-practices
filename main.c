#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

typedef enum {
    STATE_BOTH_OFF = 0,     // Both LEDs off
    STATE_RED_OFF_GREEN_ON, // Red off, Green on
    STATE_BOTH_ON,          // Both LEDs on
    STATE_RED_ON_GREEN_OFF  // Red on, Green off
} led_state_t;

led_state_t current_state = STATE_BOTH_OFF; // Start with both LEDs off
void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

// LED_GREEN = PTD5
void leds_init()
{
  // SIM->COPC
  // SIM->SCGC5
  // PORTD->PCR[5]
  // GPIOD->PDDR
  // GPIOD->PSOR
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);   // Green LED
  PORTE->PCR[29] = PORT_PCR_MUX(1);  // Red LED
  PTD->PDDR |= (1u << 5);            // Set as output
  PTE->PDDR |= (1u << 29);           // Set as output
}

void switches_init(void)
{
  //
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;   // SW1 (left)
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;  // SW3 (right)
    PTC->PDDR &= ~((1u << 3) | (1u << 12)); // Set as inputs
}

void update_leds_by_state(led_state_t state)
{
    switch(state) {
        case STATE_BOTH_OFF:
            PTD->PSOR = (1u << 5);  // Turn off green LED
            PTE->PSOR = (1u << 29); // Turn off red LED
            break;
            
        case STATE_RED_OFF_GREEN_ON:
            PTD->PCOR = (1u << 5);  // Turn on green LED
            PTE->PSOR = (1u << 29); // Turn off red LED
            break;
            
        case STATE_BOTH_ON:
            PTD->PCOR = (1u << 5);  // Turn on green LED
            PTE->PCOR = (1u << 29); // Turn on red LED
            break;
            
        case STATE_RED_ON_GREEN_OFF:
            PTD->PSOR = (1u << 5);  // Turn off green LED
            PTE->PCOR = (1u << 29); // Turn on red LED
            break;
    }
}
led_state_t get_next_state(led_state_t current)
{
    switch(current) {
        case STATE_BOTH_OFF:
            return STATE_RED_OFF_GREEN_ON;
        case STATE_RED_OFF_GREEN_ON:
            return STATE_BOTH_ON;
        case STATE_BOTH_ON:
            return STATE_RED_ON_GREEN_OFF;
        case STATE_RED_ON_GREEN_OFF:
            return STATE_BOTH_OFF;
        default:
            return STATE_BOTH_OFF;
    }
}
led_state_t invert_state(led_state_t current)
{
    switch(current) {
        case STATE_BOTH_OFF:
            return STATE_BOTH_ON;
        case STATE_RED_OFF_GREEN_ON:
            return STATE_RED_ON_GREEN_OFF;
        case STATE_BOTH_ON:
            return STATE_BOTH_OFF;
        case STATE_RED_ON_GREEN_OFF:
            return STATE_RED_OFF_GREEN_ON;
        default:
            return current;
    }
}

int main(void)
{
    SIM->COPC = 0; // Disable watchdog
    
    leds_init();
    switches_init();
    
    // Initialize LEDs to off state
    update_leds_by_state(current_state);
    
    while (1) {
        // Check SW1 (left button) - invert current state
        if (!(PTC->PDIR & (1u << 3))) {
            delay(); // Debounce
            if (!(PTC->PDIR & (1u << 3))) {
                current_state = invert_state(current_state);
                update_leds_by_state(current_state);
                // Wait for button release
                while (!(PTC->PDIR & (1u << 3)));
                delay(); // Additional debounce for release
            }
        }
        
        // Check SW3 (right button) - cycle to next state
        if (!(PTC->PDIR & (1u << 12))) {
            delay(); // Debounce
            if (!(PTC->PDIR & (1u << 12))) {
                current_state = get_next_state(current_state);
                update_leds_by_state(current_state);
                // Wait for button release
                while (!(PTC->PDIR & (1u << 12)));
                delay(); // Additional debounce for release
            }
        }
    }
    return 0;
}
