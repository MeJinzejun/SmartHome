#include "stm32f10x.h"
#include "stdio.h"
#include "string.h"
#include <MyUSART.h>
#include <oled.h>
#include <bmp.h>
#include <stm32f10x_tim.h>
#include "dht11.h"
#include "adc.h"
#include "fengmingqi.h"
#include "led.h"
extern char RECS[250];
extern   int FanStatus;
extern   int FengStatus;
extern   int LampStatus;

const char* WIFI ="gwp";
const char* WIFIASSWORD="12345678";
const char* ClintID="465";
const char* username="cilent&ixympzDwD9p";
const char* passwd="f753ead0f12e768ce0554d10b70ff3d1298a64f0c0dcdfff1db959b1ffea8dc8";
const char* Url="mqtt.yyzlab.com.cn";
//1688716903573/AIOTSIM2APP
const char* pubtopic="1688716903573/AIOTSIM2APP";

const char* subtopic="1688716903573/APP2AIOTSIM";
int count = 0;



extern char RECS[250];
uint16_t cnt;
// ����MQTTЭ���������
char mqttConnectCommand[128];
char mqttPublishCommand[128];
char mqttSubscribeCommand[128];

   u8 tem, hum;
  // float buf[1024];
   int rent,fan,lamp1,feng;
   const char* func1="tem";
   const char* func2="hum";
   const char* func3="light";
   const char* func4="sleet";
   const char* func5="fan";
   const char* func6="feng";
   const char* func7="lamp";



void TIM2_IRQHandler(void);




int fputc(int ch,FILE *f )   //printf�ض���  
{
	USART_SendData(USART3,(uint8_t)ch);
	while(USART_GetFlagStatus (USART3,USART_FLAG_TC) == RESET);
	return ch;
}

char esp_Init(void)
{


	memset(RECS,0,sizeof(RECS));
	printf("AT+RST\r\n");  //����
	delay_ms(2000);
	
//	memset(RECS,0,sizeof(RECS));
//	printf("ATE0\r\n");    //�رջ���
//	delay_ms(2000);
//	if(strcmp(RECS,"OK"))
//		return 1;
//	
//	printf("AT+CWMODE=1\r\n"); //Stationģʽ
//	delay_ms(2000);
//	if(strcmp(RECS,"OK"))
//		return 2;
//	
//	memset(RECS,0,sizeof(RECS));
//	printf("AT+CWJAP=\"%s\",\"%s\"\r\n",WIFI,WIFIASSWORD); //�����ȵ�
//	delay_ms(2000);
//	if(strcmp(RECS,"OK"))
//		return 3;
	
    delay_ms(1000);
	memset(RECS,0,sizeof(RECS));
	printf("AT+MQTTUSERCFG=0,1,\"%s\",\"%s\",\"%s\",0,0,\"\"\r\n",ClintID,username,passwd);//�û���Ϣ����
	delay_ms(1000);
	if(strcmp(RECS,"OK"))
		return 4;
	
	memset(RECS,0,sizeof(RECS));
	printf("AT+MQTTCONN=0,\"%s\",1883,1\r\n",Url); //���ӷ�����
	delay_ms(1000);
	if(strcmp(RECS,"OK"))
		return 5;
	
	printf("AT+MQTTSUB=0,\"%s\",1\r\n",subtopic); //������Ϣ
	delay_ms(500);
	if(strcmp(RECS,"OK"))
		return 6;
	memset(RECS,0,sizeof(RECS));
	return 0;
}


char Esp_PUB(void)
{
	memset(RECS,0,sizeof(RECS));
//	printf("AT+MQTTPUB=0,\"%s\",\"{\\\"method\\\":\\\"thing.event.property.post\\\"\\,\\\"params\\\":{\\\"%s\\\":%d\\,\\\"%s\\\":%d\\,\\\"%s\\\":%d\\,\\\"%s\\\":%d}}\",0,0\r\n",pubtopic,func1,Temperature,func2,Shidu,func3,Lock,func4,Switch2);
	printf("AT+MQTTPUB=0,\"%s\",\"TESSSSSSSSSSSSSST\",0,0\r\n",pubtopic);

	//while(RECS[0]);//�ȴ�ESP��������
	delay_ms(200);//��ʱ�ȴ����ݽ������
	if(strcmp(RECS,"ERROR")==0)
		return 1;
	return 0;
}

