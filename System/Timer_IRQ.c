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

/*�жϳ���*/
volatile U32 tscount=0;
//20ms�ж�һ��
void Timer4_IRQ(void)
{   
	if (sys_env.tty_online_ms > 1){sys_env.tty_online_ms--;}
	
	if (sys_env.stop_time > 0){sys_env.stop_time--;}
	if(tscount >0){tscount--;}
	if( blockflag> 0){blockflag--;}				
	if(adtime >0){adtime--;}
	if(runtime > 0){runtime--;}
	
	//SetWatchDog(); //���Ź�ι��
	rWTDAT=WatchDogTimer;//���Ź�ι��
	rWTCNT=WatchDogTimer;//���Ź�ι��
}
static void Timer4_Handler(void)
{	
	IRQ_ClearInt(INT_TIMER4);
	Timer4_Callback(); // ��ʱ��4�ص�����
}

void Timer4_Start(void)
{
	rTCON |= (0x1 << 20); // ��ʱ������
}
void Timer4_Stop(void)
{
	rTCON &= ~(0x1 << 20); // ��ʱ��ֹͣ
}

// 1us ~ 23ms
void Timer4_Init(unsigned short us, void (*Callback)(void), unsigned short enable_int)
{
// ��ʱ��4ʱ��Ƶ��ΪPCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 16);
	rTCFG1 |= (0x1 << 16);  // Timer4 4��Ƶ
	rTCNTB4 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 21); // ���¼���ֵ
	rTCON &= ~(0x1 << 21); // ���
	rTCON |= (0x1 << 22); // �Զ���װ��
	
	if (Callback != 0) {
		Timer4_Callback = Callback;
		IRQ_Register(INT_TIMER4, Timer4_Handler);
		if (enable_int == 1){
			IRQ_EnableInt(INT_TIMER4);
		}
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

extern void main_task(void);
void Timer3_IRQ(void)
{   
	main_task ();
	KICK_Q_SCAN(0);
	KICK_Q_SCAN(1);
	FULL_KICK_Q_SCAN(0);
	FULL_KICK_Q_SCAN(1);
	
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
	coin_cross_time++;
	if(time > 0){time--;}
}



static void Timer3_Handler(void)
{	
	IRQ_ClearInt(INT_TIMER3);
	Timer3_Callback(); // ��ʱ��3�ص�����
}
void Timer3_Start(void)
{
	rTCON |= (0x1 << 16); // ��ʱ������
}
void Timer3_Stop(void)
{
	rTCON &= ~(0x1 << 16); // ��ʱ��ֹͣ
	rTCNTB3 = 275;
	rTCON |= (0x1 << 17); // ���¼���ֵ
	rTCON &= ~(0x1 << 17); // ���
}
// 1us ~ 23ms
void Timer3_Init(unsigned short us, void (*Callback)(void), unsigned short enable_int)
{
// ��ʱ��3ʱ��Ƶ��ΪPCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 12);
	rTCFG1 |= (0x1 << 12);  // Timer3 4��Ƶ
	rTCNTB3 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 17); // ���¼���ֵ
	rTCON &= ~(0x1 << 17); // ���
	rTCON |= (0x1 << 19); // �Զ���װ��
	
	if (Callback != 0) {
		Timer3_Callback = Callback;
		IRQ_Register(INT_TIMER3, Timer3_Handler);
		if (enable_int == 1){
			IRQ_EnableInt(INT_TIMER3);
		}
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
}

static void Timer2_Handler(void)
{	
	IRQ_ClearInt(INT_TIMER2);
	Timer2_Callback(); // ��ʱ��2�ص�����
}

void Timer2_Start(void)
{
	rTCON |= (0x1 << 12); // ��ʱ������
}

void Timer2_Stop(void)
{
	rTCON &= ~(0x1 << 12); // ��ʱ��ֹͣ
}

// 1us ~ 23ms
void Timer2_Init(unsigned short us, void (*Callback)(void), unsigned short enable_int)
{
// ��ʱ��2ʱ��Ƶ��ΪPCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff << 8);	
	rTCFG0 |= (5 << 8);	
	rTCFG1 &= ~(0xf << 8);
	rTCFG1 |= (0x1 << 8);  // Timer2 4��Ƶ
	rTCNTB2 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 13); // ���¼���ֵ
	rTCON &= ~(0x1 << 13); // ���
	rTCON |= (0x1 << 15); // �Զ���װ��
	
	if (Callback != 0) {
		Timer2_Callback = Callback;
		IRQ_Register(INT_TIMER2, Timer2_Handler);
		if (enable_int == 1){
			IRQ_EnableInt(INT_TIMER2);
		}
	}
}



