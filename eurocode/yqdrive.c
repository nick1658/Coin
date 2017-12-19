#include "s3c2416.h"

volatile U32 runtime = 0;   // // ִ�м��õļ�ʱ����
unsigned short int runstep = 0;   //  �������������
volatile U32 time = 0;   // //��������ʱ�߱ҵ�����õļ�ʱ����


void deviceinit(void)	//�����Ȱ�ͨ���ϵıҵ���ȥ 
{
	int i = 0;
	int good_coin;
	
	//cy_println("begin init full coin num %d ...", coin_env.full_stack_num);
	for (i = 0; i < coin_env.full_stack_num; i++){//Ԥ�ü���ģʽʱ����ĳ��Ӳ�ҵļ���ֵ�ﵽԤ��ֵ���Ϳ��������Ӳ�ҵļ���
		//cy_println("begin init i = %d", i);
		good_coin = coin_env.full_coin_stack[i];
		if (coin_num[good_coin] >= *pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_set){
			coin_num[good_coin] = 0;
			*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_current) = 0;
			*pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_full_flag = 0;
		}
	}
	//cy_println("finish init = %d", i);	
	disp_allcount();
	coin_env.full_stack_num = 0;
	ch0_count =0;
	ch1_count =0;
	ch2_count =0;
	coin_env.ad0_step = 0;		
	coin_env.ad1_step = 0;
	coin_env.ad2_step = 0;
	ccstep = 0;
	
	for (i = 0; i < COIN_Q_LEN; i++){//��ʼ��Ӳ�Ҷ���
		coin_env.coin_Q[i] = FREE_Q_FLAG;
	}
	coin_env.coin_Q_index = 0;
	coin_env.coin_Q_remain = 0;
	
	processed_coin_info.coinnumber = 0;
	blockflag = ADBLOCKT;
	sys_env.stop_flag = 0;
	STORAGE_DIR_P();//��ת
	runstep =0; //�������������	
	//cy_println("finish init coin_env");		
}

void IR_detect_func(void)
{
////////////////////////////////////////////////////////////////////////////////////////////
	static int coin_in_flag = 0;
////////////////////////////////////////////////////////////////////////////////////////////
	if((COIN_DETECT == IR_DETECT_ON)){//������������ؼ�⵽Ӳ��
		if ((coin_in_flag == 0)){
			if ((para_set_value.data.coin_full_rej_pos == 2)){//��������˵ڶ����޳���ҹ���
				if(coin_env.coin_Q[coin_env.coin_Q_index] == COIN_FULL_FLAG){ // Ԥ�����ѵ��������
					if (coin_env.full_kick_Q[coin_env.full_kick_Q_index] == 0){
						coin_env.full_kick_Q[coin_env.full_kick_Q_index] = para_set_value.data.kick_start_delay_t2;
						coin_env.full_kick_Q_index++;
						coin_env.full_kick_Q_index %= KICK_Q_LEN;
					}else{//�޳���λ2����׷β����
						SEND_ERROR(KICK2COINERROR);
						dbg ("kick2 error alertflag = %d %s, %d", KICK2COINERROR,  __FILE__, __LINE__);
					}
				}else if(coin_env.coin_Q[coin_env.coin_Q_index] == FREE_Q_FLAG){ // �������г������󣬼�����׼
					SEND_ERROR(KICKCOINFULL);
					dbg ("kick2 error alertflag = %d %s, %d", KICKCOINFULL,  __FILE__, __LINE__);
				}else if(coin_env.coin_Q[coin_env.coin_Q_index] == COIN_NG_FLAG){ //���©�ߴ���
					SEND_ERROR(COINNGKICKERROR);
					dbg ("���©�ߴ��� alertflag = %d %s, %d", COINNGKICKERROR,  __FILE__, __LINE__);
				}		
				coin_env.coin_Q[coin_env.coin_Q_index] = FREE_Q_FLAG;
				coin_env.coin_Q_index++;
				coin_env.coin_Q_index %= COIN_Q_LEN;
			}
			coin_in_flag = 1;
		}
	}else{
		coin_in_flag = 0;
	}
}

void runfunction(void)   //������������
{
	switch (runstep){
		case 0:
			STORAGE_MOTOR_STARTRUN();	//  ��ת�̵��
			runstep = 1;
			break;
		case 1:
			break;
		case 20://Ԥ���������Զ�ͣ��
			STORAGE_MOTOR_STOPRUN();	//  ͣת�̵��
			runtime = 100; // 2��
			runstep = 100;
			break;
		case 40://�ֶ�ͣ��
			STORAGE_MOTOR_STOPRUN();	//  ͣת�̵��
			runtime = 100; // 2��
			runstep = 120;
			break;
		case 100:
			if (runtime == 0){
				prepic_num = JSJM;
				SEND_ERROR(COUNT_FINISHED);
			}
			break;
		case 120:
			if (runtime == 0){
				comscreen(Disp_Indexpic[JSJM],Number_IndexpicB);	 // back to the  picture before alert
				sys_env.workstep = 0;
			}
			break;
		default:break;
	}
}


