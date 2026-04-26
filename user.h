#ifndef USER_H
#define	USER_H

#include "types.h"

void config_user(void);

TASK acionaMotor(void);
TASK ligaLed(void);
TASK apagaLed(void);


TASK LED_1(void);
TASK LED_2(void);
TASK LED_3(void);

TASK LED_1_mutex(void);
TASK LED_2_mutex(void);

TASK LED_1_prio();
TASK LED_2_prio();
TASK LED_3_prio();


#endif	/* USER_H */

