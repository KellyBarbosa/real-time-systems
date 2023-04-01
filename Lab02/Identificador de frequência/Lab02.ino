#include "libs.h"
#include "i2s.h"
#include "const.h"
#include "tasks.h"
#include "edf.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Sistemas de tempo real - Lab 02.");
  display7seg.begin(0x70);
  i2s_configure(I2S_WS, I2S_SD, I2S_SCK, I2S_PORT, SAMPLES, FREQUENCY);
  startTimer();

  addTask(fillBuffer, 32, 32, -1);
  addTask(computeFFT, 64, 64, 0);
  addTask(computeMagnitude, 64, 64, 1);
  addTask(applyMovingAverage, 64, 64, 2);
  addTask(findTheMax, 256, 256, 3);
  addTask(display, 256, 256, 4);
}

void startTimer()
{
  timer = timerBegin(0, 80, true);
  timerAttachInterrupt(timer, &calculateDeadline, true);
  timerAlarmWrite(timer, TIME_MS * 1000, true);
  timerAlarmEnable(timer);
}

void loop()
{
  while (true)
  {
    runTasks();
  }
}