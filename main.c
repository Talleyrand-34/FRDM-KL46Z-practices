#include "MKL46Z4.h"
#include "lcd.h"

// LED (RG)
// LED_GREEN = PTD5 (pin 98)
// LED_RED = PTE29 (pin 26)

// SWITCH
// RIGHT (SW1) = PTC3 (pin 73)
// LEFT (SW2) = PTC12 (pin 88)

// Enable IRCLK (Internal Reference Clock)
void irclk_ini()
{
    MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
    MCG->C2 = MCG_C2_IRCS(0); // 0: 32KHZ internal reference clock; 1: 4MHz IRC
}

void delay(void)
{
    volatile int i;
    for (i = 0; i < 1000000; i++);
}

// Configuración de pines
void pin_setup() {
    // Habilitar reloj para los puertos C, D y E
    SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK | SIM_SCGC5_PORTD_MASK | SIM_SCGC5_PORTE_MASK;

    // Configurar LED verde (PTD5)
    PORTD->PCR[5] = PORT_PCR_MUX(1);
    PTD->PDDR |= (1 << 5);

    // Configurar LED rojo (PTE29)
    PORTE->PCR[29] = PORT_PCR_MUX(1);
    PTE->PDDR |= (1 << 29);

    // Configurar botón derecho (PTC3)
    PORTC->PCR[3] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);

    // Configurar botón izquierdo (PTC12)
    PORTC->PCR[12] = PORT_PCR_MUX(1) | PORT_PCR_PE_MASK | PORT_PCR_PS_MASK | PORT_PCR_IRQC(0xA);

    NVIC_EnableIRQ( PORTC_PORTD_IRQn); // Reemplazar con el identificador correcto
}

void led_green_on() {
    PTD->PCOR = (1 << 5);
}

void led_red_on() {
    PTE->PCOR = (1 << 29);
}

void leds_off() {
    PTD->PSOR = (1 << 5);
    PTE->PSOR = (1 << 29);
}

volatile int hits = 0, misses = 0;
volatile int current_led = -1; // -1: ninguno, 0: rojo, 1: verde

void PORTC_IRQHandler() {
    if ((PORTC->ISFR & (1 << 3))) { // Botón derecho presionado
        if (current_led == 1) {
            hits++;
        } else {
            misses++;
        }
        PORTC->ISFR = (1 << 3); // Limpiar bandera de interrupción del botón derecho
        lcd_display_dec(hits * 100 + misses); // Actualizar el LCD con aciertos*100+fallos
    }

    if ((PORTC->ISFR & (1 << 12))) { // Botón izquierdo presionado
        if (current_led == 0) {
            hits++;
        } else {
            misses++;
        }
        PORTC->ISFR = (1 << 12); // Limpiar bandera de interrupción del botón izquierdo
        lcd_display_dec(hits * 100 + misses); // Actualizar el LCD con aciertos*100+fallos
    }
}

int main(void)
{
    irclk_ini(); // Activar reloj interno para usar con LCD

    lcd_ini();
    lcd_display_dec(0); // Inicializar el LCD con "0000"

    pin_setup();

    // Secuencia fija de LEDs
    volatile unsigned int sequence = 0x32B14D98;
    unsigned int index = 0;

    while (index < 32) {
        leds_off();
        if ((sequence >> index) & 1) { // Bit impar: encender LED verde
            led_green_on();
            current_led = 1;
        } else { // Bit par: encender LED rojo
            led_red_on();
            current_led = 0;
        }

        delay();
        index++;
    }

    // Mostrar resultado final en LCD y parpadear LEDs
    while (1) {
        lcd_display_dec(hits * 100 + misses); // Mostrar resultado final en el LCD
        delay();
        leds_off();
        delay();
        led_green_on(); // Parpadeo alternado de LEDs al final del juego
        led_red_on();
        delay();
        leds_off();
        delay();
    }

    return 0;
}
