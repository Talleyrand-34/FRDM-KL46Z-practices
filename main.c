#include "MKL46Z4.h"

// LED (RG)
// LED_GREEN = PTD5
// LED_RED = PTE29

void delay(void)
{
  volatile int i;

  for (i = 0; i < 1000000; i++);
}

/* // LED_GREEN = PTD5 */
/* void led_green_init() */
/* { */
/*   SIM->COPC = 0; */
   /* SIM->SCGC5|=1<<12; */
/*   SIM_SCGC5_PORTD(1); */
/**/
/*   PORTD->PCR[5]=1; */
/*    */
   /* GPIOD->PDDR */
/*   // usar PSOR */
   /* GPIOD->PSOR */
/* } */
/**/
/* void led_green_toggle() */
/* { */
/*   // */
/* } */
/**/
/* // LED_RED = PTE29 */
/* void led_red_init() */
/* { */
/*   // */
/* } */
/**/
/* void led_red_toggle(void) */
/* { */
/*   // */
/* } */

// LED_GREEN = PTD5
void led_green_init()
{
  SIM->COPC = 0;
  SIM->SCGC5 |= SIM_SCGC5_PORTD_MASK;
  PORTD->PCR[5] = PORT_PCR_MUX(1);
  PTD->PDDR |= (1u << 5);
}

void led_green_toggle()
{
  PTD->PTOR = (1u << 5);
}

// LED_RED = PTE29
void led_red_init()
{
  SIM->SCGC5 |= SIM_SCGC5_PORTE_MASK;
  PORTE->PCR[29] = PORT_PCR_MUX(1);
  PTE->PDDR |= (1u << 29);
}

void led_red_toggle(void)
{
  PTE->PTOR = (1u << 29);
}


int main(void)
{
  led_green_init();
  led_red_init();

  while (1) {
    led_green_toggle();
    delay();
    led_red_toggle();
  }

  return 0;
}
