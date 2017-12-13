/*
 * TQ2416 UART
 */

#ifndef __TQ2416_UART_H__
#define __TQ2416_UART_H__


#include "def.h"


#define PCLK	(800000000/3/2/2)

void uart_init(void);
void Uart0_Printf(const char *fmt,...);  //����
void Uart0_sendchar(U8 data);
void Uart0_SendString(const char *pt);
void Uart0_SendByte(int data);

void uart1_init(void);
void Uart1_Printf(const char *fmt,...);   //����

void uart2_init(void);   //���� 
void Uart2_sendchar(U8 data);    //����




//unsigned char uart_getc(void);
//void uart_putc(unsigned char c);
//void uart_puts(unsigned char *s);

//void uart_send32(unsigned int d, unsigned char cr);
//void uart_send16(unsigned char d, unsigned char cr);
//void uart_send8(unsigned char d, unsigned char cr);


//void uart_init_irq(void);
//void __irq HandleUART0(void);

extern char uart0_cmd[16];  // ����2���� �ֽ� ����

extern void uart0_cmd_result (void);

#endif
