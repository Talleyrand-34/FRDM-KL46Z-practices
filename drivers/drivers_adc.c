#include "drivers_adc.h"
#include "MKL46Z4.h"

void ADC_InitSensor(void) {
    // Enable clock for PORTE
    SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;

    // Configure PTE22 (ADC0_SE3) for analog function
    PORTE->PCR[22] &= ~PORT_PCR_MUX_MASK;
    PORTE->PCR[22] |= PORT_PCR_MUX(0); // MUX = 0 for analog

    // Enable clock for ADC0
    SIM->SCGC6 |= SIM_SCGC6_ADC0_MASK;

    // Configure ADC0
    // 12-bit single-ended, bus clock, divide by 2
    ADC0->CFG1 = ADC_CFG1_ADIV(1) | ADC_CFG1_MODE(1) | ADC_CFG1_ADICLK(0);
    
    // Set voltage reference to default (Vdda and Vssa)
    ADC0->SC2 = ADC_SC2_REFSEL(0);
    
    // Enable hardware averaging of 32 samples for stability
    ADC0->SC3 = ADC_SC3_AVGE_MASK | ADC_SC3_AVGS(3); // 3 corresponds to 32 samples

    // Start calibration
    ADC0->SC3 |= ADC_SC3_CAL_MASK;
    while (ADC0->SC3 & ADC_SC3_CAL_MASK);

    // Check for calibration failure
    if (ADC0->SC3 & ADC_SC3_CALF_MASK) {
        // Handle calibration failure (optional, for robustness)
        // For this example, we'll clear the flag and continue.
        ADC0->SC3 |= ADC_SC3_CALF_MASK;
    }
}

uint16_t ADC_ReadSensor(void) {
    // Start conversion on channel 3 (ADC0_SE3)
    ADC0->SC1[0] = ADC_SC1_ADCH(3);

    // Wait for conversion to complete
    while (!(ADC0->SC1[0] & ADC_SC1_COCO_MASK));

    // Read and return the result
    return (uint16_t)ADC0->R[0];
}
