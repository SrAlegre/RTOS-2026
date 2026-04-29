#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdint.h>

void adc_config(void);
void adc_on(void);
uint16_t adc_read(uint8_t channel);

// --- Definições para a API ---

// Enumerações para borda de interrupção
typedef enum { FALLING_EDGE = 0, RISING_EDGE = 1 } edge_t;

// Enumerações para canais PWM
typedef enum { PWM_CHANNEL_1 = 1, PWM_CHANNEL_2 = 2 } pwm_ch_t;

// --- Protótipos ADC ---
void adc_init_api(void);
uint16_t adc_read_safe(uint8_t channel);

// --- Protótipos PWM ---
void pwm_init(pwm_ch_t channel);
void pwm_set_duty(pwm_ch_t channel, uint8_t duty_percent);

// --- Protótipos Interrupção Externa ---
// A interrupção externa usará um semáforo para acordar uma tarefa
void ext_int0_init(edge_t edge, sem_t* sem_to_post);

#endif	/* IO_H */
