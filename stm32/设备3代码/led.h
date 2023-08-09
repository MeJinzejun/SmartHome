#ifndef LED_H
#define LED_H

#include <stm32f10x.h>

//#define A GPIO
//#define RED GPIO_Pin_3   //P «¥Û–¥
//#define GREEN GPIO_Pin_15
//#define BLUE GPIO_Pin_12

void led_init(void);
void led_on(uint16_t pin);
void led_off(uint16_t pin);
void led_on_R(uint16_t pin);
void led_off_R(uint16_t pin);

#endif
