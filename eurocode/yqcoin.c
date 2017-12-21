//////  ���������� ����  ��ѧϰ

#include "s3c2416.h"

/////////////////////////////////////////////////
 U16 ccstep = 0;

S16 coin_value0 =0;
S16 coin_value1 =0;
S16 coin_value2 =0;

 U32 ch0_counttemp =0;  // ǰһ��ͨ��0 ͨ����Ӳ�Ҽ��� 
 U32 ch1_counttemp =0;  // ǰһ��ͨ��1 ͨ����Ӳ�Ҽ��� 
 U32 ch2_counttemp =0;  // ǰһ��ͨ��2 ͨ����Ӳ�Ҽ��� 

 S16 coin_maxvalue0 = AD0STDSET;
 S16 coin_minvalue0 = AD0STDSET;
 S16 coin_maxvalue1 = AD1STDSET;
 S16 coin_minvalue1 = AD1STDSET;
 S16 coin_maxvalue2 = AD2STDSET;
 S16 coin_minvalue2 = AD2STDSET;
s_processed_coin_info processed_coin_info;

//4*9
U16 coine[COINCNUM][COIN_TYPE_NUM]=     // �ɱ��־���
{
{100,	50,	50,10,10,5,2,1,10,1000,	500},	//����� ����ֵ ����  ���ڼ�����
{200,	100,50,20,10,5,2,1,0, 0, 	0},//ŷԪ�� ����ֵ ����
{100, 	50,	25,10, 5,1,0,0,0, 0, 	0},//��Ԫ�� ����ֵ ����	 ʵ������100����û����������ֱ����0
{200,	100,50,20,10,5,2,1,0, 0, 	0},//Ӣ���� ����ֵ ����
};


void prepare_coin_cmp_value (void)
{		
	if (ad1_min > 0){
		coin_env.cmp_use_index = 1;
	}else if ( ad0_min > 0){
		coin_env.cmp_use_index = 0;
	}else if (ad2_min > 0){
		coin_env.cmp_use_index = 2;
	}else{
		coin_env.cmp_use_index = 0;
	}	
#ifdef SAMPLE_METHOD_0
	coin_value0 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD0;
	coin_value1 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD1 - coin_value0;
	coin_value2 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD2 - Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD1;
#endif
#ifdef SAMPLE_METHOD_1
	coin_value0 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD0;
	coin_value1 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD1;
	coin_value2 = Detect_AD_Value_buf_p[coin_env.AD_min_index[coin_env.cmp_use_index]].AD2;
#endif
	if (sys_env.AD_buf_sending == 0){
		sys_env.AD_buf_sending = 1;
		sys_env.Detect_AD_buf_p = Detect_AD_Value_buf_p;
		sys_env.AD_buf_index++;
		sys_env.AD_buf_index %= AD_BUF_GROUP_LEN;
		Detect_AD_Value_buf_p = Detect_AD_Value_buf[sys_env.AD_buf_index];
	}
}

S16 is_good_coin (void)
{
	//��α����ʱ���ϻ������Ż�һ��
	S16 i;
	for (i = 0; i < COIN_TYPE_NUM; i++){
		if   ( (( coin_value0 >= coin_cmp_value[i].compare_min0) && ( coin_value0 <= coin_cmp_value[i].compare_max0))
			&& (( coin_value1 >= coin_cmp_value[i].compare_min1) && ( coin_value1 <= coin_cmp_value[i].compare_max1))
			&& (( coin_value2 >= coin_cmp_value[i].compare_min2) && ( coin_value2 <= coin_cmp_value[i].compare_max2)))
		{
			if (sys_env.save_good_data){
				GOOD_value_buf[good_value_index].AD0 = coin_value0;
				GOOD_value_buf[good_value_index].AD1 = coin_value1;
				GOOD_value_buf[good_value_index].AD2 = coin_value2;
				GOOD_value_buf[good_value_index].use_index = coin_env.cmp_use_index;
				good_value_index++;
				if (good_value_index >= GOOD_BUF_LENGTH)
					good_value_index = 0;
			}
			return i;
		}	
	}
	if (sys_env.save_ng_data){
		NG_value_buf[ng_value_index].AD0 = coin_value0;
		NG_value_buf[ng_value_index].AD1 = coin_value1;
		NG_value_buf[ng_value_index].AD2 = coin_value2;
		NG_value_buf[ng_value_index].use_index = coin_env.cmp_use_index;
		ng_value_index++;
		if (ng_value_index >= NG_BUF_LENGTH)
			ng_value_index = 0;
	}
	return -1;
}

