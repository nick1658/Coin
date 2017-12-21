/*============================================================================
 Name                : sqlite3.c
 Author              : linxijun
============================================================================*/
#include "s3c2416.h"

//2048�� 1��64ҳ 1ҳ2048�ֽ�  1��= 64*2048 = 131072 =128K  ��N��ĵ�ַ  64*2048*N + *
//���� ����Ϊ107K  ǰ���� ������ ����ô����ID�ı�����ͻô��  2*64*2048= 256K ���� һ��128K
//ǰ10�鲻Ҫ������ һ����Ϊһ������ 

// 10*64*2048 ����ַ ��10��湫������ 
//��11�鿪ʼ ��һ�����ֵ� ��ʼ��ַ�� ((11 + ��������)*64+ҳ��)*2048 + ҳ�ڵ�ַ ��һҳ�� �����ֵĲ����洢  2048���ֽ�  
//�ڶ�ҳ��ʼ ����ʷ���ݱ��� ((11 + ��������)*64+ҳ��)*2048+ (2048+ҳ�ڵ�ַ( ������*HISTORYSAVANUM))��Ϊ һ����ַ�� ����22�� 
//1000��ʾ�����������뵼����
//20150407֮ǰ��a5 5a
//20150408 SD�������� Ϊa7 7a
//20150408 �޵�Ƭ��SD��������Ϊa6 6a 
#define FLAG1NANDFLASH	0xa5  
#define FLAG2NANDFLASH	0x5a

u_coin_parameter_value para_set_value;
s_country_coin pre_value;

S16 test_read_r_code (S16 r_code)
{
	if (r_code == 0)
	{
		//cy_println ("Read OP Complete");  
		return 0;
	}
	else if (r_code == 1)
	{
		cy_println (" Parameter Error, Buf must be not null!!!");  
	}
	else if (r_code == 2)
	{
		cy_println ("ECC Error, Read Op failed!!!");  
	}
	return -1;
}
S16 test_write_r_code (S16 r_code)
{
	if (r_code == 0)
	{
		//cy_println ("Write OP Complete");  
		return 0;
	}
	else if (r_code == 1)
	{
		cy_println (" Parameter Error, Buf must be not null!!!");  
	}
	else if (r_code == 2)
	{
		cy_println ("BAD BLOCK, Write failed!!!");  
	}
	else if (r_code == 3)
	{
		cy_println ("BAD BLOCK, Mark failed!!!");  
	}
	else if (r_code == 4)
	{
		cy_println ("BAD BLOCK, Mark Succeesfull!!!");  
	}
	return -1;
}
S16 test_erase_r_code (S16 r_code)
{
	if (r_code == 0)
	{
		//cy_println ("Erase OP Complete");  
		return 0;
	}
	else if (r_code == 3)
	{
		cy_println ("BAD BLOCK, Mark failed!!!");  
	}
	else if (r_code == 4)
	{
		cy_println ("BAD BLOCK, Mark Succeesfull!!!");  
	}
	return -1;
}

