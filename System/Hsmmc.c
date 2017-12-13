#include "s3c2416.h"
//#define DEBUG_HSMMC
#ifdef  DEBUG_HSMMC
#define Debug cy_println
#else
#define Debug(x...) 
#endif

static unsigned char CardType; // ������
static unsigned int RCA; // ����Ե�ַ
// Hsmmc����,�ڴ�ռ��cache,����DMA����,�����Ӷ�section("No_Cache")�����ڴ�,16k
__align(4) static unsigned char Hsmmc_Buffer[16*1024] __attribute__((section("No_Cache"), zero_init));

__asm static void Delay_us(unsigned int nCount) 
{
//��ʱ1us,����ʱnCount(R0) us
Delay1	
		LDR  R1, =100  // Arm clockΪ400M		
Delay2
		SUBS R1, R1, #1  // һ��Arm clock
		BNE  Delay2      // ��ת������ˮ�ߣ�3��Arm clock
		SUBS R0, R0, #1	 // ������ȷ��nCount��Ϊ0
		BNE	 Delay1
		BX   LR
}


static void Hsmmc_ClockOn(unsigned char On)
{
	if (On) {
		rHM1_CLKCON |= (1<<2); // sdʱ��ʹ��
		while (!(rHM1_CLKCON & (1<<3))) {
			// �ȴ�SD���ʱ���ȶ�
		}
	} else {
		rHM1_CLKCON &= ~(1<<2); // sdʱ�ӽ�ֹ
	}
}

static void Hsmmc_SetClock(unsigned int Div)
{
	Hsmmc_ClockOn(0); // �ر�ʱ��	
	// ѡ��SCLK_HSMMC:EPLLout
	rCLKSRC &= ~(1<<17); // HSMMC1 EPLL(96M)
	rHM1_CONTROL2 = 0xc0000120; // SCLK_HSMMC
	rHM1_CONTROL3 = (0<<31) | (0<<23) | (0<<15) | (0<<7);
	// SDCLKƵ��ֵ��ʹ���ڲ�ʱ��
	rHM1_CLKCON &= ~(0xff<<8);
	rHM1_CLKCON |= (Div<<8) | (1<<0);
	while (!(rHM1_CLKCON & (1<<1))) {
		// �ȴ��ڲ�ʱ�����ȶ�
	}
	Hsmmc_ClockOn(1); // ȫ��ʱ��

}

static int Hsmmc_WaitForCommandDone()
{
	unsigned int i;	
	int ErrorState;
	// �ȴ���������
	for (i=0; i<20000000; i++) {
		if (rHM1_NORINTSTS & (1<<15)) { // ���ִ���
			break;
		}
		if (rHM1_NORINTSTS & (1<<0)) {
			do {
				rHM1_NORINTSTS = (1<<0); // ����������λ
			} while (rHM1_NORINTSTS & (1<<0));			
			return 0; // ����ͳɹ�
		}
	}
	ErrorState = rHM1_ERRINTSTS & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
	rHM1_NORINTSTS = rHM1_NORINTSTS; // ����жϱ�־
	rHM1_ERRINTSTS = rHM1_ERRINTSTS; // ��������жϱ�־	
	do {
		rHM1_NORINTSTS = (1<<0); // ����������λ
	} while (rHM1_NORINTSTS & (1<<0));
	
	Debug("Command error, rHM1_ERRINTSTS = 0x%x ", ErrorState);	
	return ErrorState; // ����ͳ���	
}

static int Hsmmc_WaitForTransferDone()
{
	int ErrorState;
	unsigned int i;
	// �ȴ����ݴ������
	for (i=0; i<20000000; i++) {
		if (rHM1_NORINTSTS & (1<<15)) { // ���ִ���
				break;
		}											
		if (rHM1_NORINTSTS & (1<<1)) { // ���ݴ�����								
			do {
				rHM1_NORINTSTS |= (1<<1); // ����������λ
			} while (rHM1_NORINTSTS & (1<<1));	
			rHM1_NORINTSTS = (1<<3); // ���DMA�жϱ�־								
			return 0;
		}
		Delay_us(1);
	}

	ErrorState = rHM1_ERRINTSTS & 0x1ff; // ����ͨ�Ŵ���,CRC�������,��ʱ��
	rHM1_NORINTSTS = rHM1_NORINTSTS; // ����жϱ�־
	rHM1_ERRINTSTS = rHM1_ERRINTSTS; // ��������жϱ�־
	Debug("Transfer error, rHM1_ERRINTSTS = 0x%04x\n\r", ErrorState);	
	do {
		rHM1_NORINTSTS = (1<<1); // ���������������λ
	} while (rHM1_NORINTSTS & (1<<1));
	
	return ErrorState; // ���ݴ������		
}

