#include "user.h"
#include "kernel.h"
#include "sync.h"
#include "com.h"
#include "io.h"
#include <xc.h>

// Objetos de Sincronizacao e Comunicacao
pipe_t pipe_adc;
mutex_t mutex_recurso;
sem_t sem_processamento;

void config_user(void)
{
    // Configura Pinos
    TRISD = 0x00; // PORTD como saida (LEDs de status)
    
    // Inicializa Perifericos
    adc_config();
    pwm_init();
    
    // Inicializa Primitivas do SO
    pipe_init(&pipe_adc);
    mutex_init(&mutex_recurso);
    sem_init(&sem_processamento, 0);

    // Exporta simbolos para o assembly (necessario para o contexto do professor)
    asm("global _tarefaLeituraADC");
    asm("global _tarefaProcessamento");
    asm("global _tarefaControlePWM");
    asm("global _tarefaFeedbackLED");
    asm("global _tarefaOneShot");
}

// TAREFA 1: Le ADC e envia para o PIPE (Prioridade 5)
TASK tarefaLeituraADC(void) {
    uint16_t valor;
    while(1) {
        valor = adc_read(0); // Le canal AN0
        pipe_write(&pipe_adc, (uint8_t)(valor >> 2)); // Envia 8 bits significativos
        os_delay(10); // Espera 10 ticks
    }
}

// TAREFA 2: Processa dado do PIPE e libera semaforo (Prioridade 5 - Round Robin com T1)
TASK tarefaProcessamento(void) {
    uint8_t dado;
    while(1) {
        dado = pipe_read(&pipe_adc,&dado);
        // Processamento ficticio
        PORTD = dado; 
        sem_post(&sem_processamento); // Avisa que dado esta pronto para o PWM
    }
}

// TAREFA 3: Controle de PWM baseado no semaforo (Prioridade 7 - Maior)
TASK tarefaControlePWM(void) {
    while(1) {
        sem_wait(&sem_processamento);
        // Protege o acesso ao PWM com Mutex (exemplo de uso)
        mutex_lock(&mutex_recurso);
        pwm_set_duty(PORTD << 2); 
        mutex_unlock(&mutex_recurso);
    }
}

// TAREFA 4: Feedback Visual (Prioridade 3 - Menor)
TASK tarefaFeedbackLED(void) {
    while(1) {
        LATDbits.LATD7 = ~LATDbits.LATD7; // Pisca LED de heartbeat
        os_delay(50);
    }
}

// TAREFA 5: One-Shot (Disparada por Interrupcao Externa)
TASK tarefaOneShot(void) {
    // Executa uma acao unica e termina
    LATDbits.LATD6 = 1;
    os_delay(20);
    LATDbits.LATD6 = 0;
    
    // Como o SO do professor nao tem "exit_task", 
    // a tarefa one-shot deve se auto-suspender ou ficar em loop infinito
    // No hw.c, ela eh recriada a cada interrupcao.
    while(1) {
        os_task_change_state(WAITING, 0); // Suspende a si mesma
    }
}