U32 coin_num[COIN_TYPE_NUM];    //������ ��������

void cy_precoincount(void)
{
	switch(ccstep)
	{
		case 0:
		{			
			if ( (ch0_counttemp != ch0_count) )	//mean there is a coin come
			{
				ch0_counttemp = ch0_count;
				//ch1_counttemp = ch1_count;
				//ch2_counttemp = ch2_count;
				
				processed_coin_info.coinnumber++;
				
				prepare_coin_cmp_value ();
				ccstep = 8; 
			}
			break;
		}	
		case 8:
		{	 			
			S16 good_coin;
			good_coin = is_good_coin ();
			if (sys_env.stop_flag != 1){//������ڷ�ת״̬
				sys_env.stop_flag = 0;
				sys_env.stop_time = STOP_TIME;//�ޱ�ͣ��ʱ��10��
			}
			if ((good_coin < 0) ||  ((para_set_value.data.coin_full_rej_pos == 1) && 
									 ((*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_set) == 0) ||
									  (*pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_full_flag == 1)))){ //�ٱ� ����ֵС��0
				if (coin_env.full_kick_Q[coin_env.full_kick_Q_index] == 0){
					coin_env.kick_Q[coin_env.kick_Q_index] = para_set_value.data.kick_start_delay_t1;
					coin_env.kick_Q_index++;
					coin_env.kick_Q_index %= KICK_Q_LEN;
					processed_coin_info.total_ng++;
					coin_env.coin_Q[coin_env.coin_Q_remain] = COIN_NG_FLAG;//
				}else{//�޳���λ1����׷β����
					SEND_ERROR(KICK1COINERROR);
					dbg ("kick1 error alertflag = %d %s, %d", KICK1COINERROR,  __FILE__, __LINE__);
				}
			}else {//���
				if (*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_set) == 9999){//ֻʹ����ֹ���
					coin_num[good_coin]++;
					processed_coin_info.total_money += pre_value.country[coinchoose].coin[good_coin].data.money;
					processed_coin_info.total_good++;
					coin_env.coin_Q[coin_env.coin_Q_remain] = COIN_GOOD_FLAG;//������޳���λ�޳�
				}else if((*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_set) == 0) ||
						  (*pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_full_flag == 1)){//�����ܴ���Ӳ�һ���Ԥ�����ѵ�
					coin_env.coin_Q[coin_env.coin_Q_remain] = COIN_FULL_FLAG;//������޳���λ�޳�
				}else{//Ԥ�ü���
					*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_current) += 1;
					coin_num[good_coin]++;
					processed_coin_info.total_money += pre_value.country[coinchoose].coin[good_coin].data.money;
					processed_coin_info.total_good++;
					coin_env.coin_Q[coin_env.coin_Q_remain] = COIN_GOOD_FLAG;//
					if( *(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_current) >= *(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_set)){// ��ǰ�ı���  ���� �ﵽ��Ԥ��ֵ
						*pre_value.country[COUNTRY_ID].coin[good_coin].data.p_pre_count_full_flag = 1; //����Ӳ��Ԥ���������������
						*(pre_value.country[COUNTRY_ID].coin[good_coin].data.p_coinval) += 1;
						coin_env.full_coin_stack[coin_env.full_stack_num] = good_coin;
						coin_env.full_stack_num++;
						if (coin_env.full_stack_num >= para_set_value.data.pre_count_stop_n){
							runstep = 20;   //��ʼͣ��
						}
					}
				}
				coin_env.coin_Q_remain++;//��α�������ͺ��⴫����֮���и�Ӳ��ѭ�����У����Ϊ16����ʾ֮�������Լ�16��Ӳ��
				coin_env.coin_Q_remain %= COIN_Q_LEN;
				