static int Hsmmc_IssueCommand(unsigned char Cmd, unsigned int Arg, unsigned char Data, unsigned char Response)
{
	unsigned int i;
	unsigned int Value;
	unsigned int ErrorState;
	// ���CMD���Ƿ�׼���÷�������
	for (i=0; i<1000000; i++) {
		if (!(rHM1_PRNSTS & (1<<0))) {
			break;
		}
	}
	if (i == 1000000) {
		Debug("CMD line time out, rHM1_PRNSTS: %04x\n\r", rHM1_PRNSTS);
		return -1; // ���ʱ
	}
	// ���DAT���Ƿ�׼����
	if (Response == Response_R1b) { // R1b�ظ�ͨ��DAT0����æ�ź�
		for (i=0; i<1000000; i++) {
			if (!(rHM1_PRNSTS & (1<<1))) {
				break;
			}		
		}
		if (i == 1000000) {
			Debug("Data line time out, rHM1_PRNSTS: %04x\n\r", rHM1_PRNSTS);			
			return -2;
		}		
	}

	rHM1_ARGUMENT = Arg; // д���������
	Value = (Cmd << 8); // command index
	// CMD12����ֹ����
	if (Cmd == 0x12) {
		Value |= (0x3 << 6); // command type
	}
	if (Data) {
		Value |= (1 << 5); // ��ʹ��DAT����Ϊ�����
	}	
	
	switch (Response) {
	case Response_NONE:
		Value |= (0<<4) | (0<<3) | 0x0; // û�лظ�,��������CRC
		break;
	case Response_R1:
	case Response_R5:
	case Response_R6:
	case Response_R7:		
		Value |= (1<<4) | (1<<3) | 0x2; // ���ظ��е�����,CRC
		break;
	case Response_R2:
		Value |= (0<<4) | (1<<3) | 0x1; // �ظ�����Ϊ136λ,����CRC
		break;
	case Response_R3:
	case Response_R4:
		Value |= (0<<4) | (0<<3) | 0x2; // �ظ�����48λ,���������CRC
		break;
	case Response_R1b:
		Value |= (1<<4) | (1<<3) | 0x3; // �ظ���æ�ź�,��ռ��Data[0]��
		break;
	default:
		break;	
	}
	rHM1_CMDREG = Value;
	
	ErrorState = Hsmmc_WaitForCommandDone();
	if (ErrorState) {
		Debug("Command = %d\r\n", Cmd);
	}	
	return ErrorState; // ����ͳ���
}

