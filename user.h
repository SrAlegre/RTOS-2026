#ifndef USER_H
#define	USER_H

#include "types.h"

void config_user(void);

TASK LED_1(void);
TASK LED_2(void);
TASK READ_ADC(void);
TASK tarefaPWN(void);
TASK LED_1_prio(void);
TASK LED_2_prio(void);
TASK LED_3_prio(void);
TASK tarefaOneShot(void);



#endif	/* USER_H */

