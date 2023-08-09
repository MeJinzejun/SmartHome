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
	
	//adc通道采样时间设置
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
		//判断是否转换完成
		while(ADC_GetFlagStatus(ADC1,ADC_FLAG_EOC)!=SET);
		//得到最近一次转换结果
		value+=ADC_GetConversionValue(ADC1);
	}
	value=value/8;
	//adc标准电压3.3v,12位，对应最大的数值位4096，那么模拟结果value*3.3/4096。结果变化太小，所以这里需要放大倍数。
	return (3300-value*3300/4096);//放大1000倍   这是得到一个数值与4096比率，然后得出在电压的多少
}
