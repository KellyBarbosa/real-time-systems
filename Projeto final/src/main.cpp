#include "libs.h"
#include "const.h"

void i2sMicrophoneTask(void *pvParameters)
{
  while (true)
  {
    esp_err_t result = i2s_read(I2S_NUM_0, audioBuffer, SAMPLES, &bytesRead, portMAX_DELAY); // no timeout
    if (result == ESP_OK && bytesRead > 0)
    {
      xSemaphoreTake(microphoneMutex, portMAX_DELAY);
      memcpy(microphoneData, audioBuffer, SAMPLES);
      xSemaphoreGive(microphoneMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
  free(audioBuffer);
  vTaskDelete(NULL);
}

double filter(double value)
{
  for (int i = FILTER_SIZE - 1; i > 0; i--)
  {
    filteredValue[i] = filteredValue[i - 1];
  }
  filteredValue[0] = value;

  sum = 0.0;
  for (int i = 0; i < FILTER_SIZE; i++)
  {
    sum += filteredValue[i];
  }

  return sum / FILTER_SIZE;
}

double calculateBackgroundNoise()
{
  sum = 0.0;
  for (int i = 0; i < SAMPLES; i++)
  {
    sum += (microphoneData[i] * microphoneData[i]);
  }
  return sum / SAMPLES;
}

void computeFFT(void *pvParameters)
{
  while (true)
  {
    if (xSemaphoreTake(microphoneMutex, portMAX_DELAY) == pdTRUE)
    {
      for (size_t i = 0; i < SAMPLES; i++)
      {
        vReal[i] = microphoneData[i];
        vImag[i] = 0.0;
      }
      FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
      FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
      FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
      freq = FFT.MajorPeak(vReal, SAMPLES, FREQUENCY);
      vTaskDelay(pdMS_TO_TICKS(100));
      filteredFreq = filter(freq);
      vTaskDelay(pdMS_TO_TICKS(300));
      printf("F= %.2f Hz\n", filteredFreq);
      xSemaphoreGive(microphoneMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskPotency(void *pvParameters)
{
  while (true)
  {
    if (xSemaphoreTake(microphoneMutex, portMAX_DELAY) == pdTRUE)
    {
      if (((microphoneData[0] * microphoneData[0]) - (calculateBackgroundNoise())) > 100)
      {
        printf("Potência acima do ruído de fundo detectada!\n");
        gpio_set_level(GPIO_NUM_4, 1);
      }
      else
      {
        gpio_set_level(GPIO_NUM_4, 0);
      }
      xSemaphoreGive(microphoneMutex);
    }
    vTaskDelay(pdMS_TO_TICKS(10));
  }
}

void taskVAD(void *pvParameters)
{
  while (true)
  {
    printf("Algoritmo VAD\n");
  }
}

void init_microfone()
{
  i2s_config_t i2s_config = {
      .mode = (i2s_mode_t)(I2S_MODE_MASTER | I2S_MODE_RX),
      .sample_rate = FREQUENCY,
      .bits_per_sample = I2S_BITS_PER_SAMPLE_16BIT,
      .channel_format = I2S_CHANNEL_FMT_ONLY_LEFT,
      .communication_format = I2S_COMM_FORMAT_I2S_MSB,
      .intr_alloc_flags = 0,
      .dma_buf_count = 8,
      .dma_buf_len = SAMPLES,
      .use_apll = false,
  };
  i2s_pin_config_t pin_config = {
      .bck_io_num = I2S_SCK,
      .ws_io_num = I2S_WS,
      .data_out_num = -1,
      .data_in_num = I2S_SD};
  i2s_driver_install(I2S_PORT, &i2s_config, 0, NULL);
  i2s_set_pin(I2S_PORT, &pin_config);
  i2s_zero_dma_buffer(I2S_PORT);
}

void confLED()
{
  gpio_reset_pin(GPIO_NUM_5);
  gpio_set_direction(GPIO_NUM_5, GPIO_MODE_OUTPUT);
  gpio_reset_pin(GPIO_NUM_4);
  gpio_set_direction(GPIO_NUM_4, GPIO_MODE_OUTPUT);
}

void createTasks()
{
  xTaskCreatePinnedToCore(i2sMicrophoneTask, "i2sMicrophoneTask", 2048, NULL, 1, NULL, 0);
  xTaskCreate(computeFFT, "computeFFT", 2048, NULL, 1, NULL);
  xTaskCreate(taskPotency, "taskPotency", 2048, NULL, 1, NULL);
  // xTaskCreate(taskVAD, "taskVAD", 2048, NULL, 1, NULL);
}

extern "C" void app_main()
{
  microphoneMutex = xSemaphoreCreateMutex();
  init_microfone();
  createTasks();
  confLED();
}