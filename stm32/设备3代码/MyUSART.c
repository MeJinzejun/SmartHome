#include "stm32f10x.h"                  // Device header
#include "MyUSART.h"
#include "test.h"
#include <string.h>

char RECS[250];
unsigned char i;
   const char* Refunc1="fan_on";
   const char* Refunc2="fan_off";
   const char* Refunc3="feng_on";
   const char* Refunc4="feng_off";
   const char* Refunc5="lamp_on";
   const char* Refunc6="lamp_off";
   int FanStatus;
   int FengStatus;
   int LampStatus;
void MyUSART_Init(void)
{
	//GPIO端口配置
	USART_InitTypeDef USART_InitStructure;    
	NVIC_InitTypeDef NVIC_InitStructure;     
	GPIO_InitTypeDef GPIO_InitStructure;    
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB , ENABLE); 
 	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE);    

   // USART3_Rx (PB.11)       
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IPU;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);    

   // USART3_Tx (PB.10)   
   GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;    
   GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;    
   GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;    
   GPIO_Init(GPIOB, &GPIO_InitStructure);
   
   //USART3 NVIC配置    
   NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority=0 ;//抢占优先级0
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);    
   
   //USART3 初始化设置 
   USART_InitStructure.USART_BaudRate = 115200;    
   USART_InitStructure.USART_WordLength = USART_WordLength_8b;    
   USART_InitStructure.USART_StopBits = USART_StopBits_1;    
   USART_InitStructure.USART_Parity = USART_Parity_No;    
   USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;    
   USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    


   //初始化 USART3     
   USART_Init(USART3, &USART_InitStructure);
  // 开启串口接收中断   
   USART_ITConfig(USART3, USART_IT_RXNE, ENABLE);    
   // 使能USART3     
   USART_Cmd(USART3, ENABLE);         
}

void delay_ms(uint32_t ms)
{
    uint32_t i;
    for(i = 0; i < ms; i++)
    {
        uint32_t j;
        for(j = 0; j < 5000; j++);
    }
}
	

char* MyUSART_GetString(void)
{
	return RECS;
}
void MyUSART_SendString(char* str)
{
	uint8_t stri=0;
	while(str[stri] != '\0')
		USART_SendData (USART3,str[stri++]);
}
void CommandAnalyse(void)
{
	if(strncmp(RECS,"+MQTTSUBRECV:",13)==0)
	{
		uint8_t i=0;
		while(RECS[i++] != '\0')             
		{
			if(strncmp((RECS+i),Refunc1,6)==0)
			{
				       
				FanStatus=1;
			}
			if(strncmp((RECS+i),Refunc2,7)==0)
			{
				
				FanStatus=0;
			}
            if(strncmp((RECS+i),Refunc3,7)==0)
			{
				
				FengStatus=1;
			}
            if(strncmp((RECS+i),Refunc4,8)==0)
			{
				      
				FengStatus=0;
			}
			if(strncmp((RECS+i),Refunc5,7)==0)
			{
				
				LampStatus=1;
			}
            if(strncmp((RECS+i),Refunc6,8)==0)
			{
				
				LampStatus=0;
			}
            
           
		}
	}
}

void USART3_IRQHandler()
{
	if(USART_GetITStatus(USART3,USART_IT_RXNE))
	{
		RECS[i++]=USART_ReceiveData(USART3);
		if((RECS[i-2]=='\r')|(RECS[i-1]=='\n'))  
		{
			RECS[i-2]='\0';
			i = 0;
			CommandAnalyse();
		}
	}
}
