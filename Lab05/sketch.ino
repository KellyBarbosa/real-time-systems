#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"
#include "driver/adc.h"

#define TAM 100

QueueHandle_t xQueue;

void taskColeta(void *pvParameter) {
  float v;
  while (true) {
    v = (adc1_get_raw(ADC1_CHANNEL_5) / 4095.0 * 3.3);
    xQueueSend(xQueue, &v, pdMS_TO_TICKS(0));
    vTaskDelay(pdMS_TO_TICKS(100));
  }
}

void taskADC(void *pvParameter) {
  float v = 0.0, v_medio = 0.0, values[TAM];
  int count = 0;
  while (true) {
    if (xQueueReceive(xQueue, &v, portMAX_DELAY)) {
      values[count] = v;
      count++;
    } else {
      printf("Erro ao receber valor\n");
    }

    if (count == TAM) {
      for (int i = 0; i < TAM; i++) {
        v_medio += values[i];
      }
      printf("Voltagem: %.2fV\n", v_medio / TAM);
      count = 0;
      v_medio = 0.0;
    }
  }
}

extern "C" void app_main() {
  xQueue = xQueueCreate(TAM, sizeof(float));
  adc1_config_width(ADC_WIDTH_BIT_12);
  xTaskCreate(&taskColeta, "taskColeta", 4096, NULL, 5, NULL);
  xTaskCreate(&taskADC, "taskADC", 4096, NULL, 5, NULL);
}


