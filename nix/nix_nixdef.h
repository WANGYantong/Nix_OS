#ifndef NIX_NIXDEF_H
#define NIX_NIXDEF_H

//�����������
typedef char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef signed char S8;
typedef short S16;
typedef int S32;
typedef void (*VFUNC) (void *);	//����ָ��
typedef void (*VFUNC1) (U8);	//����ָ��

#ifndef NULL
#define NULL                ((void*)0)	//�������ɿ�ָ��
#endif

//����ֵ
#define RTN_SUCD            0	//�����ɹ�
#define RTN_FAIL            1	//����ʧ��

//USR����ģʽ����
#define MODE_USR            0x01000000

//�ֽڶ�������
#define ALIGN4MASK          0xFFFFFFFC	//4�ֽڶ���
#define ALIGN8MASK          0xFFFFFFF8	//8�ֽڶ���

//��ȡָ����ַ������
#define ADDRVAL(addr)      (*((volatile U32*)(addr)))

//���ȼ�ѡ��
#define PRIORITY256          256
#define PRIORITY128          128
#define PRIORITY64           64
#define PRIORITY32           32
#define PRIORITY16           16
#define PRIORITY8            8

//��Ӧ�����ȼ�����
#if PRIORITYNUM == PRIORITY256
#define PRIOFLAGGRP1    32
#define PRIOFLAGGRP2    4
#elif PRIORITYNUM == PRIORITY128
#define PRIOFLAGGRP1    16
#define PRIOFLAGGRP2    2
#elif PRIORITYNUM == PRIORITY64
#define PRIOFLAGGRP1    8
#define PRIOFLAGGRP2    1
#elif PRIORITYNUM == PRIORITY32
#define PRIOFLAGGRP1    4
#define PRIOFLAGGRP2    1
#elif PRIORITYNUM == PRIORITY16
#define PRIOFLAGGRP1    2
#define PRIOFLAGGRP2    1
#elif PRIORITYNUM == PRIORITY8
#define PRIOFLAGGRP1    1
#define PRIOFLAGGRP2    1
#else
#error The priority must between 8 and 256, and the power must be 2.
#endif

//TCB�б��ݼĴ�����Ľṹ��
typedef struct stackreg {
	U32 uiR0;
	U32 uiR1;
	U32 uiR2;
	U32 uiR3;
	U32 uiR4;
	U32 uiR5;
	U32 uiR6;
	U32 uiR7;
	U32 uiR8;
	U32 uiR9;
	U32 uiR10;
	U32 uiR11;
	U32 uiR12;
	U32 uiR13;
	U32 uiR14;
	U32 uiR15;
	U32 uiXpsr;
	U32 uiExc_Rtn;
} STACKREG;

typedef struct nix_list		//����ṹ
{
	struct nix_list *pstrHead;	//ͷָ��
	struct nix_list *pstrTail;	//βָ��
} NIX_LIST;

typedef struct nix_tcbque	//TCB����
{
	NIX_LIST strQueHead;	//ָ�����ȼ�����
	struct nix_tcb *pstrTcb;	//TCBָ��
} NIX_TCBQUE;

typedef struct nix_taskopt	//�������
{
	U8 ucTaskSta;		//��������״̬
	U32 uiDelayTick;	//�ӳ�ʱ��
} NIX_TASKOPT;

typedef struct nix_prioflag	//���ȼ���־��
{
#if PRIORITYNUM >= PRIORITY128
	U8 aucPrioFlagGRP1[PRIOFLAGGRP1];
	U8 aucPrioFlagGRP2[PRIOFLAGGRP2];
	U8 ucPrioFlagGrp3;
#elif PRIORITYNUM >= PRIORITY16
	U8 aucPrioFlagGRP1[PRIOFLAGGRP1];
	U8 ucPrioFlagGRP2;
#else
	U8 ucPrioFlagGRP1;
#endif
} NIX_PRIOFLAG;

typedef struct nix_taskschedtab	//���ȼ����ȱ�
{
	NIX_LIST astrList[PRIORITYNUM];	//�������ȼ��ĸ��ڵ�
	NIX_PRIOFLAG strFlag;	//���ȼ���־
} NIX_TASKSCHEDTAB;

typedef struct nix_sem		//�ź����ṹ��
{
	NIX_TASKSCHEDTAB strSemtab;	//�ź������ȱ�
	U32 uiCounter;		//�ź�������ֵ
	U32 uiSemOpt;		//�ź����ĵ���������
	U8 *pucSemMem;		//�����ź���ʱ���ڴ��ַ
	struct nix_tcb *pstrSemTask;	//��ȡ�������ź���������
} NIX_SEM;

#ifdef NIX_DEBUGCPUSHARE

typedef struct nix_cpushare {
	U32 uiSysTickVal;	//��¼�����л�������ʱ��ֵ
	U32 uiCounter;		//CPUռ�еļ���ֵ
	U32 uiCPUShare;		//CPUռ����
} NIX_CPUSHARE;

#endif

typedef struct nix_tcb		//TCB�ṹ��
{
	STACKREG strStackReg;	//��Ҫ����ļĴ�����
	NIX_TCBQUE strTaskQue;	//��������
	NIX_TCBQUE strTcbQue;	//TCB�ṹ����,�ҽӵ�ready�Լ�delay����
	NIX_TCBQUE strSemQue;	//sem����У��ҽӵ��������ź�����
	NIX_SEM *pstrSem;	//����������ź���ָ��
	U8 *pucTaskName;	//��������ָ��
	U8 *pucTaskStack;	//���������ջָ��
	U32 uiTaskFlag;		//�����־
	U8 ucTaskPrio;		//�������ȼ�
#ifdef NIX_TASKPRIOINHER
	U8 ucTaskPrioBackup;	//����ԭ�е����ȼ�
#endif
	NIX_TASKOPT strTaskOpt;	//�������
	U32 uiStillTick;	//�ӳٽ���ʱ��
#ifdef NIX_DEBUGCPUSHARE
	NIX_CPUSHARE strCpushare;	//CPUռ���ʽṹ
#endif
} NIX_TCB;

#endif