void Init(void)
{
	uint8_t Judge=0;
	OLED_Init();
	MyUSART_Init();
	esp_Init();
    OLED_Clear();
	OLED_ShowString(1,1,"Linking...",16);
	MyUSART_Init(); //��ʼ������
	//Timer_Init();

	do
	{
		Judge = esp_Init();
		OLED_Clear();
		OLED_ShowString(1,1,"error code:    ",16);
		OLED_ShowNum(90,1,Judge,1,16);
        
	}while(Judge);  //���Ӱ�����ֱ���ɹ�
	
}






int main()
{

//   u8 tem, hum;
//   int rent;

//   const char* func1="tem";
//   const char* func2="hum";
//   const char* func3="light";
//   const char* func4="rent";

    NVIC_InitTypeDef NVIC_InitStructure;
		TIM_TimeBaseInitTypeDef TIM_TimeBaseStructure;
		//TX1 (A9)  RX1(A10)
   Init();
   // usart1_init();
   adc_init();
   nvic_init();
   led_init();
    
   led_off(GPIO_Pin_15);
   led_off(GPIO_Pin_12);
   led_off_R(GPIO_Pin_3);
    
   dht11_gpio_init();
   rent_IO_IN();
   feng_IO_OUT();
   // pcled_init();
	pwm_gpio_init();	
  

		//ʱ�ӳ�ʼ��
  //	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1,ENABLE);
	//nvic��ʼ��
   /* Configure the NVIC Preemption Priority Bits */  
   NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   
   /* Enable the USARTy Interrupt */
   NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
   NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
   NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
   NVIC_Init(&NVIC_InitStructure);


	

		
		//ͨ����ʱ��2 TIM2 ��ʱ5s �ϴ���������
		RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM2, ENABLE);
		
		
		NVIC_PriorityGroupConfig(NVIC_PriorityGroup_0);
   
		/* Enable the USARTy Interrupt */
		NVIC_InitStructure.NVIC_IRQChannel = TIM2_IRQn;
		NVIC_InitStructure.NVIC_IRQChannelSubPriority = 2;
		NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
		NVIC_Init(&NVIC_InitStructure);
	
		
		
		/* Time base configuration */
		TIM_TimeBaseStructure.TIM_Period = 1000-1;   
		TIM_TimeBaseStructure.TIM_Prescaler = 72-1;
		TIM_TimeBaseStructure.TIM_ClockDivision = 0;
		TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
 
		TIM_TimeBaseInit(TIM2, &TIM_TimeBaseStructure);


		/* TIM IT enable */
		TIM_ITConfig(TIM2,TIM_IT_Update, ENABLE);
 
		/* TIM2 enable counter */
		TIM_Cmd(TIM2, ENABLE);

//    OLED_Clear();
//	
//	OLED_DrawBMP(0,0,128,8,KJ01);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ02);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ03);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ04);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ05);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ06);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ07);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ08);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ09);delay(120); 
//	OLED_DrawBMP(0,0,128,8,KJ10);delay(1000);
//	OLED_WR_Byte(0x2e,OLED_CMD);        //�رչ���	
//	OLED_Clear();
//	
//	
//	OLED_WR_Byte(0x2e,OLED_CMD);        //�رչ���
//	OLED_WR_Byte(0x29,OLED_CMD);        //ˮƽ��ֱ��ˮƽ�������� 29/2a
//	OLED_WR_Byte(0x00,OLED_CMD);        //�����ֽ�
//	OLED_WR_Byte(0x08,OLED_CMD);        //��ʼҳ 0
//	OLED_WR_Byte(0x07,OLED_CMD);        //����ʱ����
//	OLED_WR_Byte(0x08,OLED_CMD);        //��ֹҳ 7
//	OLED_WR_Byte(0x01,OLED_CMD);        //��ֱ����ƫ����

