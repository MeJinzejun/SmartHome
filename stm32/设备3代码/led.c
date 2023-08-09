#include "led.h"

void led_init(void)//GB
{
	//定义结构体变量     //特别注意的地方是这个地方千万不能重复初始化rcc
	GPIO_InitTypeDef initStruct;
    GPIO_InitTypeDef initStruct1;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	initStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_12;			//设置待操作的引脚
	initStruct.GPIO_Speed = GPIO_Speed_10MHz; 	//设置待操作引脚的 工作频率
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//设置代操作引脚的 工作方式：推挽输出
	GPIO_Init(GPIOA, &initStruct);

   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	initStruct1.GPIO_Pin = GPIO_Pin_3;			//设置待操作的引脚
	initStruct1.GPIO_Speed = GPIO_Speed_10MHz; 	//设置待操作引脚的 工作频率
	initStruct1.GPIO_Mode = GPIO_Mode_Out_PP;	//设置代操作引脚的 工作方式：推挽输出
	
    GPIO_Init(GPIOB, &initStruct1);

}

void led_on(uint16_t pin)
{

	GPIO_ResetBits(GPIOA, pin);
    
}
void led_off(uint16_t pin)
{
 
    GPIO_SetBits(GPIOA, pin);  
}
void led_on_R(uint16_t pin)
{

	GPIO_ResetBits(GPIOB, pin);
    
}   

void led_off_R(uint16_t pin)
{
 

    GPIO_SetBits(GPIOB,pin);
    
   
}
