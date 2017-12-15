#ifndef __OS_CPU_H__
#define __OS_CPU_H__

#ifdef  OS_CPU_GLOBALS
#define OS_CPU_EXT
#else
#define OS_CPU_EXT  extern
#endif

/*
***********************************************************
*  uCOS-IIʹ�õ����������Ͷ���
***********************************************************
*/
typedef unsigned char  BOOLEAN; /* ��������               */
typedef unsigned char  INT8U;   /* �޷���8λ���ͱ���      */
typedef signed   char  INT8S;   /* �з���8λ���ͱ���      */
typedef unsigned short INT16U;  /* �޷���16λ���ͱ���     */
typedef signed   short INT16S;  /* �з���16λ���ͱ���     */
typedef unsigned int   INT32U;  /* �޷���32λ���ͱ���     */
typedef signed   int   INT32S;  /* �з���32λ���ͱ���     */
typedef float          FP32;    /* �����ȸ�����(32λ����) */
typedef double         FP64;    /* ˫���ȸ�����(64λ����) */

/*
************************************************************
*  ջ����
************************************************************
*/
typedef INT32U         OS_STK;   /* ��ջ��32λ���        */
#define OS_STK_GROWTH  1         /*  ��ջ�Ǵ������³���   */

/*
************************************************************
*  �ٽ�������,�����ж�����
************************************************************
*/
#define OS_CRITICAL_METHOD      3 /* �ж�״̬�����ڱ����� */
typedef INT32U         OS_CPU_SR;
#define OS_ENTER_CRITICAL()    {cpu_sr = CPU_SR_Save();}	/* ���ж� */
#define OS_EXIT_CRITICAL()     {CPU_SR_Restore(cpu_sr);}    /* �ָ��ж�״̬ */

/*
************************************************************
*  ���ʵ�ֵĺ�������
************************************************************
*/
#define OS_TASK_SW()           OSCtxSw() /* ���������л�����*/
OS_CPU_SR CPU_SR_Save(void);
void CPU_SR_Restore(OS_CPU_SR cpu_sr);
void OSCtxSw(void);
void OSIntCtxSw(void);
void OSStartHighRdy(void);

#endif 
