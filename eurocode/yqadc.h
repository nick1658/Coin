
#ifndef __TQ2416_YQADC_H__
#define __TQ2416_YQADC_H__


#include "def.h"
#include "yqsqlite3.h"

#define KICK_Q_LEN 2
#define COIN_Q_LEN 16
#define SAMPLE_METHOD_1

typedef struct _AD_Value_S
{
	S16 use_index;
	S16 AD0;
	S16 AD1;
	S16 AD2;
} AD_Value;

typedef struct
{
	S16 compare_min0;
	S16 compare_max0;
	S16 compare_min1;
	S16 compare_max1;
	S16 compare_min2;
	S16 compare_max2;
}s_coin_compare_value;

typedef struct
{
	S16 std_down_value0;
	S16 std_up_value0;
	S16 ad0_averaged_value;
	S16 ad1_averaged_value;
	S16 ad2_averaged_value;
	S16 ad0_step;
	S16 ad1_step;
	S16 ad2_step;
	S16 cmp_use_index;
	U16 AD_min_index[3];
	U16 full_kick;
	U16 full_stack_num;
	U16 full_coin_stack[COIN_TYPE_NUM]; //���Ҷ�ջ
	U16 kick_remain;
	U16 kick_total;
	//S16 kick_start_t1;
	U16 kick_keep_t1;
	U16 kick_Q[KICK_Q_LEN];//�޳�����
	U16 kick_Q_index;
	U16 full_kick_remain;
	U16 full_kick_total;
	//S16 full_kick_start_t2;
	U16 full_kick_keep_t2;
	U16 full_kick_Q[KICK_Q_LEN];//�޳�����
	U16 full_kick_Q_index;
	
	U16 coin_Q[COIN_Q_LEN];//Ӳ�Ҿ�������
	U16 coin_Q_index;
	U16 coin_Q_remain;
}s_coin_env;

//����������    �����  ������������

//#define AD0STDVALUE  pre_value.country[coinchoose].coin[CNNM0].std0   //800   //3.25    ���ڲ��Բ��ȵĳ���
//#define AD1STDVALUE  pre_value.country[coinchoose].coin[CNNM0].std1  //940   //3.25
//#define AD2STDVALUE  pre_value.country[coinchoose].coin[CNNM0].std2  //940   //3.25
#define ADOFFSET  40


#define ADSAMPNUM0  16    //every ad channel sample number for getting average value 2  8 

#define  WAVEUP0  32   //1  //8ȷ�ϲ����½� ����  4
#define  WAVEMAX0  16 //1  //8 15   //ȷ�ϲ��η�ֵ  ���� 50 40   65     20
#define  WAVE0fall 16 //2 //15  //������ص��ο�ֵ���µļ���20 
#define  WAVEUPT0  16  //3 //4//8   // ȷ��˫����  ����һ ȷ�ϲ�������  ����   10 4      �󵼷�  3

#define WAVE_DOWN_VALUE 30
#define WAVE_UP_VALUE 30
#define WAVE_COIN_TWO_DOWN_VALUE 30


//��· ��׼ֵ ��׼ֵ ����   ��Ϊд�뵽1Ԫֵ��ȥ�������ο���ֵ ���бҵĳ�ʼ��Χ
#define AD0STDSET 900    //��Ƶ
#define AD1STDSET 900	 //��Ƶ
#define AD2STDSET 900	 //��Ƶ
#define ADADJACTERTIME 50


extern  U16 ad0_min;     //����ÿöӲ�ҹ�ȥ��� ���ֵ
extern  U16 ad1_min;     //����ÿöӲ�ҹ�ȥ��� ���ֵ
extern  U16 ad2_min;     //����ÿöӲ�ҹ�ȥ��� ���ֵ

#define ADBLOCKT  20  //10  //30     //130 00---30     //ʱ�� ��α������ ���� �±�ʱ��
extern volatile U16 blockflag;      //�±ұ�־����
extern volatile U32 adtime;

extern U32 ch0_count;  //ͨ��1 ͨ����Ӳ�Ҽ��� 
extern U32 ch1_count;  //ͨ��1 ͨ����Ӳ�Ҽ��� 
extern U32 ch2_count;  //ͨ��2 ͨ����Ӳ�Ҽ��� 

extern U32 ch0_coin_come;  //ͨ�� ��Ӳ�� ���
extern U32 coin_cross_time;
//extern U32 ch1_come;  //ͨ�� ��Ӳ�� ���
		
extern  S16 std_ad0;
extern  S16 std_ad1;
extern  S16 std_ad2;

void ad2_valueget(void);  //adc function   get value
void ad1_valueget(void);  //adc function   get value
void ad0_valueget(void);  //adc function   get value


extern  int temperstd;   //20��  20*10 +600  = 800MV;  800/3300 *1024 = 248.24  // �����������ѹ��ʽ VO = (10mV/C * T) + 600
U16 adstd_test(void);   //ֻ���ڵ�ѹֵ�ڱ�׼״̬ʱ�ſ����������еĵ����ADת��  ������ֵ������ 1010���Ұ�
U16 adstd_sample(void);    //��׼ֵ����  
U16 cy_adstd_adj (void);
void print_std_value(void);
void coin_env_init (void);

#define AD0STDNUM 10    //���׼ֵ ʱ�Ĳ�������
#define AD1STDNUM 10
#define AD2STDNUM 10


U16 adstd_offset(void);
#define OFFSETMAX 200
#define OFFSETMIN 200   //990


#define OFFSET1MAX 200
#define OFFSET1MIN 200   //990

#define OFFSET2MAX 200
#define OFFSET2MIN 200   //990


extern  s_coin_compare_value coin_cmp_value[COIN_TYPE_NUM];



/////////////////////////////////
#define AD_BUF_GROUP_LEN 2
#define DATA_BUF_LENGTH	2*1024
#define ADJ_BUF_LENGTH 4096
#define NG_BUF_LENGTH	4096
#define GOOD_BUF_LENGTH	4096
extern AD_Value AD_Value_buf[DATA_BUF_LENGTH];

//extern volatile AD_Value Adj_AD_Value_buf[ADJ_BUF_LENGTH];
extern AD_Value Detect_AD_Value_buf[AD_BUF_GROUP_LEN][DATA_BUF_LENGTH];
extern AD_Value *Detect_AD_Value_buf_p;


extern volatile AD_Value NG_value_buf[NG_BUF_LENGTH];//��Ҳ������ݻ�����
extern volatile AD_Value GOOD_value_buf[GOOD_BUF_LENGTH];//��Ҳ������ݻ�����
extern volatile U32 ng_value_index;
extern volatile U32 good_value_index;

extern volatile U32 sample_data_buf_index;
extern volatile U32 detect_sample_data_buf_index;
extern volatile U32 adj_data_buf_index;

extern volatile U32 start_sample;
//extern S16 coin_env.ad0_averaged_value;

extern int ad0_value_changed;

extern void send_sample_data (AD_Value ad_value_buf[], int counter);

void setStdValue (void);

extern int L_H_min;
extern int L_M_min;
extern int M_H_min;

extern int L_H_max;
extern int L_M_max;
extern int M_H_max;


extern s_coin_env coin_env;

void cy_ad0_valueget(void);
void cy_ad1_valueget(void);
void cy_ad2_valueget(void);

#endif

