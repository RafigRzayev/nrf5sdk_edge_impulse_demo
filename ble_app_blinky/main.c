#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_log_default_backends.h"
#include "nrf_gpio.h"
#include "nrf_delay.h"
#include <stdlib.h>
#include <stdint.h>
#include "edge-impulse-sdk/dsp/numpy_types.h"
#include "edge-impulse-sdk/porting/ei_classifier_porting.h"
#include "edge-impulse-sdk/classifier/ei_classifier_types.h"

// Pins
#define BUTTON_1      13 
#define BUTTON_2      14 
#define BUTTON_3      15
#define BUTTON_4      16
#define LED           17
// Simulated sensor reading values
#define BUTTON_1_VAL  150
#define BUTTON_2_VAL  220
#define BUTTON_3_VAL  235
#define BUTTON_4_VAL  245

#define DELAY_MS      300

#define FEATURES_SIZE 1

static float features[FEATURES_SIZE];

int get_feature_data(size_t offset, size_t length, float *out_ptr) {
    memcpy(out_ptr, features + offset, length * sizeof(float));
    return 0;
}

EI_IMPULSE_ERROR run_classifier(signal_t *, ei_impulse_result_t *, bool);

void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);
  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

void led_init(void) {
  nrf_gpio_cfg_output(LED);
  nrf_gpio_pin_set(LED);
}

void button_init(void) {
  nrf_gpio_cfg_input(BUTTON_1, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(BUTTON_2, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(BUTTON_3, NRF_GPIO_PIN_PULLUP);
  nrf_gpio_cfg_input(BUTTON_4, NRF_GPIO_PIN_PULLUP);
}

void process_input(uint8_t val) {
  features[0] = val;
  signal_t signal;
  signal.total_length = FEATURES_SIZE;
  signal.get_data = &get_feature_data;
  ei_impulse_result_t result;
  EI_IMPULSE_ERROR res = run_classifier(&signal, &result, true);
  if (result.classification[0].value > 0.5) {
    nrf_gpio_pin_set(LED);
    NRF_LOG_INFO("Weather is not sunny\r\n");
  }
  else {
    nrf_gpio_pin_clear(LED);
    NRF_LOG_INFO("Weather is sunny\r\n");
  }
}

int main(void) {
  log_init();
  led_init();
  button_init();
  NRF_LOG_INFO("Example started.");
  while (true) {
    if (!nrf_gpio_pin_read(BUTTON_1)) {
      NRF_LOG_INFO("BUTTON 1 pressed, sensor reading %d \r\n", BUTTON_1_VAL);
      process_input(BUTTON_1_VAL);
      nrf_delay_ms(DELAY_MS);
    }
    if (!nrf_gpio_pin_read(BUTTON_2)) {
      NRF_LOG_INFO("BUTTON 2 pressed, sensor reading %d \r\n", BUTTON_2_VAL);
      process_input(BUTTON_2_VAL);
      nrf_delay_ms(DELAY_MS);
    }
    if (!nrf_gpio_pin_read(BUTTON_3)) {
      NRF_LOG_INFO("BUTTON 3 pressed, sensor reading %d \r\n", BUTTON_3_VAL);
      process_input(BUTTON_3_VAL);
      nrf_delay_ms(DELAY_MS);
    }
    if (!nrf_gpio_pin_read(BUTTON_4)) {
      NRF_LOG_INFO("BUTTON 4 pressed, sensor reading %d \r\n", BUTTON_4_VAL);
      process_input(BUTTON_4_VAL);
      nrf_delay_ms(DELAY_MS);
    }
  }
}

