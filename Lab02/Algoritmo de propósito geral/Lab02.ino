#include "const.h"
#include "tasks.h"
#include "edf.h"

void setup()
{
  Serial.begin(115200);
  Serial.println("Sistemas de tempo real - Lab 02.");
  startTimer();

  addTask(task1, 1000, 1000, -1);
  addTask(task2, 500, 500, -1);
  addTask(task3, 2000, 2000, -1);
  addTask(task4, 250, 250, -1);
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