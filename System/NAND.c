#include "NAND.h"
#include "s3c2416.h"



void delay_us(unsigned int t)
{
	volatile int i;
	while (t--) {
		for (i=0 ; i<9 ; i++)
			;
	}
}
void delay_ms1(unsigned int t)
{
	volatile int i;
	while (t--) {
		for (i=0 ; i<5555 ; i++)
			;
	}
}


void rNF_Reset()
{
	NF_CE_L();//nf =0 select chip
	NF_CLEAR_RB();//RnB 0 to 1, transition is detected,clear this
	NF_CMD(NAND_CMD_RESET);  //NFCMMD=0xff memory command value 0xff��ʲô����
	NF_DETECT_RB(); //if RnB is 0,waiting ,if 1,go on  
	NF_CE_H();// nf =1 not select chip
}

void rNF_Init(void)
{
	//NANAFLASH CONTROL CONFIG
	rNFCONF=(rNFCONF&~(0x1<<3))|(0x1<<2);
	//pagesize_ext = 1 large size nandflash--- pagesize set 2048byte/page 
	rNFCONF = ( rNFCONF&~(0x3<<23) )|(0x2<<23)|(0x7<<12)|(0x7<<8)|(0x7<<4);
	//indicate will use 4bit ECC,CLE/ALE duration = HCLK*0x7,TWRPH0/1 duration = HCLK*(0X7+1),
	rNFCONT = ( rNFCONT&~(0x1<<18) ) |(1<<18)|(0<<16)|(1<<0); // Init NFCONT
	//encoding 4bit ECC,disable lock,nandflash work	
	rNFSTAT |= ((1<<6)|(1<<5)|(1<<4));
	//nf chip select(nfchoose),init main/spare ECC encoder/decoder
	rNFSBLK=0;
	//nf programable start block address 
	rNFEBLK=0X00ffffff;
	//nf programable end block address   2M?
	
	rNF_Reset();
}

unsigned char BlockErase(unsigned int pageaddr)
{
	unsigned char EraseStatus;
		
	rNF_Reset();
	
	NF_CE_L();
	rNFSTAT |= (1<<4); // RnB Clear

	NF_CMD(NAND_CMD_ERASE1);
	NF_ADDR(pageaddr&0xFF);
	NF_ADDR((pageaddr>>8)&0xFF);
	NF_ADDR((pageaddr>>16)&0xFF);
	//NF_ADDR((blocknum>>16)&0xFF);
	NF_CMD(NAND_CMD_ERASE2);
	NF_DETECT_RB();
	rNFSTAT |= (1<<4); // RnB Clear
	
	NF_CMD(NAND_CMD_STATUS);
	EraseStatus = NF_DATA_R();
	
	NF_nFCE_H();

	if(EraseStatus & 0x01)	return FALSE;	
	
	return	TRUE;		
}
unsigned char rNF_WritePage(unsigned int page_num, unsigned char *buf)
{
	unsigned int i;
	unsigned char stat;
	
	
	//rNF_wInit();
	NF_CE_L();
	NF_CLEAR_RB();

	NF_CMD(NAND_CMD_SEQIN);
	NF_ADDR(0x00);
	NF_ADDR(0x00);
	NF_ADDR((page_num) & 0xFF);
	NF_ADDR((page_num>>8) & 0xFF);
	NF_ADDR((page_num>>16) & 0xFF);
 
	for (i=0; i<2048; i++)
	{
		NF_DATA_W(buf[i]);
		delay_us(5);
	}

	NF_CMD(NAND_CMD_PAGEPROG);
	delay_ms1(1);
//	Delay(1);
	NF_CMD(NAND_CMD_STATUS);

	do {
		stat = NF_RDDATA8();
	} while (!(stat & 0x40));
	NF_CE_H();

	NF_nFCE_H();
	return TRUE;
}

