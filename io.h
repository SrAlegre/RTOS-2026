#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdint.h>

void adc_config(void);
uint16_t adc_read(uint8_t channel);

void pwm_init(void);
void pwm_set_duty(uint16_t duty);




#endif	/* IO_H */
