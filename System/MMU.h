#ifndef __MMU_H__
#define __MMU_H__

#ifdef __cplusplus
extern "C" {
#endif

// ICacheʹ�ܿ���λ
#define R1_I		(1<<12)
// DCacheʹ�ܿ���λ
#define R1_C		(1<<2)
// ��ַ������ʹ�ܿ���λ
#define R1_A		(1<<1)
// MMUʹ�ܿ���λ
#define R1_M   		(1<<0)

// �α�ʶ�����Զ�(1MB)��ʽ����ӳ��
#define DESC_SEC	((1<<1)|(1<<4))
// cache_on, write_back cache��������е����	
#define CB			(3<<2)
// cache_on, write_through 	
#define CNB			(2<<2)
// cache_off,WR_BUF on cache����������е����	
#define NCB         	(1<<2)  
// cache_off,WR_BUF off
#define NCNB		(0<<2)

// supervisor=RW, user=RW
#define AP_RW		(3<<10)
// supervisor=RW, user=RO
#define AP_RO		(2<<10) 
// supervisor=RW, user=No access
#define AP_NO		(1<<10) 

// �κη��ʶ�������"Domain fault"
#define DOMAIN_FAULT	(0x0)
// ʹ���������е����ý���Ȩ�޼��
#define DOMAIN_CHK	(0x1) 
// ������Ȩ�޼�飬�����κη���
#define DOMAIN_NOTCHK	(0x3)
// ����0���Ȩ��
#define DOMAIN0		(0x0<<5)
// ����1���Ȩ��
#define DOMAIN1		(0x1<<5)

// ��0����Ϊ��������Ȩ�޽���Ȩ�޼��
#define DOMAIN0_ATTR	(DOMAIN_CHK<<0) 
// ��1����Ϊ���ɷ���
#define DOMAIN1_ATTR	(DOMAIN_FAULT<<2) 

// ����������0�ɶ�д����cache д����
#define RW_CB		(AP_RW|DOMAIN0|CB|DESC_SEC)
#define RW_CNB		(AP_RW|DOMAIN0|CNB|DESC_SEC)
#define RW_NCB		(AP_RW|DOMAIN0|NCB|DESC_SEC)
#define RW_NCNB		(AP_RW|DOMAIN0|NCNB|DESC_SEC)
// ����������1���ɷ���
#define RW_FAULT	(AP_RW|DOMAIN1|NCNB|DESC_SEC)

void MMU_EnableICache(void); // ʹ��ICache
void MMU_DisableICache(void); // ��ֹICache
void MMU_EnableDCache(void); // ʹ��DCache
void MMU_DisableDCache(void); // ��ֹDCache
void MMU_EnableAlignFault(void); // ʹ�ܶ�����
void MMU_EnableMMU(void); // ʹ��MMU
void MMU_DisableMMU(void); // ��ֹMMU
void MMU_SetTTBase(int Base); // Set TTBase
void MMU_SetDomain(int Domain); // Set Domain
void MMU_InvalidateICache(void); // ��ЧICache
void MMU_InvalidateDCache(void); // ��ЧDCache
void MMU_InvalidateTLB(void); // ��ЧTLB
void MMU_SetProcessId(unsigned int pid); // Process ID

void MMU_Init(void); // MMUҳ���ʼ��
unsigned int *MMU_GetTableBase(void); // ��ȡMMUҳ���ڴ��ַ
void MMU_SetMTT(unsigned int vaddrStart,unsigned int vaddrEnd,
				unsigned int paddrStart,unsigned int attr);


#ifdef __cplusplus
}
#endif

#endif /*__MMU_H__*/