unsigned char rNF_RamdomRead(unsigned int page_num, unsigned int addr)
{
	unsigned char temp;
	NF_CE_L();
	NF_CLEAR_RB();
	NF_CMD(NAND_CMD_READ0);
	NF_ADDR(0x00);
	NF_ADDR(0x00);
	NF_ADDR((page_num) & 0xFF);
	NF_ADDR((page_num>>8) & 0xFF);
	NF_ADDR((page_num>>16) & 0xFF);

	NF_CMD(NAND_CMD_READSTART);
	NF_DETECT_RB();
	NF_CMD(NAND_CMD_RNDOUT);
	NF_ADDR((unsigned char) (addr & 0xFF));
	NF_ADDR((unsigned char) ((addr>>8) & 0x0F));
	NF_CMD(NAND_CMD_RNDOUTSTART);
	temp = NF_RDDATA8();
	NF_nFCE_H();
	return(temp);
}


void rLB_ReadPage(unsigned int addr, unsigned char * to)
{
	unsigned int  i;

	//rNF_Init();
	rNF_Reset();

	//  Enable the chip
	NF_nFCE_L();   
	NF_CLEAR_RB();

	// Issue Read command
	NF_CMD(CMD_READ);

	//  Set up address
	NF_ADDR(0x00);
	NF_ADDR(0x00);
	NF_ADDR((addr) & 0xff);
	NF_ADDR((addr >> 8) & 0xff);
	NF_ADDR((addr >> 16) & 0xff);

	NF_CMD(NAND_CMD_READSTART);

	NF_DETECT_RB();		// wait tR(max 12us)

	for (i = 0; i < 2048; i++)
	{
		to[i] =  NF_RDDATA8();
	}

	NF_nFCE_H();
}





//////////////////////////////////////////////////////////////////////
// BlockΪNand�����ţ�PageΪ��Ӧ���е�ҳ�ţ�AddressΪҳ�������ַ��
// LengthΪ��ȡ���ȣ�BufferΪ���ݻ�����
static unsigned char Nand_RamdomRead(unsigned int Block, unsigned int Page,
				unsigned int Address, unsigned short Length,
				unsigned char *Buffer)
{
	unsigned short i;
	if (Address + Length > 2048+64) {
		return 1; // ҳ�ڵ�ַ��д�볤�Ȳ��ܳ�����ҳ��Χ
	}
	if (Buffer == (void *)0 || Length == 0) {
		return 2; // ��������
	}	
		
	Page += (Block << 6); // Blockת��Ϊҳ��	
	NF_CE_ENABLE();
	NF_CLEAR_RB();

	NF_CMD(NAND_CMD_READ0); // page read cycle 1
	NF_ADDR(0); // column address
	NF_ADDR(0); // columu address
	NF_ADDR(Page & 0xff); // ����3�ֽڵ�ҳ��ַ
	NF_ADDR((Page>>8) & 0xff);
	NF_ADDR((Page>>16) & 0xff);	
	NF_CMD(NAND_CMD_READSTART); // page read cycle 2	
	
	NF_WAIT_READY(); // �ȴ�ҳ�����
	NF_CMD(NAND_CMD_RNDOUT); // ramdom read cycle 1
	NF_ADDR(Address & 0xff); // 2 cycle address in page
	NF_ADDR((Address>>8) & 0xf);
	NF_CMD(NAND_CMD_RNDOUTSTART); // ramdom read cycle 2
	
	for (i=0; i<Length; i++) {
		Buffer[i] = NF_READ_BYTE(); // ��ȡLength��������
	}	
	
	NF_CE_DISABLE();
	return 0;
}

