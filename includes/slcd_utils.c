#include "MKL46Z4.h"
#include "fsl_slcd.h"

// Mapeo de caracteres a segmentos para display de 7 segmentos
// Formato: bits HGFEDCBA donde cada bit controla un segmento
static const uint8_t segmentMap[10] = {
    0x3F, // 0: segmentos ABCDEF
    0x06, // 1: segmentos BC
    0x5B, // 2: segmentos ABDEG
    0x4F, // 3: segmentos ABCDG
    0x66, // 4: segmentos BCFG
    0x6D, // 5: segmentos ACDFG
    0x7D, // 6: segmentos ACDEFG
    0x07, // 7: segmentos ABC
    0x7F, // 8: segmentos ABCDEFG
    0x6F  // 9: segmentos ABCDFG
};

// Configuración de pines del LCD para FRDM-KL46Z
// COM0: LCD_P40 (backplane)
// COM1: LCD_P52 (backplane)
// COM2: LCD_P19 (backplane)
// COM3: LCD_P18 (backplane)

void SLCD_Init_Simple(void) {
  slcd_config_t slcdConfig;
  slcd_clock_config_t clkConfig;

  // Habilitar clock del LCD
  SIM->SCGC5 |= SIM_SCGC5_SLCD_MASK;

  // Configuración del reloj
  clkConfig.clkSource = kSLCD_AlternateClk1; // MCGIRCLK
  clkConfig.altClkDivider = kSLCD_AltClkDivFactor1;
  clkConfig.clkPrescaler = kSLCD_ClkPrescaler01;

  // Obtener configuración por defecto
  SLCD_GetDefaultConfig(&slcdConfig);

  // Personalizar configuración
  slcdConfig.clkConfig = &clkConfig;
  slcdConfig.loadAdjust = kSLCD_HighLoadOrSlowestClkSrc;
  slcdConfig.dutyCycle = kSLCD_1Div4DutyCycle;
  slcdConfig.powerSupply = kSLCD_InternalVll3UseChargePump;
  slcdConfig.voltageTrim = kSLCD_RegulatedVolatgeTrim08;

  // Habilitar todos los pines del LCD (0-63)
  slcdConfig.slcdLowPinEnabled = 0xFFFFFFFF;
  slcdConfig.slcdHighPinEnabled = 0xFFFFFFFF;

  // Configurar backplanes (COM0-COM3 en pines 40, 52, 19, 18)
  slcdConfig.backPlaneLowPin = (1U << 18) | (1U << 19);
  slcdConfig.backPlaneHighPin = (1U << (40 - 32)) | (1U << (52 - 32));

  slcdConfig.faultConfig = NULL;

  // Inicializar SLCD
  SLCD_Init(LCD, &slcdConfig);

  // Iniciar display
  SLCD_StartDisplay(LCD);
}

void SLCD_Print(const char *str) {
  uint8_t i;

  // Limpiar display primero
  for (i = 0; i < 64; i++) {
    LCD->WF8B[i] = 0;
  }

  // Para FRDM-KL46Z, el LCD tiene 4 dígitos
  // Digit 1: pines 37(COM0), 17(COM1), 7(COM2), 8(COM3)
  // Digit 2: pines 53(COM0), 38(COM1), 10(COM2), 11(COM3)
  // Digit 3: pines 10(COM0), 9(COM1), 53(COM2), 38(COM3) - diferente mapeo
  // Digit 4: pines 11(COM0), 8(COM1), 37(COM2), 7(COM3) - diferente mapeo

  // Mapeo simplificado para 4 dígitos
  const uint8_t digitPins[4][4] = {
      {37, 17, 7, 8},   // Dígito 1 (más a la izquierda)
      {53, 38, 10, 11}, // Dígito 2
      {10, 9, 53, 38},  // Dígito 3
      {11, 8, 37, 7}    // Dígito 4 (más a la derecha)
  };

  // Escribir hasta 4 caracteres
  for (i = 0; i < 4 && str[i] != '\0'; i++) {
    if (str[i] >= '0' && str[i] <= '9') {
      uint8_t digit = str[i] - '0';
      uint8_t segments = segmentMap[digit];
      uint8_t phase;

      // Mapear segmentos a fases (COM0-COM3)
      for (phase = 0; phase < 4; phase++) {
        uint8_t pin = digitPins[i][phase];
        if (segments & (1 << phase)) {
          LCD->WF8B[pin] |= (1 << phase);
        }
      }
    }
  }
}
