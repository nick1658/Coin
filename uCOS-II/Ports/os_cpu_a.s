
		PRESERVE8               
        AREA  OS_CPU_A, CODE, READONLY
;***************************************************************************
;        START MULTITASKING
;    void OSStartHighRdy(void)
;(1) ��ֹ�ж��л�������ģʽ,��������������ڹ���ģʽ
;(2) ���������л����Ӻ��������ȵ���OSTaskSwHook()����
;(3) ���uCOS-II�ں����������У�OSRunning = 1
;(4) ���������ȼ�����TCB���õ�����ջָ�룬SP�л�������ջ
;(5) ��ջSP�е�����ջ����������״̬�Ĵ���CPSR��R0-R12��LR������ִ������
;***************************************************************************
I_Bit           EQU     0x80 ; IRQ�жϽ�ֹλ
F_Bit           EQU     0x40 ; FIQ�жϽ�ֹλ
Mode_SVC		EQU		0x13 ; ����ģʽ
Mode_SYS		EQU		0x1f ; ϵͳģʽ
	IMPORT	OSTaskSwHook
    IMPORT  OSRunning
    IMPORT  OSTCBHighRdy
	EXPORT  OSStartHighRdy
OSStartHighRdy
	MSR     CPSR_c, #(I_Bit+F_Bit+Mode_SVC)	; ��ֹ�ж��л�������ģʽ

	LDR     R0, =OSTaskSwHook ; ���������л����Ӻ���
	MOV     LR, PC            ; ׼���������ص�ַ
	BX      R0	          ; ֧��Thumb��ARM���

	LDR		R0, =OSRunning    ;����OSRunningΪ1
	MOV		R1, #1
	STRB	R1, [R0]

	LDR     R0, =OSTCBHighRdy     ; ���������ȼ�����TCB
	LDR     R0, [R0]              ; �������ջָ��
	LDR     SP, [R0]              ; �л���������ջ

	LDMFD   SP!, {R0}             ; ��ջ�������CPSR
	MSR     SPSR_cxsf, R0
	LDMFD   SP!, {R0-R12, LR, PC}^ ; ��ջ�������������

;*************************************************************************************
;    PERFORM A CONTEXT SWITCH (From task level) - OSCtxSw()
;     PERFORM A CONTEXT SWITCH (From interrupt level) - OSIntCtxSw()
;             void OSCtxSw(void)   void OSIntCtxSw(void)
;(1) ���浱ǰ�����������(R0-R12��LR�������ϵ�PC��ַ��״̬�Ĵ���CPSR)����ǰ����ջ��
;(2) ���ݵ�ǰ����TCB(������ƿ�)����õ�ǰ����ջָ�룬���ѵ�ǰ����SPջ�����ջָ��
;(3) ���������л����Ӻ��������ȵ���OSTaskSwHook()����
;(4) �Ѽ������е�������ȼ��������ȼ����µ���ǰ���ȼ�������
;(5) �Ѽ������е�������ȼ�����TCB(������ƿ�)��ַ���µ���ǰTCB(������ƿ�)��ַ������
;(6) ���������ȼ�����ջָ�룬SP�л���������ȼ�����ջ������ջ������������ģ�ִ��������	 
;****************************************************************************************
Mode_THUMB	EQU		0x20 ; THUMBģʽ
	IMPORT  OSTCBCur
    IMPORT  OSTCBHighRdy
    IMPORT  OSPrioCur
    IMPORT  OSPrioHighRdy

    EXPORT  OSCtxSw
	EXPORT  OSIntCtxSw
OSCtxSw
	STMFD   SP!, {LR} ; ѹջ��ǰ����PC
	STMFD   SP!, {LR} ; ѹջ��ǰ����LR
	STMFD   SP!, {R0-R12}  ; ѹջ��ǰ����R0-R12
	MRS     R0, CPSR       ; ��õ�ǰ����CPSR
	TST     LR, #1         ; ���������Ƿ�����Thumbģʽ
	ORRNE   R0, R0, #Mode_THUMB  ; ��Thumb��״̬�ĳ�Thumbģʽ
	STMFD   SP!, {R0}            ; ѹջCPSR

	LDR     R0, =OSTCBCur     ; ��õ�ǰ����TCB
	LDR     R1, [R0]          ; ��TCB��õ�ǰ����ջָ��
	STR     SP, [R1]          ; SPջ�������ǰ����ջָ��

OSIntCtxSw
	LDR     R0, =OSTaskSwHook ; ���������л����Ӻ���
	MOV     LR, PC            ; ׼���������ص�ַ
	BX      R0

	LDR     R0, =OSPrioCur    ; ��õ�ǰ�������ȼ�����ָ��
	LDR     R1, =OSPrioHighRdy ; ���������ȼ��������ȼ�����ָ��
	LDRB    R2, [R1]           ; ���������ȼ��������ȼ�
	STRB    R2, [R0]           ; �������ǰ�������ȼ�ָ�������

	LDR     R0, =OSTCBCur      ; ��õ�ǰ����TCB����ָ��
	LDR     R1, =OSTCBHighRdy  ; ���������ȼ�����TCB����ָ��
	LDR     R2, [R1]           ; ������ȼ�TCB��ַ�������ǰ����TCBָ��
	STR     R2, [R0]

	LDR     SP, [R2]             ; SP�л���������ȼ�����ջ

	LDMFD   SP!, {R0}            ; ��ջ�������CPSR
	MSR     SPSR_cxsf, R0
	LDMFD   SP!, {R0-R12, LR, PC}^  ; ��ջ�������������

;**************************************************************************************
;    CRITICAL SECTION FUNCTIONS
; �ٽ������ʴӾֲ���������/�ָ��ж�״̬�ٿ����ж�
;**************************************************************************************
	EXPORT  CPU_SR_Save
	EXPORT  CPU_SR_Restore
CPU_SR_Save
	MRS 	R0, CPSR
	ORR 	R1, R0, #0xC0 ; ����IRQ,FIQ����ֹ�ж�
	MSR 	CPSR_c, R1
	BX  	LR	      ; ��ֹ�ж�,�����ж�״̬��R0��

CPU_SR_Restore		
	MSR     CPSR_c, R0
	BX  	LR


	EXPORT  System_call
System_call
	SWI		12
	BX  	LR
	
	IMPORT SWI_Handler
	EXPORT  SWI_IRQ