// BlockΪNand�����ţ�PageΪ��Ӧ���е�ҳ�ţ�AddressΪҳ�������ַ��
// LengthΪд�볤�ȣ�BufferΪ���ݻ�����
static unsigned char Nand_RamdomWrite(unsigned int Block,unsigned int Page,
				unsigned int Address, unsigned short Length,
				unsigned char *Buffer)
{
	unsigned short i;
	unsigned char State;
	if (Address + Length > 2048+64) {
		return 1; // ҳ�ڵ�ַ��д�볤�Ȳ��ܳ�����ҳ��Χ
	}
	if (Buffer == (void *)0 || Length == 0) {
		return 2; // ��������
	}
	Page += (Block << 6); // Blockת��Ϊҳ��	
	NF_CE_ENABLE();
	NF_CLEAR_RB();

	NF_CMD(NAND_CMD_SEQIN); // ҳд����1
	NF_ADDR(0); // column address
	NF_ADDR(0); // columu address
	NF_ADDR(Page & 0xff); // 3�ֽ�ҳ��ַ
	NF_ADDR((Page>>8) & 0xff);
	NF_ADDR((Page>>16) & 0xff);	

	NF_CMD(NAND_CMD_RNDIN); // ҳ�����д����
	NF_ADDR(Address & 0xff); // 2�ֽ�ҳ�ڵ�ַ
	NF_ADDR((Address>>8) & 0xf);		
	for (i=0; i<Length; i++) {
		NF_WRITE_BYTE(Buffer[i]); // д��Length������
	}
	NF_CMD(NAND_CMD_PAGEPROG); // ҳд����2

	NF_WAIT_READY(); // �ȴ�д��	
	NF_CMD(NAND_CMD_STATUS); // ��ȡд���״̬
	do {
		State = NF_READ_BYTE();
	} while(!(State & (1<<6))); // �ȴ�״̬���Ready
	
	NF_CE_DISABLE();	
	if (State & (1<<0)) {
		return 3; // ramdom write error
	}
	return 0;
}

unsigned char Nand_IsBadBlock(unsigned int Block)
{
	unsigned char Data;
	// ÿ��block��һҳspare����6�ֽڷ�0xff���Ϊ�û�
	Nand_RamdomRead(Block, 0, 2054, 1, &Data);
	if (Data != 0xff) {
		return 1; // ����
	}
	return 0;
}

unsigned char Nand_MarkBadBlock(unsigned int Block)
{
	// ÿ��block��һҳspare����6�ֽڱ�Ƿ�0xff����
	unsigned char Data = 0x55;
	return Nand_RamdomWrite(Block, 0, 2054, 1, &Data);
}

// BlockΪNand�����ţ�PageΪ��Ӧ���е�ҳ�ţ�BufferΪ���ݻ�����
int Nand_ReadPage(unsigned int Block, unsigned int Page,
							unsigned char *Buffer)
{
	unsigned int i;
	unsigned int MECC, SECC;
	if (Buffer == (unsigned char *)0) {
		return 1; // ������Ϊ�գ���������
	}
	Page &= (64-1); // 64 page in one block
	Page += (Block << 6); // Blockת��Ϊҳ��
	NF_INIT_MECC(); // main��ECC���
	NF_INIT_SECC(); // spare��ECC���
	NF_MECC_UNLOCK(); // main��ECC��������ʼECC����
	NF_CE_ENABLE(); // ʹ��Ƭѡ
	NF_CLEAR_RB(); // �����ݴ����־
	
	NF_CMD(NAND_CMD_READ0); // page read cycle 1
	NF_ADDR(0); // column address
	NF_ADDR(0); // columu address
	NF_ADDR(Page & 0xff); // д��3�ֽڵ�ҳ��ַ
	NF_ADDR((Page>>8) & 0xff);
	NF_ADDR((Page>>16) & 0xff);	
	NF_CMD(NAND_CMD_READSTART); // page read cycle 2

	NF_WAIT_READY(); // �ȴ��������	
	for (i=0; i<2048; i++) { // ��ȡmain������
		Buffer[i] = NF_READ_BYTE();
	}
	NF_MECC_LOCK(); // ����main ECC
	NF_SECC_UNLOCK(); // ����spare ECC

	MECC = NF_READ_WORD(); // spare��ǰ4�ֽ�Ϊmain��ECC
	// main����ECC���뵽NFMECCD0/1����Ӧ��λ��
	rNFMECCD0=((MECC&0xff00)<<8) | (MECC&0xff);	
	rNFMECCD1=((MECC&0xff000000)>>8) | ((MECC&0xff0000)>>16);
	NF_SECC_LOCK(); // ����spare ECC
	// spare����5,6�����ֽ�Ϊspare��ECC,ʣ�²���δʹ��
	SECC = NF_READ_WORD();
	// spare����ECC���뵽NFMECCD0/1����Ӧ��λ��	
	rNFSECCD=((SECC&0xff00)<<8)|(SECC&0xff);	
	NF_CE_DISABLE();
	
	// check whether spare/main area bit fail error occurred
	if ((rNFECCERR0 & 0xf) == 0) {
		return 0; // ���ݶ�ȡ��ȷ
	} else {
		return 2; // ECC���鲻һ�£����ݶ�ȡ����
	}
}

