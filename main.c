/*
 RTOS - Turma 2026/1
 */

#include "kernel.h"
#include "user.h"

int main() {
    os_config();
    
    os_create_task(2, tarefaLeituraADC, 1);
    os_create_task(3, tarefaProcessamento, 2);
    os_create_task(4, tarefaControlePWM, 3);
    os_create_task(5, tarefaFeedbackLED, 4);
    
    os_start();

    while (1) {

    }

    return 0;
}
