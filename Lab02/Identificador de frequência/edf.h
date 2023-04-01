// Função para adicionar as tarefas
void addTask(void (*function)(), unsigned long period, unsigned long deadline, int taskDependent)
{
  if (taskCounter <= MAX_TASKS)
  {
    tasks[taskCounter].function = function;
    tasks[taskCounter].deadline = deadline;
    tasks[taskCounter].period = period;
    tasks[taskCounter].taskDependent = taskDependent;
    tasks[taskCounter].alreadyExecuted = false;
    taskCounter++;
  }
}

// Função para calcular o deadline
void calculateDeadline()
{
  minIndex = 0;
  minDeadline = tasks[0].deadline;
  for (int i = 0; i < taskCounter; i++)
  {
    if (tasks[i].deadline < minDeadline)
    {
      minDeadline = tasks[i].deadline;
      minIndex = i;
    }
  }
}

// Função para executar as tarefas
void runTasks()
{
  // Executa a tarefa com o prazo final mais próximo
  if (millis() >= tasks[minIndex].deadline && (tasks[minIndex].taskDependent != -1 ? tasks[tasks[minIndex].taskDependent].alreadyExecuted : true))
  {
    tasks[minIndex].function(); // Executa a tarefa
    tasks[minIndex].alreadyExecuted = true;
    tasks[minIndex].deadline += tasks[minIndex].period; // Atualiza o deadline da tarefa
  }
}