#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"

#define ESP_INTR_FLAG_DEFAULT 0

TaskHandle_t t_handle[2] = {NULL, NULL};

void IRAM_ATTR button_isr(void *arg)
{
  xTaskNotifyGive(t_handle[0]);
  xTaskNotifyGive(t_handle[1]);
}

void taskLED(void *pvParameter)
{
  while (true)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    if (gpio_get_level(GPIO_NUM_5))
    {
      gpio_set_level(GPIO_NUM_5, 0);
    }
    else
    {
      gpio_set_level(GPIO_NUM_5, 1);
    }
  }
}

void taskMensagem(void *pvParameter)
{
  while (true)
  {
    ulTaskNotifyTake(pdTRUE, portMAX_DELAY);
    printf("O botão foi clicado!\n");
  }
}

void createTasks()
{
  xTaskCreate(&taskLED, "taskLED", 2048, NULL, 5, &t_handle[0]);
  xTaskCreate(&taskMensagem, "taskMensagem", 2048, NULL, 5, &t_handle[1]);
}

void confLED()
{
  gpio_reset_pin(GPIO_NUM_5);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
}

void confBTN()
{
  gpio_reset_pin(GPIO_NUM_12);
  gpio_set_direction(GPIO_NUM_12, GPIO_MODE_INPUT);
  gpio_set_intr_type(GPIO_NUM_12, GPIO_INTR_POSEDGE);
}

void confISR()
{
  gpio_install_isr_service(ESP_INTR_FLAG_DEFAULT);
  gpio_isr_handler_add(GPIO_NUM_12, button_isr, NULL);
}

extern "C" void app_main()
{
  confLED();
  confBTN();
  confISR();
  createTasks();
}