// BlockΪNand�����ţ�PageΪ��Ӧ���е�ҳ�ţ�BufferΪ���ݻ�����
int Nand_WritePage(unsigned int Block, unsigned int Page, 
							unsigned char *Buffer)
{
	unsigned int i;
	unsigned char State;
	unsigned int MECC, SECC;
	if (Buffer == (unsigned char *)0) {
		return 1; // ���ݻ����������
	}
	if (Nand_IsBadBlock(Block)) {
		return 2; // �ǻ��飬���ػ��������
	}
	
	Page &= (64-1); // 1 block���64ҳ
	Page += (Block << 6); // blockת����ҳ
	NF_INIT_MECC(); // main��ECC���
	NF_INIT_SECC(); // spare��ECC���
	NF_MECC_UNLOCK(); // main��ECC��������ʼECC����
	NF_CE_ENABLE(); // ʹ��Ƭѡ
	NF_CLEAR_RB(); // �����ݴ����־
	
	NF_CMD(NAND_CMD_SEQIN); // page program cycle 1
	NF_ADDR(0); // column address
	NF_ADDR(0); // columu address
	NF_ADDR(Page & 0xff); // д��3�ֽ�ҳ��ַ
	NF_ADDR((Page>>8) & 0xff);
	NF_ADDR((Page>>16) & 0xff);	
	
	for (i=0; i<2048; i++) { // д��2k���ݵ�main��
		NF_WRITE_BYTE(Buffer[i]);		
	}
	NF_MECC_LOCK(); // ����main ECC
	MECC = rNFMECC0; // 4�ֽ�дmain�����ݵ�ECC	
	NF_SECC_UNLOCK(); // ����spare ECC
	NF_WRITE_BYTE(MECC&0xff);// д4�ֽ�main ECC��spare��
	NF_WRITE_BYTE((MECC>>8) & 0xff);	
	NF_WRITE_BYTE((MECC>>16) & 0xff);	
	NF_WRITE_BYTE((MECC>>24) & 0xff);
	NF_SECC_LOCK(); // ����spare ECC
	SECC = rNFSECC; // 2�ֽڵ�spareд����ECC
	NF_WRITE_BYTE(SECC & 0xff); // ����д��SECC
	NF_WRITE_BYTE((SECC>>8) & 0xff);
	NF_CMD(NAND_CMD_PAGEPROG); // page program cycle 2
	
	NF_WAIT_READY(); // �ȴ�д��
	NF_CMD(NAND_CMD_STATUS); // ��ȡnand״̬
	do {
		State = NF_READ_BYTE();
	} while(!(State & (1<<6))); // �ȴ�״̬���Ready
	
	NF_CE_DISABLE();
	
	// �Ƿ�д�ɹ�,��0λΪ0��pass,��Ȼfail
	if (State & (1<<0)) {
		if (Nand_MarkBadBlock(Block)) { // ��־����
			return 3; // д���ɹ����һ����ǲ��ɹ�
		} else {
			return 4; // д���ɹ������ǳɹ�
		}
	}
	return 0;
}
int Nand_ReadID(Nand_ID_Info *pInfo)
{
	if (pInfo == (Nand_ID_Info *)0) {
		return 1; // ��������
	}
	NF_CE_ENABLE();
	NF_CLEAR_RB();
	NF_CMD(NAND_CMD_READID); // ���Ͷ�ID����
	NF_ADDR(0x0); // д0x0��ַ
	
	pInfo->Maker = NF_READ_BYTE(); // Maker:0xEC
	pInfo->Device = NF_READ_BYTE(); // Device:0xDA
	pInfo->ID_Data3 = NF_READ_BYTE(); //0x10
	pInfo->ID_Data4 = NF_READ_BYTE(); //0x95
	pInfo->ID_Data5 = NF_READ_BYTE();  //0x44	
	
	NF_CE_DISABLE();
	return 0;
}

