#ifndef IO_H
#define	IO_H

#include <xc.h>
#include <stdint.h>

void adc_config(void);
void adc_on(void);
uint16_t adc_read(uint8_t channel);



#endif	/* IO_H */
