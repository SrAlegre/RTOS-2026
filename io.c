
#include "io.h"

void adc_config(void) {
    ADCON0bits.CHS = 0b00000; // Canal zero (AN0)
    ADCON1bits.PVCFG = 0b00; // VDD
    ADCON1bits.NVCFG = 0b00; // VSSe
    ADCON2bits.ADFM = 1; // Justifica??o do dado (direita)
    ADCON2bits.ACQT = 0b110; // Tempo de aquisi??o do sinal
    ADCON2bits.ADCS = 0b100; // Fosc/4
    TRISAbits.RA0 = 1;
    ANSELAbits.ANSA0 = 1;
    ADCON0bits.ADON = 1;
}

uint16_t adc_read(uint8_t channel) {
    ADCON0bits.CHS = channel; // Seleciona canal
    ADCON0bits.GO = 1; // Inicia conversao
    while (ADCON0bits.GO); // Espera fim
    return ((uint16_t) ADRESH << 8) | ADRESL;
}


void pwm_init(void) {
    // Configura CCP1 para PWM
    PR2 = 0xFF; // Periodo
    CCP1CON = 0x0C; // Modo PWM
    T2CON = 0x04; // Timer 2 ON, Prescaler 1:1
    TRISCbits.RC2 = 0; // Saida do PWM
}

void pwm_set_duty(uint16_t duty) {
    // Duty de 10 bits
    CCPR1L = (duty >> 2) & 0xFF;
    CCP1CONbits.DC1B = duty & 0x03;
}