unsigned char Nand_EraseBlock(unsigned int Block)
{
	unsigned char State;
	NF_CE_ENABLE();
	NF_CLEAR_RB();

	NF_CMD(NAND_CMD_ERASE1); // erase block command cycle 1
	// write 3 cycle block address[A28:A18]
	NF_ADDR((Block<<6) & 0xff); // [A19:A18]
	NF_ADDR((Block>>2) & 0xff); // [A27:A20]
	NF_ADDR((Block>>10) & 0xff); // A28

	NF_CMD(NAND_CMD_ERASE2); // erase block command cycle 2

	NF_WAIT_READY();
	NF_CMD(NAND_CMD_STATUS);
	do {
		State = NF_READ_BYTE();
	} while(!(State & (1<<6))); // �ȴ�״̬���Ready
	
	NF_CE_DISABLE();
	// �Ƿ��д�ɹ�,��0λΪ0��pass,��Ȼfail
	if (State & (1<<0)) {
		if (Nand_MarkBadBlock(Block)) {
			return 3; // �������ɹ����һ����ǲ��ɹ�
		} else {
			return 4; // �������ɹ������ǳɹ�
		}
	}	
	return 0; // �ɹ�����
}



// AddressΪNand�ֽ�ƫ�Ƶ�ַ��BufferΪ���ݻ�����
// LengthΪ��ȡ����
unsigned char Nand_ReadSkipBad(unsigned int Address,
			unsigned char *Buffer, unsigned int Length)
{
	unsigned int BlockIndex, PageIndex;
	unsigned int ReadBytes;
	unsigned char i;
	unsigned char State;
	
	if (Length == 0 || Buffer == (void *)0) {
		return 1; // ��������
	}
	if ((Address & 0x7ff) != 0) {
		return 2; // nand��ַ��ҳ����,����С��λΪ1ҳ
	}

	State = 0;
	ReadBytes = 0; // �Ѷ��ֽ���
	PageIndex = (Address >> 11) & 0x3f; // ���е�ҳƫ��λ��
	BlockIndex = Address >> 17; // ���λ��
	while (1) {
		if (Nand_IsBadBlock(BlockIndex)) {
			BlockIndex++; // ����,������ȡ��һ��
			continue;
		}
		for (i=PageIndex; i<64; i++) { // ��ȡһ��block
			if (Nand_ReadPage(BlockIndex, i, Buffer)) {
				if (State == 1) {
					State=2; // read error twice
					// �ٴζ�����,��Ϊ�ǻ���,ȡ������������
					ReadBytes -= 2048 * (i-PageIndex);
					// �����ڴ�λ�õ���ȡ���֮ǰ
					Buffer -= 2048 * (i-PageIndex);				
					break;
				} else {
					State = 1;
					i -= 1; // �����������ٴζ���ҳ
					continue;
				}
			}
			ReadBytes += 2048; // ��ȡ��һҳ����
			if (ReadBytes >= Length) {
				break; // ��ȡ�����㹻���˳�
			}
			State = 0;
			Buffer += 2048; // ��һҳ�ڴ�洢λ��	
		}
		if (State == 2) { // ���ζ�ȡ������,��������һ��
			BlockIndex++;
			State = 0;
			continue;
		}
		if (ReadBytes >= Length) {
			break; // �����˳�ѭ��
		}
		PageIndex = 0; // ��һ��ӵ�һҳ��ʼ��
		BlockIndex++; // ����һ����	
	}
	return 0;
}

