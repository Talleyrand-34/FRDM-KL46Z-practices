#include "drivers_led.h"
#include "MKL46Z4.h"

#define TPM_PERIOD_TICKS (48000 - 1) // For ~1kHz PWM with 48MHz clock

void LED_Init(void) {
    // Enable clocks for PORTD and PORTE
    SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // Configure PTD5 (Green LED) for TPM0_CH5 function
    PORTD->PCR[5] &= ~PORT_PCR_MUX_MASK;
    PORTD->PCR[5] |= PORT_PCR_MUX(4); // MUX = 4 for TPM0_CH5

    // Configure PTE29 (Red LED) for TPM0_CH2 function
    PORTE->PCR[29] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[29] |= PORT_PCR_MUX(3); // MUX = 4 for TPM0_CH2

    // Enable clock for TPM0
    SIM->SCGC6 |= SIM_SCGC6_TPM0_MASK;

    // Select MCGFLLCLK as TPM clock source
    SIM->SOPT2 |= SIM_SOPT2_TPMSRC(1);

    // Configure TPM0
    TPM0->SC = 0; // Disable timer to configure
    TPM0->MOD = TPM_PERIOD_TICKS;

    // Configure channel 5 (Green LED) for Edge-aligned PWM, Low-true pulses
    TPM0->CONTROLS[5].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
    TPM0->CONTROLS[5].CnV = 0; // Start with LED off

    // Configure channel 2 (Red LED) for Edge-aligned PWM, Low-true pulses
    TPM0->CONTROLS[2].CnSC = TPM_CnSC_MSB_MASK | TPM_CnSC_ELSA_MASK;
    TPM0->CONTROLS[2].CnV = 0; // Start with LED off

    // Set prescaler to 1 and start the timer
    TPM0->SC = TPM_SC_PS(0) | TPM_SC_CMOD(1);
}

void LED_SetGreen(float brightness) {
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;
    
    // Update TPM channel value
    TPM0->CONTROLS[5].CnV = (uint32_t)(brightness * TPM_PERIOD_TICKS);
}

void LED_SetRed(float brightness) {
    if (brightness < 0.0f) brightness = 0.0f;
    if (brightness > 1.0f) brightness = 1.0f;

    // Update TPM channel value
    TPM0->CONTROLS[2].CnV = (uint32_t)(brightness * TPM_PERIOD_TICKS);
}
