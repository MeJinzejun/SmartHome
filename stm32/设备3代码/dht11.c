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

//dht11��GPIO���ų�ʼ��  PB12
void dht11_gpio_init(void)
{
	GPIO_InitTypeDef initStruct;
	int i = 100;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	initStruct.GPIO_Pin = GPIO_Pin_12|GPIO_Pin_5|GPIO_Pin_8;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOB, &initStruct);
	
	//д��ߵ�ƽ,������һ��ʱ�� ��1s��
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
    GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
	while(i--)
		delay_10ms();
}


void rent_IO_IN(void)	//rent ����
{
	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_5;
	initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOB, &initStruct);
}




void feng_IO_OUT(void)	//feng���
{
	GPIO_InitTypeDef initStruct;	
	initStruct.GPIO_Pin = GPIO_Pin_8;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_Init(GPIOB, &initStruct);
}

void feng_on_out(void)	//��
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_SET);
   //  GPIO_ResetBits(GPIOB, GPIO_Pin_6); 
}

void feng_off_out(void)	//
{
   GPIO_WriteBit(GPIOB, GPIO_Pin_8, Bit_RESET);
     // GPIO_SetBits(GPIOB, GPIO_Pin_6);
}




void dht11_IO_IN(void)	//dht11 ����
{
	GPIO_InitTypeDef initStruct;
	initStruct.GPIO_Pin = GPIO_Pin_12;
	initStruct.GPIO_Mode = GPIO_Mode_IN_FLOATING; //��������
	GPIO_Init(GPIOB, &initStruct);
}
void dht11_IO_OUT(void)	//dht11 ���
{
	GPIO_InitTypeDef initStruct;	
	initStruct.GPIO_Pin = GPIO_Pin_12;
	initStruct.GPIO_Speed = GPIO_Speed_50MHz;
	initStruct.GPIO_Mode = GPIO_Mode_Out_PP; //�������
	GPIO_Init(GPIOB, &initStruct);
}

int dht11_init(void)
{
again:
	dht11_IO_OUT();
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
	delay_10ms();
	//1�����͵�ƽ���� ���� 18ms
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_RESET);
	delay_10ms();
	delay_10ms();
	//2�����ߵ�ƽ���� 20~40us
	GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
	//�����������빦�ܣ�����30us�󣬶�ȡ dht11��Ӧ�ĵ�ƽ
	dht11_IO_IN();	
	delay_10us();
	delay_10us();
	delay_10us();
	
	if( RESET == GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){//3���ж� dht11�Ƿ����͵�ƽ 80us����Ӧ
		while( !GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );//���Ϊ�͵�ƽ����ѭ���ȴ�����ƽ���ߵ�ʱ�򣬽�����ѭ����
		while( GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12) );//���Ϊ�ߵ�ƽ����ѭ���ȴ�����ƽ���͵�ʱ�򣬽�����ѭ��
		return 0;//�����ú�������ʾ����׼����ȡ dht11�ظ�������
	} 
	goto again;//dht11 δ����Ӧ�����¿�ʼ
}
int dht11_read(u8 *temp, u8 *hum)
{
	int i, count=0;
	u8 data[5] = {0};
	if( dht11_init() == 0){
		for(i = 0 ; i < 40; i++){ //ѭ��40�Σ���ȡdht11��Ӧ�� 40bit����	
			while(!GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12));//�ȴ���Ӧʱ�͵�ƽ 50us����
			count = 0;
			while(GPIO_ReadInputDataBit(GPIOB, GPIO_Pin_12)){
				delay_10us();
				count++;		//��¼�ߵ�ƽ�Ĵ���
			}
			if(count > 5)
				// i/8 :��ʾ0��1��2��3��4������ݣ�1 << (7 - i % 8)��ÿ������ռ8λ�����ݴ��� ��λ��ǰ
				data[i/8] |= 1 << (7 - i % 8);	
		}
		delay_10ms();
		//���ݽ�����ɺ󣬽�PB12��λ ���ģʽ
		dht11_IO_OUT();
		//����״̬����ƽ����
		GPIO_WriteBit(GPIOB, GPIO_Pin_12, Bit_SET);
		//�����������Ƿ���ȷ
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





