#include "scheduler.h"
#include "types.h"
#include "kernel.h"
#include "os_config.h"

// Fila de aptos
extern ready_queue_t r_queue;

void scheduler() {
#if DEFAULT_SCHEDULER == RR_SCHEDULER  
    r_queue.pos_task_running = RR_scheduler();
#elif DEFAULT_SCHEDULER == PRIOR_SCHEDULER
    r_queue.pos_task_running = priority_scheduler();
#elif DEFAULT_SCHEDULER == RR_PRIOR_SCHEDULER
    r_queue.pos_task_running = priority_rr_scheduler();
#endif  
//    r_queue.task_running = &r_queue.TASKS[r_queue.pos_task_running];
}

uint8_t RR_scheduler() {
    static uint8_t prox;
    static uint8_t tentativas;
    
    prox = r_queue.pos_task_running;
    tentativas = 0;

    do {
        //prox = (prox + 1) % r_queue.size; //função ___awmod usada varias veses % e ocupa muita ram
        //Depois de trocado o uso, economia de 7% de RAM
        prox++;
        if (prox >= r_queue.size)
            prox = 0;
        
        tentativas++;
        if (tentativas >= (MAX_USER_TASKS + 1)) return 0;
    } while (GET_STATE(r_queue.TASKS[prox]) != READY ||
            r_queue.TASKS[prox].task_ptr == idle);

    return prox;
}

uint8_t priority_scheduler(void) {
    static uint8_t i;
    static uint8_t prox;
    static uint8_t current_task;
    
    prox = r_queue.pos_task_running;

    while (GET_STATE(r_queue.TASKS[prox]) != READY)
        //prox = (prox + 1) % r_queue.size;
        prox++;

        if (prox >= r_queue.size)
            prox = 0;

    current_task = GET_PRIO(r_queue.TASKS[prox]);

    for (i = 1; i < r_queue.size; i++) {
        if (GET_STATE(r_queue.TASKS[i]) == READY &&
                GET_PRIO(r_queue.TASKS[i]) > current_task) {
            prox = i;
            current_task = GET_PRIO(r_queue.TASKS[i]);
        }
    }

    return prox;
}

uint8_t priority_rr_scheduler(void) {
    static uint8_t max_prio;
    static uint8_t found;
    static uint8_t i;
    static uint8_t current;
    static uint8_t idx;
    
    max_prio = 0;
    found = 0;

    // Encontra a maior prioridade entre tarefas prontas
    for (i = 0; i < r_queue.size; i++) {
        if (GET_STATE(r_queue.TASKS[i]) == READY) {
            if (!found || GET_PRIO(r_queue.TASKS[i]) > max_prio) {
                max_prio = GET_PRIO(r_queue.TASKS[i]);
                found = 1;
            }
        }
    }
    if (!found)return 0; // se não achar entao vai mandar o idle
    current = r_queue.pos_task_running;

    for (i = 0; i < r_queue.size; i++) {
        //idx = (current + i) % r_queue.size;
        idx = current + i;
        if (idx >= r_queue.size)
            idx -= r_queue.size;
        if (GET_STATE(r_queue.TASKS[i]) == READY &&
            GET_PRIO(r_queue.TASKS[i]) == max_prio) {   
            return idx;
        }
    }
    return 0; // fallback
}