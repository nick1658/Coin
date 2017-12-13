
#ifndef __TQ2416_YQCOIN_H__
#define __TQ2416_YQCOIN_H__



#include "def.h"
#include "yqsqlite3.h"


typedef struct
{
	U32 total_money;
	U32 total_good;
	U32 total_ng;
	U32 total_coin;
	U32 coinnumber;
	S16 ad2_averaged_value;
}s_processed_coin_info;

extern s_processed_coin_info processed_coin_info;

#define COINCNUM 4     //Ŀǰ ���� ������ ���ұ���  �������� 
extern int alertname[COIN_TYPE_NUM];// ���־��� 

extern U32 coin_num[COIN_TYPE_NUM];    //������ ��������
void cy_precoincount(void);	// ��α����
extern  U16 ccstep;

extern  U32 ch0_counttemp;  // ǰһ��ͨ��0 ͨ����Ӳ�Ҽ��� 
extern  U32 ch1_counttemp;  // ǰһ��ͨ��1 ͨ����Ӳ�Ҽ��� 
extern  U32 ch2_counttemp;  // ǰһ��ͨ��2 ͨ����Ӳ�Ҽ��� 
extern  U32 coinlearnnumber;

void cy_coinlearn(void);   //��ѧϰ�ĳ���

extern  S16 coin_maxvalue0;
extern  S16 coin_minvalue0;
extern  S16 coin_maxvalue1;
extern  S16 coin_minvalue1;
extern  S16 coin_maxvalue2;
extern  S16 coin_minvalue2;


extern  U16 coine[COINCNUM][COIN_TYPE_NUM];

void detect_read(void);   

#define FREE_Q_FLAG 0xA0
#define COIN_NG_FLAG 0xA1
#define COIN_FULL_FLAG 0xA2
#define COIN_GOOD_FLAG 0xA3
#define COIN_PROCESSED_FLAG 0xA4

/*****************
����궨��    ����
*****************/

#define SWITCHERROR 11      //��û�й���
#define COINNGKICKERROR 12
#define DETECTERROR 13     // �Ҷ��� 
#define KICKCOINFULL 14     //ת�̶ı�
#define KICK1COINERROR 15
#define KICK2COINERROR 16
#define PRESSMLOCKED  17      //ѹ�Ҵ��±�
#define COUNT_FINISHED 18  	// 1 MCU�ӵ� ����ֶ����� ֹͣ	
#define ADSTDEEROR    20       //��ʾ���������б�
#define RTCREEROR    21       //��ʾREAD time wrong
#define READOUTDATA    22       // ����  ����  �ڴ���
#define COMPLETE_UPDATE 30


#define ALERT0 0  //���ַADDR_TTS ����"�����׼ֵ"
#define ALERT1 1   // //���ַADDR_TTS ����"�����׼ֵ"
#define ALERT3 3  //���ַADDR_TTS ����"ת�̶±�"
#define ALERT4 4   // //���ַADDR_TTS ����"ѹ�Ҵ��±�"
#define ALERT5 5   // �߱��쳣
#define ALERT6 6  //���ַADDR_TTS ����"�Ҷ���"
#define ALERT7 7   // //���ַADDR_TTS ����"ȷ���賤��1����Ч"
#define ALERT8 8   // //���ַADDR_TTS ����"�뵼�����ݣ��ڴ�����"
#define ALERT9 9   // //���ַADDR_TTS ����"��ֶ����� ��ȡ��"


#define ALERT20 20   // //���ַADDR_TTS ����"һԪԤ��ֵ�ѵ�"
#define ALERT21 21   // //���ַADDR_TTS ����"0.5ԪԤ��ֵ�ѵ�"
#define ALERT22 22   // //���ַADDR_TTS ����"0.1ԪԤ��ֵ�ѵ�"
#define ALERT23 23   // //���ַADDR_TTS ����"0.05ԪԤ��ֵ�ѵ�"
#define ALERT24 24   // //���ַADDR_TTS ����"0.02ԪԤ��ֵ�ѵ�"
#define ALERT25 25   // //���ַADDR_TTS ����"0.01ԪԤ��ֵ�ѵ�"
#define ALERT26 26   // //���ַADDR_TTS ����"10ԪԤ��ֵ�ѵ�"
#define ALERT27 27   // //���ַADDR_TTS ����"5ԪԤ��ֵ�ѵ�"

extern  U16 prepic_prenum;      // ���ڼ�¼ ����ǰ�Ľ��� 
void alertfuc(U16 alertflag); //����






#endif
