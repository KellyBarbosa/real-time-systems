#include <LinkedList.h>
#include <Gaussian.h>
#include <GaussianAverage.h>
#include "arduinoFFT.h"
#include "Adafruit_LEDBackpack.h"
#include "i2s.h"

// Pinos do microfone
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

// Informações para leitura de dados do microfone, cálculo da FFT e aplicação do filtro de média móvel
#define SAMPLES 1024
#define FREQUENCY 16000
#define FILTER_SIZE 10 // Quanto maior o número, melhor a resolução, porém maior será o tempo necessário para o sistema estabilizar

Adafruit_7segment display_7seg = Adafruit_7segment();
arduinoFFT FFT = arduinoFFT();
GaussianAverage filtered_value(FILTER_SIZE);
hw_timer_t *timer = NULL;

int16_t sBuffer[SAMPLES];
double vReal[SAMPLES], vImag[SAMPLES], freq, filtered_freq;
bool changeCycle, cycle1, cycle2;

void setup()
{
  Serial.begin(115200);
  Serial.println("Sistemas de tempo real - Lab 01.");
  display_7seg.begin(0x70);
  i2s_configure(I2S_WS, I2S_SD, I2S_SCK, I2S_PORT, SAMPLES, FREQUENCY);
  startTimer();
  cycle1 = true;
  cycle2 = false;
  changeCycle = false;
}

void loop()
{
  while (true)
  {
    if (!changeCycle && cycle1)
    {
      task1();
      task2();
      task3();
      task4();
      task1();
      cycle1 = false;
      cycle2 = true;
    }
    if (changeCycle && cycle2)
    {
      task1();
      task2();
      task3();
      for (int i = 0; i < 3; i++)
      {
        task1();
      }
      cycle1 = true;
      cycle2 = false;
    }
  }
}

void task1()
{
  fillBuffer();
}
void task2()
{
  computeFFT();
}
void task3()
{
  filtered_freq = filter(freq);
}
void task4()
{
  display(filtered_freq);
}

void fillBuffer()
{
  size_t bytesIn;
  i2s_read(I2S_PORT, &sBuffer, SAMPLES, &bytesIn, portMAX_DELAY);
}

void computeFFT()
{
  for (uint16_t i = 0; i < SAMPLES; i++)
  {
    vReal[i] = sBuffer[i];
    vImag[i] = 0.0;
  }

  FFT.Windowing(vReal, SAMPLES, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(vReal, vImag, SAMPLES, FFT_FORWARD);
  FFT.ComplexToMagnitude(vReal, vImag, SAMPLES);
  freq = FFT.MajorPeak(vReal, SAMPLES, FREQUENCY);
}

double filter(double value)
{
  filtered_value += value;
  filtered_value.process();
  return filtered_value.mean;
}

void display(double value)
{
  display_7seg.println(value);
  display_7seg.writeDisplay();
}

void flowControl()
{
  changeCycle = !changeCycle;
}

void startTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &flowControl, true);
  timerAlarmWrite(timer, 40000, true);
  timerAlarmEnable(timer);
}