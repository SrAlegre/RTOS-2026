/*
 RTOS - Turma 2026/1
 */

#include "kernel.h"
#include "user.h"

int main() {
    os_config();

    //os_create_task(2, acionaMotor, 5);
    //os_create_task(3, ligaLed, 5);
    //os_create_task(4, apagaLed, 5);
    //
        os_create_task(2, LED_1, 1);
        os_create_task(3, LED_2, 1);
        os_create_task(4, READ_ADC, 1);
        os_create_task(5, tarefaPWN, 1);

//    os_create_task(2, LED_1_prio, 1); // LED1 prioridade 1 (executa por último)
//    os_create_task(3, LED_2_prio, 3); // LED2 prioridade 3 (executa primeiro)
//    os_create_task(4, LED_3_prio, 2); // LED3 prioridade 2 (executa em segundo)


    os_start();

    while (1) {

    }

    return 0;
}
