#include "board.h"
#include "fsl_debug_console.h"
#include "fsl_device_registers.h"
#include "pin_mux.h"
#include <string.h>

#define MAX_BUFFER 128
#define BLOCK_SIZE 4 // Bloques de 4 bytes (32 bits)

// Desactivar watchdog
// Rompe en mi placa si no se hace
// Lo que dije del bucle
static void watchdog_disable(void) { SIM->COPC = 0; }

// Inicializar timer
void timer_init(void) {
  SIM->SCGC6 |= SIM_SCGC6_TPM1_MASK;
  SIM->SOPT2 = (SIM->SOPT2 & ~SIM_SOPT2_TPMSRC_MASK) | SIM_SOPT2_TPMSRC(1);
  TPM1->SC = TPM_SC_PS(0);
  TPM1->MOD = 0xFFFF;
  TPM1->SC |= TPM_SC_CMOD(1);
}

// Obtener timestamp actual
uint32_t timestamp(void) { return TPM1->CNT; }

// Imprimir bits en grupos de 8 (por byte)
void print_bits(uint8_t *data, int len) {
  for (int i = 0; i < len; i++) {
    for (int bit = 7; bit >= 0; bit--) {
      PUTCHAR((data[i] & (1 << bit)) ? '1' : '0');
    }
    PUTCHAR(' ');

    if ((i + 1) % BLOCK_SIZE == 0) {
      PRINTF("\r\n");
    }
  }
  PRINTF("\r\n");
}

// Imprimir texto en bloques
void print_text_blocks(char *text, int len) {
  for (int i = 0; i < len; i += BLOCK_SIZE) {
    for (int j = 0; j < BLOCK_SIZE && (i + j) < len; j++) {
      PUTCHAR(text[i + j]);
    }
    PRINTF("\r\n");
  }
}

// Declarar funciones reverse externas
extern unsigned int reverse1(unsigned int x);
extern unsigned int reverse2(unsigned int x);
extern unsigned int reverse3(unsigned int x);
extern unsigned int reverse4(unsigned int x);
extern unsigned int reverse5(unsigned int x);
extern unsigned int reverse6(unsigned int x);

int main(void) {
  watchdog_disable();
  BOARD_InitPins();
  BOARD_BootClockRUN();
  BOARD_InitDebugConsole();
  timer_init();

  // Array de punteros a función
  unsigned int (*funciones_reverse[])(unsigned int) = {
      reverse1, reverse2, reverse3, reverse4, reverse5, reverse6};

  // Array de nombres correspondientes
  const char *nombres_funciones[] = {"reverse1 (C)",
                                     "reverse2 (ASM)",
                                     "reverse3 (ASM)",
                                     "reverse4 (C)",
                                     "reverse5 (C) (1 optimized)",
                                     "reverse6 (C) (4 optimized)"};

  int num_funciones = sizeof(funciones_reverse) / sizeof(funciones_reverse[0]);

  PRINTF("=== Benchmark de funciones reverse ===\r\n");
  PRINTF("Introduce texto (máx %d caracteres):\r\n\r\n", MAX_BUFFER - 1);

  char buffer[MAX_BUFFER];
  int longitud = 0;

  while (1) {
    char tecla = GETCHAR();
    PUTCHAR(tecla); // Echo

    // Enter presionado
    if (tecla == '\r' || tecla == '\n') {
      buffer[longitud] = '\0';

      // Calcular longitud con padding a múltiplo de 4
      int longitud_total =
          ((longitud + BLOCK_SIZE - 1) / BLOCK_SIZE) * BLOCK_SIZE;

      // Buffer con padding de ceros
      uint8_t datos[MAX_BUFFER] = {0};
      memcpy(datos, buffer, longitud);

      // Mostrar resultados
      PRINTF("\r\n\n--- Texto recibido ---\r\n%s\r\n\r\n", buffer);

      PRINTF("--- Bloques de entrada ---\r\n");
      print_text_blocks(buffer, longitud);

      PRINTF("\r\n--- Bits originales ---\r\n");
      print_bits(datos, longitud_total);

      // Probar cada función reverse usando punteros
      for (int f = 0; f < num_funciones; f++) {
        uint8_t resultado[MAX_BUFFER];
        memcpy(resultado, datos, longitud_total);

        // Medir tiempo
        uint16_t inicio = (uint16_t)timestamp();

        // Aplicar reverse a cada bloque de 32 bits
        for (int i = 0; i < longitud_total; i += BLOCK_SIZE) {
          uint32_t bloque;
          memcpy(&bloque, &resultado[i], BLOCK_SIZE);

          // Llamar a la función usando el puntero
          bloque = funciones_reverse[f](bloque);

          memcpy(&resultado[i], &bloque, BLOCK_SIZE);
        }

        uint16_t fin = (uint16_t)timestamp();
        uint16_t tiempo = fin - inicio;

        // Mostrar resultado
        PRINTF("\r\n--- %s ---\r\n", nombres_funciones[f]);
        print_bits(resultado, longitud_total);
        PRINTF("Tiempo: %u ticks\r\n", tiempo);
      }

      PRINTF("\r\n\n");
      longitud = 0; // Reiniciar para nueva entrada
    }
    // Caracter válido
    else if (tecla >= 32 && tecla < 127 && longitud < MAX_BUFFER - 1) {
      buffer[longitud++] = tecla;
    }
  }
}
