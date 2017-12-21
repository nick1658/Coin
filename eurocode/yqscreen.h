
#ifndef __TQ2416_YQSCREEN_H__
#define __TQ2416_YQSCREEN_H__


#include "def.h"


void comscreen(U8* str,short int length);  //���� ָ����Һ����
extern volatile U16 touch_flag;
extern volatile U16 uartcount;  // ����2���� �ֽ� ����

void touchget_from_uart0(void);     //receive the touch from dgus at real time 
void touchget_from_uart2(void);     //receive the touch from dgus at real time 
//int dgus_rtc(char str[]);  // read back the RTC
#define TIMEBP1 50     //beep time  for  kai ji
#define TIMEBP2 100     //beep time  for  alert
void dgus_tfbeep(int bztime); 			 	 //uart1,dgus deep control
void dgus_chinese(U16 addr,S8 str[],U16 strnum);    // dgus  chinese
void dgus_tf2word(int addr,long name);  	  //transfer 2word data variables to specify address 
void dgus_tf1word(int addr,long data);  	  //transfer 1word data variables to specify address 
void disp_KJAmount(void); // initial addr on zhu jiemian ze zs forge
void disp_data(int addr1,int addr2,int addr3); 	 //pre picture ,specify address display data variable
void disp_preselflearn(int max0,int min0,int max1,int min1,int max2,int min2);    //pre coin admax admin when self learning 
void counter_clear (void); //


/////////////////����/////////////////////////

#define	ADDR_CPZE 	0x0000 	 // ���ݱ�����ʾ �������� ���
#define	ADDR_CPZS 	0x0002 	 // ���ݱ�����ʾ   �������� ����
#define	ADDR_CPFG 	0x0004 	 // ���ݱ�����ʾ  �������� ���

#define ADDR_CRUN    0x06  	 // ����ֵ���� ��������  ������ť 
#define ADDR_PGH	0x0007   // ���ݱ�����ʾ  ����
#define ADDR_PGH1	0x07  	 // ����ֵ����   ����
#define	ADDR_TZBC 	0x08  	//  ����ֵ����
#define	ADDR_DBDATA 0x09  	//  ����ֵ���� 
#define	ADDR_BJZX 	0x0A  	//  ����ֵ����  bujian zhixing jiemian 

#define	ADDR_XD10 	0x000B  	// ���ݱ�����ʾ  �굥  jiemian 1
#define	ADDR_XD5 	0x000D  	// ���ݱ�����ʾ  �굥   jiemian 0.5
#define	ADDR_XD1 	0x000F  	// ���ݱ�����ʾ   �굥   jiemian 0.1
#define	ADDR_XD05 	0x0011  	// ���ݱ�����ʾ  �굥  jiemian 0.05
#define	ADDR_XD02 	0x0013  	// ���ݱ�����ʾ  �굥  jiemian 0.02
#define	ADDR_XD01 	0x0015  	// ���ݱ�����ʾ   �굥 jiemian 0.01
#define	ADDR_XDB1 	0x0017  	// ���ݱ�����ʾ  �굥   jiemian ��1 ������ŷԪ��7��
#define	ADDR_XDB2 	0x0019  	// ���ݱ�����ʾ   �굥  jiemian ��2 ������ŷԪ��8��
//�����
#define	ADDR_XD1000 	0x001B  	// ���ݱ�����ʾ   �굥  jiemian �����10Ԫ
#define	ADDR_XD500 	0x001D  	// ���ݱ�����ʾ   �굥  jiemian �����5Ԫ

#define	ADDR_XDZS 	ADDR_CPZS 	// ���ݱ�����ʾ  �굥  jiemian ZS 
#define	ADDR_XDFG 	ADDR_CPFG 	// ���ݱ�����ʾ  �굥  jiemian FORGE
//�굥   �ӱ���  ����ʹ�õĵ�ַ  001B -001C  001D- 001E
//#define ADDR_YZC0D  0x001F   	// ͼ�������ʾ   Ԥ��,	mianzhi  coin name ���0
#define ADDR_YZ_RESET  0x1F   		// ����ֵ����   Ԥ��,	mianzhi  coin name
//#define ADDR_YZSL  0x0020  		// ���ݱ�����ʾ ,  Ԥ��,		shuliang  
// 1f 20  0021 23 24 25 26 27 28 
#define ADDR_YRUN    0x22	//  ����ֵ����  Ԥ�ü���

#define	ADDR_YZC0JS 	0x0080 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 JI SHU	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC1JS 	0x0082 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC2JS 	0x0082 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC3JS 	0x0084 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC4JS 	0x0084 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC5JS 	0x0086 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC6JS 	0x0088 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC7JS 	0x008A 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC8JS 	0x0084 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
//�����
#define	ADDR_YZC9JS 	0x0590 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���10 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC10JS 	0x0592 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable ���5 ��Ԥ�ü������� ʵʱ��ʾ

#define	ADDR_YZC0ZT 	0x0090 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC1ZT 	0x0091 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC2ZT 	0x0091 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC3ZT 	0x0092 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC4ZT 	0x0092 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC5ZT 	0x0093 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC6ZT 	0x0094// ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC7ZT 	0x0095 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC8ZT 	0x0092 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC9ZT 	0x0098 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ
#define	ADDR_YZC10ZT 	0x0099 	 // ���ݱ�����ʾ ,  Ԥ��,YUZHI COIN 0 ZHUANG TAI	data variable ���0 ��Ԥ�ü������� ʵʱ��ʾ


#define	ADDR_YZS0 	0x0029 	 // ���ݱ�����ʾ ,  Ԥ��,	data variable
#define	ADDR_YZS1	0x002B  //  ���ݱ�����ʾ ,  Ԥ��,,	data variable
#define	ADDR_YZS2	0x002B  //  ���ݱ�����ʾ ,  Ԥ��,  , data variable
#define	ADDR_YZS3 	0x002D  //  ���ݱ�����ʾ ,  Ԥ��,,	data variable
#define	ADDR_YZS4	0x002D  //  ���ݱ�����ʾ ,  Ԥ��,	data variable
#define	ADDR_YZS5	0x002D  //  ���ݱ�����ʾ ,  Ԥ��, data variable
#define	ADDR_YZS6	0x002F  //  ���ݱ�����ʾ ,  Ԥ��, data variable
#define	ADDR_YZS7	0x0031  //  ���ݱ�����ʾ ,  Ԥ��, data variable
#define	ADDR_YZS8	0x0033  //  ���ݱ�����ʾ ,  Ԥ��, data variable

//�����
#define	ADDR_YZS9	0x0539  //  ���ݱ�����ʾ ,  Ԥ��10, data variable
#define	ADDR_YZS10	0x053b  //  ���ݱ�����ʾ ,  Ԥ��5, data variable

#define	ADDR_PRIT 	0x39  	//  ����ֵ���� print
//-003A
#define ADDR_CNTB   0x003B  //  ͼ�������ʾ  zi xuexi  jiemian learning ,coin name tubiao dispaly
#define ADDR_CNTB1   0x3B   //  ����ֵ����  ,  zi xuexi  jiemian learning ,coin name tubiao dispaly
#define ADDR_LRUN    0x3C	//   ����ֵ����  ,zi xuexi  jiemian run
#define	ADDR_A0MA 	0x003D  //   ���ݱ�����ʾ ,  zi xuexi  jiemian learning ,pre AD0  max
#define	ADDR_A0MI 	0x003E  //   ���ݱ�����ʾ , zi xuexi  jiemian learning ,pre AD0  min
#define	ADDR_A1MA 	0x003F  //   ���ݱ�����ʾ , zi xuexi  jiemian learning ,pre AD1  max
#define	ADDR_A1MI 	0x0040  //   ���ݱ�����ʾ , zi xuexi  jiemian learning ,pre AD1  min
#define	ADDR_A2MA 	0x0041  //   ���ݱ�����ʾ , zi xuexi  jiemian learning ,pre AD2  max
#define	ADDR_A2MI 	0x0042  //   ���ݱ�����ʾ , zi xuexi  jiemian learning ,pre AD2  min
#define ADDR_GSTB   0x0043  //   ͼ�������ʾ   zi xuexi  jiemian learning ,ganshe tubiao dispaly addr
#define GSKB  0x00   //������ǵ�ַ��ֻ��һ������ ��ʾѡ0ͼ�� no ganshe  tubiao

#define	ADDR_TTS 	0x0044  //    ͼ�������ʾ  ����Ҫ��ʼ�� baojing jiemian TXT tishi 

#define ADDR_KICK1_M	0x0045   //  ͼ�������ʾ  bujian zhixing jiemian, cyline 1
#define ADDR_KICK2_M	0x0046   //  ͼ�������ʾ  bujian zhixing jiemian, cyline 2
#define ADDR_STORAGE_MOTOR	0x0047   //  ͼ�������ʾ  bujian zhixing jiemian, pan motor right zheng
#define ADDR_DETCET1	0x0048   //  ͼ�������ʾ  bujian zhixing jiemian, pan motor left fan
#define ADDR_DEBUG	0x0049   //  ͼ�������ʾ  bujian zhixing jiemian, chuangsong motor
#define ADDR_MODE	0x004A   //  ͼ�������ʾ  bujian zhixing jiemian, yabi motor
#define ADDR_ZXLD	0x004B   //  ͼ�������ʾ  bujian zhixing jiemian, three  motor run together


