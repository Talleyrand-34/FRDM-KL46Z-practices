#include "FreeRTOS.h"
#include "MKL46Z4.h"
#include "lcd.h"
#include "queue.h"
#include "semphr.h"
#include "task.h"

// Feliz Navidad profe
//  Ho ho ho

// Definiciones
#define MAX_THREADS 5
#define QUEUE_SIZE 20
#define PRODUCER_DELAY_MS 500
#define CONSUMER_DELAY_MS 700

// Variables globales
QueueHandle_t messageQueue;
SemaphoreHandle_t displayMutex;

volatile uint8_t numProductores = 0;
volatile uint8_t numConsumidores = 0;
volatile uint8_t itemsEnCola = 0;
volatile uint8_t botonPresionado = 0; // 0=ninguno, 1=izq, 2=der

TaskHandle_t productorHandles[MAX_THREADS] = {NULL};
TaskHandle_t consumidorHandles[MAX_THREADS] = {NULL};

// Prototipos
void irclk_ini(void);
void initButtons(void);
void updateDisplay(void);
void vProductorTask(void *pvParameters);
void vConsumidorTask(void *pvParameters);
void vBotonTask(void *pvParameters);
void adjustProducers(uint8_t newCount);
void adjustConsumers(uint8_t newCount);

// Función auxiliar para división por 10
static inline uint8_t div10(uint8_t val) {
  uint8_t q = (val >> 1) + (val >> 2);
  q = q + (q >> 4);
  q = q + (q >> 8);
  q = q >> 3;
  uint8_t r = val - ((q << 3) + (q << 1));
  return q + (r > 9);
}

static inline uint8_t mod10(uint8_t val) {
  uint8_t q = div10(val);
  return val - ((q << 3) + (q << 1));
}

// Enable IRCLK (Internal Reference Clock)
void irclk_ini(void) {
  MCG->C1 = MCG_C1_IRCLKEN(1) | MCG_C1_IREFSTEN(1);
  MCG->C2 = MCG_C2_IRCS(0); // 0 = 32KHZ internal reference clock
}

// Inicializar botones
void initButtons(void) {
  // Habilitar reloj para PORTC
  SIM->SCGC5 |= SIM_SCGC5_PORTC_MASK;

  // SW1 - PTC3 (botón izquierdo) - Interrupt on falling edge (0xA)
  PORTC->PCR[3] = PORT_PCR_MUX(1) |  // GPIO mode
                  PORT_PCR_PE_MASK | // Pull enable
                  PORT_PCR_PS_MASK | // Pull-up
                  (0xA << 16);       // Falling edge interrupt

  // SW3 - PTC12 (botón derecho) - Interrupt on falling edge (0xA)
  PORTC->PCR[12] = PORT_PCR_MUX(1) |  // GPIO mode
                   PORT_PCR_PE_MASK | // Pull enable
                   PORT_PCR_PS_MASK | // Pull-up
                   (0xA << 16);       // Falling edge interrupt

  // Configurar como entradas
  GPIOC->PDDR &= ~((1 << 3) | (1 << 12));

  // Limpiar flags de interrupción
  PORTC->ISFR = 0xFFFFFFFF;

  // Habilitar interrupciones en NVIC para PORTC/PORTD (IRQ 31)
  NVIC_ClearPendingIRQ(31);
  NVIC_EnableIRQ(31);
  NVIC_SetPriority(31, 3);
}

// Actualizar display LCD
void updateDisplay(void) {
  if (xSemaphoreTake(displayMutex, pdMS_TO_TICKS(100)) == pdPASS) {
    // Formato del display: II:PC
    // II = Items en cola (2 dígitos a la izquierda)
    // PC = Productores y Consumidores (1 dígito cada uno a la derecha)

    // Mostrar items en cola (00-99) en los dos primeros dígitos
    // Mostrar productores y consumidores en los dos últimos dígitos
    uint8_t decenas_items = div10(itemsEnCola);
    uint8_t unidades_items = mod10(itemsEnCola);

    // Usar lcd_display_time que muestra formato XX:YY
    // XX = items en cola, YY = productores (decenas) y consumidores (unidades)
    uint8_t prod_cons = numProductores * 10 + numConsumidores;

    lcd_display_time(itemsEnCola, prod_cons);

    xSemaphoreGive(displayMutex);
  }
}

// Tarea Productor
void vProductorTask(void *pvParameters) {
  uint32_t dato = 0;
  uint8_t producerId = (uint8_t)((uint32_t)pvParameters);

  while (1) {
    // Simular obtención de dato (delay)
    vTaskDelay(pdMS_TO_TICKS(PRODUCER_DELAY_MS));

    // Generar dato arbitrario: ID del productor en bits altos + contador
    dato = (producerId << 24) | ((dato & 0x00FFFFFF) + 1);

    // Intentar enviar dato a la cola
    if (xQueueSend(messageQueue, &dato, 0) == pdPASS) {
      // Dato enviado exitosamente
      taskENTER_CRITICAL();
      if (itemsEnCola < 99)
        itemsEnCola++;
      taskEXIT_CRITICAL();
      updateDisplay();
    }
  }
}

