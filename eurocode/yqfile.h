


#ifndef __TQ2416_YQFILE_H__
#define __TQ2416_YQFILE_H__

#include "def.h"
extern S8 alertflag;  //�����־λ




//������  Ӳ�Ҳ���  ��ȡ�� ���� �������� 
#define BITNUM 60  //��ȡÿ���ļ���ֵλ��



//���ݱ��� ���� ����ֵ
//���ֶ���
#define CN0 0   // �����
#define CN1 1   // ŷԪ
#define CN2 2   // ��Ԫ
#define CN3 3   // Ӣ��

///////tubiao number of coin name 
#define CNNM0 0   // 1Ԫ
#define CNNM1 1		// 5��ͭ
#define CNNM2 2		// 5�Ǹ� 
#define CNNM3 3		// 1�Ǵ���
#define CNNM4 4		// 1��С��
#define CNNM5 5		// 5��
#define CNNM6 6		// 2��
#define CNNM7 7		// 1��
#define CNNM8 8		// 1��С��




#define MAXNUM0 15
#define MINNUM0 15
#define MAXNUM1 15
#define MINNUM1 15
#define MAXNUM2 15
#define MINNUM2 15

extern U8 coinchoose;    // ����ѡ��
#define WKTFORS 0x00  // 2*0.25S // ��ҵ�� ���ֹͣ  ʱ��
void yqi2c_init(void);   //Ӳ����ֻ� I2C��ʼ�� 



struct coinamount    
{
	U16 db_total_item_num;
	U32 ze_coin;
	U32 zs_coin;
	U32 fg_coin;
};
//extern  struct coinamount accoinamount[COINCNUM];

struct stdval_coin      //������ѧϰ   ���׼ֵ
{
	U16 stdnum;
	U16 std0;
	U16 std1;
	U16 std2;
};
extern  struct stdval_coin stdvalue_coin[COINCNUM][9];  //���ݱ��־���  ���� ��ֵ �Ļ�׼����ֵ

//////////////////////////////////////////////////////////////
struct preval_coin      //������ѧϰ    �����ֵ ��Сֵ  
{
	S16 coinnum;
	S16 coin_max0;
	S16 coin_min0;
	S16 coin_max1;
	S16 coin_min1;
	S16 coin_max2;
	S16 coin_min2;
};
extern struct preval_coin prevalue_coin[COINCNUM][9];// ���ݱ��־��� ��ǰ�ı��� Ӳ�� ��ѧϰֵ��Χ 

struct learndvalue
{
	U16 d0value;
	U16 d1value;
	U16 d2value;
};
//extern struct learndvalue prelearndvalue[COINCNUM][9];	   //���ݱ��ּ���� ѡ��Ƚ� ����ѧϰֵ�Ĳ�ֵ��Χ
//


#define COIN0STD0  pre_value.country[coinchoose].coin[CNNM0].data.std0
#define COIN0STD1  pre_value.country[coinchoose].coin[CNNM0].data.std1
#define COIN0STD2  pre_value.country[coinchoose].coin[CNNM0].data.std2


#define COIN1STD0  pre_value.country[coinchoose].coin[CNNM1].data.std0
#define COIN1STD1  pre_value.country[coinchoose].coin[CNNM1].data.std1
#define COIN1STD2  pre_value.country[coinchoose].coin[CNNM1].data.std2


#define COIN2STD0  pre_value.country[coinchoose].coin[CNNM2].data.std0
#define COIN2STD1  pre_value.country[coinchoose].coin[CNNM2].data.std1
#define COIN2STD2  pre_value.country[coinchoose].coin[CNNM2].data.std2


#define COIN3STD0  pre_value.country[coinchoose].coin[CNNM3].data.std0
#define COIN3STD1  pre_value.country[coinchoose].coin[CNNM3].data.std1
#define COIN3STD2  pre_value.country[coinchoose].coin[CNNM3].data.std2


#define COIN4STD0  pre_value.country[coinchoose].coin[CNNM4].data.std0
#define COIN4STD1  pre_value.country[coinchoose].coin[CNNM4].data.std1
#define COIN4STD2  pre_value.country[coinchoose].coin[CNNM4].data.std2


#define COIN5STD0  pre_value.country[coinchoose].coin[CNNM5].data.std0
#define COIN5STD1  pre_value.country[coinchoose].coin[CNNM5].data.std1
#define COIN5STD2  pre_value.country[coinchoose].coin[CNNM5].data.std2


#define  COIN6STD0  pre_value.country[coinchoose].coin[CNNM6].data.std0
#define  COIN6STD1  pre_value.country[coinchoose].coin[CNNM6].data.std1
#define  COIN6STD2  pre_value.country[coinchoose].coin[CNNM6].data.std2


#define COIN7STD0  pre_value.country[coinchoose].coin[CNNM7].data.std0
#define COIN7STD1  pre_value.country[coinchoose].coin[CNNM7].data.std1
#define COIN7STD2  pre_value.country[coinchoose].coin[CNNM7].data.std2


#define COIN8STD0  pre_value.country[coinchoose].coin[CNNM8].data.std0
#define COIN8STD1  pre_value.country[coinchoose].coin[CNNM8].data.std1
#define COIN8STD2  pre_value.country[coinchoose].coin[CNNM8].data.std2

extern U8 dgus_readt[6];  // only need y-m-d h:m  ��ʱ��
void print_func(void);   //��ӡ ���� 


// ����������ŵĸ�ֵ JM11 JM01 JM21L04 JM21L03 EM02 EM01 �� EM03
#define  SONUM 8     		 //������� ����   

#define  EM_STOP  7 	//EM03   SDDATA1/GPE8

#define EM_P04I1   5  //EM02  SDDATA0/GPE7
#define EM_P05I0   4  //EM02  SDDATA0/GPE7

#define  EM_KICK  4		//EM02  SDDATA0/GPE7
#define  EM_KICKBACK  5		//EM01 SDCMD/GPE6 

#define  MT_STORE  1	//JM01 chuansong SPIMOSI/GPE12 
#define  MT_PUSH  0	//JM11 PRESS nCD_SD/EINT16/GPG8 
#define  MT_PANRIGHT  2		//JM21 L03  SDDATA2/GPE9
#define  MT_PANLEFT  3	 //JM21 L04  SDDATA3/GPE10


#define DEVSTARTRUN 0	// �豸���� 
#define DEVSTOPRUN 1	// �豸ֹͣ���� 

#define BEEPSTARTRUN 1	// �豸���� 
#define BEEPSTOPRUN 0	// �豸ֹͣ���� 

// ���� ���� 1 �� ���� 1 ת���� ѹ�� ����  ���� 
#define SINUM 8     //  �������������� ����






#endif
