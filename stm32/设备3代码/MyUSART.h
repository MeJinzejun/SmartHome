#ifndef __MyUSART_H
#include <stdint.h>
#define __MyUSART_H
void 	MyUSART_Init(void);
char* MyUSART_GetString(void);
void MyUSART_SendString(char* str);
void CommandAnalyse(void);
void delay_ms(uint32_t ms);

#endif