#define ADDR_ZXD1	0x004C   //  ���ݱ�����ʾ ����Ҫ��ʼ��  bujian zhixing jiemian, detector 
#define ADDR_ZXD2	0x004D   //   ���ݱ�����ʾ  ����Ҫ��ʼ��  bujian zhixing jiemian, detector 
#define ADDR_ZXD3	0x004E   //   ���ݱ�����ʾ  ����Ҫ��ʼ��  bujian zhixing jiemian, detector 
#define ADDR_ZXD4	0x004F   //   ���ݱ�����ʾ   ����Ҫ��ʼ��  bujian zhixing jiemian, detector 
#define ADDR_ZXD5	0x0050   //   ���ݱ�����ʾ  ����Ҫ��ʼ��  bujian zhixing jiemian, detector 
#define ADDR_ZXD6	0x0051   //   ���ݱ�����ʾ  ����Ҫ��ʼ�� bujian zhixing jiemian, detector 
#define ADDR_ZXD7	0x0052   //   ���ݱ�����ʾ  ����Ҫ��ʼ�� bujian zhixing jiemian, detector 
#define ADDR_ZXD8	0x005C   //   ���ݱ�����ʾ  ����Ҫ��ʼ�� bujian zhixing jiemian, detector 
//0052
#define	ADDR_STDH 	0x0053  //  ���ݱ�����ʾ   ����Ҫ��ʼ��  jizhun tiaoshi jiemian h frequence std display,	data variable
#define	ADDR_STDM 	0x0054  //  ���ݱ�����ʾ    ����Ҫ��ʼ�� jizhun tiaoshi jiemian m frequence std display,	data variable
#define	ADDR_STDL 	0x0055  //  ���ݱ�����ʾ    ����Ҫ��ʼ�� jizhun tiaoshi jiemian l frequence std display,	data variable
#define	ADDR_STDT 	0x005A  //  ���ݱ�����ʾ    ����Ҫ��ʼ�� jizhun tiaoshi jiemian �¶�ֵ frequence std display,	data variable

#define	ADDR_KICK_DELAY_T1 	0x0056  //  ���ݱ�����ʾ  �޸ı��� ����Ҫ��ʼ��   canshu shezhi jiemian , kick out delay time
#define	ADDR_KICK_KEEP_T1 	0x0057  //  ���ݱ�����ʾ �޸ı��� ����Ҫ��ʼ��      canshu shezhi jiemian , kick back delay time
#define	ADDR_KICK_DELAY_T2 	0x0058  //  ���ݱ�����ʾ �޸ı��� ����Ҫ��ʼ��      canshu shezhi jiemian , pan motor blocked detector delay time
#define	ADDR_KICK_KEEP_T2 	0x0059  //  ���ݱ�����ʾ  �޸ı��� ����Ҫ��ʼ��     canshu shezhi jiemian , STORE motor blocked detector delay time
#define	ADDR_MOTOR_IDLE_T	0x005A 	//        canshu shezhi jiemian , STORE motor blocked detector delay time
#define	ADDR_PRE_COUNT_STOP_N 0x005B 	//        canshu shezhi jiemian 


//5E --5F
#define	ADDR_ZLE 	0x0060  // ���ݱ�����ʾ  zhu jiemian leiji money display,	data variable       NINTIAL  REALTIME  KEEPIN
#define	ADDR_ZLS	0x0062  // ���ݱ�����ʾ   zhu jiemian leiji number display,data variable       NINTIAL  REALTIME  KEEPIN
#define	ADDR_ZLF 	0x0064  // ���ݱ�����ʾ   zhu jiemian leiji forge display,	data variable       NINTIAL  REALTIME  KEEPIN
#define	ADDR_ZLH 	0x0066  // ���ݱ�����ʾ   zhu jiemian leiji history number display,	data variable       NINTIAL  REALTIME  KEEPIN
//68-6d
#define	ADDR_CNCH 	0x006F  //  ͼ�������ʾ ���� ͼ��
#define	ADDR_CNCH1 	0x6F 	//  ����ֵ����  , ���� ͼ��     


#define ADDR_LEVEL100 	0x200   // 1ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL50 	0x201   // 0.5ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL10 	0x202   // 0.1ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL5 	0x203   // 0.05ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL2 	0x204   // 0.02ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL1 	0x205   // 0.01ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL1000 	0x206   // 10ԪӲ�ҵ���ֵȼ�����
#define ADDR_LEVEL500 	0x207   // 5ԪӲ�ҵ���ֵȼ�����

