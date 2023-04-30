#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include "driver/gpio.h"

#define YELLOW GPIO_NUM_5
#define PURPLE GPIO_NUM_4
#define CIAN  GPIO_NUM_2
#define N_LEDS 3

typedef struct {
  gpio_num_t pin;
  int time;
} Task;

Task leds[N_LEDS] = {{YELLOW, 1000}, {PURPLE, 500}, {CIAN, 250}};

void runTasks(void *pvParameter) {
  gpio_num_t pin = ((Task *)pvParameter)->pin;
  int time = ((Task *)pvParameter)->time;;
  while (true) {
    gpio_set_level(pin, 1);
    vTaskDelay(pdMS_TO_TICKS(time));
    gpio_set_level(pin, 0);
    vTaskDelay(pdMS_TO_TICKS(time));
  }
}

void confLEDS() {
  for (int i = 0; i < N_LEDS; i++) {
    gpio_reset_pin(leds[i].pin);
    gpio_set_direction(leds[i].pin, GPIO_MODE_OUTPUT);
  }
}

void createTasks() {
  for (int i = 0; i < N_LEDS; i++) {
    xTaskCreate(
      runTasks, // task function
      "led_task" +i, // task name
      2048, // stack size in words
      &leds[i], // pointer to parameters
      5, // priority
      NULL); // out pointer to task handle
  }
}

extern "C" void app_main()
{
  confLEDS();
  createTasks();
}
