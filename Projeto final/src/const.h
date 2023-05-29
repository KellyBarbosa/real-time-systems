#define FREQUENCY (16000)
#define SAMPLES (1024)
#define FILTER_SIZE 25

#define I2S_PORT I2S_NUM_0
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32

SemaphoreHandle_t microphoneMutex;
arduinoFFT FFT = arduinoFFT();
int16_t *audioBuffer = (int16_t *)malloc(SAMPLES);
int16_t microphoneData[SAMPLES];
size_t bytesRead = 0;

double vReal[SAMPLES], vImag[SAMPLES], filteredValue[FILTER_SIZE];
double freq, filteredFreq, sum;