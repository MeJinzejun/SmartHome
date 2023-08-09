#include "adc.h"
#include <stm32f10x.h>

void nvic_init(void)
{
	NVIC_InitTypeDef initStruct;
	initStruct.NVIC_IRQChannel=USART1_IRQn;
	initStruct.NVIC_IRQChannelPreemptionPriority=0;
	initStruct.NVIC_IRQChannelSubPriority=2;
	initStruct.NVIC_IRQChannelCmd=ENABLE;
	NVIC_Init(&initStruct);
}

void delay(int m)
{
	int i=0;
	while (m--)
	{
		for (i=0;i<8050;i++);
	}
}


void adc_init(void)
{
	GPIO_InitTypeDef InitStructure;
	ADC_InitTypeDef adcstruct;
	//cc ->3.3 gnd ->gnd ao ->PA1
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA |RCC_APB2Periph_ADC1,ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_ADC1,ENABLE);
	InitStructure.GPIO_Pin = GPIO_Pin_1 ; 
	InitStructure.GPIO_Speed = GPIO_Speed_10MHz; 
	InitStructure.GPIO_Mode = GPIO_Mode_AIN; 
	GPIO_Init(GPIOA, &InitStructure);
	
	ADC_DeInit(ADC1);
	adcstruct.ADC_Mode=ADC_Mode_Independent;
	adcstruct.ADC_ScanConvMode=ENABLE;
	adcstruct.ADC_ContinuousConvMode=ENABLE;
	adcstruct.ADC_DataAlign=ADC_DataAlign_Right;
	adcstruct.ADC_ExternalTrigConv=ADC_ExternalTrigConv_None;
	adcstruct.ADC_NbrOfChannel=1;
	ADC_Init(ADC1,&adcstruct);
	
	//adcͨ������ʱ������
	ADC_RegularChannelConfig(ADC1,ADC_Channel_1,1,ADC_SampleTime_55Cycles5);
	ADC_Cmd(ADC1,ENABLE);
	ADC_SoftwareStartConvCmd(ADC1,ENABLE);
}

uint16_t adc_read(void)
{
	int i=0;
	uint16_t value;
	for(i=0;i<8;i++)
	{
		//�ж��Ƿ�ת�����
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)!=SET);
		//�õ����һ��ת�����
		value+=ADC_GetConversionValue(ADC1);
	}
	value=value/8;
	//adc��׼��ѹ3.3v,12λ����Ӧ������ֵλ4096����ôģ����value*3.3/4096������仯̫С������������Ҫ�Ŵ�����
	return (3300-value*3300/4096);//�Ŵ�1000��   ���ǵõ�һ����ֵ��4096���ʣ�Ȼ��ó��ڵ�ѹ�Ķ���
}
