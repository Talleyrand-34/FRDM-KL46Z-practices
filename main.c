#include "MKL46Z4.h"
#include "lcd.h"
#include "aux.h" 

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWICH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Global vars
volatile unsigned int hits = 0; 
volatile unsigned int misses = 0; 
volatile int current_led = 0; // 0: red, 1: green


// Enable IRCLK (Internal Reference Clock) Activa el reloj interno de 32 kHz para que pueda ser usado por la pantalla LCD.
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

// Creamos las interrupciones:
void interrupts() {
    // SW1
    PORTC->PCR[3] |= (0xA << 16);

    // SW2
    PORTC->PCR[12] |= (0xA << 16);

    NVIC_EnableIRQ(31); // Habilitar interrupción para el puerto D
}


// PORTD-Handler Routine

void PORTDIntHandler(void) {
    if (PORTC->ISFR & (1 << 3)) { // SW1 pressed
        if (current_led == 1) { 
            hits++; 
        } else { 
            misses++; 
        }
        PORTC->ISFR |= (1 << 3); // Clear flag
    }

    if (PORTC->ISFR & (1 << 12)) { // SW2 pressed
        if (current_led == 0) { // Si el LED es rojo (correcto)
            hits++; 
        } else { 
            misses++; 
        }
        PORTC->ISFR |= (1 << 12); // Clear flag
    }

    // Actualizar la pantalla LCD con los aciertos y fallos
    lcd_display_time(hits, misses);
}


int main(void) {
    irclk_ini(); // Internal clock
    lcd_ini(); // LCD
    lcd_display_time(0, 0); // LCD intial state
    init_environment(); // Leds and buttons init
    interrupts(); 

    // 'Random' sequence :-)
    volatile unsigned int sequence = 0x32B14D98;
    unsigned int index = 0;

    while (index < 32) {
        if (sequence & (1 << index)) { 
            led_green_toggle();
            current_led = 1; 
        } else {
            led_red_toggle();
            current_led = 0;
        }
        
        delay();
        leds_off();
        index++;
    }


    // Game over
    NVIC_DisableIRQ(31); 

    while (1) {
        // Resultado
        lcd_display_time(hits, misses);

        led_green_toggle();
        led_red_toggle();
        delay();

        leds_off();
        delay();
    }

    return 0;
}
