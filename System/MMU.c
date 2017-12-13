
#include "MMU.h"

// MMUҳ���ڱ�������ʼ������֮ǰ����,����һ��16k����16k��СMMU�ڴ��,
// �����ñ������������ȫ���ڴ�飬һ�������ñ�������ʼ���ⲿ���ڴ�
// ��__attribute__����Ϊδ��ʼ���Σ���Ҫ�ñ�������ʼ��
__align(0x4000) static unsigned char MMU_PageTable[0x4000] 
	__attribute__((section("MMU_Mem"), zero_init));

unsigned int *MMU_GetTableBase(void)
{
	return (unsigned int *)MMU_PageTable;
}

void MMU_Init(void)
{
	// �����û��������л�ַ��0x0����������ӳ�䵽�û�����������λ�ô�
	extern unsigned int __CodeAddr__;	
	MMU_DisableDCache(); // ����DCache
	MMU_DisableICache(); // ����ICache
	MMU_InvalidateDCache(); // ʹ16K DCache��Ч
	MMU_InvalidateICache(); // ʹ16K ICache��Ч
	MMU_EnableICache(); // �ӿ�ִ��MMU_Init
	MMU_DisableMMU(); // ����MMU
	MMU_InvalidateTLB(); // ʹ��Чת����
	
	extern unsigned int Image$$RW_NO_CACHE$$Base;
	extern unsigned int Image$$RW_MMU$$Base;
	
	unsigned int No_Cache_Start = (unsigned int)&Image$$RW_NO_CACHE$$Base;
	unsigned int No_Cache_End = (unsigned int)&Image$$RW_MMU$$Base;
	
	//MMU_SetMTT(int vaddrStart,int vaddrEnd,int paddrStart,int attr)
	// 0���쳣������ӳ�䵽__CodeAddr__�����������ʱ�������û�������
	// 0��ַ�ж�����������ӳ�䵽��������RAM��ַ������Ĭ��1:1ӳ��
	// �Ĵ����ռ䲻�ܿ���cache��д���棬�Ĵ���ӦΪvolatile����
	// steppingstone(Nand Boot)
    MMU_SetMTT(0x00000000, 0x00000000, __CodeAddr__, RW_CB);
	//SROM Bank0 Reserve
    MMU_SetMTT(0x00100000, 0x07f00000, 0x00100000, RW_NCNB);
	//SROM Bank1 Reserve
    MMU_SetMTT(0x08000000, 0x0ff00000, 0x08000000, RW_NCNB);
	//SROM Bank2 Reserve
    MMU_SetMTT(0x10000000, 0x17f00000, 0x10000000, RW_NCNB);
	//SROM Bank3 Reserve
    MMU_SetMTT(0x18000000, 0x1ff00000, 0x18000000, RW_NCNB);
	//SROM Bank4 Reserve
    MMU_SetMTT(0x20000000, 0x27f00000, 0x20000000, RW_NCNB);
	//SROM Bank5 Reserve
    MMU_SetMTT(0x28000000, 0x2ff00000, 0x28000000, RW_NCNB);
	//Bank6 DDR2 64M
    MMU_SetMTT(0x30000000, 0x33f00000, 0x30000000, RW_CB);  
	//SDRAM Bank6-2 Reserve
    MMU_SetMTT(0x34000000, 0x37f00000, 0x34000000, RW_NCNB);
	//SDRAM Bank7 Reserve
    MMU_SetMTT(0x38000000, 0x3ff00000, 0x38000000, RW_NCNB);
	// steppingstone(IROM boot)
    MMU_SetMTT(0x40000000, 0x40000000, 0x40000000, RW_NCNB);
	// ���⹦�ܼĴ���
    MMU_SetMTT(0x40100000, 0x47f00000, 0x40100000, RW_NCNB);//SFR
    MMU_SetMTT(0x48000000, 0x5af00000, 0x48000000, RW_NCNB);//SFR
    MMU_SetMTT(0x5b000000, 0x5c000000, 0x5b000000, RW_NCNB);//SFR
    MMU_SetMTT(0x5c100000, 0xfff00000, 0x5c100000, RW_FAULT);//not used  
	// no cache�ڴ������Ϊ��cache,��д����
	if (No_Cache_End > No_Cache_Start) {
		MMU_SetMTT(No_Cache_Start, No_Cache_End-1, No_Cache_Start, RW_NCB);
	}
    MMU_SetTTBase((int)MMU_PageTable); // ����MMU��ҳ���ַ��16K����
    //DOMAIN1: no_access, DOMAIN0,2~15=client(AP is checked)	
    MMU_SetDomain(0x55555550|DOMAIN1_ATTR|DOMAIN0_ATTR);//�����Ȩ��
    MMU_SetProcessId(0x0);
    MMU_EnableAlignFault(); // ʹ�ܶ�����

    MMU_EnableMMU(); // ����MMU���õ��������ַ
    MMU_EnableICache(); // ����ICache
    MMU_EnableDCache(); // MMU��������DCache
}

/* ʹ�ö�ӳ�� */
void MMU_SetMTT(unsigned int vaddrStart,unsigned int vaddrEnd,
					unsigned int paddrStart,unsigned int attr)
{
    unsigned int *pTT;
    unsigned int i, nSec;
    
    pTT = (unsigned int *)MMU_PageTable + (vaddrStart>>20);
    nSec = (vaddrEnd>>20) - (vaddrStart>>20);
    
    for(i=0; i<=nSec; i++) {
		*pTT++ = attr | (((paddrStart>>20)+i)<<20);
	}
}

__asm void MMU_DisableMMU(void)
{
	mrc p15,0,r0,c1,c0,0
	bic r0,r0,#R1_M
	mcr p15,0,r0,c1,c0,0
	bx lr
}

__asm void MMU_EnableMMU(void)
{
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#R1_M
	mcr p15,0,r0,c1,c0,0
	bx lr
}
__asm void MMU_DisableICache(void)
{
	mrc p15,0,r0,c1,c0,0
	bic r0,r0,#R1_I
	mcr p15,0,r0,c1,c0,0
	bx lr
}

__asm void MMU_EnableICache(void)
{
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#R1_I
	mcr p15,0,r0,c1,c0,0
	bx lr
}

__asm void MMU_DisableDCache(void)
{
	mrc p15,0,r0,c1,c0,0
	bic r0,r0,#R1_C
	mcr p15,0,r0,c1,c0,0
	bx lr
}

__asm void MMU_EnableDCache(void)
{
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#R1_C
	mcr p15,0,r0,c1,c0,0
	bx lr
}

__asm void MMU_InvalidateDCache(void)
{
	mcr p15,0,r0,c7,c6,0
	bx lr
}

__asm void MMU_InvalidateICache(void)
{
	mcr p15,0,r0,c7,c5,0
	bx lr
}

__asm void MMU_InvalidateTLB(void)
{
	mcr p15,0,r0,c8,c7,0
	bx lr
}

__asm void MMU_SetTTBase(int Base)
{
	mcr p15,0,r0,c2,c0,0
	bx lr
}

__asm void MMU_SetDomain(int Domain)
{
	mcr p15,0,r0,c3,c0,0
	bx lr
}

__asm void MMU_SetProcessId(unsigned int Pid)
{
	mcr p15,0,r0,c13,c0,0
	bx lr
}

__asm void MMU_EnableAlignFault(void)
{
	mrc p15,0,r0,c1,c0,0
	orr r0,r0,#R1_A
	mcr p15,0,r0,c1,c0,0
	bx lr
}

