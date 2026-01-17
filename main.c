#include "MKL46Z4.h"
#include "board.h"
#include "clock_config.h"
#include "pin_mux.h"

// Custom drivers
#include "drivers/drivers_led.h"
#include "fsl_debug_console.h"
#include "fsl_i2c.h"
#include "lcd.h"
#include "mag3110.h"
#include "mma8451.h"

/*******************************************************************************
 * Definitions
 ******************************************************************************/
#define I2C_MASTER_CLK_SRC I2C0_CLK_SRC
#define I2C_MASTER_CLK_FREQ CLOCK_GetFreq(I2C0_CLK_SRC)
#define I2C_BAUDRATE 100000U

#define BOARD_ACCEL_I2C_BASE I2C0
#define BOARD_MAG_I2C_BASE I2C0

// Magnetometer Calibration Offsets - To be calculated at runtime
int32_t mag_x_offset = 0;
int32_t mag_y_offset = 0;

/*******************************************************************************
 * Prototypes
 ******************************************************************************/
void init_i2c_sensors(void);
static void delay(volatile uint32_t nof);
static int32_t atan2_approx(int32_t y, int32_t x);
static void watchdog_disable(void);
void calibrate_magnetometer(void);

/*******************************************************************************
 * Globals
 ******************************************************************************/
mag_data_t mag_data;
accel_data_t accel_data;

/*******************************************************************************
 * Code
 ******************************************************************************/

void calibrate_magnetometer(void) {
  mag_data_t current_mag_data;

  struct {
    int16_t min_x;
    int16_t max_x;
    int16_t min_y;
    int16_t max_y;
  } mag_calibration_data = {.min_x = INT16_MAX,
                            .max_x = INT16_MIN,
                            .min_y = INT16_MAX,
                            .max_y = INT16_MIN};

  PRINTF(
      "Magnetometer Calibration: Please rotate the board 360 degrees...\r\n");
  lcd_clear();

  // Calibration loop for ~50 seconds
  for (int i = 0; i < 200; i++) {
    MAG3110_ReadData(BOARD_MAG_I2C_BASE, &current_mag_data);

    if (current_mag_data.x < mag_calibration_data.min_x) {
      mag_calibration_data.min_x = current_mag_data.x;
    }
    if (current_mag_data.x > mag_calibration_data.max_x) {
      mag_calibration_data.max_x = current_mag_data.x;
    }
    if (current_mag_data.y < mag_calibration_data.min_y) {
      mag_calibration_data.min_y = current_mag_data.y;
    }
    if (current_mag_data.y > mag_calibration_data.max_y) {
      mag_calibration_data.max_y = current_mag_data.y;
    }

    lcd_clear();
    // lcd_display_dec(i*100/200);
    lcd_display_dec(i);

    delay(250000);
  }

  mag_x_offset = (mag_calibration_data.min_x + mag_calibration_data.max_x) / 2;
  mag_y_offset = (mag_calibration_data.min_y + mag_calibration_data.max_y) / 2;

  PRINTF("Calibration complete!\r\n");
  PRINTF("X_OFFSET: %d, Y_OFFSET: %d\r\n", mag_x_offset, mag_y_offset);
  lcd_clear();
  delay(500000);
}

// Returns heading in degrees from 0-359
static int32_t atan2_approx(int32_t y, int32_t x) {
  if (x == 0 && y == 0) {
    return 0;
  }

  int32_t angle;
  int32_t abs_y = y > 0 ? y : -y;
  int32_t abs_x = x > 0 ? x : -x;

  if (abs_x >= abs_y) {
    angle = (450 * abs_y) / abs_x;
  } else {
    angle = 900 - (450 * abs_x) / abs_y;
  }

  if (x < 0 && y >= 0) { // Quadrant II
    angle = 1800 - angle;
  } else if (x < 0 && y < 0) { // Quadrant III
    angle = 1800 + angle;
  } else if (x >= 0 && y < 0) { // Quadrant IV
    angle = 3600 - angle;
  }

  return angle / 10;
}

static void delay(volatile uint32_t nof) {
  while (nof--) {
    __NOP();
  }
}

static void watchdog_disable(void) {
  // Disables the Watchdog timer
  SIM->COPC = 0;
}

int main(void) {
  watchdog_disable();
  BOARD_InitBootPins();
  BOARD_InitBootClocks();
  BOARD_InitDebugConsole();

  PRINTF("Magnetometer PWM LED Control\r\n");

  init_i2c_sensors();
  LED_Init();
  lcd_ini();

  calibrate_magnetometer();

  while (1) {
    MAG3110_ReadData(BOARD_MAG_I2C_BASE, &mag_data);

    // Apply calibration and axis transformation
    int32_t board_x = mag_data.x - mag_x_offset;
    int32_t board_y = mag_data.y - mag_y_offset;
    // int32_t compass_x = -board_x;
    // int32_t compass_y = board_y;
    int32_t compass_x = board_x;
    int32_t compass_y = board_y;

    uint16_t heading = atan2_approx(compass_y, compass_x);
    // heading = 360 - heading; // Reverse direction
    // if (heading == 360)
    //   heading = 0; // Wrap 360 to 0

    float red_brightness = 0.0f;
    float green_brightness = 0.0f;

    if (heading <= 180) {
      red_brightness = heading / 180.0f;
      green_brightness = 1.0f - red_brightness;
    } else {
      green_brightness = (heading - 180.0f) / 180.0f;
      red_brightness = 1.0f - green_brightness;
    }

    LED_SetGreen(green_brightness);
    LED_SetRed(red_brightness);

    PRINTF("X: %d, Y: %d, H: %d, G: %d, R: %d\r\n", compass_x, compass_y,
           heading, (int)(green_brightness * 100), (int)(red_brightness * 100));

    lcd_clear();
    lcd_display_dec(heading);

    delay(250000);
  }
}

void init_i2c_sensors(void) {
  i2c_master_config_t masterConfig;
  I2C_MasterGetDefaultConfig(&masterConfig);
  masterConfig.baudRate_Bps = I2C_BAUDRATE;
  I2C_MasterInit(BOARD_ACCEL_I2C_BASE, &masterConfig, I2C_MASTER_CLK_FREQ);
  delay(10000);
  MAG3110_Init(BOARD_MAG_I2C_BASE);
  // MMA8451_Init(BOARD_ACCEL_I2C_BASE); // Missing!
}