// Tarea Consumidor
void vConsumidorTask(void *pvParameters) {
  uint32_t dato;

  (void)pvParameters;

  while (1) {
    // Esperar a recibir dato de la cola
    if (xQueueReceive(messageQueue, &dato, portMAX_DELAY) == pdPASS) {
      // Dato recibido
      taskENTER_CRITICAL();
      if (itemsEnCola > 0)
        itemsEnCola--;
      taskEXIT_CRITICAL();
      updateDisplay();

      // Simular procesamiento del dato (delay)
      vTaskDelay(pdMS_TO_TICKS(CONSUMER_DELAY_MS));
    }
  }
}

// Tarea para manejar botones
void vBotonTask(void *pvParameters) {
  (void)pvParameters;

  while (1) {
    vTaskDelay(pdMS_TO_TICKS(50));

    if (botonPresionado == 1) {
      // Botón izquierdo presionado: cambiar productores
      botonPresionado = 0;
      uint8_t newCount = numProductores + 1;
      if (newCount > 5)
        newCount = 0;
      adjustProducers(newCount);
    } else if (botonPresionado == 2) {
      // Botón derecho presionado: cambiar consumidores
      botonPresionado = 0;
      uint8_t newCount = numConsumidores + 1;
      if (newCount > 5)
        newCount = 0;
      adjustConsumers(newCount);
    }
  }
}

// Ajustar número de productores activos
void adjustProducers(uint8_t newCount) {
  uint8_t i;

  taskENTER_CRITICAL();

  // Suspender productores excedentes
  for (i = newCount; i < MAX_THREADS; i++) {
    if (productorHandles[i] != NULL) {
      vTaskSuspend(productorHandles[i]);
    }
  }

  // Crear o reanudar productores necesarios
  for (i = 0; i < newCount; i++) {
    if (productorHandles[i] == NULL) {
      // Crear nueva tarea de productor
      xTaskCreate(vProductorTask, "Prod", configMINIMAL_STACK_SIZE + 64,
                  (void *)((uint32_t)i), tskIDLE_PRIORITY + 1,
                  &productorHandles[i]);
    } else {
      // Reanudar tarea existente
      vTaskResume(productorHandles[i]);
    }
  }

  numProductores = newCount;
  taskEXIT_CRITICAL();
  updateDisplay();
}

// Ajustar número de consumidores activos
void adjustConsumers(uint8_t newCount) {
  uint8_t i;

  taskENTER_CRITICAL();

  // Suspender consumidores excedentes
  for (i = newCount; i < MAX_THREADS; i++) {
    if (consumidorHandles[i] != NULL) {
      vTaskSuspend(consumidorHandles[i]);
    }
  }

  // Crear o reanudar consumidores necesarios
  for (i = 0; i < newCount; i++) {
    if (consumidorHandles[i] == NULL) {
      // Crear nueva tarea de consumidor
      xTaskCreate(vConsumidorTask, "Cons", configMINIMAL_STACK_SIZE + 64, NULL,
                  tskIDLE_PRIORITY + 1, &consumidorHandles[i]);
    } else {
      // Reanudar tarea existente
      vTaskResume(consumidorHandles[i]);
    }
  }

  numConsumidores = newCount;
  taskEXIT_CRITICAL();
  updateDisplay();
}

void PORTDIntHandler(void) {
  BaseType_t xHigherPriorityTaskWoken = pdFALSE;
  uint32_t flags = PORTC->ISFR;

  // SW1 - PTC3
  if (flags & (1 << 3)) {
    botonPresionado = 1;
    PORTC->ISFR = (1 << 3);
  }

  // SW3 - PTC12
  if (flags & (1 << 12)) {
    botonPresionado = 2;
    PORTC->ISFR = (1 << 12);
  }

  portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
}

// Función main
int main(void) {
  // Deshabilitar watchdog
  SIM->COPC = 0;

  // Inicializar reloj interno para LCD
  irclk_ini();

  // Inicializar LCD
  lcd_ini();

  // Inicializar botones
  initButtons();

  // Crear objetos de sincronización de FreeRTOS
  messageQueue = xQueueCreate(QUEUE_SIZE, sizeof(uint32_t));
  displayMutex = xSemaphoreCreateMutex();

  if (messageQueue != NULL && displayMutex != NULL) {
    // Mostrar estado inicial: 00:00 (0 items, 0 productores, 0 consumidores)
    lcd_display_time(0, 0);

    // Crear tarea para manejar eventos de botones
    xTaskCreate(vBotonTask, "Btn", configMINIMAL_STACK_SIZE + 32, NULL,
                tskIDLE_PRIORITY + 2, NULL);

    // Iniciar el scheduler de FreeRTOS
    vTaskStartScheduler();
  }

  // No debería llegar aquí
  while (1) {
    __asm("NOP");
  }

  return 0;
}

// Hook de FreeRTOS: detecta desbordamiento de stack
void vApplicationStackOverflowHook(TaskHandle_t xTask, char *pcTaskName) {
  (void)xTask;
  (void)pcTaskName;
  while (1) {
    __asm("NOP");
  }
}

// Hook de FreeRTOS: detecta fallo en asignación de memoria
void vApplicationMallocFailedHook(void) {
  while (1) {
    __asm("NOP");
  }
}
