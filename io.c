#include "io.h"
#include "kernel.h"

// Mutex para proteger o conversor A/D
mutex_t m_adc;

// Ponteiro global para o semáforo da interrupção externa
sem_t* ext_int0_sem = NULL;

// ========================================================
// DRIVER DE ADC (Thread-Safe)
// ========================================================
void adc_init_api(void) {
    mutex_init(&m_adc);
    
    // Configuração básica do ADC (Ajuste conforme o clock do seu PIC)
    ADCON1 = 0x00; // Referência VDD e VSS
    ADCON2 = 0xA9; // Resultado justificado à direita, 12 TAD, FOSC/8
    ADCON0bits.ADON = 1; // Liga o módulo ADC
}

uint16_t adc_read_safe(uint8_t channel) {
    uint16_t result;
    
    // 1. Bloqueia o ADC para a tarefa atual
    mutex_lock(&m_adc);
    
    // 2. Seleciona o canal e aguarda o tempo de aquisição
    ADCON0bits.CHS = channel;
    os_delay(1); // Usamos o delay do RTOS em vez de travar a CPU
    
    // 3. Inicia a conversão
    ADCON0bits.GO_nDONE = 1;
    
    // 4. Aguarda a conversão terminar (Yield libera a CPU para outra tarefa enquanto espera)
    while (ADCON0bits.GO_nDONE) {
        os_yield(); 
    }
    
    // 5. Lê o resultado (10 bits)
    result = (ADRESH << 8) | ADRESL;
    
    // 6. Libera o ADC para outras tarefas
    mutex_unlock(&m_adc);
    
    return result;
}

// ========================================================
// DRIVER DE PWM
// ========================================================
void pwm_init(pwm_ch_t channel) {
    // Configura o Timer2 para o PWM (Exemplo para ~1kHz em um PIC a 16MHz)
    PR2 = 0xFF;
    T2CON = 0x06; // Prescaler 1:16, Timer2 ON
    
    if (channel == PWM_CHANNEL_1) {
        TRISCbits.RC2 = 0; // Configura pino CCP1 como saída
        CCP1CON = 0x0C;    // Modo PWM
        CCPR1L = 0;        // Duty cycle inicial = 0%
    } else if (channel == PWM_CHANNEL_2) {
        TRISCbits.RC1 = 0; // Configura pino CCP2 como saída
        CCP2CON = 0x0C;    // Modo PWM
        CCPR2L = 0;        // Duty cycle inicial = 0%
    }
}

void pwm_set_duty(pwm_ch_t channel, uint8_t duty_percent) {
    // Limita o percentual a 100%
    if (duty_percent > 100) duty_percent = 100;
    
    // Calcula o valor para o registrador (0 a 255 baseado no PR2 = 0xFF)
    uint8_t duty_value = (uint8_t)((duty_percent * 255) / 100);
    
    // Atualização atômica para evitar flickering
    DISABLE_ALL_INTERRUPTS();
    if (channel == PWM_CHANNEL_1) {
        CCPR1L = duty_value;
    } else if (channel == PWM_CHANNEL_2) {
        CCPR2L = duty_value;
    }
    ENABLE_ALL_INTERRUPTS();
}

// ========================================================
// DRIVER DE INTERRUPÇÃO EXTERNA (INT0)
// ========================================================
void ext_int0_init(edge_t edge, sem_t* sem_to_post) {
    // Salva o ponteiro do semáforo que será ativado na ISR
    ext_int0_sem = sem_to_post;
    
    TRISBbits.RB0 = 1; // Configura INT0 (RB0) como entrada
    ANSELBbits.ANSB0 = 0; // Garante que é I/O digital
    
    if (edge == RISING_EDGE) {
        INTCON2bits.INTEDG0 = 1;
    } else {
        INTCON2bits.INTEDG0 = 0;
    }
    
    INTCONbits.INT0IF = 0; // Limpa a flag
    INTCONbits.INT0IE = 1; // Habilita a interrupção INT0
}

void adc_config(void) {
    ADCON0bits.CHS = 0b00000; // Canal zero (AN0)
    ADCON1bits.PVCFG = 0b00; // VDD
    ADCON1bits.NVCFG = 0b00; // VSS
    ADCON2bits.ADFM = 1; // Justifica??o do dado (direita)
    ADCON2bits.ACQT = 0b110; // Tempo de aquisi??o do sinal
    ADCON2bits.ADCS = 0b100; // Fosc/4
    TRISAbits.RA0 = 1;
    ANSELAbits.ANSA0 = 1;
}

void adc_on(void) {
    ADCON0bits.ADON = 1;
}

uint16_t adc_read(uint8_t channel) {
    ADCON0bits.CHS = channel; // Seleciona canal
    ADCON0bits.GO = 1; // Inicia conversao
    while (ADCON0bits.GO); // Espera fim
    return ((uint16_t) ADRESH << 8) | ADRESL;
}