//				if(coin_env.coin_Q[coin_env.coin_Q_remain] != FREE_Q_FLAG){ // ��������׷β����/*�����Ȳ��ã���Ϊ�ڶ�������������ûװ*/
//					SEND_ERROR(DETECTERROR);
//					cy_println ("detect 2 error alertflag = %d %s, %d", DETECTERROR,  __FILE__, __LINE__);
//				}
			}
			processed_coin_info.total_coin++;
			ccstep = 0;
			sys_env.coin_over = 1;
			break;
		}		
	}
}


// ����ADֵ ������ȡ AD ���ֵ��Сֵ
U32 coinlearnnumber = 0;   //count the coin number all proceed
void cy_coinlearn(void)
{
	switch(ccstep)
	{
		case 0:{			
			if ( (ch0_counttemp != ch0_count) && (ch1_counttemp != ch1_count) && (ch2_counttemp != ch2_count)){	//mean there is a coin come
				ch0_counttemp = ch0_count;
				ch1_counttemp = ch1_count;
				ch2_counttemp = ch2_count;
				processed_coin_info.coinnumber++;
				coinlearnnumber++;
				ccstep = 1;				
			}
			break;
		}	
		case 1:{						
			prepare_coin_cmp_value ();	
			coin_env.coin_Q[coin_env.coin_Q_remain] = COIN_GOOD_FLAG;//������޳���λ�޳�
			coin_env.coin_Q_remain++;
			coin_env.coin_Q_remain %= COIN_Q_LEN;
		  
//			if(coin_env.coin_Q[coin_env.coin_Q_remain] != FREE_Q_FLAG){ // ��������׷β���� /*�����Ȳ��ã���Ϊ�ڶ�������������ûװ*/
//				SEND_ERROR(DETECTERROR);
//				cy_println ("detect 2 error alertflag = %d %s, %d", DETECTERROR,  __FILE__, __LINE__);
//			}
			ccstep = 8; 	
			break;
		}
		case 8:{//�ҳ����ֵ����Сֵ
			if( ( coin_value0 > coin_maxvalue0)){     //0
				coin_maxvalue0 = coin_value0;
			}
			if( ( coin_value0 < coin_minvalue0)){
				coin_minvalue0 = coin_value0;
			}
			if( ( coin_value1 > coin_maxvalue1)) {  //  1
				coin_maxvalue1 = coin_value1;
			}
			if( ( coin_value1 < coin_minvalue1)){
				coin_minvalue1 = coin_value1;
			}	
			if( ( coin_value2 > coin_maxvalue2)) {  //  2
				coin_maxvalue2 = coin_value2;
			}
			if( ( coin_value2 < coin_minvalue2)){
				coin_minvalue2 = coin_value2;
			}
			ccstep = 0; 
			sys_env.coin_over = 1;	
			break;
		}
		case 11:{
			dgus_tf1word(ADDR_A0MA,coin_maxvalue0);	//	 real time ,pre AD0  max
			dgus_tf1word(ADDR_A0MI,coin_minvalue0);	//	 real time ,pre AD0  min
			ccstep = 12; 
			break;	
		}
 		case 12:{
			dgus_tf1word(ADDR_A1MA,coin_maxvalue1);	//	 real time ,pre AD1  max	
			dgus_tf1word(ADDR_A1MI,coin_minvalue1);	//	 real time ,pre AD1  min
			ccstep = 13; 
			break;	
		}
		case 13:{
			dgus_tf1word(ADDR_A2MA,coin_maxvalue2);	//	 real time ,pre AD2  max
			dgus_tf1word(ADDR_A2MI,coin_minvalue2);	//	 real time ,pre AD2  min
			ccstep = 0; 
			break;			
		}	
											
	}
	return;
}
/*************************
**************************/
void detect_read(void)
{	
	if(COIN_DETECT == IR_DETECT_ON)    // ������䴫����
		dgus_tf1word(ADDR_DETCET1,1);	//���ڵ� 
	else
		dgus_tf1word(ADDR_DETCET1,0);	//δ���ڵ�	
}

