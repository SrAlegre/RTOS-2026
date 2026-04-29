#include "hw.h"
#include "kernel.h"
#include "scheduler.h"
#include "os_config.h"
#include "sync.h"

// Quantum do algoritmo Round-Robin
uint8_t rr_quantum = QUANTUM;


void setup_hardware(void)
{
    // Configuração do timer
    INTCONbits.TMR0IE   = 1;
    INTCONbits.TMR0IF   = 0;
    T0CONbits.T08BIT    = 1; // 8 bits
    T0CONbits.T0CS      = 0; // Instrução interna
    T0CONbits.PSA       = 0; // Ativa preescaler
    T0CONbits.T0PS      = 0b111; // 1:256
    T0CONbits.TMR0ON    = 1; // Ativa timer
    TMR0                = 0;
}

// Precisa importar o ponteiro do semáforo do io.c
extern sem_t* ext_int0_sem;

void __interrupt() ISR(void)
{
    if (INTCONbits.TMR0IF) {
        INTCONbits.TMR0IF = 0;
        
        // Verifica se tem tarefa com delay > 0
        for (int i = 0; i < r_queue.size; i++) {
            if (r_queue.TASKS[i].task_delay > 0) {
                r_queue.TASKS[i].task_delay--;
                if (r_queue.TASKS[i].task_delay == 0) {
                    r_queue.TASKS[i].task_state = READY;
                }                
            }
        }
        
        // NOVA LÓGICA: Tratamento da Interrupção Externa 0
        if (INTCONbits.INT0IE && INTCONbits.INT0IF) {
            INTCONbits.INT0IF = 0; // Limpa a flag imediatamente
        
            // Se houver um semáforo vinculado, libera ele
            if (ext_int0_sem != NULL) {
                sem_post(ext_int0_sem);
            }
        }
        
        // Verifica o quantum para saber se há necessidade de 
        // mudar a tarefa que está em execução.
        rr_quantum--;
        if (rr_quantum == 0) {
            rr_quantum = QUANTUM;
            SAVE_CONTEXT(READY);
            scheduler();
            RESTORE_CONTEXT();
        }
    }
}
