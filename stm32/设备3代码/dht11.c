#include <stm32f10x.h>
#include "dht11.h"

//#define GPIO_Pin_12  GPIO_Pin_4


void delay_10us(void)
{
	int i = 100;
	while(i--);
}
void delay_10ms(void)
{
	int n = 10, i = 0;
	while(n--)
		for(i = 0; i < 8050; i++);
}

//dht11的GPIO引脚初始化  PB12
void dht11_gpio_init(void)
{
	GPIO_InitTypeDef initStruct;
	int i = 100;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	initStruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_5|GPIO_Pin_8;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &initStruct);
	
	//写入高电平,并持续一段时间 （1s）
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
	while(i--)
		delay_10ms();
}


void rent_IO_IN(void)	//rent 输入
{
	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_5;
	initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB, &initStruct);
}




void feng_IO_OUT(void)	//feng输出
{
	GPIO_InitTypeDef initStruct;	
	initStruct.GPIO_Pin = GPIO_Pin_8;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_Init(GPIOB, &initStruct);
}

void feng_on_out(void)	//开
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
   //  GPIO_ResetBits(GPIOB, GPIO_Pin_6); 
}

void feng_off_out(void)	//
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
     // GPIO_SetBits(GPIOB, GPIO_Pin_6);
}




void dht11_IO_IN(void)	//dht11 输入
{
	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_12;
	initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //浮空输入
	GPIO_Init(GPIOB, &initStruct);
}
void dht11_IO_OUT(void)	//dht11 输出
{
	GPIO_InitTypeDef initStruct;	
	initStruct.GPIO_Pin = GPIO_Pin_12;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP; //推挽输出
	GPIO_Init(GPIOB, &initStruct);
}

int dht11_init(void)
{
again:
	dht11_IO_OUT();
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
	delay_10ms();
	//1、拉低电平持续 至少 18ms
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
	delay_10ms();
	delay_10ms();
	//2、拉高电平持续 20~40us
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
	//设置引脚输入功能，方便30us后，读取 dht11响应的电平
	dht11_IO_IN();	
	delay_10us();
	delay_10us();
	delay_10us();
	
	if( RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){//3、判断 dht11是否拉低电平 80us给响应
		while( !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );//如果为低电平，则循环等待，电平拉高的时候，结束该循环。
		while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );//如果为高电平，则循环等待，电平拉低的时候，结束该循环
		return 0;//结束该函数，表示可以准备读取 dht11回复的数据
	} 
	goto again;//dht11 未给响应，重新开始
}
int dht11_read(u8 *temp, u8 *hum)
{
	int i, count=0;
	u8 data[5] = {0};
	if( dht11_init() == 0){
		for(i = 0 ; i < 40; i++){ //循环40次，读取dht11响应的 40bit数据	
			while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12));//等待响应时低电平 50us结束
			count = 0;
			while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				delay_10us();
				count++;		//记录高电平的次数
			}
			if(count > 5)
				// i/8 :表示0、1、2、3、4五个数据，1 << (7 - i % 8)：每个数据占8位，数据传输 高位在前
				data[i/8] |= 1 << (7 - i % 8);	
		}
		delay_10ms();
		//数据接收完成后，将PB12置位 输出模式
		dht11_IO_OUT();
		//空闲状态，电平拉高
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
		//检验检测数据是否正确
		if(data[4] != (data[0] + data[1] + data[2] + data[3]))
			return -1;
		else {
			*temp = data[2];
			*hum = data[0];
			return 0;
		}
	}
	return -1;
}





