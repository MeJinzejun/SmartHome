#ifndef FENGMINGQI_H
#define FENGMINGQI_H

#include <stm32f10x.h>

void pwm_gpio_init(void);
void pwm_set(int period,int pulse);

#endif
