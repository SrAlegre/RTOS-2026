#include "user.h"
#include <xc.h>
#include "kernel.h"
#include "sync.h"
#include "com.h"
#include"io.h"


sem_t s;
pipe_t p;
mutex_t m_led;
char dado;

void config_user() {
    //asm("global _tarefaPWN");
    TRISBbits.RB0 = 1;

    TRISCbits.RC6 = 0;
    TRISCbits.RC7 = 0;
    TRISDbits.RD0 = 0;
    ANSELDbits.ANSD0 = 0;
    ANSELCbits.ANSC6 = 0;
    ANSELCbits.ANSC7 = 0;

    INTCONbits.INT0IE = 1; // Habilita INT0
    INTCON2bits.INTEDG0 = 0; // Interrupt on falling edge 
    INTCONbits.INT0IF = 0; // Flag interrupt


    asm("global _LED_1, _LED_2,_READ_ADC,_tarefaPWN");
    // asm("global _LED_1_prio,_LED_2_prio,_LED_3_prio");



    adc_config();
    pwm_init();
    sem_init(&s, 0);
    pipe_init(&p);
    mutex_init(&m_led);
}

TASK LED_1(void) {
    char seq[] = {'L', 'L', 'D', 'L', 'D', 'D'};
    uint8_t pos = 0;
    while (1) {
        PORTCbits.RC6 = ~PORTCbits.RC6; 
        pipe_write(&p, seq[pos]); 
        pos = (pos + 1) % 6;
        sem_post(&s); 
        mutex_lock(&m_led);
        os_delay(5);
        mutex_unlock(&m_led);
    }
}

TASK LED_2(void) {
    while (1) {
        sem_wait(&s); // aguarda sinal da tarefa 1
        pipe_read(&p, &dado);
        if (dado == 'L')
            PORTDbits.RD0 = 1; // liga LED RD0
        else if (dado == 'D')
            PORTDbits.RD0 = 0; // desliga LED RD0
    }
}

TASK READ_ADC(void) {
    while (1) {
        if (adc_read() >= 123) {
            PORTCbits.RC7 = 1; // LED RC7 ligado se temp ? 60°C
        } else {
            PORTCbits.RC7 = 0; // LED RC7 desligado
        }
        os_delay(10);
    }
}

TASK tarefaPWN(void) {
    while (1) {
        pwm_set_duty(128);
        os_delay(5);
    }
}

//teste

TASK LED_1_prio() {
    while (1) {
        PORTCbits.RC6 = 1;
        os_delay(5);
        PORTCbits.RC6 = 0;
    }
}

TASK LED_2_prio() {
    while (1) {
        PORTCbits.RC7 = 1;
        os_delay(5);
        PORTCbits.RC7 = 0;
    }
}

TASK LED_3_prio() {
    while (1) {
        PORTDbits.RD0 = 1;
        os_delay(5);
        PORTDbits.RD0 = 0;
    }
}

TASK tarefaOneShot(void) {
    while (1) {

        LATDbits.LATD1 = 1;
        os_delay(10);
        LATDbits.LATD1 = 0;
        os_task_change_state(WAITING, 0);
        mutex_unlock(&m_led);


    }

}