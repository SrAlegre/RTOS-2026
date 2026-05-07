#ifndef TYPES_H
#define	TYPES_H

#include <stdint.h>
#include "os_config.h"

typedef void TASK;

typedef enum {READY = 0,
              WAITING,
              RUNNING,
              WAITING_SEM
             } state_t;

// ===== MACROS =====

#define GET_STATE(t) \
    ((t).state_prio & 0x03)

#define SET_STATE(t, s) \
    ((t).state_prio = (uint8_t)(((t).state_prio & 0xFC) | (uint8_t)(s)))

#define GET_PRIO(t) \
    (((t).state_prio >> 2) & 0x07)

#define SET_PRIO(t, p) \
    ((t).state_prio = (uint8_t)(((t).state_prio & 0x03) | ((uint8_t)(p) << 2)))

#define GET_STATE_PTR(t) \
    ((t)->state_prio & 0x03)

#define SET_STATE_PTR(t, s) \
    ((t)->state_prio = (uint8_t)(((t)->state_prio & 0xFC) | (uint8_t)(s)))

#define GET_PRIO_PTR(t) \
    (((t)->state_prio >> 2) & 0x07)

#define SET_PRIO_PTR(t, p) \
    ((t)->state_prio = (uint8_t)(((t)->state_prio & 0x03) | ((uint8_t)(p) << 2)))
             
             
typedef void (*f_ptr)(void);

typedef struct hw_stack {
    uint8_t TOSL_REG;
    uint8_t TOSH_REG;
    uint8_t TOSU_REG;    
} hw_stack_t;

typedef struct sw_stack {
    hw_stack_t stack[MAX_STACK_SIZE];
    uint8_t stack_size;
} sw_stack_t;
             
typedef struct tcb {
    uint8_t task_id;
    f_ptr   task_ptr;
    uint8_t task_delay;
    uint8_t state_prio;

    // Registradores
    uint8_t W_REG;
    uint8_t STATUS_REG;
    uint8_t BSR_REG;
    uint8_t FSR0L_REG;
    uint8_t FSR0H_REG;
    
    // Pilha de hardware
    sw_stack_t task_stack;
} tcb_t;

// Fila de aptos
typedef struct ready_queue {
    tcb_t TASKS[MAX_USER_TASKS+1];
    uint8_t size;
    //tcb_t *task_running;
    uint8_t pos_task_running;
} ready_queue_t;

#endif	/* TYPES_H */