// 512λ��sd����չ״̬λ
int Hsmmc_GetSdState(unsigned char *pState)
{
	int ErrorState;
	unsigned int i;
	if (CardType == SD_HC || CardType == SD_V2 || CardType == SD_V1) {
		if (Hsmmc_GetCardState() != 4) { // ������transfer status
			return -1; // ��״̬����
		}		
		Hsmmc_IssueCommand(CMD55, RCA<<16, 0, Response_R1);
		
		rHM1_SYSAD = (unsigned int)Hsmmc_Buffer; // �����ַ	
		rHM1_BLKSIZE = (7<<12) | (64<<0); // ���DMA�����С,blockΪ512λ64�ֽ�			
		rHM1_BLKCNT = 1; // д����ζ�1 block��sd״̬����
		rHM1_ARGUMENT = 0; // д���������	

		// DMA����ʹ��,������		
		rHM1_TRNMOD = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (1<<0);	
		// ��������Ĵ���,��״̬����CMD13,R1�ظ�
		rHM1_CMDREG = (CMD13<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
		ErrorState = Hsmmc_WaitForCommandDone();
		if (ErrorState) {
			Debug("CMD13 error\r\n");
			return ErrorState;
		}
		ErrorState = Hsmmc_WaitForTransferDone();
		if (ErrorState) {
			Debug("Get sd status error\r\n");
			return ErrorState;
		}
		for (i=0; i<64; i++) {
			*pState++ = Hsmmc_Buffer[i];
		}
		return 0;
	}
	return -1; // ��sd��
}

int Hsmmc_Get_CSD(unsigned char *pCSD)
{
	unsigned int i;
	unsigned int Response[4];
	int State = 1;

	if (CardType != SD_HC && CardType != SD_V1 && CardType != SD_V2) {
		return State; // δʶ��Ŀ�
	}
	// ȡ����ѡ��,�κο������ظ�,��ѡ��Ŀ�ͨ��RCA=0ȡ��ѡ��,
	// ���ص�stand-by״̬
	Hsmmc_IssueCommand(CMD7, 0, 0, Response_NONE);
	for (i=0; i<1000; i++) {
		if (Hsmmc_GetCardState() == 3) { // CMD9��������standy-by status
			Debug("Get CSD: Enter to the Stand-by State\n\r");					
				break; // ״̬��ȷ
		}
			Delay_us(100);
	}
	if (i == 1000) {
		return State; // ״̬����
	}	
	// �����ѱ�ǿ����Ϳ��ض�����(CSD),��ÿ���Ϣ
	if (!Hsmmc_IssueCommand(CMD9, RCA<<16, 0, Response_R2)) {
		pCSD++; // ·����һ�ֽ�,CSD��[127:8]λ��λ�Ĵ����е�[119:0]
		Response[0] = rHM1_RSPREG0;
		Response[1] = rHM1_RSPREG1;
		Response[2] = rHM1_RSPREG2;
		Response[3] = rHM1_RSPREG3;	
		Debug("CSD: ");
		for (i=0; i<15; i++) { // �����ظ��Ĵ����е�[119:0]��pCSD��
			*pCSD++ = ((unsigned char *)Response)[i];
			Debug("%02x", *(pCSD-1));
		}
		State = 0; // CSD��ȡ�ɹ�
	}

	Hsmmc_IssueCommand(CMD7, RCA<<16, 0, Response_R1); // ѡ��,���ص�transfer״̬
	return State;
}

// R1�ظ��а�����32λ��card state,��ʶ���,������һ״̬ͨ��CMD13��ÿ�״̬
int Hsmmc_GetCardState(void)
{
	if (Hsmmc_IssueCommand(CMD13, RCA<<16, 0, Response_R1)) {
		return -1; // ������
	} else {
		return ((rHM1_RSPREG0>>9) & 0xf); // ����R1�ظ��е�[12:9]��״̬
	}
}


static int Hsmmc_SetBusWidth(unsigned char Width)
{
	int State;
	if ((Width != 1) || (Width != 4)) {
		return -1;
	}
	State = -1; // ���ó�ʼΪδ�ɹ�
	rHM1_NORINTSTSEN &= ~(1<<8); // �رտ��ж�
	Hsmmc_IssueCommand(CMD55, RCA<<16, 0, Response_R1);
	if (Width == 1) {
		if (!Hsmmc_IssueCommand(CMD6, 0, 0, Response_R1)) { // 1λ��
			rHM_HOSTCTL &= ~(1<<1);
			State = 0; // ����ɹ�
		}
	} else {
		if (!Hsmmc_IssueCommand(CMD6, 2, 0, Response_R1)) { // 4λ��
			rHM_HOSTCTL |= (1<<1);
			State = 0; // ����ɹ�
		}
	}
	rHM1_NORINTSTSEN |= (1<<8); // �򿪿��ж�	
	return State; // ����0Ϊ�ɹ�
}

int Hsmmc_EraseBlock(unsigned int StartBlock, unsigned int EndBlock)
{
	unsigned int i;

	if (CardType == SD_V1 || CardType == SD_V2) {
		StartBlock <<= 9; // ��׼��Ϊ�ֽڵ�ַ
		EndBlock <<= 9;
	} else if (CardType != SD_HC) {
		return -1; // δʶ��Ŀ�
	}	
	Hsmmc_IssueCommand(CMD32, StartBlock, 0, Response_R1);
	Hsmmc_IssueCommand(CMD33, EndBlock, 0, Response_R1);
	if (!Hsmmc_IssueCommand(CMD38, 0, 0, Response_R1b)) {
		for (i=0; i<10000; i++) {
			if (Hsmmc_GetCardState() == 4) { // ������ɺ󷵻ص�transfer״̬
				Debug("erasing complete!\n\r");
				return 0; // �����ɹ�				
			}
			Delay_us(1000);			
		}		
	}		

	Debug("Erase block failed\n\r");
	return 1; // ����ʧ��
}

int Hsmmc_ReadBlock(unsigned char *pBuffer, unsigned int BlockAddr, unsigned int BlockNumber)
{
	unsigned int Address = 0;
	unsigned int ReadBlock;
	unsigned int i;
	int ErrorState;
	
	if (pBuffer == 0 || BlockNumber == 0) {
		return -1;
	}

	// �����ж�ʹ��,������Ӧ���ж��ź�
	rHM1_NORINTSIGEN &= ~0xffff; // ��������ж�ʹ��
	rHM1_NORINTSIGEN |= (1<<1); // ��������ж�ʹ��
	
	while (BlockNumber > 0) {
		for (i=0; i<1000; i++) {
			if (Hsmmc_GetCardState() == 4) { // ��д��������transfer status
				break; // ״̬��ȷ
			}
			Delay_us(100);
		}
		if (i == 1000) {
			return -2; // ״̬����
		}		
		
		if (BlockNumber <= sizeof(Hsmmc_Buffer)/512) {
			ReadBlock = BlockNumber; // ��ȡ�Ŀ���С�ڻ���32 Block(16k)
			BlockNumber = 0; // ʣ���ȡ����Ϊ0
		} else {
			ReadBlock = sizeof(Hsmmc_Buffer)/512; // ��ȡ�Ŀ�������32 Block,�ֶ�ζ�
			BlockNumber -= ReadBlock;
		}
		// ����sd������������׼,��˳��д��������������Ӧ�ļĴ���
		rHM1_SYSAD = (unsigned int)Hsmmc_Buffer; // �����ַ,�ڴ�����Ϊ�ر�cache,��DMA����			
		rHM1_BLKSIZE = (7<<12) | (512<<0); // ���DMA�����С,blockΪ512�ֽ�			
		rHM1_BLKCNT = ReadBlock; // д����ζ�block��Ŀ
		

		if (CardType == SD_HC) {
			Address = BlockAddr; // SDHC��д���ַΪblock��ַ
		} else if (CardType == SD_V1 || CardType == SD_V2) {
			Address = BlockAddr << 9; // ��׼��д���ַΪ�ֽڵ�ַ
		}	
		BlockAddr += ReadBlock; // ��һ�ζ���ĵ�ַ
		rHM1_ARGUMENT = Address; // д���������		
		
		if (ReadBlock == 1) {
			// ���ô���ģʽ,DMA����ʹ��,������
			rHM1_TRNMOD = (0<<5) | (1<<4) | (0<<2) | (1<<1) | (1<<0);
			// ��������Ĵ���,�����CMD17,R1�ظ�
			rHM1_CMDREG = (CMD17<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;
		} else {
			// ���ô���ģʽ,DMA����ʹ��,�����			
			rHM1_TRNMOD = (1<<5) | (1<<4) | (1<<2) | (1<<1) | (1<<0);
			// ��������Ĵ���,����CMD18,R1�ظ�	
			rHM1_CMDREG = (CMD18<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;			
		}	
		ErrorState = Hsmmc_WaitForCommandDone();
		if (ErrorState) {
			Debug("Read Command error\r\n");
			return ErrorState;
		}		
		ErrorState = Hsmmc_WaitForTransferDone();
		if (ErrorState) {
			Debug("Read block error\r\n");
			return ErrorState;
		}
		// ���ݴ���ɹ�,����DMA��������ݵ�ָ���ڴ�
		for (i=0; i<ReadBlock*512; i++) {
			*pBuffer++ = Hsmmc_Buffer[i];
		}		
	}
	return 0; // ���п����
}

int Hsmmc_WriteBlock(unsigned char *pBuffer, unsigned int BlockAddr, unsigned int BlockNumber)
{
	unsigned int Address = 0;
	unsigned int WriteBlock;
	unsigned int i;
	int ErrorState;
	
	if (pBuffer == 0 || BlockNumber == 0) {
		return -1; // ��������
	}
	
	rHM1_NORINTSIGEN &= ~0xffff; // ��������ж�ʹ��
	// ���ݴ�������ж�ʹ��
	rHM1_NORINTSIGEN |= (1<<0);
	
	while (BlockNumber > 0) {
		for (i=0; i<1000; i++) {
			if (Hsmmc_GetCardState() == 4) { // ��д��������transfer status
				break; // ״̬��ȷ
			}
			Delay_us(100);
		}
		if (i == 1000) {
			return -2; // ״̬�����Programming��ʱ
		}
		
		if (BlockNumber <= sizeof(Hsmmc_Buffer)/512) {
			WriteBlock = BlockNumber;// д��Ŀ���С�ڻ���32 Block(16k)
			BlockNumber = 0; // ʣ��д�����Ϊ0
		} else {
			WriteBlock = sizeof(Hsmmc_Buffer)/512; // д��Ŀ�������32 Block,�ֶ��д
			BlockNumber -= WriteBlock;
		}
		if (WriteBlock > 1) { // ���д,����ACMD23������Ԥ��������
			Hsmmc_IssueCommand(CMD55, RCA<<16, 0, Response_R1);
			Hsmmc_IssueCommand(CMD23, WriteBlock, 0, Response_R1);
		}
		
		for (i=0; i<WriteBlock*512; i++) {
			Hsmmc_Buffer[i] = *pBuffer++; // ��д���ݴ�ָ���ڴ���������������
		}
		// ����sd������������׼,��˳��д��������������Ӧ�ļĴ���		
		rHM1_SYSAD = (unsigned int)Hsmmc_Buffer; // �����ַ,�ڴ�����Ϊ�ر�cache,��DMA����		
		rHM1_BLKSIZE = (7<<12) | (512<<0); // ���DMA�����С,blockΪ512�ֽ�		
		rHM1_BLKCNT = WriteBlock; // д��block��Ŀ	
		
		if (CardType == SD_HC) {
			Address = BlockAddr; // SDHC��д���ַΪblock��ַ
		} else if (CardType == SD_V1 || CardType == SD_V2) {
			Address = BlockAddr << 9; // ��׼��д���ַΪ�ֽڵ�ַ
		}
		BlockAddr += WriteBlock; // ��һ��д��ַ
		rHM1_ARGUMENT = Address; // д���������			
		
		if (WriteBlock == 1) {
			// ���ô���ģʽ,DMA����д����
			rHM1_TRNMOD = (0<<5) | (0<<4) | (0<<2) | (1<<1) | (1<<0);
			// ��������Ĵ���,����дCMD24,R1�ظ�
			rHM1_CMDREG = (CMD24<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;			
		} else {
			// ���ô���ģʽ,DMA����д���
			rHM1_TRNMOD = (1<<5) | (0<<4) | (1<<2) | (1<<1) | (1<<0);
			// ��������Ĵ���,���дCMD25,R1�ظ�
			rHM1_CMDREG = (CMD25<<8)|(1<<5)|(1<<4)|(1<<3)|0x2;					
		}
		ErrorState = Hsmmc_WaitForCommandDone();
		if (ErrorState) {
			Debug("Write Command error\r\n");
			return ErrorState;
		}		
		ErrorState = Hsmmc_WaitForTransferDone();
		if (ErrorState) {
			Debug("Write block error\r\n");
			return ErrorState;
		}
	}
	return 0; // д����������
}

static S16 Hsmmc_init_flag = 0;
int Hsmmc_Init()
{
	unsigned int i;
	unsigned int OCR;

	if (Hsmmc_init_flag)
		return 0;
	cy_print ("Init SD Card\n");
	// ����HSMMC1�Ľӿ���������
	rGPLCON &= ~((0xffff<<0) | (0xf<<16));
	rGPLCON |= (0xaaaa<<0) | (0xa<<16);
	rGPLUDP &= ~((0xffff<<0) | (0xf<<16)); // ��������ֹ
		
	rHM1_SWRST = 0x7; // ��λHSMMC
	Hsmmc_SetClock(0x80); // SDCLK=96M/256=375K
	rHM1_TIMEOUTCON = (0xe << 0); // ���ʱʱ��
	rHM1_HOSTCTL &= ~(1<<2); // �����ٶ�ģʽ
	rHM1_NORINTSTS = rHM1_NORINTSTS; // ����ж�״̬��־
	rHM1_ERRINTSTS = rHM1_ERRINTSTS; // ��������ж�״̬��־
	rHM1_NORINTSTSEN = 0x7fff; // [14:0]�ж�ʹ��
	rHM1_ERRINTSTSEN = 0x3ff; // [9:0]�����ж�ʹ��
	Hsmmc_IssueCommand(CMD0, 0, 0, Response_NONE); // ��λ���п�������״̬
	
	CardType = UnusableCard; // �����ͳ�ʼ��������
	if (Hsmmc_IssueCommand(CMD8, 0x1aa, 0, Response_R7)) { // û�лظ�,MMC/sd v1.x/not card
			for (i=0; i<1000; i++) {
				Hsmmc_IssueCommand(CMD55, 0, 0, Response_R1);
				if (!Hsmmc_IssueCommand(CMD41, 0, 0, Response_R3)) { // CMD41�лظ�˵��Ϊsd��
					OCR = rHM1_RSPREG0; // ��ûظ���OCR(���������Ĵ���)ֵ
					if (OCR & 0x80000000) { // ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
						CardType = SD_V1; // ��ȷʶ���sd v1.x��
						Debug("SD card version 1.x is detected\n\r");
						break;
					}
				} else {
					// MMC��ʶ��
				}
				Delay_us(100);				
			}
	} 
	else 
	{ // sd v2.0
		if (((rHM1_RSPREG0&0xff) == 0xaa) && (((rHM1_RSPREG0>>8)&0xf) == 0x1)) { // �жϿ��Ƿ�֧��2.7~3.3v��ѹ
			OCR = 0;
			for (i=0; i<1000; i++) {
				Hsmmc_IssueCommand(CMD55, 0, 0, Response_R1);
				Hsmmc_IssueCommand(CMD41, OCR | 1 << 30, 0, Response_R3); // reday̬
				OCR = rHM1_RSPREG0;
				if (OCR & 0x80000000) { // ���ϵ��Ƿ�����ϵ�����,�Ƿ�busy
					if (OCR & (1<<30)) { // �жϿ�Ϊ��׼�����Ǹ�������
						CardType = SD_HC; // ��������
						Debug("SDHC card is detected\n\r");
					} else {
						CardType = SD_V2; // ��׼��
						Debug("SD version 2.0 standard card is detected\n\r");
					}
					break;
				}
				Delay_us(100);
			}
		}
	}
	switch (CardType)
	{
		case SD_HC:
			Debug ("CardType = SD_HC");
			break;
		case SD_V1:
			Debug ("CardType = SD_V1");
			break;
		case SD_V2:
			Debug ("CardType = SD_V2");
			break;
		default:Debug ("CardType = Unknown");break;
	}
	if (CardType == SD_HC || CardType == SD_V1 || CardType == SD_V2) 
	{
		Hsmmc_IssueCommand(CMD2, 0, 0, Response_R2); // ���󿨷���CID(��ID�Ĵ���)��,����ident
		Hsmmc_IssueCommand(CMD3, 0, 0, Response_R6); // ���󿨷����µ�RCA(����Ե�ַ),Stand-by״̬
		RCA = (rHM1_RSPREG0 >> 16) & 0xffff; // �ӿ��ظ��еõ�����Ե�ַ

		Hsmmc_IssueCommand(CMD7, RCA<<16, 0, Response_R1); // ѡ���ѱ�ǵĿ�,transfer״̬
		Debug("Enter to the transfer state\n\r");		
		Hsmmc_SetClock(0x2); // ����SDCLK = 96M/4 = 24M
		
		if (!Hsmmc_SetBusWidth(4)) 
		{
			Debug("Set bus width error\n\r");
			return 1; // λ�����ó���
		}
		if (Hsmmc_GetCardState() == 4) 
		{ // ��ʱ��Ӧ��transfer̬
			if (!Hsmmc_IssueCommand(CMD16, 512, 0, Response_R1)) 
			{ // ���ÿ鳤��Ϊ512�ֽ�
				rHM1_NORINTSTS = 0xffff; // ����жϱ�־
				Hsmmc_init_flag = 1;
				Debug("Card Initialization succeed\n\r");
				return 0; // ��ʼ���ɹ�
			}
		}
	}
	Debug("Card Initialization failed\n\r");
	return 1; // �������쳣
}

int Hsmmc_exist (void)
{
	return Hsmmc_init_flag;
}
