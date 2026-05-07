#ifndef KERNEL_H
#define	KERNEL_H

#include "types.h"
#include <xc.h>

// Fila de aptos
extern ready_queue_t r_queue;

// Quantum do algoritmo Round-Robin
extern uint8_t rr_quantum;


// Chamadas de sistema
void os_delay(uint8_t time);
void os_create_task(uint8_t id, f_ptr func, uint8_t prior);
void os_task_exit();
void os_yield(void);
void os_config(void);
void os_start(void);
void os_task_change_state(state_t new_state, tcb_t *task_handle);

TASK idle();

#define DISABLE_ALL_INTERRUPTS() INTCONbits.GIE = 0;
#define ENABLE_ALL_INTERRUPTS() INTCONbits.GIE = 1;

#define CURRENT_TASK \
   r_queue.TASKS[r_queue.pos_task_running]

#define SAVE_CONTEXT(state) { \
    if (GET_STATE(CURRENT_TASK) == RUNNING) { \
        SET_STATE(CURRENT_TASK, state); \
        CURRENT_TASK.BSR_REG = BSR; \
        CURRENT_TASK.W_REG = WREG; \
        CURRENT_TASK.STATUS_REG = STATUS; \
        CURRENT_TASK.FSR0H_REG = FSR0H; \
        CURRENT_TASK.FSR0L_REG = FSR0L; \
        CURRENT_TASK.task_stack.stack_size = 0; \
\
        while (STKPTR > 0) { \
            CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSL_REG = TOSL; \
            CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSH_REG = TOSH; \
            CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSU_REG = TOSU; \
\
            CURRENT_TASK.task_stack.stack_size++; \
\
            asm("POP"); \
        } \
    } \
}
#define RESTORE_CONTEXT() { \
    if (GET_STATE(CURRENT_TASK) == READY) { \
        SET_STATE(CURRENT_TASK, RUNNING); \
\
        BSR = CURRENT_TASK.BSR_REG; \
        WREG = CURRENT_TASK.W_REG; \
        STATUS = CURRENT_TASK.STATUS_REG; \
        FSR0H = CURRENT_TASK.FSR0H_REG; \
        FSR0L = CURRENT_TASK.FSR0L_REG; \
\
        while (CURRENT_TASK.task_stack.stack_size > 0) { \
\
            CURRENT_TASK.task_stack.stack_size--; \
\
            TOSL = CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSL_REG; \
            TOSH = CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSH_REG; \
            TOSU = CURRENT_TASK.task_stack.stack[CURRENT_TASK.task_stack.stack_size].TOSU_REG; \
\
            asm("PUSH"); \
        } \
\
        asm("RETURN"); \
    } \
}
#endif	/* KERNEL_H */

