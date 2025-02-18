#include "MKL46Z4.h"

// LED_GREEN = PTD5
// LED_RED = PTE29
// SW1 = PTC3
// SW3 = PTC12

void delay(void)
{
    volatile int i;
    for (i = 0; i < 100000; i++);
}

void leds_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1u << 5);
    PTE->PDDR |= (1u << 29);
}

void switches_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PTC->PDDR &= ~((1u << 3) | (1u << 12));
}

void update_leds(int sw1_state, int sw3_state)
{
    if (sw1_state == 0 && sw3_state == 0) {
        PTD->PCOR = (1u << 5);  // Turn on green LED
        PTE->PSOR = (1u << 29); // Turn off red LED
    } else {
        PTD->PSOR = (1u << 5);  // Turn off green LED
        PTE->PCOR = (1u << 29); // Turn on red LED
    }
}

int main(void)
{
    SIM->COPC = 0;
    
    leds_init();
    switches_init();
    
    int sw1_state = 0;
    int sw3_state = 0;

    while (1) {
        // Check SW1 state
        if (!(PTC->PDIR & (1u << 3))) {
            delay();
            if (!(PTC->PDIR & (1u << 3))) {
                sw1_state = !sw1_state;
                while (!(PTC->PDIR & (1u << 3)));
            }
        }
        
        // Check SW3 state
        if (!(PTC->PDIR & (1u << 12))) {
            delay();
            if (!(PTC->PDIR & (1u << 12))) {
                sw3_state = !sw3_state;
                while (!(PTC->PDIR & (1u << 12)));
            }
        }
        
        // Update LEDs based on switch states
        update_leds(sw1_state, sw3_state);
    }

    return 0;
}

