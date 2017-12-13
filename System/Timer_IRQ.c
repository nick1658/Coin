/**************************************************************/
/*The initial Timer4*/
/**************************************************************/
#include "def.h"
#include "S3C2416.h"
#include "Exception.h"


static void (*Timer4_Callback)(void);
static void (*Timer3_Callback)(void);
static void (*Timer2_Callback)(void);
static void (*Timer1_Callback)(void);
static void (*Timer0_Callback)(void); 

static unsigned int timer_tick = 0;
static void timer_update (void)
{
	timer_tick++;
}

/*中断程序*/
volatile U32 tscount=0;
//20ms中断一次
void Timer4_IRQ(void)
{   
	static U32 T4_count = 0;
	T4_count++;
	time_20ms++;
	if (sys_env.tty_online_ms > 1){sys_env.tty_online_ms--;}
	
	if (sys_env.stop_time > 0){sys_env.stop_time--;}
	if (T4_count > 25){
		T4_count = 0;
		LED2_NOT;
	}
	if(tscount >0){tscount--;}
	if( blockflag> 0){blockflag--;}				
	if(adtime >0){adtime--;}
	if(runtime > 0){runtime--;}
	
	//SetWatchDog(); //看门狗喂狗
	rWTDAT=WatchDogTimer;//看门狗喂狗
	rWTCNT=WatchDogTimer;//看门狗喂狗
}
static void Timer4_Handler(void)
{	
	Timer4_Callback(); // 定时器4回调函数
	IRQ_ClearInt(INT_TIMER4);
}

void Timer4_Start(void)
{
	rTCON |= (0x1 << 20); // 定时器开启
}
void Timer4_Stop(void)
{
	rTCON &= ~(0x1 << 20); // 定时器停止
}

// 1us ~ 65ms
void Timer4_Init(unsigned short us, void (*Callback)(void))
{
// 定时器4时钟频率为PCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 16);
	rTCFG1 |= (0x1 << 16);  // Timer4 4分频
	rTCNTB4 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 21); // 更新计数值
	rTCON &= ~(0x1 << 21); // 清除
	rTCON |= (0x1 << 22); // 自动重装载
	
	if (Callback != 0) {
		Timer4_Callback = Callback;
		IRQ_Register(INT_TIMER4, Timer4_Handler);
		IRQ_EnableInt(INT_TIMER4);
	}
}


#define KICK_Q_SCAN(N) if (coin_env.kick_Q[N] > 0) {\
	coin_env.kick_Q[N]--; \
	if (coin_env.kick_Q[N] == 0){ \
		EMKICK1(STARTRUN);	  \
		coin_env.kick_keep_t1 = para_set_value.data.kick_keep_t1;\
	}\
}
#define FULL_KICK_Q_SCAN(N) if (coin_env.full_kick_Q[N] > 0) {\
	coin_env.full_kick_Q[N]--; \
	if (coin_env.full_kick_Q[N] == 0){ \
		EMKICK2(STARTRUN);	  \
		coin_env.full_kick_keep_t2 = para_set_value.data.kick_keep_t2;\
	}\
}

void Timer3_IRQ(void)
{   
	KICK_Q_SCAN(0);
	KICK_Q_SCAN(1);
//	KICK_Q_SCAN(2);
//	KICK_Q_SCAN(3);
//	KICK_Q_SCAN(4);
//	KICK_Q_SCAN(5);
//	KICK_Q_SCAN(6);
//	KICK_Q_SCAN(7);
//	KICK_Q_SCAN(8);
//	KICK_Q_SCAN(9);
	FULL_KICK_Q_SCAN(0);
	FULL_KICK_Q_SCAN(1);
//	FULL_KICK_Q_SCAN(2);
//	FULL_KICK_Q_SCAN(3);
//	FULL_KICK_Q_SCAN(4);
//	FULL_KICK_Q_SCAN(5);
//	FULL_KICK_Q_SCAN(6);
//	FULL_KICK_Q_SCAN(7);
//	FULL_KICK_Q_SCAN(8);
//	FULL_KICK_Q_SCAN(9);
	
	if (coin_env.kick_keep_t1 > 0){
		coin_env.kick_keep_t1--;
		if (coin_env.kick_keep_t1 == 0){
			EMKICK1(STOPRUN);
		}
	}
	if (coin_env.full_kick_keep_t2 > 0){
		coin_env.full_kick_keep_t2--;
		if (coin_env.full_kick_keep_t2 == 0){
			EMKICK2(STOPRUN);	
		}
	}
	if(time > 0){time--;}
}



static void Timer3_Handler(void)
{	
	Timer3_Callback(); // 定时器3回调函数
	IRQ_ClearInt(INT_TIMER3);
}
void Timer3_Start(void)
{
	rTCON |= (0x1 << 16); // 定时器开启
}
void Timer3_Stop(void)
{
	rTCON &= ~(0x1 << 16); // 定时器停止
	rTCNTB3 = 275;
	rTCON |= (0x1 << 17); // 更新计数值
	rTCON &= ~(0x1 << 17); // 清除
}
// 1us ~ 65ms
void Timer3_Init(unsigned short us, void (*Callback)(void))
{
// 定时器3时钟频率为PCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 12);
	rTCFG1 |= (0x1 << 12);  // Timer3 4分频
	rTCNTB3 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 17); // 更新计数值
	rTCON &= ~(0x1 << 17); // 清除
	rTCON |= (0x1 << 19); // 自动重装载
	
	if (Callback != 0) {
		Timer3_Callback = Callback;
		IRQ_Register(INT_TIMER3, Timer3_Handler);
		IRQ_EnableInt(INT_TIMER3);
	}
}



