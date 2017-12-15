/**************************************************************
The initial Watch Dog
**************************************************************/
#include "S3C2416.h"

/*---------------------------------------------------------------------------*/


void wt_handler (void)
{
	cy_println( "WatchDog Handler");
}
//��ʼ�����Ź�
void watchdog_reset(void)      //���Ź���λ����
{
	cy_println( "WatchDog setup! %dS\n",WatchDogTimer/10000);
	rWTCON=((37<<8)|(3<<3)); 
	/*---------------------------------------------------*/
			//���Ź�ʱ������ T = WTCNT * t_watchdog
			//���Ź�ι��
	rWTDAT=WatchDogTimer;
	rWTCNT=WatchDogTimer;
	/*---------------------------------------------------*/
	rWTCON &= ~(2<<1);    //��ֹ���Ź��ж�
	//rWTCON |= (2<<1);    //�����Ź��ж�
	//IRQ_Register(INT_WDT_AC97, wt_handler);
	//IRQ_EnableInt(INT_WDT_AC97);

	rWTCON|=((1<<5)|(1<<0));   //�����Ź���ʱ������λ
//	while(1);
}
/*---------------------------------------------------------------------------*/
//���Ź�ι�� 
void SetWatchDog(void)
{
	rWTDAT=WatchDogTimer;
	rWTCNT=WatchDogTimer;
}
/*---------------------------------------------------------------------------*/
