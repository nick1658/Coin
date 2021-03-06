//;----------------------------------------------------------

//;----------------------------------------------------------
#include "s3c2416.h"
#include "Nand.h"

S8 alertflag = 0; 		 //报错标志位

U32 time_20ms;
U32 time_20ms_old;


#define CODE_FLAG		(*(volatile unsigned *)0x33F80000)  		//MPLL lock time conut
__align(4) U32 code_flag __attribute__((at(0x32104000), zero_init));


int check_ad1_ad2_value (void);

//主函数

int main(void)
{

	
	U16 i=0;
	rWTCON = 0;	// 关闭开门狗
 	port_Init();
	uart_init();//115200bps
	
	system_env_init ();
	coin_env_init ();
	//////////////  
	alertflag = 0; 		 //报错标志位

	coinchoose = CN0;   // 币种选择 国家级别的
	touch_flag =0;   // 串口2接收 标志位 
	uartcount = 0;  // 串口2接收 字节 计数 
	sys_env.coin_index = 0;   //当前  学习 币种 名称 
	tscount = 0;
	blockflag = ADBLOCKT; //此变量在yqadc.c文件中定义
	adtime = 0;    //定时中断里 计时
	
	coin_env.ad0_step =0;   // AD 步骤号  
	coin_env.ad1_step =0;   // AD 步骤号  
	coin_env.ad2_step =0;   // AD 步骤号 
	
	std_ad0 = 0;
	std_ad1 = 0;
	std_ad2 = 0;
	
//	db_id = 0;   //历史数据 表格已经显示 数
	
	ch0_count = 0;
	ch1_count = 0;
	ch2_count = 0;
	
	ch0_coin_come = 0;;
	
	coinlearnnumber =0;  //自学习 数量
	prepic_prenum = 0;     // 用于记录 报错前的界面 
	db_id = 0;   //历史数据 表格已经显示 数
	
	coin_maxvalue0 = AD0STDSET;
	coin_minvalue0 = AD0STDSET;
	coin_maxvalue1 = AD1STDSET;
	coin_minvalue1 = AD1STDSET;
	coin_maxvalue2 = AD2STDSET;
	coin_minvalue2 = AD2STDSET;

	std_ad0 = AD0STDSET;
	std_ad1 = AD1STDSET;
	std_ad2 = AD2STDSET;

	for(i = 0;i<TSGET_NUM;i++) 
	{
		touchnum[i] = 0;
	}
	////////////////

	uart1_init();//串口打印机
	uart2_init();//屏幕
	cy_println ("\n#####    Program For YQ ##### ");
	i = 1;
	Timer_Init ();
	watchdog_reset();/*初始化看门狗,T = WTCNT * t_watchdog*/
	
	//init RTC************************************************************
	RTC_Time Time = {
		2017, 6, 28, 3, 12, 0, 0
	};	
	

	adc_init();    //初始化ADC 	
//	init_Iic();
//	yqi2c_init();
    rNF_Init();
	initial_nandflash();    //nandflash
	Hsmmc_Init ();//SD卡
	cy_println ("Hsmmc_init_flag is %d", Hsmmc_exist ());
	
	ini_picaddr();	 //初始化触摸屏变量
	/*下面把触摸屏上的一些变量初始化*/
	ini_screen ();
	prepic_num = JSJM;
	prepic_prenum = JSJM;
	cy_println ("Coin Detector System start");
	if (code_flag == 0x55555555){//如果是JTAG下载启动，就更新程序到Flash		
		U8 r_code;
		extern unsigned int __CodeAddr__;
		extern unsigned int __CodeSize__;	
	#if 1
		cy_println ("Code_flag = 0x%x, Begin Write code to flash...", code_flag);
		r_code = WriteAppToAppSpace ((U32)__CodeAddr__, __CodeSize__);
		if (r_code == 0)
			cy_println ("write code to nand flash block 10 page 0 nand addr 0 completed");   
		else
			cy_println ("write code to nand flash block 10 page 0 nand addr 0 failed");  
	#endif
		code_flag = 0;
	}else{
		cy_println ("Code_flag = 0x%x, No Need Update code!", code_flag);
	}
	
	//begin init RTC************************************************************
	while(1) {    // read time 
		comscreen(dgus_readt,6);	//read time
		tscount = 50;//1000ms 延时
		while(touch_flag == 0 ){if (tscount == 0) break;}//1秒后还没收到触摸屏的信息，有可能没有接屏，直接跳过
		Time.Year 	= HEX_TO_DEC (touchnum[6]) + 2000;	
		Time.Month 	= HEX_TO_DEC (touchnum[7]);	
		Time.Day 	= HEX_TO_DEC (touchnum[8]);	
		Time.Week 	= HEX_TO_DEC (touchnum[9]);	
		Time.Hour 	= HEX_TO_DEC (touchnum[10]);	
		Time.Min 	= HEX_TO_DEC (touchnum[11]);	
		Time.Sec 	= HEX_TO_DEC (touchnum[12]);
		touch_flag = 0;
		break;
	}
	
	RTC_Init(&Time);
	RTC_GetTime(&Time);
	cy_println("Time: %4d/%02d/%02d %02d:%02d:%02d", Time.Year,
				Time.Month, Time.Day, Time.Hour, Time.Min, Time.Sec);
	//end init RTC************************************************************

	LED1_ON;
	/*开机预热，如果时间不够，可适当延长*/
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发	  
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发
//	delay_ms(ELECTRICTIME);    //开机 延时 这些时间再给 单片机发
	
 	sys_env.workstep = 0; //停机状态
	print_system_env_info ();//串口打印编译信息和系统环境变量，便于调试。
	setStdValue	();//设置鉴伪基准值，后面每次启动之前都会设置一次，因为鉴伪基准值会随温度在一定范围内变化
	adstd_offset ();//设置补偿值，后面每次启动之前都会补偿一次，因为鉴伪基准值会随温度在一定范围内变化
	
	comscreen(Disp_Indexpic[JSJM],Number_IndexpicB);	  // 跳转到主界面
	//i = CRC16 (iap_code_buf, sizeof(iap_code_buf));
	while(1)
	{
		switch (sys_env.workstep)
		{
			case 0:{ 
				ALL_STOP();//停掉所有的输出
				sys_env.workstep = 1;
				refresh_data ();
				cy_println ("50,stop;");//停机
				break;
			}
			case 1://待机状态
				break;
			case 3:{  //清分启动
				sys_env.workstep = 6;  //主函数 步骤号		
				deviceinit();//初始化变量
				break;
			}
			case 6: {    
				setStdValue	();//设置鉴伪基准值
				if( adstd_offset() == 1){//  检测基准值，并进行补偿
					sys_env.stop_time = STOP_TIME;//无币停机时间
					sys_env.workstep =10;
					if ((sys_env.auto_clear == 1) || para_set_value.data.coin_full_rej_pos == 3){//如果设置自动清零，则每次启动都清零计数
						for (i = 0; i < COIN_TYPE_NUM; i++){
							dgus_tf1word(pre_value.country[COUNTRY_ID].coin[i].data.hmi_state_ico_addr, 0);   //图标  绿
							*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_full_flag = 0; //
							*pre_value.country[COUNTRY_ID].coin[i].data.p_pre_count_current = 0; //
							coin_num[i] = 0;
							dgus_tf1word(pre_value.country[COUNTRY_ID].coin[i].data.hmi_pre_count_cur_addr, 0);	//更新计数值 
							coin_env.full_stack_num = 0;
						}
						processed_coin_info.total_money =0;
						processed_coin_info.total_coin = 0;
						processed_coin_info.total_good = 0;
						processed_coin_info.total_ng = 0;
						processed_coin_info.coinnumber = 0;
						good_value_index = 0;
						ng_value_index = 0;
						disp_allcount();
						disp_data(ADDR_CPZE,ADDR_CPZS,ADDR_CPFG);			//when counting pre ze zs foege data variable 
					}
					time_20ms = 0;
				}else{
					SEND_ERROR(ADSTDEEROR);   //传感器下有币
					dbg("the voltage is wrong \r\n");
					//cmd ();
				}
				break;
			}
			case 10:{        //main  proceed
				cy_ad0_valueget();    //check coin wave and get ADSAMPNUM of ad  values
				cy_ad1_valueget();    //check coin wave and get ADSAMPNUM of ad  values
				cy_ad2_valueget();    //check coin wave and get ADSAMPNUM of ad  values
				//////////////////////////////////////////////////////////////////////
				cy_precoincount();   //鉴伪、计数
				IR_detect_func();   //第二个踢币程序
				runfunction();	 //转盘动作函数
					 
				if(blockflag == 0){//堵币
					SEND_ERROR(PRESSMLOCKED);
				}
			
				if (sys_env.stop_time == 0){
					sys_env.stop_flag++;
					if (sys_env.stop_flag == 1){
						STORAGE_DIR_N();//反转
						sys_env.stop_time = 50;//反转一秒
					}else if (sys_env.stop_flag == 2){
						STORAGE_DIR_P();//正转
						sys_env.stop_time = STOP_TIME;//无币停机时间10秒
					}else if (sys_env.stop_flag == 3){
						STORAGE_MOTOR_STOPRUN();	//  转盘电机
						sys_env.stop_time = STOP_TIME;//无币停机时间10秒
					}else if (sys_env.stop_flag == 4){
						time_20ms_old = time_20ms;
						comscreen(Disp_Indexpic[JSJM],Number_IndexpicB);	 // back to the  picture before alert
						sys_env.workstep =0;	
						if (processed_coin_info.total_coin > 0){
							LOG ("\n----------------------------------------------------------------------");
							LOG ("[Start At %4d-%02d-%02d %02d:%02d:%02d] ", 
								Time.Year, Time.Month, Time.Day, Time.Hour, Time.Min, Time.Sec);
							LOG("   币种  数量(枚)  金额(元)");
							LOG("   1分     %4d     %d.%d%d",coin_num[8],((coine[coinchoose][8]*coin_num[8])/100),(((coine[coinchoose][8]*coin_num[8])%100)/10),(((coine[coinchoose][8]*coin_num[8])%100)%10));
							LOG("   2分     %4d     %d.%d%d",coin_num[7],((coine[coinchoose][7]*coin_num[7])/100),(((coine[coinchoose][7]*coin_num[7])%100)/10),(((coine[coinchoose][7]*coin_num[7])%100)%10));
							LOG("   5分     %4d     %d.%d%d",coin_num[6],((coine[coinchoose][6]*coin_num[6])/100),(((coine[coinchoose][6]*coin_num[6])%100)/10),(((coine[coinchoose][6]*coin_num[6])%100)%10));
							LOG("   1角     %4d     %d.%d%d",(coin_num[3]+coin_num[4]+coin_num[5]),((coine[coinchoose][3]*(coin_num[3]+coin_num[4]+coin_num[5]))/100),(((coine[coinchoose][3]*(coin_num[3]+coin_num[4]+coin_num[5]))%100)/10),(((coine[coinchoose][3]*(coin_num[3]+coin_num[4]+coin_num[5]))%100)%10));
							LOG("   5角     %4d     %d.%d%d",(coin_num[1]+coin_num[2]),((coine[coinchoose][1]*(coin_num[1]+coin_num[2]))/100),(((coine[coinchoose][1]*(coin_num[1]+coin_num[2]))%100)/10),(((coine[coinchoose][1]*(coin_num[1]+coin_num[2]))%100)%10));
							LOG("   1元     %4d     %d.%d%d",coin_num[0],((coine[coinchoose][0]*coin_num[0])/100),(((coine[coinchoose][0]*coin_num[0])%100)/10),(((coine[coinchoose][0]*coin_num[0])%100)%10));
							LOG("");
							LOG("   详细信息:  ");
							LOG("   异币:     %d 枚",processed_coin_info.total_ng);
							LOG("   金额:     %d.%d%d 元",(processed_coin_info.total_money/100),((processed_coin_info.total_money%100)/10),((processed_coin_info.total_money%100)%10));
							LOG("   总数:     %d + %d = %d 枚",processed_coin_info.total_good, processed_coin_info.total_ng, processed_coin_info.total_coin);
							LOG("   本次清分耗时: %d Sec 速度: %d / Min", ((time_20ms - (STOP_TIME * 3) - 100) / 50),
														((processed_coin_info.total_coin * 3000) / (time_20ms_old - STOP_TIME * 3 - 50)));
						}
					}
				}
				break;
			}
			/////////////////
			case 13: {//特征学习
				sys_env.workstep = 20;  //主函数 步骤号		
				deviceinit();
				break;
			}
			case 20:{
				if( adstd_offset() == 1){//检测 基准值    不调试到正常值  不能进行 自学习
					sys_env.workstep =22;
				}else{
					SEND_ERROR(ADSTDEEROR);   //  请调整基准值
					//cy_print("the voltage is wrong\n");
				}
				break;
			}
			case 22:{
				cy_ad0_valueget();    //check coin wave and get ADSAMPNUM of ad  values
				cy_ad1_valueget();    //check coin wave and get ADSAMPNUM of ad  values
				cy_ad2_valueget();    //check coin wave and get ADSAMPNUM of ad  values
					
				cy_coinlearn();   //特征学习
				IR_detect_func();   //第二个踢币程序
				runfunction();	 //转盘动作函数
				
				if(blockflag == 0){//堵币
					SEND_ERROR(PRESSMLOCKED);
				}
				break;
			}
			case 88:{//报错	
				ALL_STOP();
				alertfuc(alertflag);
				//cy_print("sys_env.workstep is %d-->%s %d\n",sys_env.workstep, __FILE__, __LINE__);
				break;
			}
			case 100:{////基准调试
				cy_adstd_adj ();
				break;
			}
			case 101:{////红外传感器读取
				detect_read ();
				break;
			}
			case 103:{// 进行特征值波形采样，上传电脑
				adstd_sample ();
				break;
			}
			default:{
				break;
			}
		}
		if (sys_env.tty_online_ms == 1){
			sys_env.tty_online_ms = 0;
			update_finish ();
		}
		if( touch_flag ==1){
			touchresult();//判断触摸 状态的函数
			touch_flag =0;
		}
		if (sys_env.uart0_cmd_flag == 1){
			vTaskCmdAnalyze ();//串口命令处理函数
			sys_env.uart0_cmd_flag = 0;
		}
	}	
}
