#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"

#define N_LEDS 3
#define N_BTN 2
#define PURPLE GPIO_NUM_5
#define GREEN GPIO_NUM_4
#define BLUE  GPIO_NUM_2
#define BTN1 GPIO_NUM_14
#define BTN2 GPIO_NUM_12

gpio_num_t leds[N_LEDS] = {PURPLE, GREEN, BLUE};
gpio_num_t btn[N_BTN] = {BTN1, BTN2};
TaskHandle_t t_handle[N_BTN] = {NULL, NULL};
bool isPaused[N_BTN] = {false, false};

void purpleLED(void *pvParameter) {
  while (true) {
    gpio_set_level(leds[0], 1);
    vTaskDelay(pdMS_TO_TICKS(1000));
    gpio_set_level(leds[0], 0);
    vTaskDelay(pdMS_TO_TICKS(1000));

    if (!gpio_get_level(btn[0]) && isPaused[0]) {
      vTaskResume(t_handle[0]);
      isPaused[0] = !isPaused[0];
    } else if (!gpio_get_level(btn[0]) && !isPaused[0]) {
      vTaskSuspend(t_handle[0]);
      isPaused[0] = !isPaused[0];
    }
  }
}

void greenLED(void *pvParameter) {
  while (true) {
    gpio_set_level(leds[1], 1);
    vTaskDelay(pdMS_TO_TICKS(500));
    gpio_set_level(leds[1], 0);
    vTaskDelay(pdMS_TO_TICKS(500));

    if (!gpio_get_level(btn[1]) && isPaused[1]) {
      vTaskResume(t_handle[1]);
      isPaused[1] = !isPaused[1];
    } else if (!gpio_get_level(btn[1]) && !isPaused[1]) {
      vTaskSuspend(t_handle[1]);
      isPaused[1] = !isPaused[1];
    }
  }
}

void blueLED(void *pvParameter) {
  while (true) {
    gpio_set_level(leds[2], 1);
    vTaskDelay(pdMS_TO_TICKS(250));
    gpio_set_level(leds[2], 0);
    vTaskDelay(pdMS_TO_TICKS(250));
  }
}

void createTasks() {
  xTaskCreate( &purpleLED, "purple_led", 2048, NULL, 5, NULL);
  xTaskCreate( &greenLED, "green_led", 2048, NULL, 5, &t_handle[0]);
  xTaskCreate( &blueLED, "blue_led", 2048, NULL, 5, &t_handle[1]);
}

void confLED() {
  for (int i = 0; i < N_LEDS; i++) {
    gpio_reset_pin(leds[i]);
    gpio_set_direction(leds[i], GPIO_MODE_OUTPUT);
  }
}

void confBTN() {
  for (int i = 0; i < N_BTN; i++) {
    gpio_reset_pin(btn[i]);
    gpio_set_direction(btn[i], GPIO_MODE_INPUT);
  }
}

extern "C" void app_main()
{
  confLED();
  confBTN();
  createTasks();
}