#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "driver/gpio.h"
#include "driver/adc.h"

#define QUEUE_SIZE 10
#define CIAN GPIO_NUM_5
#define ADC_READING_INTERVAL_MS 3000
#define AVERAGE_INTERVAL_MS 15000
#define LIMIT_AVERAGE 2.3
#define READING BIT0
#define AVERAGE BIT1

QueueHandle_t xQueue;
SemaphoreHandle_t xSemaphore;
EventGroupHandle_t xCreatedEventGroup;
hw_timer_t *timer = NULL;
TickType_t last_reading_time, last_average_time;

void adcReadingTask(void *pvParameter)
{
  float value_read;
  while (true)
  {
    if (xEventGroupWaitBits(xCreatedEventGroup, READING, true, true, portMAX_DELAY))
    {
      value_read = (adc1_get_raw(ADC1_CHANNEL_5) / 4095.0 * 3.3);
      printf("%d° coleta - enviando %.2fV para fila\n", uxQueueMessagesWaiting(xQueue) + 1, value_read);
      xQueueSend(xQueue, &value_read, portMAX_DELAY);
      xSemaphoreGive(xSemaphore);
    }
  }
}

void averageTask(void *pvParameter)
{
  float value_received = 0.0, sum = 0.0, average = 0.0;
  int counter = 0;
  while (true)
  {
    if (xEventGroupWaitBits(xCreatedEventGroup, AVERAGE, true, true, portMAX_DELAY))
    {
      if (xSemaphoreTake(xSemaphore, portMAX_DELAY))
      {
        while (uxQueueMessagesWaiting(xQueue) != 0)
        {
          if (xQueueReceive(xQueue, &value_received, portMAX_DELAY))
          {
            sum += value_received;
            counter++;
            printf("Recebendo %.2fV da fila\n", value_received);
          }
        }

        average = sum / counter;
        printf("Média dos valores recebidos: %.2fV\n\n", average);
        if (average > LIMIT_AVERAGE)
        {
          gpio_set_level(CIAN, 1);
        }
        else
        {
          gpio_set_level(CIAN, 0);
        }

        sum = 0.0;
        counter = 0;
      }
    }
  }
}

void IRAM_ATTR taskManager()
{
  if (xTaskGetTickCount() - last_reading_time >= pdMS_TO_TICKS(ADC_READING_INTERVAL_MS))
  {
    last_reading_time = xTaskGetTickCount();
    xEventGroupSetBits(xCreatedEventGroup, READING);
  }
  if ((xTaskGetTickCount() - last_average_time >= pdMS_TO_TICKS(AVERAGE_INTERVAL_MS)) && uxQueueMessagesWaiting(xQueue) == 5)
  {
    last_average_time = xTaskGetTickCount();
    xEventGroupSetBits(xCreatedEventGroup, AVERAGE);
  }
}

void startTimerISR()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &taskManager, true);
  timerAlarmWrite(timer, 1000000, true);
  timerAlarmEnable(timer);
}

void firstExecution()
{
  last_reading_time = xTaskGetTickCount();
  xEventGroupSetBits(xCreatedEventGroup, READING);
  last_average_time = xTaskGetTickCount();
  xEventGroupSetBits(xCreatedEventGroup, AVERAGE);
}

extern "C" void app_main()
{

  xQueue = xQueueCreate(QUEUE_SIZE, sizeof(float));
  xSemaphore = xSemaphoreCreateBinary();
  xCreatedEventGroup = xEventGroupCreate();

  adc1_config_width(ADC_WIDTH_BIT_12);
  gpio_reset_pin(CIAN);
  gpio_set_direction(CIAN, GPIO_MODE_OUTPUT);

  xTaskCreate(&adcReadingTask, "adcReadingTask", 2048, NULL, 1, NULL);
  xTaskCreate(&averageTask, "averageTask", 2048, NULL, 1, NULL);

  firstExecution();

  startTimerISR();
}