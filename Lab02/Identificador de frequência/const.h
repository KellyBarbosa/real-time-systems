// Pinos do microfone
#define I2S_WS 25
#define I2S_SD 33
#define I2S_SCK 32
#define I2S_PORT I2S_NUM_0

// Informações para leitura de dados do microfone, cálculo da FFT, aplicação do filtro de média móvel, tarefas e interrupção
#define SAMPLES 1024
#define FREQUENCY 16000
#define FILTER_SIZE 100
#define MAX_TASKS 10
#define TIME_MS 10

Adafruit_7segment display7seg = Adafruit_7segment();
arduinoFFT FFT = arduinoFFT();
GaussianAverage filteredValue(FILTER_SIZE);
hw_timer_t *timer = NULL;

int16_t sBuffer[SAMPLES];
double vReal[SAMPLES], vImag[SAMPLES], freq, filteredFreq;
int minIndex, taskCounter = 0;
unsigned long minDeadline;

// Estrutura de uma tarefa
typedef struct
{
  void (*function)();     // Ponteiro para a função que representa a tarefa
  unsigned long deadline; // Prazo final da tarefa, em milissegundos
  unsigned long period;   // Período de repetição da tarefa, em milissegundos
  int taskDependent;      // Índice da tarefa que deve preceder a tarefa atual pelo menos uma vez
  bool alreadyExecuted;   // Indica se a tarefa atual já foi executada alguma vez
} Task;

Task tasks[MAX_TASKS];