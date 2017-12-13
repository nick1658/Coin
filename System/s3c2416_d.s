;/********************************************************************/
;/* s3c2416.s: start code for samsung s3c2416/50/51(arm9)            */
;/********************************************************************/

; Clock setting(External Crystal 12M):
; MPLLCLK = 800M, EPLLCLK = 96M
; ARMCLK = 400M, HCLK = 133M
; DDRCLK = 266M, SSMCCLK = 66M, PCLK = 66M
; HSMMC1 = 24M

; Standard definitions of Mode bits and Interrupt (I & F) flags in PSRs
Mode_USR        EQU     0x10
Mode_FIQ        EQU     0x11
Mode_IRQ        EQU     0x12
Mode_SVC        EQU     0x13
Mode_ABT        EQU     0x17
Mode_UND        EQU     0x1B
Mode_SYS        EQU     0x1F
; when I bit is set, IRQ is disabled
I_Bit           EQU     0x80 
; when F bit is set, FIQ is disabled
F_Bit           EQU     0x40            

; Stack Configuration
UND_Stack_Size  EQU     0x00000020
SVC_Stack_Size  EQU     0x00000020
ABT_Stack_Size  EQU     0x00000020
FIQ_Stack_Size  EQU     0x00000100
IRQ_Stack_Size  EQU     0x00000400
USR_Stack_Size  EQU     0x00001000
ISR_Stack_Size  EQU     (UND_Stack_Size + SVC_Stack_Size + \
						ABT_Stack_Size + FIQ_Stack_Size + \
						IRQ_Stack_Size)
                AREA    STACK, NOINIT, READWRITE, ALIGN=3
Stack_Mem       SPACE   USR_Stack_Size
__initial_sp    SPACE   ISR_Stack_Size
Stack_Top

; Heap Configuration
Heap_Size       EQU     0x00000200
                AREA    HEAP, NOINIT, READWRITE, ALIGN=3
__heap_base
Heap_Mem        SPACE   Heap_Size
__heap_limit

; Internal Memory Base Addresses
IRAM_BASE       EQU     0x40000000

; Watchdog Timer Base Address
WT_BASE         EQU     0x53000000  

; Interrupt Register Base Address
INT_BASE		EQU		0x4A000000
INTMSK1_OFS		EQU		0x08
INTSUBMSK_OFS	EQU		0x1C
INTMSK2_OFS		EQU		0x48

; Clock Base Address
CLOCK_BASE		EQU		0x4C000000
LOCKCON0_OFS    EQU		0x00
LOCKCON1_OFS    EQU		0x04
MPLLCON_OFS		EQU		0x10
EPLLCON_OFS		EQU		0x18
CLKSRC_OFS		EQU		0x20
CLKDIV0_OFS		EQU		0x24
CLKDIV1_OFS		EQU		0x28
CLKDIV2_OFS		EQU		0x2C

;----------------------- CODE -------------------------------------------
                PRESERVE8
                
;  Area Definition and Entry Point
;  Startup Code must be linked first at Address at which it expects to run.

            AREA    RESET, CODE, READONLY
;		ENTRY
            ARM
Vectors     B       Reset_Handler        
            LDR     PC, Undef_Addr
            LDR     PC, SWI_Addr
            LDR     PC, PAbt_Addr
            LDR     PC, DAbt_Addr
			LDR		PC, Notuse_Addr
JmpIRQ		B		IRQ_SaveContext
;JmpFIQ		B	JmpFIQ;	
			LDR		PC, FIQ_Addr

			IMPORT	Undef_Handler
			IMPORT	SWI_Handler
			IMPORT	PAbt_Handler
			IMPORT	DAbt_Handler
			IMPORT	IRQ_Handler
			IMPORT	FIQ_Handler				
Reset_Addr  DCD   Reset_Handler
Undef_Addr  DCD   Undef_Handler
SWI_Addr    DCD   SWI_Handler
PAbt_Addr   DCD   PAbt_Handler
DAbt_Addr   DCD   DAbt_Handler
Notuse_Addr DCD   0           ; Reserved Address 
FIQ_Addr	DCD	  FIQ_Handler
IRQ_SaveContext
; �����ж������ģ�֧���ж�Ƕ��				
			SUB	LR, LR, #4 ; ���㷵�ص�ַ
			STMFD SP!, {R0-R12, LR} ;���мĴ���ѹջ����
			MRS 	R0, SPSR ; �����ж�ǰ��CPSR(�����ڵ�SPSR)
			STMFD	SP!, {R0} ;
			MSR	CPSR_cxsf, #Mode_SYS+I_Bit ; �л���ϵͳģʽ	
			STMFD 	SP!, {LR} ; ѹջϵͳģʽLR
				
			LDR 	R0, =IRQ_Handler ;ϵͳģʽ�½���IRQ���봦��
			MOV		LR, PC ; ׼�������ķ��ص�ַ
 			BX		R0 ; �����жϴ�����
			
			LDMFD	SP!, {LR} ; ��ջϵͳģʽLR
			MSR	CPSR_cxsf, #Mode_IRQ+I_Bit ; �л���IRQģʽ					
			LDMFD 	SP!, {R0} ; �����ж�ǰ��CPSR				
			MSR   	SPSR_cxsf, R0
			LDMFD  SP!, {R0-R12, PC}^ ; ^��ͬʱ��spsr�ָ���cpsr

        EXPORT  Reset_Handler
Reset_Handler
;/***********************************************************************/
; ���Ź��ر�
		LDR     R0, =WT_BASE 
		LDR     R1, =0
		STR     R1, [R0]		

