#include <stm32f10x.h>

void pwm_gpio_init(void)
{
	//PA  -->TIM2_CH1
	GPIO_InitTypeDef initstruct;
	//RCC_DeInit();
	//RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA,ENABLE);
	initstruct.GPIO_Pin=GPIO_Pin_9;
	initstruct.GPIO_Speed=GPIO_Speed_10MHz;
	initstruct.GPIO_Mode=GPIO_Mode_AF_PP;
	GPIO_Init(GPIOB,&initstruct);
}

void pwm_set(int period,int pulse)
{
	//1���ȶ�ʱ����ʼ��
	TIM_TimeBaseInitTypeDef initstruct;
	//����ṹ��
	TIM_OCInitTypeDef ocinitstruct;
	//��������ʱ��Դ
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	initstruct.TIM_Prescaler=72-1;//Ԥ��Ƶ
	initstruct.TIM_Period=period;//��������
	initstruct.TIM_ClockDivision=TIM_CKD_DIV1;//ʱ�ӷָ�
	initstruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&initstruct);
	
	//2,��tim2��ʱ���� ch1ͨ����ʼ��
	//���ö�ʱ������pwmģʽ���
	ocinitstruct.TIM_OCMode=TIM_OCMode_PWM1;
	ocinitstruct.TIM_OutputState=TIM_OutputState_Enable;//���ö�ʱ�����ʹ��
	ocinitstruct.TIM_Pulse=pulse;//���ñȽϼĴ�����Ҳ����ռ�ձ�
	ocinitstruct.TIM_OCPolarity=TIM_OCPolarity_High;//����������ԣ�����0��pulse�ж�������ߵ�ƽ������֮�������ת��
	TIM_OC4Init(TIM4,&ocinitstruct);
	
	//��ʱ��ʹ��
	TIM_Cmd(TIM4,ENABLE);
}


	
