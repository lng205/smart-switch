#include "config.h"
#define FOSC 22118400L      //System frequency
uint32_t baud=9600;           //UART baudrate
sbit SRD1 = P3^3;
sbit SRD2 = P3^4;
void Delay200ms();
void Delay120ms();
uint8_t mode = 0;
uint16_t delaytime;
uint16_t atoi_16(const char *nptr);
uint8_t flag;
/************************************************************************
�� �� ���� ���ڳ�ʼ��
���������� STC10L08XE ��Ƭ�����ڳ�ʼ������
���غ����� none
����˵���� none
**************************************************************************/
void UartIni(void)
{
    SCON = 0x50;            //8-bit variable UART
    TMOD = 0x21;            //Set Timer1��2 as 8-bit auto reload mode
    TH1 = TL1 = -(FOSC/12/32/baud); //Set auto-reload vaule
    TR1 = 1;                //Timer1 start run
    ES = 1;                 //Enable UART interrupt
    EA = 1;                 //Open master interrupt switch
		EX0=0; //-->IE0
		ET0=1; //-->TF0 ����λ��1��������TF0��1ʱ���ж�ϵͳ������
}
/************************************************************************
���������� 	���ڷ���һ�ֽ�����
��ڲ�����	DAT:�����͵�����
�� �� ֵ�� 	none
����˵����	none
**************************************************************************/
void UARTSendByte(uint8_t DAT)
{
	ES  =  0;
	TI=0;
	SBUF = DAT;
	while(TI==0);
	TI=0;
	ES = 1;
}
/************************************************************************
���������� ���ڷ����ַ�������
��ڲ����� 	*DAT���ַ���ָ��
�� �� ֵ�� none
����˵���� API ���ⲿʹ�ã�ֱ�ۣ�
**************************************************************************/
void PrintCom(uint8_t *DAT)
{
	while(*DAT)
	{
	 	UARTSendByte(*DAT++);
	}	
}


//****************** INTERRUPT *******************
void UART_SER (void) interrupt 4 //�ж�����4-�ⲿ�ж���Ϊ���������ж�
{
	uint8_t str [5];//��ʱʱ���ַ���
	char *nptr;//�ַ�ָ��
	if(RI)                           
	{
		RI=0;
		if(!mode)//�ж����ַ�
		{
			switch(SBUF)
			{
				case '1':	 /*�������*/
					motor_run(1);
					PrintCom("01 switch on\r\n");
					break;
				case '2':	 /*����ء�*/
					motor_run(0);
					PrintCom("02 switch off\r\n");
					break;
				case '3':
					TR0 = 1;//������ʱ��
					flag = 1;
					PrintCom("03 switch on\r\n");
					break;
				case '4':
					TR0 = 1;
					flag = 0;
					PrintCom("04 switch off\r\n");
					break;
				case '5':
					*nptr = str;//��ȡ�ַ���
					mode = 1;
					break;
				case '6':
					*nptr = str;
					mode = 2;
			}
		}
		else//�����Ϊ���ַ��ź�
		{
			switch(mode)
			{
				case 1:
					if(SBUF == 0)
					{
						mode = 0;
						delaytime = atoi_16(str);//�ַ���ת16λ����
						break;
					}
						*nptr = SBUF;
						nptr++;
					break;
			}
		}
	}
} 

uint16_t atoi_16(const char *nptr) //�ַ���ת16λ��������
{
	uint16_t n = 0;
	uint16_t m = 0;
	while(48 <= *nptr && *nptr <= 57)
	{
		n = (*nptr - 48) + m;
		nptr++;
		m = n*10;
	}
	return n;
}

void Timer0_Routine() interrupt 1
{
	static uint16_t c = 0;
	static t = 0;
	TH0=76;	 //��װ��ֵ (50ms)
	TL0=0;
	c++;
	if(c==1200)
	{
		t++;
		c=0;
		if(t == delaytime)
		{
			motor_run(flag);
			TR0 = 0;
		}
	}
}