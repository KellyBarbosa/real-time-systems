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
}

void computeMagnitude()
{
  freq = FFT.MajorPeak(vReal, SAMPLES, FREQUENCY);
}

void applyMovingAverage()
{
  filteredValue += freq;
  filteredValue.process();
}

void findTheMax()
{
  filteredFreq = filteredValue.mean;
}

void display()
{
  display7seg.println(filteredFreq);
  display7seg.writeDisplay();
}