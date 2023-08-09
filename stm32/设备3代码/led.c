#include "led.h"

void led_init(void)//GB
{
	//����ṹ�����     //�ر�ע��ĵط�������ط�ǧ�����ظ���ʼ��rcc
	GPIO_InitTypeDef initStruct;
    GPIO_InitTypeDef initStruct1;
   RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	initStruct.GPIO_Pin = GPIO_Pin_15 | GPIO_Pin_12;			//���ô�����������
	initStruct.GPIO_Speed = GPIO_Speed_10MHz; 	//���ô��������ŵ� ����Ƶ��
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;	//���ô��������ŵ� ������ʽ���������
	GPIO_Init(GPIOA, &initStruct);

   //RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB,ENABLE);
	initStruct1.GPIO_Pin = GPIO_Pin_3;			//���ô�����������
	initStruct1.GPIO_Speed = GPIO_Speed_10MHz; 	//���ô��������ŵ� ����Ƶ��
	initStruct1.GPIO_Mode = GPIO_Mode_Out_PP;	//���ô��������ŵ� ������ʽ���������
	
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
