#ifndef USER_H
#define	USER_H

#include "types.h"

void config_user(void);

TASK tarefaLeituraADC(void);
TASK tarefaProcessamento(void);
TASK tarefaControlePWM(void);
TASK tarefaFeedbackLED(void);
TASK tarefaOneShot(void);

#endif	/* USER_H */

