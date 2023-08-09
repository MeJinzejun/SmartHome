#ifndef DHT11_H
#define DHT11_H

#include <stm32f10x.h>

void delay_10us(void);
void delay_10ms(void);

void dht11_gpio_init(void);
void dht11_IO_IN(void);
void dht11_IO_OUT(void);
int dht11_init(void);
int dht11_read(u8 *temp, u8 *hum);
void rent_IO_IN(void);
void feng_IO_OUT(void);
void feng_on_out(void);
void feng_off_out(void);
#endif