const char *coin_tips [] = {"һԪ", "���", "���", "һ��", "һ��", "һ��", "���", "����", "һ��"};

 U16 prepic_prenum =0;      // ���ڼ�¼ ����ǰ�Ľ��� 
void alertfuc(U16 errorflag) //����
{
	char str_buf[256];
	dbg ("alert flag is %d", errorflag);
	switch(errorflag)
	{	
		case COINNGKICKERROR:
			ALERT_MSG ("��ʾ", "©�ߴ�����ע���飡");
			break;
		case DETECTERROR:
			ALERT_MSG ("��ʾ", "�ڶ����޳���λ���ۼ���û�м�����");
			break;
		case KICKCOINFULL:
			ALERT_MSG ("��ʾ", "�ڶ����޳���λ������׼ȷ����©�ߣ�");
			break;
		case KICK1COINERROR:
			ALERT_MSG ("��ʾ", "��һ���޳���λ�޳�����");
			break;	
		case KICK2COINERROR:
			ALERT_MSG ("��ʾ", "�ڶ����޳���λ�޳�����");
			break;		
		case PRESSMLOCKED:
			ALERT_MSG ("��ʾ", "����±ң��������򴫸������ٴ�����ǰ�������㣡");
			break;	
		case ADSTDEEROR:
			ALERT_MSG ("��ʾ", "�������쳣���������׼ֵ��Ȼ�����ԣ�");
			break;	
		case COUNT_FINISHED:
			switch (coin_env.full_stack_num){
				case 1:
					if (coin_env.full_coin_stack[0] < 8){
						sprintf (str_buf, "�����%s��ֽͲ��", coin_tips[coin_env.full_coin_stack[0]]);
					}else{
						sprintf (str_buf, "����Խ��: %d", coin_env.full_coin_stack[0]);
					}
					break;
				case 2:
					if ((coin_env.full_coin_stack[0] < 8) && 
						(coin_env.full_coin_stack[1] < 8)){
						sprintf (str_buf, "�����%s��%s��ֽͲ��", 	coin_tips[coin_env.full_coin_stack[0]], 
																	coin_tips[coin_env.full_coin_stack[1]]);
					}else{
						sprintf (str_buf, "����Խ��: %d, %d", coin_env.full_coin_stack[0], coin_env.full_coin_stack[1]);
					}
					break;
				case 3:
					if ((coin_env.full_coin_stack[0] < 8) && 
						(coin_env.full_coin_stack[1] < 8) &&
						(coin_env.full_coin_stack[2] < 8)){
						sprintf (str_buf, "�����%s��%s��%s��ֽͲ��", 	coin_tips[coin_env.full_coin_stack[0]], 
																		coin_tips[coin_env.full_coin_stack[1]], 
																		coin_tips[coin_env.full_coin_stack[2]]);
					}else{
						sprintf (str_buf, "����Խ��: %d, %d, %d", coin_env.full_coin_stack[0], coin_env.full_coin_stack[1], coin_env.full_coin_stack[2]);
					}
					break;
				default:sprintf (str_buf, "ERROR: 1001");break;
			}
			ALERT_MSG ("��ʾ", str_buf);
			break;	
		case COMPLETE_UPDATE:
			ALERT_MSG ("��ʾ", "���������ɣ�����������");
			break;
		default:
			ALERT_MSG ("��ʾ", "�쳣������������");
			break;
	}
	return;
}
	


