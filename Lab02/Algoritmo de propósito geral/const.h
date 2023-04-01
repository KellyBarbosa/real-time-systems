#define MAX_TASKS 10
#define TIME_MS 10

hw_timer_t *timer = NULL;
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