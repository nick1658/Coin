
; DRAM controller base address
DRAM_BASE		EQU 	0x48000000
BANKCFG_OFS		EQU		0x00
BANKCON1_OFS		EQU		0x04
BANKCON2_OFS		EQU		0x08
BANKCON3_OFS		EQU		0x0C
REFRESH_OFS		EQU		0x10
TIMEOUT_OFS		EQU		0x14

        PRESERVE8
        AREA    LOWLEVELINIT, CODE, READONLY
        ARM
		EXPORT  ERAM_Init
; K4T51163QJ-BCE79(DDR2@400M 5-5-5),64MB,Raw Addr A0~A12,Column Addr A0~A9
; ����DDR0 13λ�е�ַ��10λ�е�ַ��DDR2�ӿڣ�16λ����
; DDR�������nRAS,nCAS,nWE,nCS�������߷ֱ�
; Active����,���м�bank��ַ
; Read����,��Active��,���е�ַ��
; Write����,��Active��,���е�ַд
; Precharge����,�ر�bank,����A[10]ȷ���ر�ָ��������bank
; AUTO REFRESH or SELF REFRESH����,ˢ������
; LOAD MODE REGISTER����,дģʽ�Ĵ���
ERAM_Init
		LDR		R0, =DRAM_BASE
		LDR		R1, =(2<<17)+(2<<11)+(0<<6)+(1<<1)+(1<<0)
		STR		R1, [R0, #BANKCFG_OFS]
; DQS delay 3,Write buffer,Auto pre-charge,bank address �ڸ�λ 				
		LDR		R1, =(3<<28)+(1<<26)+(1<<8)+(0<<7)+ \
							 (1<<6)+(0<<5)+(1<<4)
		STR		R1, [R0, #BANKCON1_OFS]		
				
; s3c2416 ddr2�Ĵ�����clk����ֵ�����HCLK��
; RAS [23:20] Row active time 45ns HCLK=133M DDR2=266M 6clock
; Active���Precharge�������Сʱ��45ns
; ARFC [19:16] Row cycle time tRFC=105ns 14clock
; ָ��ˢ��ʱ��105ns
; CAS Latency [5:4] CAS latency control 12.5ns 2clock
; Read/Write������󾭹�5tCK=12.5ns���ݲ���Ч
; tRCD [3:2] RAS to CAS delay 12.5ns 2clock
; Active�����辭5tCK=12.5ns��ŷ���Read/Write����
; tRP [1:0] Row pre-charge time 12.5ns 2clock
; Precharge�������Active����5tCK=12.5ns
; ������Active�����������Сʱ�� tRC=tRAS+tRP=57.5ns
		LDR		R1, =(6<<20)+(13<<16)+(3<<4)+(2<<2)+(2<<0)
		STR		R1, [R0, #BANKCON2_OFS]	

; issue a PALL(pre-charge all) command,��Precharge����
		LDR		R1, [R0, #BANKCON1_OFS]
		BIC		R1, R1, #0x03
		ORR		R1, R1, #0x01
		STR		R1, [R0, #BANKCON1_OFS]

; issue an EMRS(extern mode register) command to EMR(2)
		LDR		R1, =(0x2<<30)+(0<<23)+(0<<19)+(0<<16)
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		ORR		R1, R1, #0x03				
		STR		R1, [R0, #BANKCON1_OFS]

; issue an EMRS(extern mode register) command to EMR(3)
		LDR		R1, =(0x3<<30)
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		ORR		R1, R1, #0x03				
		STR		R1, [R0, #BANKCON1_OFS]
				
; issue an EMRS to enable DLL and RDQS, nDQS, ODT disable
		LDR		R1, =0xFFFF0000
		LDR		R2, [R0, #BANKCON3_OFS]
		BIC		R2, R2, R1
		LDR		R1, =(0x1<<30)+(0<<28)+(0<<27)+(1<<26)+ \
					 (7<<23)+(0<<19)+(0<<22)+(0<<18)+ \
					 (0x0<<17)+(0<<16)				
		ORR		R1, R1, R2
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		ORR		R1, R1, #0x03				
		STR		R1, [R0, #BANKCON1_OFS]

; issue a mode register set command for DLL reset 
		LDR		R1, =0x0000FFFF
		LDR		R2, [R0, #BANKCON3_OFS]
		BIC		R2, R2, R1
		LDR		R1, =(0x1<<9)+(1<<8)+(0<<7)+(3<<4)						
		ORR		R1, R1, R2
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		BIC		R1, R1, #0x03
		ORR		R1, R1, #0x02				
		STR		R1, [R0, #BANKCON1_OFS]
				
; Issue a PALL(pre-charge all) command	
		LDR		R1, [R0, #BANKCON1_OFS]
		BIC		R1, R1, #0x03
		ORR		R1, R1, #0x01
		STR		R1, [R0, #BANKCON1_OFS]
				
; Issue 2 or more auto-refresh commands
		LDR		R1, =0x20
		STR		R1, [R0, #REFRESH_OFS]
				

; Issue a MRS command with LOW to A8 to initialize device operation
		LDR		R1, =0x0000FFFF
		LDR		R2, [R0, #BANKCON3_OFS]
		BIC		R2, R2, R1
		LDR		R1, =(0x1<<9)+(0<<8)+(0<<7)+(3<<4)					
		ORR		R1, R1, R2
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		BIC		R1, R1, #0x03
		ORR		R1, R1, #0x02				
		STR		R1, [R0, #BANKCON1_OFS]
				
; Wait 200 clock, execute OCD Calibration
		LDR		R1, =200
0		SUBS		R1, R1, #1
		BNE		%B0
				
; Issue a EMRS1 command to over OCD Mode Calibration
		LDR		R1, =0xFFFF0000
		LDR		R2, [R0, #BANKCON3_OFS]
		BIC		R2, R2, R1
		LDR		R1, =(0x1<<30)+(0<<28)+(0<<27)+(1<<26)+ \
					 (0<<23)+(0<<19)+(0<<22)+(0<<18)+ \
					 (0x0<<17)+(0<<16)				
		ORR		R1, R1, R2
		STR		R1, [R0, #BANKCON3_OFS]
		LDR		R1, [R0, #BANKCON1_OFS]
		ORR		R1, R1, #0x03				
		STR		R1, [R0, #BANKCON1_OFS]

; Refresh period is 7.8us, HCLK=100M, REFCYC=780
		LDR		R1, =780
		STR		R1, [R0, #REFRESH_OFS]

; issue a Normal mode
		LDR		R1, [R0, #BANKCON1_OFS]
		BIC		R1, R1, #0x03
		STR		R1, [R0, #BANKCON1_OFS]
				
		BX		LR
				

; SD/MMC Device Boot Block Assignment
eFuseBlockSize  		EQU		1
SdReservedBlockSize		EQU		1
BL1BlockSize			EQU		16
SdBL1BlockStart		EQU		SdReservedBlockSize + \
						eFuseBlockSize + BL1BlockSize
globalBlockSizeHide		EQU		0x40003FFC
CopyMovitoMem			EQU		0x40004008

; Nand controller base address
NFCONF			EQU		0x4E000000

				EXPORT	__CodeSize__
				EXPORT	__CodeAddr__
; ����������ƺ������Թ������������
				EXPORT  CopyCodeToRAM
; ����Nand�����ĳ�ʼ��������Nand������
				IMPORT	Nand_Init
				IMPORT	Nand_ReadSkipBad
; �����������������ţ���ȷ����������λ�ã��������ɵĴ�С
				IMPORT  ||Image$$ER_ROM0$$Base||
				IMPORT  ||Load$$ER_ROM0$$Length||	
				IMPORT  ||Load$$ER_ROM1$$Length||	
				IMPORT  ||Load$$RW_RAM$$RW$$Length||

; ����������������������λ��
__CodeAddr__	DCD		||Image$$ER_ROM0$$Base||
; �������������豣��Ĵ����Լ����ʼ���ı�����С
__CodeSize__	DCD		||Load$$ER_ROM0$$Length|| + \
				||Load$$ER_ROM1$$Length|| + \
				||Load$$RW_RAM$$RW$$Length||	
									
CopyCodeToRAM
		STMFD	SP!, {LR} ; ���淵�ص�ַ
; �ж�NFCONF�����λΪ1��˵�������豸ΪNand		
		LDR		R0, =NFCONF
		LDR		R1, [R0]
		AND		R1, R1, #0x80000000
		CMP		R1, #0x80000000
		BNE		MMC_SD_Boot	

Nand_Boot		
		BL		Nand_Init ; Nand��ʼ��
		MOV		R0, #0
		LDR		R1, __CodeAddr__	
		LDR		R2, __CodeSize__
		BL		Nand_ReadSkipBad; ����Nand������
		MOVS	R0, R0 ; ����ֵȷ�������ɹ�����ʧ��
Nand_Boot_Loop	
		BNE		Nand_Boot_Loop; ���ط�0˵������ʧ��
		B		AfterCopy
		
MMC_SD_Boot
; ����Ҫ����ʼ��
		LDR		R3, =0 
; ����sd�����뵽����ִ�����ڴ���봦
		LDR		R2, __CodeAddr__
				
; �������Ĵ�С����block�ƣ�����512�ֽڵ���1��block
; ����Ĵ�С����Code RO-data RW-data(�����豣�����ʼ����RW�ĳ�ʼֵ)
; ���뱣����ROM��,Ӧ�Ӽ�����õ�ROM�Ĵ�С,������ִ����,����������ѹ��
; ����α����ڼ������ROM��
		LDR		R0, __CodeSize__
		LDR		R1, =0x1ff
		TST		R0, R1 ; �Ƿ���һ��block(512Bytes)
		BEQ		%F0 ; ����ǡ��block���룬���üӶ�һ��block
		ADD		R0, R0, #512				
0		LSR		R1, R0, #9 ; �õ������block��С

; ���������SD/MMC���е�block������ַ
		LDR		R4, =SdBL1BlockStart	
		LDR		R0, =globalBlockSizeHide
		LDR		R0, [R0] ; SD/MMC����block��
		SUB		R0, R4 ; ��ȥ�����鼰BL1��С
		CMP		R1, #16 ; ���벻��8k,ֱ��BL1������
		BLS		ParameterOK ; ��������16��block��ת	
		SUB		R0, R1 ; �ټ�ȥ����Ĵ�С�������blockλ��

; ����IROM Movi������������������IROM������������ʱ��25M	
ParameterOK
		LDR		R4, =CopyMovitoMem
		LDR		R4, [R4]
		MOV		LR, PC ; ׼�������ķ��ص�ַ
		BX		R4
		MOVS	R0, R0 ; ����ֵȷ�������ɹ�����ʧ��
MMC_SD_Boot_Loop			
		BEQ		MMC_SD_Boot_Loop ; ����0˵������ʧ��
				
AfterCopy	
		LDMFD	SP!, {PC} ; ��������			

		END
				