void initial_nandflash(void)    //nandflash
{

	U16 i=0, j;

	cy_println ("sizeof (s_coin_parameter_value)   = %d", sizeof (s_coin_parameter_value));
	cy_println ("sizeof (u_coin_cmp_value) = %d", sizeof (u_coin_cmp_value));
	
	cy_println ("sizeof (para_set_value)   = %d", sizeof (para_set_value));
	cy_println ("sizeof (u_coin_pre_value) = %d", sizeof (u_coin_pre_value));
	
	ASSERT(sizeof (para_set_value) > PAGE_BYTE_SIZE);
	ASSERT(sizeof (u_coin_pre_value) > PAGE_BYTE_SIZE);
	
	cy_println ("Start Check Data ...!");
	test_read_r_code (Nand_ReadPage(PUBULIC_DATA_START_BLOCK_NUM, PUBULIC_DATA_START_PAGE_NUM, (U8 *)&para_set_value));
		
	if(para_set_value.data.magic_num != MAGIC_NUM){    //������������Ҫ��ʼ��    ����ÿ�����ҵ���Ϣ�� ��ʼ���� �ڽ������Ӳ��ʱ���õ���ֵ��ȷ��
		cy_print("#####   FIRST USE ##### \r\n");
		para_set_value.data.magic_num = MAGIC_NUM;
		para_set_value.data.coin_id = 0;
		para_set_value.data.country_id = 0;
		para_set_value.data.db_total_item_num = 0;
		para_set_value.data.op_id = 0;
		para_set_value.data.rej_level = 0;
		para_set_value.data.kick_start_delay_t1 = 1;
		para_set_value.data.kick_start_delay_t2 = 1;
		para_set_value.data.kick_keep_t1 = 80;
		para_set_value.data.kick_keep_t2 = 80;
		para_set_value.data.coin_full_rej_pos = 1;
		para_set_value.data.motor_idle_t = 400;
		para_set_value.data.adj_offset_position = 4096;
		para_set_value.data.pre_count_stop_n = 1;
		
		for (i = 0; i < COIN_TYPE_NUM; i++){
			para_set_value.data.precoin_set_num[i] = 9999;
		}
	
		test_erase_r_code (Nand_EraseBlock(PUBULIC_DATA_START_BLOCK_NUM ));
		cy_println ("erase block %d completed", PUBULIC_DATA_START_BLOCK_NUM);
		test_write_r_code (Nand_WritePage(PUBULIC_DATA_START_BLOCK_NUM, PUBULIC_DATA_START_PAGE_NUM, (U8 *)&para_set_value));
		cy_println ("write block %d page %d completed", PUBULIC_DATA_START_BLOCK_NUM, PUBULIC_DATA_START_PAGE_NUM);
		
		for (i = 0; i < COUNTRY_NUM; i++){
			for (j = 0; j < COIN_TYPE_NUM; j++){
				pre_value.country[i].coin[j].data.max0 = 1023;
				pre_value.country[i].coin[j].data.min0 = 1023;
				pre_value.country[i].coin[j].data.max1 = 1023;
				pre_value.country[i].coin[j].data.min1 = 1023;
				pre_value.country[i].coin[j].data.max2 = 1023;
				pre_value.country[i].coin[j].data.min2 = 1023;
				pre_value.country[i].coin[j].data.std0 = 900;
				pre_value.country[i].coin[j].data.std1 = 900;
				pre_value.country[i].coin[j].data.std2 = 900;
				pre_value.country[i].coin[j].data.offsetmax0 = 10;
				pre_value.country[i].coin[j].data.offsetmin0 = -10;
				pre_value.country[i].coin[j].data.offsetmax1 = 10;
				pre_value.country[i].coin[j].data.offsetmin1 = -10;
				pre_value.country[i].coin[j].data.offsetmax2 = 10;
				pre_value.country[i].coin[j].data.offsetmin2 = -10;
			}
			test_write_r_code (Nand_WritePage(COUNTRY0_COIN_PRE_VALUE_START_BLOCK_NUM, (COUNTRY0_COIN_PRE_VALUE_START_PAGE_NUM + i), (U8 *)(&(pre_value.country[i]))));
			cy_println ("write block %d page %d completed", 
						(COUNTRY0_COIN_PRE_VALUE_START_BLOCK_NUM ),
						(COUNTRY0_COIN_PRE_VALUE_START_PAGE_NUM + i)); 
		}
	}
	else
	{
		cy_println ("Check Data Completed!");
		//read_coin_value(); 	//���� �������� ����
	}
	
}


void Writekick_value(void)		//д�� ��ǰ�߱�ʱ��  �߱ҳ���ʱ�� ת�̶�תʱ��  ���ʹ�����ʱ�� 
{
	write_para ();	
}

void read_kick_value (void)
{
	test_read_r_code (Nand_ReadPage(PUBULIC_DATA_START_BLOCK_NUM, PUBULIC_DATA_START_PAGE_NUM, (U8 *)&para_set_value));
}
void write_kick_value (void)
{
	test_write_r_code (Nand_WritePage(PUBULIC_DATA_START_BLOCK_NUM, PUBULIC_DATA_START_PAGE_NUM, (U8 *)&para_set_value));
}

void read_para(void)  //���� ��ǰ���� ��ʷ���� ��
{
	read_kick_value ();
	read_coin_value ();
}