void Timer1_IRQ (void)
{
	IRQ_DisableInt(INT_TIMER1);
	cy_println ("\n--------------------------------------------------------------------------Timer1_IRQ");
}
static void Timer1_Handler(void)
{	
	IRQ_ClearInt(INT_TIMER1);
	Timer1_Callback(); // ��ʱ��1�ص�����
}
void Timer1_Start(void)
{
	rTCON |= (0x1 << 8); // ��ʱ������
}
void Timer1_Stop(void)
{
	rTCON &= ~(0x1 << 8); // ��ʱ��ֹͣ
}

// 1us ~ 23ms
void Timer1_Init(unsigned short us, void (*Callback)(void), unsigned short enable_int)
{
// ��ʱ��1ʱ��Ƶ��ΪPCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff);	
	rTCFG0 |= (5);	
	rTCFG1 &= ~(0xf << 4);
	rTCFG1 |= (0x1 << 4);  // Timer1 4��Ƶ
	rTCNTB1 = ((unsigned int)us*2750)/1000;
	rTCON |= (0x1 << 9); // ���¼���ֵ
	rTCON &= ~(0x1 << 9); // ���
	rTCON |= (0x1 << 11); // �Զ���װ��
	
	if (Callback != 0) {
		Timer1_Callback = Callback;
		IRQ_Register(INT_TIMER1, Timer1_Handler);
		if (enable_int == 1){
			IRQ_EnableInt(INT_TIMER1);
		}
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

u32 is_system_ticks_handler = 0;

static void Timer0_Handler(void)//��SYSģʽ��ִ��
{	
	IRQ_ClearInt(INT_TIMER0);
	OSIntEnter ();
	Timer0_Callback(); // ��ʱ��0�ص�����
	is_system_ticks_handler = 1;
}

extern unsigned int Int_Offset;
void is_system_ticks (void)//��SVCģʽ��ִ��
{
	if (is_system_ticks_handler == 1){
		OSIntExit ();
	}else if(rSRCPND1 & (1 << INT_TIMER0)){
		IRQ_ClearInt(INT_TIMER0);
		OSIntEnter ();
		Timer0_Callback(); // ��ʱ��0�ص�����
		OSIntExit ();
	}
	is_system_ticks_handler = 0;
}

void Timer0_Start(void)
{
	rTCON |= (0x1 << 0); // ��ʱ������
}
void Timer0_Stop(void)
{
	rTCON &= ~(0x1 << 0); // ��ʱ��ֹͣ
}

// 1us ~ 23ms
void Timer0_Init(unsigned short us, void (*Callback)(void), unsigned short enable_int)
{
// ��ʱ��0ʱ��Ƶ��ΪPCLK(66M)/(5+1)/4=2750KHZ
	if (us == 0) {
		return;
	}
	rTCFG0 &= ~(0xff);	
	rTCFG0 |= (5);	
	rTCFG1 &= ~(0xf);
	rTCFG1 |= (0x1);  // Timer0 4��Ƶ
	rTCNTB0 = ((unsigned int)us*2751)/1000;
	rTCON |= (0x1 << 1); // ���¼���ֵ
	rTCON &= ~(0x1 << 1); // ���
	rTCON |= (0x1 << 3); // �Զ���װ��
	
	if (Callback != 0) {
		Timer0_Callback = Callback;
		IRQ_Register(INT_TIMER0, Timer0_Handler);
		if (enable_int == 1){
			IRQ_EnableInt(INT_TIMER0);
		}
	}
}

void Init_OS_ticks (void)
{
	Timer0_Callback = OSTimeTick;
	Timer0_Init(1000, OSTimeTick, 1);//1ms
	Timer0_Start ();
}


void Timer_Init (void)
{
	Timer1_Callback = timer_update;
	Timer2_Callback = timer_update;
	Timer3_Callback = timer_update;
	Timer4_Callback = timer_update;
	
	//Timer1_Init(1000, OSTimeTick);//1ms
	Timer1_Init(23000, Timer1_IRQ, 0);//1ms
	//Timer2_Init(500, Timer2_IRQ, 1);//0.5ms
	Timer3_Init(100, Timer3_IRQ, 1);//0.1ms
	Timer4_Init(20000, Timer4_IRQ, 1);//20ms
	
	Timer4_Start ();
	Timer3_Start ();
	//Timer2_Start ();
	Timer1_Start ();
}