//	
//	OLED_DrawBMP(44,3,84,7,KJ11);
//	OLED_ShowCHinese(15,1,8);//��
//	OLED_ShowCHinese(31,1,9);//��
//	OLED_ShowCHinese(47,1,10);//ʦ
//	OLED_ShowCHinese(63,1,11);//��
//	OLED_ShowCHinese(79,1,12);//ѧ
//	OLED_ShowCHinese(95,1,13);//Ժ

//	OLED_WR_Byte(0x2F,OLED_CMD);        //��������

//	delay(3000);
//	OLED_WR_Byte(0x2e,OLED_CMD);        //�رչ���
	OLED_Clear();
	
	OLED_DrawBMP(90,0,122,4,ui3);
	
	OLED_ShowCHinese(0,0,0);//��
	OLED_ShowCHinese(18,0,1);//��
	OLED_ShowString(36,0,":",16);
	OLED_ShowCHinese(64,0,4);//���϶ȷ���
		
	OLED_ShowCHinese(0,2,3);//ʪ
	OLED_ShowCHinese(18,2,1);//��
	OLED_ShowString(36,2,":",16);
	OLED_ShowString(64,2,"%",16);
	OLED_ShowString(70,2,"RH",16);
	
	OLED_ShowCHinese(0,4,5);//��
	OLED_ShowCHinese(18,4,6);//��
	OLED_ShowCHinese(36,4,7);//ǿ
	OLED_ShowCHinese(54,4,1);//��
	OLED_ShowString(72,4,":",16);

 //  printf("AT+MQTTPUB=0,\"%s\",\"spd\",0,0\r\n",pubtopic);
    while(1){  
         
//        OLED_ShowNum(45,0,tem,2,16);		
//        OLED_ShowNum(45,2,hum,2,16);
//		OLED_ShowNum(80,4,value,4,16);
        
        if(FanStatus==1)
        {
         //���ȿ�   
            feng_on_out();
            fan=1;
            
        }
        else{
            feng_off_out();
            fan=0;
        }
        
        if(FengStatus==1)
        {
        //��������
            pwm_set(500,100);
            delay(1000);
            pwm_set(500,2000);
            feng=1;
        }
       else{
           pwm_set(500,2000);
           feng=0;
       } 
       if(LampStatus==1)
       {
       //�ƿ�
         
            led_on(GPIO_Pin_12);
            led_off(GPIO_Pin_15);
            led_off_R(GPIO_Pin_3);
            lamp1=1;
       }
       else{
          
            led_off(GPIO_Pin_12);
            led_off(GPIO_Pin_15);
            led_off_R(GPIO_Pin_3);
           lamp1=0;
       
       }

      }
       
}



void TIM2_IRQHandler(void)
{

	 if(TIM_GetITStatus(TIM2, TIM_IT_Update) != RESET)
   {
			TIM_ClearITPendingBit(TIM2, TIM_IT_Update);
			
			count++;
          if(count==1000)
          {
        int value=adc_read();
        dht11_read(&tem,&hum);    
        OLED_ShowNum(45,0,tem,2,16);		
        OLED_ShowNum(45,2,hum,2,16);
		OLED_ShowNum(80,4,value,4,16);              
        if(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_5)==SET)
        {
             rent=1;
        }
        else{
            rent=0;
        }
        
        
   printf("AT+MQTTPUB=0,\"%s\",\"{\\\"%s\\\":%f\\,\\\"%s\\\":%f\\,\\\"%s\\\":%f\\,\\\"%s\\\":%d\\,\\\"%s\\\":%d\\,\\\"%s\\\":%d\\,\\\"%s\\\":%d\\}\",0,0\r\n",pubtopic,func1,(float)tem,func2,(float)hum,func3,(float)value,func4,rent,func5,fan,func6,feng,func7,lamp1);
       count = 0;
          }
  }  
}
