void write_para (void)	//д�� ��ǰ���� ��ʷ���� ��
{
	test_erase_r_code (Nand_EraseBlock(PUBULIC_DATA_START_BLOCK_NUM ));
	write_kick_value ();
	write_coin_value ();
}

						
void read_coin_value(void) 	 // read  COIN  0--8
{
	U32 i;
	for (i = 0; i < COUNTRY_NUM; i++)
	{
		test_read_r_code (Nand_ReadPage(COUNTRY0_COIN_PRE_VALUE_START_BLOCK_NUM, COUNTRY0_COIN_PRE_VALUE_START_PAGE_NUM + i, (U8 *)(&(pre_value.country[i]))));
	}
	
	//ӳ��Ӳ�ҽ��
	pre_value.country[COUNTRY_ID].coin[0].data.money = MONEY_1_00; //һԪ
	pre_value.country[COUNTRY_ID].coin[1].data.money = MONEY_0_50; //���ͭ
	pre_value.country[COUNTRY_ID].coin[2].data.money = MONEY_0_50; //��Ǹ�
	pre_value.country[COUNTRY_ID].coin[3].data.money = MONEY_0_10; //һ�Ǵ���
	pre_value.country[COUNTRY_ID].coin[4].data.money = MONEY_0_10; //һ��С��
	pre_value.country[COUNTRY_ID].coin[5].data.money = MONEY_0_10; //һ��С��
	pre_value.country[COUNTRY_ID].coin[6].data.money = MONEY_0_05; //���
	pre_value.country[COUNTRY_ID].coin[7].data.money = MONEY_0_02; //����
	pre_value.country[COUNTRY_ID].coin[8].data.money = MONEY_0_01; //һ��
	pre_value.country[COUNTRY_ID].coin[9].data.money = MONEY_10_00; //�����10Ԫ
	pre_value.country[COUNTRY_ID].coin[10].data.money = MONEY_5_00; //�����5Ԫ
	
	//ӳ��Ԥ�ü���ֵ
	pre_value.country[COUNTRY_ID].coin[0].data.coin_type = 0;
	pre_value.country[COUNTRY_ID].coin[1].data.coin_type = 1;
	pre_value.country[COUNTRY_ID].coin[2].data.coin_type = 1;
	pre_value.country[COUNTRY_ID].coin[3].data.coin_type = 2;
	pre_value.country[COUNTRY_ID].coin[4].data.coin_type = 2;
	pre_value.country[COUNTRY_ID].coin[5].data.coin_type = 2;
	pre_value.country[COUNTRY_ID].coin[6].data.coin_type = 6;
	pre_value.country[COUNTRY_ID].coin[7].data.coin_type = 7;
	pre_value.country[COUNTRY_ID].coin[8].data.coin_type = 8;
	pre_value.country[COUNTRY_ID].coin[9].data.coin_type = 9;
	pre_value.country[COUNTRY_ID].coin[10].data.coin_type = 10;
	
	
	//ӳ�䴥����Ԥ�ü���Ԥ��ֵ��ʾ��ַ
	pre_value.country[COUNTRY_ID].coin[0].data.hmi_pre_count_cur_addr = ADDR_YZC0JS; //һԪ
	pre_value.country[COUNTRY_ID].coin[1].data.hmi_pre_count_cur_addr = ADDR_YZC1JS; //���ͭ
	pre_value.country[COUNTRY_ID].coin[2].data.hmi_pre_count_cur_addr = ADDR_YZC2JS; //��Ǹ�
	pre_value.country[COUNTRY_ID].coin[3].data.hmi_pre_count_cur_addr = ADDR_YZC3JS; //һ�Ǵ���
	pre_value.country[COUNTRY_ID].coin[4].data.hmi_pre_count_cur_addr = ADDR_YZC4JS; //һ��С��
	pre_value.country[COUNTRY_ID].coin[5].data.hmi_pre_count_cur_addr = ADDR_YZC5JS; //һ��С��
	pre_value.country[COUNTRY_ID].coin[6].data.hmi_pre_count_cur_addr = ADDR_YZC6JS; //���
	pre_value.country[COUNTRY_ID].coin[7].data.hmi_pre_count_cur_addr = ADDR_YZC7JS; //����
	pre_value.country[COUNTRY_ID].coin[8].data.hmi_pre_count_cur_addr = ADDR_YZC8JS; //һ��
	pre_value.country[COUNTRY_ID].coin[9].data.hmi_pre_count_cur_addr = ADDR_YZC9JS; //�����10Ԫ
	pre_value.country[COUNTRY_ID].coin[10].data.hmi_pre_count_cur_addr = ADDR_YZC10JS; //�����5Ԫ
	//ӳ�䴥����Ԥ�ü���Ԥ������ֵ��ʾ��ַ
	pre_value.country[COUNTRY_ID].coin[0].data.hmi_pre_count_set_addr = ADDR_YZS0; //һԪ
	pre_value.country[COUNTRY_ID].coin[1].data.hmi_pre_count_set_addr = ADDR_YZS1; //���ͭ
	pre_value.country[COUNTRY_ID].coin[2].data.hmi_pre_count_set_addr = ADDR_YZS2; //��Ǹ�
	pre_value.country[COUNTRY_ID].coin[3].data.hmi_pre_count_set_addr = ADDR_YZS3; //һ�Ǵ���
	pre_value.country[COUNTRY_ID].coin[4].data.hmi_pre_count_set_addr = ADDR_YZS4; //һ��С��
	pre_value.country[COUNTRY_ID].coin[5].data.hmi_pre_count_set_addr = ADDR_YZS5; //һ��С��
	pre_value.country[COUNTRY_ID].coin[6].data.hmi_pre_count_set_addr = ADDR_YZS6; //���
	pre_value.country[COUNTRY_ID].coin[7].data.hmi_pre_count_set_addr = ADDR_YZS7; //����
	pre_value.country[COUNTRY_ID].coin[8].data.hmi_pre_count_set_addr = ADDR_YZS8; //һ��
	pre_value.country[COUNTRY_ID].coin[9].data.hmi_pre_count_set_addr = ADDR_YZS9; //�����10Ԫ
	pre_value.country[COUNTRY_ID].coin[10].data.hmi_pre_count_set_addr = ADDR_YZS10; //�����5Ԫ
	
	//ӳ�䴥����Ԥ��״̬ͼ����ʾ��ַ
	pre_value.country[COUNTRY_ID].coin[0].data.hmi_state_ico_addr = ADDR_YZC0ZT; //һԪ
	pre_value.country[COUNTRY_ID].coin[1].data.hmi_state_ico_addr = ADDR_YZC1ZT; //���ͭ
	pre_value.country[COUNTRY_ID].coin[2].data.hmi_state_ico_addr = ADDR_YZC2ZT; //��Ǹ�
	pre_value.country[COUNTRY_ID].coin[3].data.hmi_state_ico_addr = ADDR_YZC3ZT; //һ�Ǵ���
	pre_value.country[COUNTRY_ID].coin[4].data.hmi_state_ico_addr = ADDR_YZC4ZT; //һ��С��
	pre_value.country[COUNTRY_ID].coin[5].data.hmi_state_ico_addr = ADDR_YZC5ZT; //һ��С��
	pre_value.country[COUNTRY_ID].coin[6].data.hmi_state_ico_addr = ADDR_YZC6ZT; //���
	pre_value.country[COUNTRY_ID].coin[7].data.hmi_state_ico_addr = ADDR_YZC7ZT; //����
	pre_value.country[COUNTRY_ID].coin[8].data.hmi_state_ico_addr = ADDR_YZC8ZT; //һ��
	pre_value.country[COUNTRY_ID].coin[9].data.hmi_state_ico_addr = ADDR_YZC9ZT; //�����10Ԫ
	pre_value.country[COUNTRY_ID].coin[10].data.hmi_state_ico_addr = ADDR_YZC10ZT; //�����5Ԫ
	//ӳ�䴥����Ԥ��ֵ��������ʾ��ַ
	pre_value.country[COUNTRY_ID].coin[0].data.hmi_pre_count_reach_alarm_addr = ALERT20; //һԪ
	pre_value.country[COUNTRY_ID].coin[1].data.hmi_pre_count_reach_alarm_addr = ALERT21; //���ͭ
	pre_value.country[COUNTRY_ID].coin[2].data.hmi_pre_count_reach_alarm_addr = ALERT21; //��Ǹ�
	pre_value.country[COUNTRY_ID].coin[3].data.hmi_pre_count_reach_alarm_addr = ALERT22; //һ��С��
	pre_value.country[COUNTRY_ID].coin[4].data.hmi_pre_count_reach_alarm_addr = ALERT22; //һ��С��
	pre_value.country[COUNTRY_ID].coin[5].data.hmi_pre_count_reach_alarm_addr = ALERT23; //���
	pre_value.country[COUNTRY_ID].coin[6].data.hmi_pre_count_reach_alarm_addr = ALERT24; //����
	pre_value.country[COUNTRY_ID].coin[7].data.hmi_pre_count_reach_alarm_addr = ALERT25; //һ��
	pre_value.country[COUNTRY_ID].coin[8].data.hmi_pre_count_reach_alarm_addr = ALERT22; //һ�Ǵ���
	pre_value.country[COUNTRY_ID].coin[9].data.hmi_pre_count_reach_alarm_addr = ALERT26; //�����10Ԫ
	pre_value.country[COUNTRY_ID].coin[10].data.hmi_pre_count_reach_alarm_addr = ALERT27; //�����5Ԫ
	
	for (i = 0; i < COIN_TYPE_NUM; i++)
	{
		//precoin_set[pre_value.country[COUNTRY_ID].coin[i].data.coin_type].pre_count_set = 50;
		//para_set_value.data.precoin_set_num[i] = 50;
		pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_set 	= &precoin_set[pre_value.country[COUNTRY_ID].coin[i].data.coin_type].pre_count_set;
		pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_current = &precoin_set[pre_value.country[COUNTRY_ID].coin[i].data.coin_type].coinnum;
		pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_full_flag = &precoin_set[pre_value.country[COUNTRY_ID].coin[i].data.coin_type].full_flag;
		pre_value.country[COUNTRY_ID].coin[i].data.p_coinval = &precoin_set[pre_value.country[COUNTRY_ID].coin[i].data.coin_type].coinval;
		
		*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_set = para_set_value.data.precoin_set_num[pre_value.country[COUNTRY_ID].coin[i].data.coin_type];
		*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_full_flag = 0;
	}
}