SWI_IRQ
	STMFD	SP!, {R1-R12,  LR}
	LDR 	R4, [LR, #-4]                                   ; LR - 4 Ϊָ��" swi xxx" �ĵ�ַ����
	BIC   	R4, R4, #0xFF000000                    ; ȡ��ARMָ��24λ������
    LDR     R2, =SWI_Handler
	MOV		LR, PC 
	BX		R2
	ADD		R0, R0, R4
	LDMIA     SP!, {R1-R12, PC}^  
	
;****************************************************************************************
;   IRQ����
; �����쳣��Ӧ����ʱ�������ı��洦��
;(1) ��ʱ��ʹ�õ�һЩ�Ĵ��������õ��ļĴ���ѹջ��IRQջ��
;(2) �л�������ģʽ����ֹ�жϣ����������ڹ���ģʽ���ⲽ���л�SP�����жϴ�ϵ�����ջ��
;(3) �ѱ���������������ѹ�������ջ
;(4) �����ж�Ƕ�׼���,�ж��������жϻ����ж�Ƕ��,�ж�Ƕ�ײ��ø�������ջ
;(5) ���ж�Ƕ��,���ݵ�ǰ����TCB(������ƿ�)���ջָ�룬���Ѵ������SPջ�����ջָ��
;(6) ����OSIntEnter()���������ж�Ƕ�׼Ӽ���
;(7) �л���ϵͳģʽ����ѹջLR���ⲽ��Ϊ��ʹ��ϵͳģʽջ�������жϺ�������������ջ��ʹ�á�
;(8) ����IRQ_Handler()����ʵ�ʴ���IRQ�жϷ������жϷ����п��ٴ�IRQ�жϣ�֧���ж�Ƕ�� 
;(9) �жϷ���ִ����󣬳�ջLR�����л�������ģʽ����ֹ�жϣ���ʱSP���л�����������������ջ��
;(10) ����OSIntExit()���������ж�Ƕ�׼�����������ж�Ƕ�׼���OSIntNestingΪ0����˵�������ж�
;    �˳���������OSIntCtxSw()�����жϼ������л�������ִ������
;(11) ����ж�Ƕ�׼���OSIntNesting��Ϊ0���ж�δȫ���˳������ջ��һ���жϵ������ģ�ִ�б�
;     Ƕ�׵���һ���жϡ�
;*****************************************************************************************
    IMPORT  OSIntEnter
    IMPORT  OSIntExit
	IMPORT	OSIntNesting
    IMPORT  IRQ_Handler
	
	
	IMPORT	is_system_ticks
		
	
    EXPORT  IRQ_SaveContext_os
IRQ_SaveContext_os
	SUB     LR, LR, #4                  ; IRQ�쳣���ص�ַLR-4
	STMFD   SP!, {R0-R2}                ; ��ʱʹ�õĹ����Ĵ���ѹ��IRQջ
	MRS     R0, SPSR                    ; �����쳣����ǰ��CPSR
	MOV     R1, LR                      ; ����LR
	MOV     R2, SP                      ; ����IRQջָ��,������ջ�����Ĵ���
	ADD     SP, SP, #(3 * 4)            ; �ָ�IRQջԭ����λ��
                                                                
	MSR     CPSR_c, #(I_Bit+F_Bit+Mode_SVC)	; ��ֹ�жϡ��л�������ģʽ�����ֳ�

	STMFD   SP!, {R1}                   ; ѹջ��������PC
	STMFD   SP!, {LR}                   ; ѹջ��������LR
	STMFD   SP!, {R3-R12}               ; ѹջ��������R12-R3
	LDMFD   R2!, {R5-R7}                ; ��IRQջ�ָ�R2-R0
	STMFD   SP!, {R5-R7}                ; ѹջ��������R2-R0
	STMFD   SP!, {R0}                   ; ѹջ��������CPSR
	
	;LDR		R0, =OSIntNesting           ; ����ж�Ƕ�׼���
	;LDRB	R1, [R0]
	;CMP		R1, #0                      ; �ж������жϻ����ж�Ƕ��
	;BNE		IntteruptNesting            ; �ж�Ƕ�ײ��ø�������ջָ��
	
	LDR     R0, =OSTCBCur               ; �����жϴ��,��ô������TCB
	LDR     R1, [R0]                    ; ��ñ��������ջָ��
	STR     SP, [R1]                    ; SPջ������������ջָ��
	
;IntteruptNesting	
	;LDR		R0, =OSIntEnter             ; ����OSIntEnter()�����ж�Ƕ�׼���
	;MOV		LR, PC
	;BX		R0

;��ʼ�����ж�
 	MSR     CPSR_c, #(I_Bit+F_Bit+Mode_SYS)	; �л���ϵͳģʽ,ʹ��ϵͳģʽջ�����ж�
	STMFD 	SP!, {LR} ; ѹջϵͳģʽLR
	
	LDR		R0, =IRQ_Handler             ; ����IRQ������
	MOV		LR, PC 
	BX		R0

	LDMFD	SP!, {LR} ; ��ջϵͳģʽLR  
	
	
	MSR     CPSR_c, #(I_Bit+F_Bit+Mode_SVC)	; �л�������ģʽ,ʹ������ջ���г�ջ	
	
	LDR		R0, =is_system_ticks           ; �ж��ǲ���ϵͳ�����ж�
	MOV		LR, PC 
	BX		R0

IntReturn
	LDMFD   SP!, {R0}                     ; �жϷ���Ƕ��,��ջ��һ���жϵ�������
	MSR     SPSR_cxsf, R0
	LDMFD   SP!, {R0-R12, LR, PC}^

    END

