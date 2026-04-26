#ifndef SYNC_H
#define	SYNC_H

#include <stdint.h>
#include "os_config.h"

// Padrão POSIX

typedef struct sem {
    int contador;
    uint8_t fila[MAX_USER_TASKS];
    uint8_t pos_input;
    uint8_t pos_output;
} sem_t;

typedef struct mutex {
    uint8_t locked;
    uint8_t owner_id;
    uint8_t fila[MAX_USER_TASKS];
    uint8_t pos_input;
    uint8_t pos_output;
    uint8_t waiting_count;
} mutex_t;


void sem_init(sem_t *sem, uint8_t valor);
void sem_wait(sem_t *sem);
void sem_post(sem_t *sem);

void mutex_init(mutex_t *m);
void mutex_lock(mutex_t *m);
void mutex_unlock(mutex_t *m);



#endif	/* SYNC_H */