void write_coin_value (void)
{
	U16 i;
	
	for (i = 0; i < COUNTRY_NUM; i++)
	{
		test_write_r_code (Nand_WritePage(COUNTRY0_COIN_PRE_VALUE_START_BLOCK_NUM, (COUNTRY0_COIN_PRE_VALUE_START_PAGE_NUM + i), (U8 *)(&(pre_value.country[i]))));
	}
}

void ini_screen (void)
{
	int i;
	dgus_tf1word(ADDR_CNCH,coinchoose);  //���ұ���ͼ�����
	dgus_tf1word(ADDR_PGH,para_set_value.data.coin_full_rej_pos);   // ����޳�����
	dgus_tf1word(ADDR_KICK_DELAY_T1, para_set_value.data.kick_start_delay_t1);	//��һ���߱���ʱʱ��
	dgus_tf1word(ADDR_KICK_KEEP_T1, para_set_value.data.kick_keep_t1);	//��һ���߱ұ���ʱ��
	dgus_tf1word(ADDR_KICK_DELAY_T2, para_set_value.data.kick_start_delay_t2);	//�ڶ����߱���ʱʱ��
	dgus_tf1word(ADDR_KICK_KEEP_T2, para_set_value.data.kick_keep_t2);	//�ڶ����߱ұ���ʱ��
	dgus_tf1word(ADDR_MOTOR_IDLE_T, para_set_value.data.motor_idle_t);	//�ޱҿ�ת�ȴ�ʱ��
	dgus_tf1word(ADDR_PRE_COUNT_STOP_N, para_set_value.data.pre_count_stop_n);	//����ͣ����������Ϊ1������һ��Ӳ�ҴﵽԤ������ͣ��
	dgus_tf1word(ADDR_LEVEL100,cn0copmaxc0[coinchoose]);	//��ֵȼ�����ʱû������
	

	//���� ��ÿ����ַ��ֵ����ʼ����һ��ֵ
	disp_KJAmount(); // initial addr on zhu jiemian ze zs forge
	disp_allcount(); //initial addr on xiandan jiemian
	disp_data(ADDR_CPZE,ADDR_CPZS,ADDR_CPFG);			//initial addr on jishu jiemian  ze zs forge data variable 
	
	dgus_tf1word(ADDR_CNTB,sys_env.coin_index);	//initial addr on zixuexi jiemian coin name tubiao
	disp_preselflearn(pre_value.country[coinchoose].coin[sys_env.coin_index].data.max0,pre_value.country[coinchoose].coin[sys_env.coin_index].data.min0,
					pre_value.country[coinchoose].coin[sys_env.coin_index].data.max1,pre_value.country[coinchoose].coin[sys_env.coin_index].data.min1,
					pre_value.country[coinchoose].coin[sys_env.coin_index].data.max2,pre_value.country[coinchoose].coin[sys_env.coin_index].data.min2);	//initial addr on zixuexi jiemian value 	

	dgus_tf1word(ADDR_GSTB,GSKB);	//initial addr on zixuexijiemian ganshe tubiao


	dgus_tf1word(ADDR_MODE, para_set_value.data.system_mode); 
	for (i = 0; i < COIN_TYPE_NUM; i++){
		if (para_set_value.data.system_mode == 0){
			*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_set = para_set_value.data.precoin_set_num[i];
		}else{
			*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_set = 9999;
		}
		dgus_tf1word(pre_value.country[COUNTRY_ID].coin[i].data.hmi_pre_count_set_addr, *pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_set);	//Ԥ��ֵ 
		dgus_tf1word(pre_value.country[COUNTRY_ID].coin[i].data.hmi_pre_count_cur_addr, 0);	//����ֵ ����
	}
}

