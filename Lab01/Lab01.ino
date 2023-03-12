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

int16_t sBuffer[SAMPLES];
double vReal[SAMPLES];
double vImag[SAMPLES];
double freq;
double filtered_freq;

// Executivo cíclico
#define INTERVAL1 50   // Ler dados do microfone a cada 0,05s
#define INTERVAL2 100  // Calcular a FFT a cada 0,1s
#define INTERVAL3 500  // Aplicar o filtro de média móvel a cada 0,5s
#define INTERVAL4 1000 // Exibir no display a cada 1s

unsigned long limitTask1 = INTERVAL1;
unsigned long limitTask2 = INTERVAL2;
unsigned long limitTask3 = INTERVAL3;
unsigned long limitTask4 = INTERVAL4;

unsigned long timeTask1;
unsigned long timeTask2;
unsigned long timeTask3;
unsigned long timeTask4;

void setup()
{
  Serial.begin(115200);
  display_7seg.begin(0x70);
  Serial.println("Sistemas de tempo real - Lab 01");

  delay(1000);
  i2s_configure(I2S_WS, I2S_SD, I2S_SCK, I2S_PORT, SAMPLES, FREQUENCY);

  timeTask1 = limitTask1;
  timeTask2 = limitTask2;
  timeTask3 = limitTask3;
  timeTask4 = limitTask4;
  delay(500);
}

void loop()
{
  task1();
  task2();
  task3();
  task4();
}

void task1()
{
  unsigned long diffTask1 = millis() - timeTask1;
  if (diffTask1 >= limitTask1)
  {
    timeTask1 = millis();
    fillBuffer();
  }
}
void task2()
{
  unsigned long diffTask2 = millis() - timeTask2;
  if (diffTask2 >= limitTask2)
  {
    timeTask2 = millis();
    computeFFT();
  }
}
void task3()
{
  unsigned long diffTask3 = millis() - timeTask3;
  if (diffTask3 >= limitTask3)
  {
    timeTask3 = millis();

    filtered_freq = filter(freq);
    Serial.print("Filtered value: ");
    Serial.println(filtered_freq);
  }
}
void task4()
{
  unsigned long diffTask4 = millis() - timeTask4;
  if (diffTask4 >= limitTask4)
  {
    timeTask4 = millis();
    display(filtered_freq);
  }
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

  Serial.print("F = ");
  Serial.print(freq, 2);
  Serial.println("Hz");
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