#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "driver/adc.h"

SemaphoreHandle_t xSemaphore;

void taskBTN(void *pvParameter) {
  while (true) {
    if (!gpio_get_level(GPIO_NUM_27)) {
      xSemaphoreGive(xSemaphore);
    }
    vTaskDelay(pdMS_TO_TICKS(150));
  }
}

void taskADC(void *pvParameter) {
  while (true) {
    if (xSemaphoreTake(xSemaphore, portMAX_DELAY)) {
      printf("ADC: %d | DAC: %d | Voltagem: %.2fV\n", adc1_get_raw(ADC1_CHANNEL_5), adc1_get_raw(ADC1_CHANNEL_5) / 16, (adc1_get_raw(ADC1_CHANNEL_5) / 4095.0 * 3.3));
    }
  }
}

void confBTN() {
  gpio_reset_pin(GPIO_NUM_27);
  gpio_set_direction(GPIO_NUM_27, GPIO_MODE_INPUT);
  gpio_set_intr_type(GPIO_NUM_27, GPIO_INTR_POSEDGE);
}

extern "C" void app_main() {
  confBTN();
  xSemaphore = xSemaphoreCreateBinary();
  adc1_config_width(ADC_WIDTH_BIT_12);
  xTaskCreate(&taskBTN, "taskBTN", 2048, NULL, 5, NULL);
  xTaskCreate(&taskADC, "taskADC", 2048, NULL, 5, NULL);
}


