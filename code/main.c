#include "config.h"
/************************************************************************************/
//	nAsrStatus ������main�������б�ʾ�������е�״̬������LD3320оƬ�ڲ���״̬�Ĵ���
//	LD_ASR_NONE:		��ʾû������ASRʶ��
//	LD_ASR_RUNING��		��ʾLD3320������ASRʶ����
//	LD_ASR_FOUNDOK:		��ʾһ��ʶ�����̽�������һ��ʶ����
//	LD_ASR_FOUNDZERO:	��ʾһ��ʶ�����̽�����û��ʶ����
//	LD_ASR_ERROR:		��ʾһ��ʶ��������LD3320оƬ�ڲ����ֲ���ȷ��״̬
/***********************************************************************************/
uint8 idata nAsrStatus=0;
void MCU_init();
void ProcessInt0(); //ʶ������
void delay(unsigned long uldata);
void User_handle(uint8 dat);//����ִ�к���
void Delay200ms();
void Led_test(void);//��Ƭ������ָʾ
uint8_t G0_flag=DISABLE;//���б�־��ENABLE:���С�DISABLE:��ֹ����
sbit LED=P4^2;//�ź�ָʾ��
extern uint8_t flag;

//��bit����λ����������λ������sbitר�Ŷ���ȫ��λ����
sbit SRD1 = P2^3;
sbit SRD2 = P2^4;

/***********************************************************
* ��    �ƣ� void  main(void)
* ��    �ܣ� ������	�������
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void  main(void)
{
  uint8 idata nAsrRes;
  P2M0 = 0xFF;
  P2M1 = 0x00;//�������
  Led_test();
  MCU_init();
  LD_Reset();
  UartIni(); /*���ڳ�ʼ��*/
  nAsrStatus = LD_ASR_NONE;		//	��ʼ״̬��û������ASR

  while(1)
  {
    switch(nAsrStatus)
    {
    case LD_ASR_RUNING:
    case LD_ASR_ERROR:
      break;
    case LD_ASR_NONE:
    {
      nAsrStatus=LD_ASR_RUNING;
      if (RunASR()==0)	/*	����һ��ASRʶ�����̣�ASR��ʼ����ASR��ӹؼ��������ASR����*/
      {
        nAsrStatus = LD_ASR_ERROR;
      }
      break;
    }
    case LD_ASR_FOUNDOK: /*	һ��ASRʶ�����̽�����ȥȡASRʶ����*/
    {
      nAsrRes = LD_GetResult();		/*��ȡ���*/
      User_handle(nAsrRes);//ִ�к���
      nAsrStatus = LD_ASR_NONE;
      break;
    }
    case LD_ASR_FOUNDZERO:
    default:
    {
      nAsrStatus = LD_ASR_NONE;
      break;
    }
    }// switch
  }// while

}
/***********************************************************
* ��    �ƣ� 	 LED�Ʋ���
* ��    �ܣ� ��Ƭ���Ƿ���ָʾ
* ��ڲ����� ��
* ���ڲ�������
* ˵    ����
**********************************************************/
void Led_test(void)
{
  LED=~ LED;
  Delay200ms();
  LED=~ LED;
  Delay200ms();
  LED=~ LED;
  Delay200ms();
  LED=~ LED;
  Delay200ms();
  LED=~ LED;
  Delay200ms();
  LED=~ LED;
}
/***********************************************************
* ��    �ƣ� void MCU_init()
* ��    �ܣ� ��Ƭ����ʼ��
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void MCU_init()
{
  P0 = 0xff;
  P1 = 0x00;
  P2 = 0x00;
  P3 = 0xff;
  P4 = 0xff;

  LD_MODE = 0;		//	����MD�ܽ�Ϊ�ͣ�����ģʽ��д
  IE0=1;
  EX0=1;
  EA=1;
}
/***********************************************************
* ��    �ƣ�	��ʱ����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void Delay200us()		//@22.1184MHz
{
  unsigned char i, j;
  _nop_();
  _nop_();
  i = 5;
  j = 73;
  do
  {
    while (--j);
  }
  while (--i);
}

void  delay(unsigned long uldata)
{
  unsigned int j  =  0;
  unsigned int g  =  0;
  while(uldata--)
    Delay200us();
}

void Delay200ms()		//@22.1184MHz
{
  unsigned char i, j, k;

  i = 17;
  j = 208;
  k = 27;
  do
  {
    do
    {
      while (--k);
    }
    while (--j);
  }
  while (--i);
}

/***********************************************************
* ��    �ƣ� �жϴ�����
* ��    �ܣ�
* ��ڲ�����
* ���ڲ�����
* ˵    ����
* ���÷�����
**********************************************************/
void ExtInt0Handler(void) interrupt 0
{
  ProcessInt0();
}
/***********************************************************
* ��    �ƣ�ִ�к���
* ��    �ܣ�ʶ��ɹ���ִ�ж������ڴ˽����޸�
* ��ڲ����� ��
* ���ڲ�������
* ˵    ����
**********************************************************/
void 	User_handle(uint8 dat)
{
  if(0==dat)
  {
    G0_flag=ENABLE;
		PrintCom("�յ�\r\n");
    LED=0;
  }
  else if(ENABLE==G0_flag)
  {
    G0_flag=DISABLE;
    LED=1;
    switch(dat)
		{
		case CODE_1:	 /*�������*/
			motor_run(1);
			PrintCom("01 switch on\r\n");
			break;
		case CODE_2:	 /*����ء�*/
			motor_run(0);
			PrintCom("02 switch off\r\n");
			break;
		case CODE_3:	/*�����ʱ������*/
			TR0 = 1;
			flag = 1;
			PrintCom("03 switch on\r\n");
			break;
		case CODE_4:	 
			TR0 = 1;
			flag = 0;
			PrintCom("04 switch off\r\n");
			break;
		default:/*text.....*/
			break;
		}
  }
  else
  {
    PrintCom("please say keyword\r\n"); /*text.....*/
  }
}


void	motor_run(uint8_t mode)
{
	if(mode) //��ת
	{
		Delay200ms();
		SRD1 = 1;
		SRD2 = 0;
		Delay200ms();
		Delay200ms();
		SRD1 = 0;
		SRD2 = 1;
		Delay200ms();
		SRD2 = 0;
	}
	else //��ת
	{
		Delay200ms();
		SRD1 = 0;
		SRD2 = 1;
		Delay200ms();
		Delay200ms();
		SRD1 = 1;
		SRD2 = 0;
		Delay200ms();
		SRD1 = 0;
	}
}
void Delay120ms()		//@22.1184MHz
{
	unsigned char i, j, k;

	_nop_();
	_nop_();
	i = 11;
	j = 22;
	k = 219;
	do
	{
		do
		{
			while (--k);
		} while (--j);
	} while (--i);
}
