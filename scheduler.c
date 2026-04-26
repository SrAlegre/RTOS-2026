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
    r_queue.task_running = &r_queue.TASKS[r_queue.pos_task_running];
}

uint8_t RR_scheduler() {
    uint8_t prox = r_queue.pos_task_running, tentativas = 0;

    do {
        prox = (prox + 1) % r_queue.size;
        tentativas++;
        if (tentativas >= (MAX_USER_TASKS + 1)) return 0;
    } while (r_queue.TASKS[prox].task_state != READY ||
            r_queue.TASKS[prox].task_ptr == idle);

    return prox;
}

uint8_t priority_scheduler(void) {
    uint8_t prox = r_queue.pos_task_running;

    while (r_queue.TASKS[prox].task_state != READY)
        prox = (prox + 1) % r_queue.size;

    uint8_t current_task = r_queue.TASKS[prox].task_priority;

    for (uint8_t i = 1; i < r_queue.size; i++) {
        if (r_queue.TASKS[i].task_state == READY &&
                r_queue.TASKS[i].task_priority > current_task) {
            prox = i;
            current_task = r_queue.TASKS[i].task_priority;
        }
    }

    return prox;
}

uint8_t priority_rr_scheduler(void) {
    uint8_t max_prio = 0;
    uint8_t found = 0;

    // Encontra a maior prioridade entre tarefas prontas
    for (uint8_t i = 0; i < r_queue.size; i++) {
        if (r_queue.TASKS[i].task_state == READY) {
            if (!found || r_queue.TASKS[i].task_priority > max_prio) {
                max_prio = r_queue.TASKS[i].task_priority;
                found = 1;
            }
        }
    }
    if (!found)return 0; // se não achar entao vai mandar o idle
    uint8_t current = r_queue.pos_task_running;

    for (uint8_t i = 0; i < r_queue.size; i++) {
        uint8_t idx = (current + i) % r_queue.size;
        if (r_queue.TASKS[idx].task_state == READY &&
            r_queue.TASKS[idx].task_priority == max_prio) {
            return idx;
        }
    }
    return 0; // fallback
}