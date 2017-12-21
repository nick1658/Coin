#include "s3c2416.h"
#include "Exception.h"

// ����32������ָ�뱣���IRQ�ж���ں���
static void (*IRQ_Table[32])(void);


void Undef_Handler(void)
{
//	LOG ("Undef_Handler");
	while(1) {

	}
}

unsigned int SWI_Handler(unsigned int p)
{
//	LOG ("SWI_Handler");
//	while(1) {

//	}
	return p+1;
}

void PAbt_Handler(void)
{
//	LOG ("PAbt_Handler");
	while(1) {

	}
}

void DAbt_Handler(void)
{
//	LOG ("DAbt_Handler");
	while(1) {
	
	}
}

void FIQ_Handler(void)
{
//	LOG ("FIQ_Handler");
	while(1) {
		
	}
}

void IRQ_Register(unsigned char Channel, void (*Func)(void))
{
	if (Channel < 32) { // 32�������ж�
		IRQ_Table[Channel] = Func; // ע����Ӧ�жϴ�����
	}
}


void IRQ_ClearInt(unsigned char Channel)
{
	rSRCPND1 |= (1 << Channel);
	rINTPND1 |= (1 << Channel);
}


void IRQ_EnableInt(uint8_t Channel)
{
	if (Channel < 32) {
		rSRCPND1 |= (0x01 << Channel);	// write 1 to clear
		rINTPND1 |= (0x01 << Channel);	// write 1 to clear	
		rINTMOD1 &= ~(1 << Channel); // IRQ ģʽ
		rINTMSK1 &= ~(1 << Channel); // ���ж�		
	}
}

void IRQ_DisableInt(uint8_t Channel)
{
	if (Channel < 32) {
		rINTMSK1 |= (1 << Channel); // ���ж�		
	}	
}

void IRQ_Handler(void)
{
	unsigned int Int_Offset;
	if (rINTPND1 != 0){// ��һ��������жϴ���			
		Int_Offset = rINTOFFSET1;
		if (IRQ_Table[Int_Offset]){ // �жϴ����輺ע��
			(IRQ_Table[Int_Offset])(); // ������Ӧ���жϴ���
		}
		//rSRCPND1 |= (0x01<<Int_Offset);	// write 1 to clear
		//rINTPND1 |= (0x01<<Int_Offset);	// write 1 to clear
	}else if (rINTPND2 != 0) {// �ڶ���������жϴ���		
		Int_Offset = rINTOFFSET2;
		//rSRCPND2 |= (0x01<<Int_Offset);	// write 1 to clear
		//rINTPND2 |= (0x01<<Int_Offset);	// write 1 to clear
		switch (Int_Offset) 
		{
			case 0:
			// INT_2D �жϴ���	
				break;
			case 4:
			// INT_PCM0 �жϴ���	
				break;
			case 6:
			// INT_I2S0 �жϴ���	
				break;
			default:
				break;
		}
	}
}



