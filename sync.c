#include "sync.h"
#include "kernel.h"
#include "scheduler.h"

// Fila de aptos
extern ready_queue_t r_queue;

void sem_init(sem_t *sem, uint8_t valor) {
    sem->contador = valor;
    sem->pos_input = 0;
    sem->pos_output = 0;
}

void sem_wait(sem_t *sem) {
    DISABLE_ALL_INTERRUPTS();

    sem->contador--;
    if (sem->contador < 0) {
        sem->fila[sem->pos_input] = r_queue.pos_task_running;
        sem->pos_input = (sem->pos_input + 1) % MAX_USER_TASKS;
        // Troca de contexto
        SAVE_CONTEXT(WAITING_SEM);
        scheduler();
        RESTORE_CONTEXT();
    }

    ENABLE_ALL_INTERRUPTS();
}

void sem_post(sem_t *sem) {
    DISABLE_ALL_INTERRUPTS();

    sem->contador++;
    if (sem->contador <= 0) {
        // Libera o processo bloqueado a mais tempo
        r_queue.TASKS[sem->fila[sem->pos_output]].task_state = READY;
        sem->pos_output = (sem->pos_output + 1) % MAX_USER_TASKS;
    }

    ENABLE_ALL_INTERRUPTS();
}

void mutex_init(mutex_t *m) {
    m->locked = 0;
    m->owner_id = 0;
    m->pos_input = 0;
    m->pos_output = 0;
    m->waiting_count = 0;
}

void mutex_lock(mutex_t *m) {
    DISABLE_ALL_INTERRUPTS();
    //mutex esta ocupado?
    if (m->locked) {//Alguem esta usando o recurso
        m->fila[m->pos_input] = r_queue.pos_task_running; //id da task atual esta na fila
        m->pos_input = (m->pos_input + 1) % MAX_USER_TASKS;
        m->waiting_count++; //numero de task esperando recurso

        SAVE_CONTEXT(WAITING);
        scheduler();
        RESTORE_CONTEXT();
    } else {
        m->locked = 1;
        m->owner_id = r_queue.task_running->task_id;
    }
    ENABLE_ALL_INTERRUPTS();
}

void mutex_unlock(mutex_t *m) {
    DISABLE_ALL_INTERRUPTS();
    if(m->locked && m->owner_id == r_queue.task_running->task_id)
    {
        if(m->waiting_count>0){
            uint8_t task_idx =m->fila[m->pos_output];
            r_queue.TASKS[task_idx].task_state = READY;
            m->pos_output = (m->pos_output+1)%MAX_USER_TASKS;
            m->waiting_count--;
            m->owner_id = r_queue.TASKS[task_idx].task_id;
        }
        else{
            m->locked=0;
            m->waiting_count=0;
        }
    }
    ENABLE_ALL_INTERRUPTS();
}
