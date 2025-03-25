#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
// see Chapter 24 in MCU doc
void irclk_ini()
{
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); //0 32KHZ internal reference clock; 1= 4MHz irc
}

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

int main(void)
{
  irclk_ini(); // Enable internal ref clk to use by LCD

  lcd_ini();
  lcd_display_dec(666);

  // 'Random' sequence :-)
  volatile unsigned int sequence = 0x32B14D98,
    index = 0;

  while (index < 32) {
    if (sequence & (1 << index)) { //odd
      //
      // Switch on green led
      // [...]
      //
      led_green_on();
    } else { //even
      //
      // Switch on red led
      // [...]
      //
      led_red_on();
    }
    // [...]
    delay();
  }

  // Stop game and show blinking final result in LCD: hits:misses
  // [...]
  //

    while (1) {
        lcd_display_dec(hits * 1000 + misses);
        delay();
        lcd_clear();
        delay();
    }

    return 0;

  return 0;
}
// Configuracion pins
void pin_setup() {
    // Habilitar o reloxo para os portos C, D e E
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // Configurar LED verde (PTD5)
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1 << 5);

    // Configurar LED vermello (PTE29)
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    PTE->PDDR |= (1 << 29);

    // Configurar botón dereito (PTC3)
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);

    // Configurar botón esquerdo (PTC12)
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);
}
volatile int hits = 0, misses = 0;
volatile int current_led = 0; // 0 para vermello, 1 para verde

void PORTC_IRQHandler() {
    if (PORTC->ISFR & (1 << 3)) { // Botón dereito
        if (current_led == 1) hits++;
        else misses++;
        PORTC->ISFR = (1 << 3);
    }
    if (PORTC->ISFR & (1 << 12)) { // Botón esquerdo
        if (current_led == 0) hits++;
        else misses++;
        PORTC->ISFR = (1 << 12);
    }
    lcd_display_dec(hits * 1000 + misses);
}
void led_green_on() {
    PTD->PCOR = (1 << 5);
    current_led = 1;
}

void led_red_on() {
    PTE->PCOR = (1 << 29);
    current_led = 0;
}

void leds_off() {
    PTD->PSOR = (1 << 5);
    PTE->PSOR = (1 << 29);
}
