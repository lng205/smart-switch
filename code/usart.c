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
函 数 名： 串口初始化
功能描述： STC10L08XE 单片机串口初始化函数
返回函数： none
其他说明： none
**************************************************************************/
void UartIni(void)
{
    SCON = 0x50;            //8-bit variable UART
    TMOD = 0x21;            //Set Timer1、2 as 8-bit auto reload mode
    TH1 = TL1 = -(FOSC/12/32/baud); //Set auto-reload vaule
    TR1 = 1;                //Timer1 start run
    ES = 1;                 //Enable UART interrupt
    EA = 1;                 //Open master interrupt switch
		EX0=0; //-->IE0
		ET0=1; //-->TF0 控制位置1，表明当TF0置1时，中断系统将介入
}
/************************************************************************
功能描述： 	串口发送一字节数据
入口参数：	DAT:带发送的数据
返 回 值： 	none
其他说明：	none
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
功能描述： 串口发送字符串数据
入口参数： 	*DAT：字符串指针
返 回 值： none
其他说明： API 供外部使用，直观！
**************************************************************************/
void PrintCom(uint8_t *DAT)
{
	while(*DAT)
	{
	 	UARTSendByte(*DAT++);
	}	
}


//****************** INTERRUPT *******************
void UART_SER (void) interrupt 4 //中断向量4-外部中断作为蓝牙串口中断
{
	uint8_t str [5];//定时时间字符串
	char *nptr;//字符指针
	if(RI)                           
	{
		RI=0;
		if(!mode)//判断首字符
		{
			switch(SBUF)
			{
				case '1':	 /*命令“开”*/
					motor_run(1);
					PrintCom("01 switch on\r\n");
					break;
				case '2':	 /*命令“关”*/
					motor_run(0);
					PrintCom("02 switch off\r\n");
					break;
				case '3':
					TR0 = 1;//启动定时器
					flag = 1;
					PrintCom("03 switch on\r\n");
					break;
				case '4':
					TR0 = 1;
					flag = 0;
					PrintCom("04 switch off\r\n");
					break;
				case '5':
					*nptr = str;//获取字符串
					mode = 1;
					break;
				case '6':
					*nptr = str;
					mode = 2;
			}
		}
		else//如果不为单字符信号
		{
			switch(mode)
			{
				case 1:
					if(SBUF == 0)
					{
						mode = 0;
						delaytime = atoi_16(str);//字符串转16位整数
						break;
					}
						*nptr = SBUF;
						nptr++;
					break;
			}
		}
	}
} 

uint16_t atoi_16(const char *nptr) //字符串转16位整数函数
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
	TH0=76;	 //重装初值 (50ms)
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