volatile uint32_t SystemTick;

void Timer2_IRQ(void)
{
//	static U32 T_count = 0;
//	T_count++;
//	
//	if (T_count > 1000){
//		T_count = 0;
//		LED1_NOT;
//	}
//	SystemTick++; // 1ms/tick
	coin_cross_time++;
}

static void Timer2_Handler(void)
{	
	Timer2_Callback(); // 定时器2回调函数
	IRQ_ClearInt(INT_TIMER2);
}

void Timer2_Start(void)
{
	rTCON |= (0x1 << 12); // 定时器开启
}

void Timer2_Stop(void)
{
	rTCON &= ~(0x1 << 12); // 定时器停止
}

// 1us ~ 65ms
void Timer2_Init(unsigned short us, void (*Callback)(void))
{
// 定时器2时钟频率为PCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 8);
	rTCFG1 |= (0x1 << 8);  // Timer2 4分频
	rTCNTB2 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 13); // 更新计数值
	rTCON &= ~(0x1 << 13); // 清除
	rTCON |= (0x1 << 15); // 自动重装载
	
	if (Callback != 0) {
		Timer2_Callback = Callback;
		IRQ_Register(INT_TIMER2, Timer2_Handler);
		IRQ_EnableInt(INT_TIMER2);
	}
}



void Timer1_IRQ (void)
{
	static U32 T_count = 0;
	T_count++;
	
	if (T_count > 1000){
		T_count = 0;
		//LED3_NOT;
	}
}
static void Timer1_Handler(void)
{	
	Timer1_Callback(); // 定时器1回调函数
	IRQ_ClearInt(INT_TIMER1);
}
void Timer1_Start(void)
{
	rTCON |= (0x1 << 8); // 定时器开启
}
void Timer1_Stop(void)
{
	rTCON &= ~(0x1 << 8); // 定时器停止
}

// 1us ~ 65ms
void Timer1_Init(unsigned short us, void (*Callback)(void))
{
// 定时器1时钟频率为PCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff);	
	rTCFG0 |= (5);	
	rTCFG1 &= ~(0xf << 4);
	rTCFG1 |= (0x1 << 4);  // Timer1 4分频
	rTCNTB1 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 9); // 更新计数值
	rTCON &= ~(0x1 << 9); // 清除
	rTCON |= (0x1 << 11); // 自动重装载
	
	if (Callback != 0) {
		Timer1_Callback = Callback;
		IRQ_Register(INT_TIMER1, Timer1_Handler);
		IRQ_EnableInt(INT_TIMER1);
	}
}



void Timer0_IRQ (void)
{
	static U32 T_count = 0;
	T_count++;
	
	if (T_count > 1000){
		T_count = 0;
		LED1_NOT;
	}
}

static void Timer0_Handler(void)
{	
	Timer0_Callback(); // 定时器0回调函数
	IRQ_ClearInt(INT_TIMER0);
}
void Timer0_Start(void)
{
	rTCON |= (0x1 << 0); // 定时器开启
}
void Timer0_Stop(void)
{
	rTCON &= ~(0x1 << 0); // 定时器停止
}

// 1us ~ 65ms
void Timer0_Init(unsigned short us, void (*Callback)(void))
{
// 定时器0时钟频率为PCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff);	
	rTCFG0 |= (5);	
	rTCFG1 &= ~(0xf);
	rTCFG1 |= (0x1);  // Timer0 4分频
	rTCNTB0 = ((unsigned int)us*2751)/1000;
	rTCON |= (0x1 << 1); // 更新计数值
	rTCON &= ~(0x1 << 1); // 清除
	rTCON |= (0x1 << 3); // 自动重装载
	
	if (Callback != 0) {
		Timer0_Callback = Callback;
		IRQ_Register(INT_TIMER0, Timer0_Handler);
		IRQ_EnableInt(INT_TIMER0);
	}
}


void Timer_Init (void)
{
	Timer0_Callback = timer_update;
	Timer1_Callback = timer_update;
	Timer2_Callback = timer_update;
	Timer3_Callback = timer_update;
	Timer4_Callback = timer_update;
	
	//Timer0_Init(1000, Timer0_IRQ);//1ms
	//Timer1_Init(1000, OSTimeTick);//1ms
	//Timer1_Init(1000, Timer1_IRQ);//1ms
	Timer2_Init(500, Timer2_IRQ);//0.5ms
	Timer3_Init(100, Timer3_IRQ);//0.1ms
	Timer4_Init(20000, Timer4_IRQ);//20ms
	Timer4_Start ();
	Timer3_Start ();
	Timer2_Start ();
	//Timer1_Start ();
	//Timer0_Start ();
}


