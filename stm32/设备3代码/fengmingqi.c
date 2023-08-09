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
	//1，先定时器初始化
	TIM_TimeBaseInitTypeDef initstruct;
	//定义结构体
	TIM_OCInitTypeDef ocinitstruct;
	//开启外设时钟源
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_TIM4,ENABLE);
	initstruct.TIM_Prescaler=72-1;//预分频
	initstruct.TIM_Period=period;//计数周期
	initstruct.TIM_ClockDivision=TIM_CKD_DIV1;//时钟分割
	initstruct.TIM_CounterMode=TIM_CounterMode_Up;
	TIM_TimeBaseInit(TIM4,&initstruct);
	
	//2,在tim2定时器的 ch1通道初始化
	//设置定时器采用pwm模式输出
	ocinitstruct.TIM_OCMode=TIM_OCMode_PWM1;
	ocinitstruct.TIM_OutputState=TIM_OutputState_Enable;//设置定时器输出使能
	ocinitstruct.TIM_Pulse=pulse;//设置比较寄存器，也就是占空比
	ocinitstruct.TIM_OCPolarity=TIM_OCPolarity_High;//设置输出极性（计数0到pulse中都是输出高电平，超过之后输出反转）
	TIM_OC4Init(TIM4,&ocinitstruct);
	
	//定时器使能
	TIM_Cmd(TIM4,ENABLE);
}


	
