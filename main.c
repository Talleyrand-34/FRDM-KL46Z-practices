#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29
// SW1 = PTC3
// SW3 = PTC12

void delay(void)
{
    volatile int i;
    for (i = 0; i < 100000; i++);
}

void led_green_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1u << 5);
}

void led_red_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    PTE->PDDR |= (1u << 29);
}

void switches_init(void)
{
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK;
    PTC->PDDR &= ~((1u << 3) | (1u << 12));
}

void led_green_toggle(void)
{
    PTD->PTOR = (1u << 5);
}

void led_red_toggle(void)
{
    PTE->PTOR = (1u << 29);
}

int main(void)
{
    SIM->COPC = 0;
    
    led_green_init();
    led_red_init();
    switches_init();
    
    int green_state = 0;
    int red_state = 0;

    while (1) {
        if (!(PTC->PDIR & (1u << 3))) {
            delay();
            if (!(PTC->PDIR & (1u << 3))) {
                green_state = !green_state;
                if (green_state) {
                    PTD->PCOR = (1u << 5);
                } else {
                    PTD->PSOR = (1u << 5);
                }
            }
        }
        
        if (!(PTC->PDIR & (1u << 12))) {
            delay();
            if (!(PTC->PDIR & (1u << 12))) {
                red_state = !red_state;
                if (red_state) {
                    PTE->PCOR = (1u << 29);
                } else {
                    PTE->PSOR = (1u << 29);
                }
            }
        }
    }

    return 0;
}

