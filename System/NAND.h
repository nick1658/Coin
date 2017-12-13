#ifndef __NAND_H__
#define __NAND_H__

#ifdef __cplusplus
extern "C" {
#endif

// Nand flash 5�ֽ�ID
typedef struct Nand_ID_Info {
	unsigned char Maker;
	unsigned char Device;
	unsigned char ID_Data3;
	unsigned char ID_Data4;
	unsigned char ID_Data5;
} Nand_ID_Info;





/*
 * Standard NAND flash commands
 */
// ҳ������1(����1)
#define NAND_CMD_READ0		0x00
// ҳ������1(����2)
#define NAND_CMD_READ1		0x01
// �����ַ������1
#define NAND_CMD_RNDOUT		0x05
// ҳд����2
#define NAND_CMD_PAGEPROG	0x10
// OOB��������
#define NAND_CMD_READOOB	0x50
// �������������1
#define NAND_CMD_ERASE1		0x60
// ��Nand״̬����
#define NAND_CMD_STATUS		0x70
// �����״̬����
#define NAND_CMD_STATUS_MULTI	0x71
// ҳд����1
#define NAND_CMD_SEQIN		0x80
// �����ַд����
#define NAND_CMD_RNDIN		0x85
// ��ID����
#define NAND_CMD_READID		0x90
// �������������2
#define NAND_CMD_ERASE2		0xd0
// Nand��λ
#define NAND_CMD_RESET		0xff

/* Extended commands for large page devices */
// ҳ������2
#define NAND_CMD_READSTART	0x30
// �����ַ������2
#define NAND_CMD_RNDOUTSTART	0xE0
#define NAND_CMD_CACHEDPROG	0x15

// ��������
#define NF_CMD(Data) 		{rNFCMD  = (Data);}
// д��ַ
#define NF_ADDR(Addr)       {rNFADDR = (Addr);}
// ����(4�ֽ�)
#define NF_READ_WORD()      (rNFDATA)
// ��һ�ֽ�
#define NF_READ_BYTE()      (rNFDATA8)
// д��(4�ֽ�)
#define NF_WRITE_WORD(Data) {rNFDATA = (Data);}
// дһ�ֽ�
#define NF_WRITE_BYTE(Data) {rNFDATA8 = (Data);} 

// ʹ��Ƭѡ
#define NF_CE_ENABLE() 		{rNFCONT &= ~(1<<1);}
// �ر�Ƭѡ
#define NF_CE_DISABLE() 	{rNFCONT |= (1<<1);}
// ���spare��ECCУ��ֵ
#define NF_INIT_SECC() 		{rNFCONT |= (1<<4);}
// ���main��ECCУ��ֵ
#define NF_INIT_MECC() 		{rNFCONT |= (1<<5);}
// ����spare��ECCУ��ֵ
#define NF_SECC_UNLOCK() 	{rNFCONT &= ~(1<<6);}
// ����spare��ECCУ��ֵ
#define NF_SECC_LOCK() 		{rNFCONT |= (1<<6);}
// ����main��ECCУ��ֵ
#define NF_MECC_UNLOCK() 	{rNFCONT &= ~(1<<7);}
// ����main��ECCУ��ֵ
#define NF_MECC_LOCK() 		{rNFCONT |= (1<<7);}

// nand���������λNFSTAT[4],�������ж�,��ͬʱ�����ж�����
#define NF_WAIT_READY()     {while(!(rNFSTAT & (1<<4)));}
// ���nand RnB����״̬,1Ϊ׼����,0Ϊæ
#define NF_GET_STATE_RB()   {(rNFSTAT & (1<<0))}
// ���nand�������־
#define NF_CLEAR_RB()       {rNFSTAT |= (1<<4);}

/* ģ��ӿں��� */
extern unsigned char Nand_ReadSkipBad(unsigned int Address, 
				unsigned char *Buffer, unsigned int Length);
extern unsigned char Nand_WriteSkipBad(unsigned int Address,
				unsigned char *Buffer, unsigned int Length);
extern void Nand_Init(void);
extern int Nand_ReadID(Nand_ID_Info *pInfo);
int Nand_ReadPage(unsigned int Block, unsigned int Page,
							unsigned char *Buffer);
int Nand_WritePage(unsigned int Block, unsigned int Page, 
							unsigned char *Buffer);
extern unsigned char Nand_EraseBlock(unsigned int Block);
extern unsigned char Nand_MarkBadBlock(unsigned int Block);
extern unsigned char Nand_IsBadBlock(unsigned int Block);
extern unsigned char WriteCodeToNand(void);
unsigned char WriteAppToNand(unsigned int CodeAddr, unsigned int CodeSize);
unsigned char WriteAppToAppSpace(unsigned int CodeAddr, unsigned int CodeSize);
////////////////////////////////////////////////////////////////

#define NF_NFCE0		(0x1<<1)
#define CMD_READ			0x00	//  Read


#define NF_CE_L()			{rNFCONT &= ~NF_NFCE0;}
#define NF_CE_H()			{rNFCONT |= NF_NFCE0;}
#define NF_DETECT_RB()			{while(!(rNFSTAT&(1<<4)));}
#define NF_nFCE_H()			{rNFCONT |= (1<<1); }
#define NF_nFCE_L()			{rNFCONT &= ~(1<<1); }

#define NF_DATA_R()			(rNFDATA8)
#define NF_DATA_W(n)			(rNFDATA8 = (unsigned char)n)
#define NF_RDDATA8()			(rNFDATA8)



void rNF_Init(void);
unsigned char BlockErase(unsigned int pageaddr);
unsigned char rNF_WritePage(unsigned int page_num, unsigned char *buf);

void rLB_ReadPage(unsigned int addr,unsigned char * to);
unsigned char rNF_RamdomRead(unsigned int page_num, unsigned int addr);

#ifdef __cplusplus
}
#endif

#endif /*__NAND_H__*/
