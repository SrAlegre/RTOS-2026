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

volatile uint8_t dado_processado;

void config_user(void)
{
    // Configura Pinos
    TRISD = 0x00; // PORTD como saida (LEDs de status)
    ANSELBbits.ANSB0 = 0;
    TRISBbits.RB0 = 1; // Configura RB0 como entrada de interrup??o externa
    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 0;
    TRISDbits.RD0 = 0;
    TRISDbits.RD1 = 0;
    ANSELDbits.ANSD0 = 0;
    ANSELCbits.ANSC6 = 0;
    ANSELCbits.ANSC7 = 0;
    INTCONbits.INT0IE = 1; // Habilita INT0
    INTCON2bits.INTEDG0 = 0; // Interrupt on falling edge 
    INTCONbits.INT0IF = 0; // Flag interrupt
    
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
    while(1) {
        uint8_t valor = (uint8_t)(adc_read(0) >> 2); //Le canal AN0 e pega os 8bits +
        pipe_write(&pipe_adc, valor); // Envia 8 bits significativos
        os_delay(10); // Espera 10 ticks
    }
}

// TAREFA 2: Processa dado do PIPE e libera semaforo (Prioridade 5 - Round Robin com T1)
TASK tarefaProcessamento(void) {
    uint8_t dado;
    while(1) {
        pipe_read(&pipe_adc, &dado);
        
        // Processamento ficticio
        dado_processado = dado;
        LATD = dado; 
        sem_post(&sem_processamento); // Avisa que dado esta pronto para o PWM
        //os_delay(1); error: (1360) no space for auto/param os_create_task@new_task
    }
}

// TAREFA 3: Controle de PWM baseado no semaforo (Prioridade 7 - Maior)
TASK tarefaControlePWM(void) {
    while(1) {
        sem_wait(&sem_processamento);
        // Protege o acesso ao PWM com Mutex (exemplo de uso)
        mutex_lock(&mutex_recurso);
        pwm_set_duty(((uint16_t)dado_processado) << 2);
        mutex_unlock(&mutex_recurso);
        //os_delay(1); error: (1359) no space for _mutex_init parameters
    }
}

// TAREFA 4: Feedback Visual (Prioridade 3 - Menor)
TASK tarefaFeedbackLED(void) {
    while(1) {
        LATDbits.LATD7 = ~LATDbits.LATD7; // Pisca LED de heartbeat
        os_delay(10);
    }
}

// TAREFA 5: One-Shot (Disparada por Interrupcao Externa)
TASK tarefaOneShot(void) {
    // Executa uma acao unica e termina
    PORTDbits.RD1 = 1;
    os_delay(10);
    PORTDbits.RD1 = 0;
    
    // Como o SO do professor nao tem "exit_task", 
    // a tarefa one-shot deve se auto-suspender ou ficar em loop infinito
    // No hw.c, ela eh recriada a cada interrupcao.
    while(1) {
        os_task_change_state(WAITING, 0); // Suspende a si mesma
    }
}