void ini_picaddr(void) //�����л�ʱ�� ��ʼ����ַ����
{
	int pi;
	read_para();		 //���� ���뵱ǰ ��ǰ�ۼ��ܶ�  ��ǰ���� ��ǰ���
	// ������ ����	������ʼ��Ϊ��
	ch0_count = 0;	   //ÿ��ͨ����Ӳ����  ��ʼ��
	ch1_count = 0;
	ch2_count = 0;
			
	coin_env.ad0_step = 0;		// ������ ����ų�ʼ��
	coin_env.ad1_step = 0;
	coin_env.ad2_step = 0;
	ccstep = 0;
	
	for(pi = 0;pi<COIN_TYPE_NUM;pi++)
	{
		coin_num[pi] = 0;
		precoin_set[pi].pre_count_set = pi + 3;	 //Ԥ��ֵ  ��ʼ��Ϊ0
		precoin_set[pi].coinnum = 0;
		precoin_set[pi].full_flag = 0;
		precoin_set[pi].coinval = 0;
	}
	processed_coin_info.total_good = 0;
	processed_coin_info.total_money = 0;
	processed_coin_info.total_coin = 0;
	processed_coin_info.total_ng = 0;
	processed_coin_info.coinnumber =0;
	sys_env.coin_index = 0;
	return;		
}