;/***********************************************************************/
; �ر����������ж�
		LDR		R0, =INT_BASE
		LDR		R1, =0xFFFFFFFF
		STR		R1, [R0, #INTMSK1_OFS]
		STR		R1, [R0, #INTMSK2_OFS]
		STR		R1, [R0, #INTSUBMSK_OFS]

;/***********************************************************************/
; ϵͳʱ������						
		LDR		R0, =CLOCK_BASE
		LDR		R1, =3600 
; MPLL����ʱ�����300us�����ⲿ����12M��
		STR		R1, [R0, #LOCKCON0_OFS]
		LDR		R1, =3600 
; EPLL����ʱ�����300us
		STR		R1, [R0, #LOCKCON1_OFS]
; PLL����ʱ����С��ҲӦ�ò��������������⣬ֻ�Ǹı�PLL�����
; �����������ȶ���ʱ�Ӹ�system
				
; MPLL(���ⲿ��·��Ƶ���)ͨ��ARM��Ƶ�����ARM clock(533M)��ͨ��Ԥ��Ƶ����
; ����HCLK(133M), DDRCLK(266M), SSMCCLK(Memory Controllers,133M),PCLK(66M)
; ����PCLK=HCLK/2,SSMCCLK=HCLK/2,����MPLL���ʱ��800M��ARM clock��Ƶ����2��
; �õ�ARM clock 400M��Ԥ��Ƶ����Ƶ����Ϊ3�����266M����HCLK��Ƶ��2��Ƶ���
; ��HCLK=266M/2=133M��HCLKDIV[1:0],PREDIV[5:4],ARMDIV[11:9],
; ARMCLK Ratio=(ARMDIV+1),HCLK Ratio=(PREDIV+1)*(HCLKDIV+1)
		LDR		R1, =(0x1<<0)+(1<<2)+(1<<3)+(0x2<<4)+(0x1<<9)
		STR		R1, [R0, #CLKDIV0_OFS]
				
; EPLL(���ⲿ��·��Ƶ���)ͨ�����Եķ�Ƶ�������SPI(CLKSRC��ѡ��MPLL��),
; DISP,I2S,UART,HSMMC1,USBHOST
		LDR		R1, =(0x0<<4)+(0x3<<6)+(0x0<<8)+(0x0<<12)+ \
							(0x0<<16)+(0x0<<24)
		STR		R1, [R0, #CLKDIV1_OFS]
				
; HSMMC0ʱ����EPPL����SPIʱ�ӿ���MPLL��
		LDR		R1, =(0x0<<0)+(0x0<<6)
		STR		R1, [R0, #CLKDIV2_OFS]
				
; ����EPLL���96M,
; MDIV=32,PDIV=1,SDIV=2,Fout=((MDIV+(KDIV/2^16))*Fin)/(PDIV*2^SDIV),KDIV=0
		LDR		R1, =(2<<0)+(1<<8)+(32<<16)+(0x0<<24)+(0x0<<25)
		STR		R1, [R0, #EPLLCON_OFS]
; ��EPLLCONд��ֵ����EPLL����ʱEPLL clock�����������
; ��ʱLOCKCON1��ʱ���ȶ�������ʱ��				
				
; �ⲿ����12M������MPLL���Ϊ800M��
; MDIV=400,PDIV=3,SDIV=1,Fout=(MDIV*Fin)/(PDIV*2^SDIV)
		LDR		R1, =(1<<0)+(3<<5)+(400<<14)+(0x0<<24)+(0x0<<25)
		STR		R1, [R0, #MPLLCON_OFS] 
; ��MPLLCONд��ֵ����MPLL����ʱMPLL clock�����������
; ��ʱLOCKCON0��ʱ���ȶ�������ʱ��

		LDR		R1, =(1<<4)+(1<<6) 
; ʱ��Դ����MPLL��EPLL���
		STR		R1, [R0, #CLKSRC_OFS]
				
;/************************************************************************/;/*************************************************************************/
; ��ջ��ʼ��
        LDR     R0, =Stack_Top

;  Enter Undefined Instruction Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_UND:OR:I_Bit:OR:F_Bit
        MOV     SP, R0
        SUB     R0, R0, #UND_Stack_Size

;  Enter Abort Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_ABT:OR:I_Bit:OR:F_Bit
        MOV     SP, R0
        SUB     R0, R0, #ABT_Stack_Size

;  Enter FIQ Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_FIQ:OR:I_Bit:OR:F_Bit
        MOV     SP, R0
        SUB     R0, R0, #FIQ_Stack_Size

;  Enter IRQ Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_IRQ:OR:I_Bit:OR:F_Bit
        MOV     SP, R0
        SUB     R0, R0, #IRQ_Stack_Size

;  Enter Supervisor Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_SVC:OR:I_Bit:OR:F_Bit
        MOV     SP, R0
        SUB     R0, R0, #SVC_Stack_Size

;  Enter System Mode and set its Stack Pointer
        MSR     CPSR_c, #Mode_SYS
        MOV     SP, R0
        SUB     SL, SP, #USR_Stack_Size

; �Ƿ�ʹ����KEIL��΢��
        IF      :DEF:__MICROLIB
        EXPORT  __initial_sp
        ELSE
        MOV     SP, R0
        SUB     SL, SP, #USR_Stack_Size
        ENDIF
; ���Ե�ַ��ת��c���
       IMPORT  main
		bl main
		   
		EXPORT Delay_ms
Delay_ms		   

Delay1	
	LDR  R1, =100000  ; Arm clockΪ400M		
Delay2
	SUBS R1, R1, #1  ; һ��Arm clock
	BNE  Delay2      ; ��ת������ˮ�ߣ�3��Arm clock
	SUBS R0, R0, #1	 ; ������ȷ��nCount��Ϊ0
	BNE	 Delay1
	BX   LR       
                                        

	   END





