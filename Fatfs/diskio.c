/*-----------------------------------------------------------------------*/
/* Low level disk I/O module skeleton for FatFs     (C)ChaN, 2013        */
/*-----------------------------------------------------------------------*/
/* If a working storage control module is available, it should be        */
/* attached to the FatFs via a glue function rather than modifying it.   */
/* This is an example of glue functions to attach various exsisting      */
/* storage control module to the FatFs module with a defined API.        */
/*-----------------------------------------------------------------------*/

#include "diskio.h"		/* FatFs lower layer API */
#include "Hsmmc.h"
#include "RTC.h"
#include "stdlib.h"

static DSTATUS State = STA_NOINIT;
/*-----------------------------------------------------------------------*/
/* Inidialize a Drive                                                    */
/*-----------------------------------------------------------------------*/

DSTATUS disk_initialize (
	BYTE pdrv				/* Physical drive nmuber (0..) */
)
{
	if (pdrv != 0) {
		return STA_NOINIT; // ��֧��driver 0
	}
	
	if (!Hsmmc_Init()) {
		State &= ~STA_NOINIT; // ��ʼ���ɹ�
	} else {
		State |= STA_NOINIT;
	}
	return State;
}



/*-----------------------------------------------------------------------*/
/* Get Disk Status                                                       */
/*-----------------------------------------------------------------------*/

DSTATUS disk_status (
	BYTE pdrv		/* Physical drive nmuber (0..) */
)
{
	if (pdrv != 0) {
		return STA_NOINIT; // ��֧��driver 0
	}
	return State;
}



/*-----------------------------------------------------------------------*/
/* Read Sector(s)                                                        */
/*-----------------------------------------------------------------------*/

DRESULT disk_read (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE *buff,		/* Data buffer to store read data */
	DWORD sector,	/* Sector address (LBA) */
	UINT count		/* Number of sectors to read (1..128) */
)
{
	if (pdrv || !count) {
		return RES_PARERR;
	}
	if (State & STA_NOINIT) {
		return RES_NOTRDY;
	}
	if (!Hsmmc_ReadBlock(buff, sector, count)) {
		return RES_OK;
	} else {
		return RES_ERROR;
	}
}



/*-----------------------------------------------------------------------*/
/* Write Sector(s)                                                       */
/*-----------------------------------------------------------------------*/

#if _USE_WRITE
DRESULT disk_write (
	BYTE pdrv,			/* Physical drive nmuber (0..) */
	const BYTE *buff,	/* Data to be written */
	DWORD sector,		/* Sector address (LBA) */
	UINT count			/* Number of sectors to write (1..128) */
)
{
	if (pdrv || !count) {
		return RES_PARERR;
	}
	if (State & STA_NOINIT) {
		return RES_NOTRDY;
	}
	if (State & STA_PROTECT) {
		return RES_WRPRT;
	}
	if (!Hsmmc_WriteBlock((unsigned char *)buff, sector, count)) {
		return RES_OK;
	} else {
		return RES_ERROR;
	}
}
#endif


/*-----------------------------------------------------------------------*/
/* Miscellaneous Functions                                               */
/*-----------------------------------------------------------------------*/

#if _USE_IOCTL
DRESULT disk_ioctl (
	BYTE pdrv,		/* Physical drive nmuber (0..) */
	BYTE cmd,		/* Control code */
	void *buff		/* Buffer to send/receive control data */
)
{
	unsigned char CSD[16];
	unsigned char SdState[64];
	unsigned int c_size, c_size_multi, read_bl_len, sector_size, au_size;	
	DRESULT Result = RES_ERROR;
	
	if (pdrv) {
		return RES_PARERR;
	}
	if (State & STA_NOINIT) {
		return RES_NOTRDY;
	}
	switch (cmd) {
	case CTRL_SYNC:
		Result = RES_OK; // дsd����������ȷ����д��ŷ���,��дͬ����
		break;
	case GET_SECTOR_COUNT: /* Get drive capacity in unit of sector (DWORD) */
		if (!Hsmmc_Get_CSD(CSD)) {
			if ((CSD[15]>>6) == 1) { // CSD v2.00->SDHC��
				c_size = ((CSD[8]&0x3f) << 16) + (CSD[7]<<8) + CSD[6]; // [69:48]
				// ������Ϊ�ֽ�(c_size+1)*512K byte,��1����512 byte��,����������Ϊ		
				*(DWORD *)buff = (c_size+1) << 10;
			} else { // CSD v1.0->sd V1.x, sd v2.00 standard
				read_bl_len = CSD[10] & 0xf; // [83:80]
				c_size_multi = ((CSD[6] & 0x3) << 1) + ((CSD[5] & 0x80) >> 7); // [49:47]
				c_size = ((WORD)(CSD[9]&0x3) << 10) + ((WORD)CSD[8]<<2) + (CSD[7]>>6); // [73:62]
				// ������Ϊ�ֽ�(c_size+1)*2^(c_size_multi+2)*2^(read_bl_len)
				// ��1���� 512 byte��,��������Ϊ
				*(DWORD *)buff = (c_size + 1) << (read_bl_len + (c_size_multi + 2) - 9);
			}
			Result = RES_OK;			
		}

		break;
	
	case GET_BLOCK_SIZE: /* Get erase block size in unit of sector (DWORD) */
		if (!Hsmmc_Get_CSD(CSD)) {
			if ((CSD[15]>>6) == 1) { // CSD v2.00, SDHC��
				if (!Hsmmc_GetSdState(SdState)) { // v2.00������С����Ϣ��CSD�в�����Ч,���sd state��ȡ��Щ��Ϣ
					au_size = (SdState[10] >> 4); // Allocation Unit(AU)[431:428],SdState[0]�������״̬�ĸ�λ
					*(DWORD *)buff = 16UL << au_size; // 1au��λΪ16k,ת��Ϊ512�ֽ�������	
					Result = RES_OK;					
				}			
			} else { // CSD v1.0,sd version 1.x, sd version 2.00 standard
				sector_size = ((CSD[5] & 0x3f) << 1) + (CSD[4] >> 7); // [45:39]
				*(DWORD *)buff = (sector_size + 1) << ((CSD[2] >> 6) - 1); // �������СΪ������С*�鳤��(512Ϊ��λ)
				Result = RES_OK;				
			}	
		}
		break;	
		
	case CTRL_ERASE_SECTOR: /* Erase a block of sectors (used when _USE_ERASE == 1) */
		if (!Hsmmc_EraseBlock(((DWORD *)buff)[0], ((DWORD *)buff)[1])) {
			Result = RES_OK;
		}
		break;	
		
	default:
		break;
	}
	return Result;
}
#endif


#if _USE_WRITE
DWORD get_fattime()
{
	RTC_Time Time;
	RTC_GetTime(&Time);			
	return (((Time.Year - 1980) << 25) | (Time.Month << 21) |
			(Time.Day << 16) | (Time.Hour << 11) |
			(Time.Min << 5) | (Time.Sec << 1));
}
#endif

//��̬�����ڴ�
void *ff_memalloc (UINT size)			
{
	return (void*)malloc(size);
}
//�ͷ��ڴ�
void ff_memfree (void* mf)		 
{
	free(mf);
}