void yqsql_exec(U16 chos)    
{
	switch(chos)
	{
		case DBINSERT:
		{
//		  	char dbsave[HISTORYSAVANUM] ={0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,0x00, 0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00, 0x00,0x00,0x00,0x00,}; 
//			U8 str_db[20];
									      //����				//����					   //���� //���				//����				//���		
			s_db_item_info * db_item_info_temp;
			s_db_item_block * db_item_block_temp;
			U16 item_index;
			U16 i = 0;
			U8 yqnddata[PAGE_BYTE_SIZE];
			for(i = 0; i < PAGE_BYTE_SIZE; i++)
			{
				yqnddata[i] = 0xff;
			}

			

			cy_println("Block %d page = %d, db_total_item_num = %d",
				HISTORY_START_BLOCK_NUM, 
				HISTORY_START_PAGE_NUM + (para_set_value.data.db_total_item_num * ITEM_SIZE / PAGE_BYTE_SIZE),
				para_set_value.data.db_total_item_num
				);
			if(para_set_value.data.db_total_item_num >= 4096)    //����4096��
			{
				SEND_ERROR(READOUTDATA);							
			}

			
			Nand_ReadPage(HISTORY_START_BLOCK_NUM, HISTORY_START_PAGE_NUM + (para_set_value.data.db_total_item_num * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);
			
			item_index = para_set_value.data.db_total_item_num % PAGE_ITEM_NUM_SIZE;
			db_item_block_temp = (s_db_item_block *)yqnddata;
			db_item_info_temp = (s_db_item_info *)&(db_item_block_temp->item_info_array[item_index]);	
			db_item_info_temp->index = para_set_value.data.db_total_item_num;

			while(1)     // read time 
			{
				comscreen(dgus_readt,6);	//read time
				while(touch_flag ==0){;}
					
				if (touchnum[7]>0)
				{
					db_item_info_temp->time[0] = touchnum[6];	
					db_item_info_temp->time[1] = touchnum[7];	
					db_item_info_temp->time[2] = touchnum[8];	
					db_item_info_temp->time[3] = touchnum[10];	
					db_item_info_temp->time[4] = touchnum[11];					
					touch_flag = 0;
					break;
				}
				else
				{
					touch_flag = 0;
				}
			}

			db_item_info_temp->ID = para_set_value.data.coin_full_rej_pos;
			db_item_info_temp->money = processed_coin_info.total_money;
			db_item_info_temp->total_good = processed_coin_info.total_good;		
			db_item_info_temp->total_ng = processed_coin_info.total_ng;	
			Nand_WritePage(HISTORY_START_BLOCK_NUM, (para_set_value.data.db_total_item_num * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);   //rNF_WritePage(YQNDHISTORYPAGENUM,yqnddata);	 //������Ϣ��һҳ	20��ĵ�0ҳ д��

			para_set_value.data.db_total_item_num += 1;
			write_para();

			cy_println("save over");
			cmd ();
			break;
		}
		case DBDELETE:
		{		
			Nand_EraseBlock(HISTORY_START_BLOCK_NUM);
			para_set_value.data.db_total_item_num = 0;
			write_para();
			cy_println("delete block %d over", HISTORY_START_BLOCK_NUM);
			cmd ();
			break;
		}
		case DBDISPLAY:
		{

			
			U8 r_code;
			s_db_item_info * db_item_info_temp;
			s_db_item_block * db_item_block_temp;
			U8 yqnddata[PAGE_BYTE_SIZE];
			
	
			U16 str_addr[6][5] = {   //���ϵ�ַ 
					{0x0100,0x0110,0x0112,0x0118,0x011E},
					{0x0120,0x0130,0x0132,0x0138,0x013E},
					{0x0140,0x0150,0x0152,0x0158,0x015E},
					{0x0160,0x0170,0x0172,0x0178,0x017E},
					{0x0180,0x0190,0x0192,0x0198,0x019E},
					{0x01A0,0x01B0,0x01B2,0x01B8,0x01BE},
					};
			S8 str_db[20];
			U32 temp = 0;
			U16 num = 0;     //��ʱ ��ʾ��6�ξͽ��� 
			
			S32	db_id_temp = 0;
				
			memset(str_db,' ',20);	
			for(num = 0; num < 6;  num++){ 
				dgus_chinese(str_addr[num][0],str_db, strlen(str_db));	 // dgus  chinese  time
				dgus_chinese(str_addr[num][0],str_db,8);	 // dgus  chinese  time
				dgus_chinese(str_addr[num][1],str_db,8);	 // dgus  chinese  gh
				dgus_chinese(str_addr[num][2],str_db,8);	 // dgus  chinese  ze
				dgus_chinese(str_addr[num][3],str_db,8);	 // dgus  chinese  zs
				dgus_chinese(str_addr[num][4],str_db,8);	 // dgus  chinese  fg		
			}	
			if (db_id <= 0){
				cmd ();
				break;
			}
					
			if ((db_id - 6) >= 0){
				db_id -= 6;
				db_id_temp = db_id;
			}else{
				db_id = 0;
				db_id_temp = db_id;
			}
			
			cy_print ("*******************************************************************************\n");
			cy_print ("Display pre page data, db_total_item_num = %d\n", para_set_value.data.db_total_item_num);
			cy_print("Block = %d page = %d addr = %d db_id = %d item_index = %d\n",
				YQNDHISTORY_DB_ID_PAGE_ADDR / BLOCK_BYTE_SIZE, 
				(YQNDHISTORY_DB_ID_PAGE_ADDR - YQNDHISTORYBLOCK) / PAGE_BYTE_SIZE, 
				YQNDHISTORY_DB_ID_PAGE_ADDR, 
				db_id,
				db_id % PAGE_ITEM_NUM_SIZE);
			
			if(db_id >= 0){
				//int temp_db_id = db_id;
				r_code = Nand_ReadPage(HISTORY_START_BLOCK_NUM, HISTORY_START_PAGE_NUM + (db_id * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);
				
				if (r_code == 0){
					cy_print ("*******************************************************************************\n");
					cy_print ("|-------|----------------|----|--------------|----------------|-----------|\n");
					cy_print ("| Index |      Time      | ID | Total amount | Total Quantity |   Other   |\n");
					cy_print ("|-------|----------------|----|--------------|----------------|-----------|\n");

					db_item_block_temp = (s_db_item_block *)yqnddata;
				
					db_item_info_temp = (s_db_item_info *)&(db_item_block_temp->item_info_array[db_id % PAGE_ITEM_NUM_SIZE]);	
					
					for(num = 0; (num < 6); ){
	 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp = db_item_info_temp->index;
						sprintf(str_db,"%06d ",temp);
						cy_print("|%s", str_db);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
						sprintf(str_db,"20%02x-%02x-%02x %02x:%02x",
							db_item_info_temp->time[0],
							db_item_info_temp->time[1],
							db_item_info_temp->time[2],
							db_item_info_temp->time[3],
							db_item_info_temp->time[4]);   //read time

						cy_print("|%s", str_db);
						dgus_chinese(str_addr[num][0],str_db,strlen(str_db));	 // dgus  chinese  time
	 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp = db_item_info_temp->ID;
						sprintf(str_db,"%04d",temp);
						cy_print("|%s", str_db);
						sprintf(str_db,"%2d",temp);
						dgus_chinese(str_addr[num][1],str_db,strlen(str_db));	 // dgus  chinese  gh
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
						temp =	db_item_info_temp->money;
						sprintf(str_db,"%10d.%d%d ",(temp/100),((temp%100)/10),((temp%100)%10));
						cy_print("|%s", str_db);
						sprintf((char*)str_db,"%d.%d",(temp/100),(temp%100));
						dgus_chinese(str_addr[num][2],str_db,strlen(str_db));	 // dgus  chinese  ze
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp =	db_item_info_temp->total_good;
						sprintf(str_db,"%15d ",temp);
						cy_print("|%s", str_db);
						sprintf((char*)str_db,"%d",temp);
						dgus_chinese(str_addr[num][3],str_db,strlen(str_db));	 // dgus  chinese  zs
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp =	db_item_info_temp->total_ng;
						sprintf(str_db,"%10d ",temp);
						cy_print("|%s|", str_db);
						sprintf((char*)str_db,"%d",temp);
						dgus_chinese(str_addr[num][4],str_db,strlen(str_db));	 // dgus  chinese  fg	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
						cy_print ("\n|-------|----------------|----|--------------|----------------|-----------|\n");
						num++;
						db_id++;
						if (db_id >= para_set_value.data.db_total_item_num)
							break;
						if (((db_id / PAGE_ITEM_NUM_SIZE) != ((db_id - 1) / PAGE_ITEM_NUM_SIZE)) && (num < 6))
						{
							Nand_ReadPage(HISTORY_START_BLOCK_NUM, HISTORY_START_PAGE_NUM + (db_id * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);     // ������Ϣҳ rLB_ReadPage(YQNDHISTORYPAGENUM,yqnddata);
							db_item_block_temp = (s_db_item_block *)yqnddata;
						}
						
						db_item_info_temp = (s_db_item_info *)&(db_item_block_temp->item_info_array[db_id % PAGE_ITEM_NUM_SIZE]);	
						
					}
				}
				else if (r_code == 1)
				{
					cy_println (" Parameter Error, Buf must be not null!!!");  
				}
				else if (r_code == 2)
				{
					cy_println ("ECC Error, Read Op failed!!!");  
				}
			}
			db_id = db_id_temp;
			cy_print ("*******************************************************************************\n");
			cmd ();
			break;
		}

		case DBDISPLAYBACK:
		{	
			U8 r_code;	
			s_db_item_info * db_item_info_temp;
			s_db_item_block * db_item_block_temp;
			U8 yqnddata[PAGE_BYTE_SIZE];
			
			U16 str_addr[6][5] = {   //���ϵ�ַ 
					{0x0100,0x0110,0x0112,0x0118,0x011E},
					{0x0120,0x0130,0x0132,0x0138,0x013E},
					{0x0140,0x0150,0x0152,0x0158,0x015E},
					{0x0160,0x0170,0x0172,0x0178,0x017E},
					{0x0180,0x0190,0x0192,0x0198,0x019E},
					{0x01A0,0x01B0,0x01B2,0x01B8,0x01BE},
					};
			S8 str_db[20];
			U32 temp = 0;
			U16 num = 0;     //��ʱ ��ʾ��6�ξͽ��� 
					
			S32	db_id_temp = 0;
					
					
			if ((db_id + 6) < para_set_value.data.db_total_item_num)
			{
				db_id += 6;
				db_id_temp = db_id;
			}
			else
			{
				cmd ();
				break;
			}
			cy_print ("*******************************************************************************\n");
			cy_print ("display next page data, db_total_item_num = %d\n", para_set_value.data.db_total_item_num);
			cy_print("Block = %d page = %d addr = %d db_id = %d item_index = %d\n",
				YQNDHISTORY_DB_ID_PAGE_ADDR / BLOCK_BYTE_SIZE, 
				(YQNDHISTORY_DB_ID_PAGE_ADDR - YQNDHISTORYBLOCK) / PAGE_BYTE_SIZE, 
				YQNDHISTORY_DB_ID_PAGE_ADDR, 
				db_id,
				db_id % PAGE_ITEM_NUM_SIZE);
			if(db_id < para_set_value.data.db_total_item_num)
			{	
				
				r_code = Nand_ReadPage(HISTORY_START_BLOCK_NUM, HISTORY_START_PAGE_NUM + (db_id * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);     // ������Ϣҳ rLB_ReadPage(YQNDHISTORYPAGENUM,yqnddata);
				if (r_code == 0)
				{
					cy_print ("*******************************************************************************\n");
					cy_print ("|-------|----------------|----|--------------|----------------|-----------|\n");
					cy_print ("| Index |      Time      | ID | Total amount | Total Quantity |   Other   |\n");
					cy_print ("|-------|----------------|----|--------------|----------------|-----------|\n");
					memset(str_db,' ',20);	

					db_item_block_temp = (s_db_item_block *)yqnddata;
					
					db_item_info_temp = (s_db_item_info *)&(db_item_block_temp->item_info_array[db_id % PAGE_ITEM_NUM_SIZE]);	
					
					for(num = 0; num <6;  num++)  //if  still have history num , clean dgus screen ,then display
					{
						dgus_chinese(str_addr[num][0],str_db,strlen(str_db));	 // dgus  chinese  time
						dgus_chinese(str_addr[num][1],str_db,strlen(str_db));	 // dgus  chinese  gh
						dgus_chinese(str_addr[num][2],str_db,strlen(str_db));	 // dgus  chinese  ze
						dgus_chinese(str_addr[num][3],str_db,strlen(str_db));	 // dgus  chinese  zs
						dgus_chinese(str_addr[num][4],str_db,strlen(str_db));	 // dgus  chinese  fg		
					}
					for(num = 0; ((num <6) && (db_id < para_set_value.data.db_total_item_num));)
					{
					
	 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp = db_item_info_temp->index;
						sprintf(str_db,"%06d ",temp);
						cy_print("|%s", str_db);
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
						sprintf(str_db,"20%02x-%02x-%02x %02x:%02x",
							db_item_info_temp->time[0],
							db_item_info_temp->time[1],
							db_item_info_temp->time[2],
							db_item_info_temp->time[3],
							db_item_info_temp->time[4]);   //read time

						cy_print("|%s", str_db);
						dgus_chinese(str_addr[num][0],str_db,strlen(str_db));	 // dgus  chinese  time
	 ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp = db_item_info_temp->ID;
						sprintf(str_db,"%04d",temp);
						cy_print("|%s", str_db);
						sprintf(str_db,"%2d",temp);
						dgus_chinese(str_addr[num][1],str_db,strlen(str_db));	 // dgus  chinese  gh
	///////////////////////////////////////////////////////////////////////////////////////////////////////////////////					
						temp =	db_item_info_temp->money;	
						sprintf(str_db,"%10d.%d%d ",(temp/100),((temp%100)/10),((temp%100)%10));
						cy_print("|%s", str_db);
						sprintf((char*)str_db,"%d.%d",(temp/100),(temp%100));
						dgus_chinese(str_addr[num][2],str_db,strlen(str_db));	 // dgus  chinese  ze
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp =	db_item_info_temp->total_good;
						sprintf(str_db,"%15d ",temp);
						cy_print("|%s", str_db);
						sprintf((char*)str_db,"%d",temp);
						dgus_chinese(str_addr[num][3],str_db,strlen(str_db));	 // dgus  chinese  zs

	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						temp =	db_item_info_temp->total_ng;	
						sprintf(str_db,"%10d ",temp);
						cy_print("|%s|", str_db);
						sprintf((char*)str_db,"%d",temp);
						dgus_chinese(str_addr[num][4],str_db,strlen(str_db));	 // dgus  chinese  fg	
	////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
						cy_print ("\n|-------|----------------|----|--------------|----------------|-----------|\n");
						num++;
						db_id++;
						
						if (((db_id / PAGE_ITEM_NUM_SIZE) != ((db_id - 1) / PAGE_ITEM_NUM_SIZE)) && (num < 6))
						{
							Nand_ReadPage(HISTORY_START_BLOCK_NUM, HISTORY_START_PAGE_NUM + (db_id * ITEM_SIZE / PAGE_BYTE_SIZE), yqnddata);
							db_item_block_temp = (s_db_item_block *)yqnddata;
						}
						
						db_item_info_temp = (s_db_item_info *)&(db_item_block_temp->item_info_array[db_id % PAGE_ITEM_NUM_SIZE]);	
					}
				}
				else if (r_code == 1)
				{
					cy_println (" Parameter Error, Buf must be not null!!!");  
				}
				else if (r_code == 2)
				{
					cy_println ("ECC Error, Read Op failed!!!");  
				}
			}
			db_id = db_id_temp;
			cy_print ("*******************************************************************************\n");
			cmd ();
			break;
		}		
		default:
		{
			break;
		}
	}
	
}