// AddressΪNand�ֽ�ƫ�Ƶ�ַ��BufferΪ���ݻ�����
// LengthΪд����
unsigned char Nand_WriteSkipBad(unsigned int Address,
			unsigned char *Buffer, unsigned int Length)
{
	unsigned int BlockIndex, PageIndex;
	unsigned int WriteBytes;
	unsigned char i;
	unsigned char State;

	if (Length == 0 || Buffer == (void *)0) {
		return 1; // ��������
	}
	if ((Address & 0x7ff) != 0) {
		return 2; // nand��ַ��ҳ����,����С��λΪ1ҳ
	}
	
	State = 0;
	WriteBytes = 0; // ��д�ֽ���
	PageIndex = (Address >> 11) & 0x3f; // ���е�ҳƫ��λ��
	BlockIndex = Address >> 17; // ��дλ��	
	while (1) 
	{
		if (Nand_EraseBlock(BlockIndex)) 
		{
			BlockIndex++; // ����,��������һ��
			continue;
		}
		for (i=PageIndex; i<64; i++) { 
			if (Nand_WritePage(BlockIndex, i, Buffer)) {// дһҳ
				if (State == 1) {
					State=2; // write error twice
					// �ٴ�д����,��Ϊ�ǻ���,ȡ�����д������
					WriteBytes -= 2048 * (i-PageIndex);
					// �����ڴ�λ�õ�д���֮ǰ	
					Buffer -= 2048 * (i-PageIndex);			
					break;
				} else {
					State = 1;
					i -= 1; // д���������ٴ�д��ҳ
					continue;
				}
			}
			WriteBytes += 2048; // дȡ��һҳ����
			if (WriteBytes >= Length) {
				break; // ����д�����˳�
			}
			State = 0;
			Buffer += 2048; // ��һҳ�ڴ�洢λ��	
		}
		if (State == 2) { // ����д������,��������һ��
			BlockIndex++;
			State = 0;
			continue;
		}
		if (WriteBytes >= Length) {
			break; // д���˳�ѭ��
		}
		PageIndex = 0; // ��һ��ӵ�һҳ��ʼд
		BlockIndex++; // ��д��һ����	
	}	
	return 0;
}

static void Nand_Reset()
{
	NF_CE_ENABLE();	
	NF_CLEAR_RB();	
	NF_CMD(NAND_CMD_RESET);
	NF_WAIT_READY();	
	NF_CE_DISABLE();	
}

void Nand_Init()
{
	// ����nand��������
	rGPACON = (rGPACON &~(0x3f<<17)) | (0x3f<<17);  
	// ����K9F2G08U0B timing(HCLK@133M)
	// TACLS=1, (tALS��tCLS-tWP=0)(ALE��CLE��Ч���豣�ֲ��ܷ���д����)
	// TWRPH0=2,tWP=12ns(��Сд������)
	// TWRPH1=1,tALH��tCLH=5ns(д�����ALE��CLE�豣����Чʱ��)
	// SLC nand 1λ�������,1-bit ECC
	rNFCONF = (1<<12)|(2<<8)|(1<<4)|(0<<0);

	// �����ؼ��nand׼�����ź���
	rNFCONT = (0<<12)|(0<<10)|(0<<9)|(0<<8)
				|(0x3<<6)|(0x3<<4)|(0x3<<1)|(1<<0);
	
	Nand_Reset();
}

// �����д����RAMλ��һ�����ȵĴ�����д��Nand 0x0ƫ�ƴ�
unsigned char WriteCodeToNand()
{
// �ڰ弶����LowLevelInit.s�����������������ɵĴ����С���������е�ַ��Ϣ
// __CodeAddr__Ϊ���뵽��������RAMλ�ã������ļ����趨
// __CodeSize__Ϊ�����ƴ���������ɵĴ�С���������������Ӻ����
	extern unsigned int __CodeAddr__;
	extern unsigned int __CodeSize__;	
	unsigned char State;
	Nand_Init(); // Nandʱ���ʼ��
	State = Nand_WriteSkipBad(0, (unsigned char *)__CodeAddr__, __CodeSize__);
	return State;
}

// �����д����RAMλ��һ�����ȵĴ�����д��Nand 0x0ƫ�ƴ�
unsigned char WriteAppToNand(unsigned int CodeAddr, unsigned int CodeSize)
{
	//Nandǰ��������bootload ��256K�������鿪ʼ��Ӧ�ó���
	unsigned char State;
	Nand_Init(); // Nandʱ���ʼ��
	State = Nand_WriteSkipBad(0, (unsigned char *)CodeAddr, CodeSize);
	return State;
}

// �����д����RAMλ��һ�����ȵĴ�����д��Nand 0x0ƫ�ƴ�
unsigned char WriteAppToAppSpace(unsigned int CodeAddr, unsigned int CodeSize)
{
	//Nandǰ��������bootload ��256K�������鿪ʼ��Ӧ�ó���
	unsigned char State;
	Nand_Init(); // Nandʱ���ʼ��
	State = Nand_WriteSkipBad(APP_NAND_ADDR, (unsigned char *)CodeAddr, CodeSize);
	return State;
}