#define ADDR_CPU_USAGE 0x300

//���� ����  
#define ALERT_MSG(TIPS,MSG) { \
	dgus_tfbeep(TIMEBP2); \
	prepic_prenum = prepic_num; \
	dgus_chinese(0x400, TIPS, strlen(TIPS)); \
	dgus_chinese(0x416, MSG, strlen(MSG)); \
	comscreen(Disp_Indexpic[BJJM],Number_IndexpicB); \
}

#define SEND_ERROR(CODE) { \
	alertflag = CODE; \
	sys_env.workstep =88; \
}
///////////////////////////////////////////////////

extern volatile U8 scrdis[5][50];  //���ݱ��� ȷ��ͼƬ
#define KJWC	scrdis[coinchoose][0]    //�������  10
#define KJYE1	scrdis[coinchoose][1]    //����Ԥ��2  6 
#define KJYE2	scrdis[coinchoose][2]    //����Ԥ��2  7
#define KJYE3	scrdis[coinchoose][3]    //����Ԥ��3  8
#define KJYE4	scrdis[coinchoose][4]    //����Ԥ��4  9

#define	JSJM	scrdis[coinchoose][5]    //��������11 13 15 133
#define JSYX	scrdis[coinchoose][6]    //��������17 19 21 135 
#define YCCS	scrdis[coinchoose][7]   //���ز���29 31 33 139 
#define XDJM	scrdis[coinchoose][8]   //�굥����35 37 39  141
#define YZJS	scrdis[coinchoose][9]   //Ԥ�ü���41 43 45  143
#define YZYX    scrdis[coinchoose][10]   //Ԥ������47 49 51   145
#define TZJM	scrdis[coinchoose][11]   //��������59 61 63  149
#define TZBC	scrdis[coinchoose][12]   //��������65 67 69 151
#define TZYX	scrdis[coinchoose][13]   //��������71 73 75 153
#define LSSJ	scrdis[coinchoose][14]   //��ʷ����77 79 81 155
#define BJJM	scrdis[coinchoose][15]   //�������� 83 85 87 157
#define BJCS	scrdis[coinchoose][16]   //��������89 91 93  159
#define JZTS	scrdis[coinchoose][17]   //��׼����95 97 99 161
#define LJXS	scrdis[coinchoose][18]    //�ۼ���ʾ107 109 111 165 
#define XTSZ	scrdis[coinchoose][19]   //ϵͳ����113 115 117  167  

#define Number_IndexpicA   255 
#define Number_IndexpicB  7 
extern U8 Disp1_Indexpic[Number_IndexpicB];         //MCU����ͼƬ�Ĵ�������  �л� ��ͼƬ ָ������
extern U8 Disp_Indexpic[Number_IndexpicA][Number_IndexpicB];         //MCU����ͼƬ�Ĵ�������  �л� ��ͼƬ ָ������

extern volatile U16 prepic_num; 	//��¼��ǰ�� ͼƬ�� ����ʾ��
S16 test_repete(void);   

#define TSGET_NUM 32 //����������֡��󳤶�
extern volatile U8 touchnum[TSGET_NUM];
extern volatile S32 db_id;   //��ʷ���� ����Ѿ���ʾ ��
void touchresult(void); 	 //manage the instruct from dgus
void  disp_allcount(void);	  // ȡ������ ��ʾ	 ��ǰ����  �굥
void disp_allcount_to_pc (void);


#define PRECOIN0  0   //����   Ԥ�ü���	 
#define PRECOIN1  1
#define PRECOIN2  2
#define PRECOIN3  3
#define PRECOIN4  4
#define PRECOIN5  5 
#define PRECOIN6  6 
#define PRECOIN7  7 
#define PRECOIN8  8 
#define PRECOIN9  9 
#define PRECOIN10  10 

struct precoin_num 	 //����   Ԥ�ü���	 
{
	U32 coinnum;
	U32 pre_count_set;
	U32 coinval; //����
	U32 full_flag;
};
extern struct precoin_num precoin_set[11];   //����   Ԥ�ü���	 
void disp_precount(long ze,long zs,long fg);      //   Ԥ�ü��� ����   ʵʱ ��ʾ  ���� ���  ����   Ԥ��ֵ  ����  ���


#define PRESETMAX 999999   // 6  ��ʾ  ֻ���赽7λ   9999999   7λ��ʾ���赽8λ
#define PRESLA  8
#define PRESLB  21
#define PREKICKMAX 99 

#endif
