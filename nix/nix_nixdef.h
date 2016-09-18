#ifndef NIX_NIXDEF_H
#define NIX_NIXDEF_H

//定义变量类型
typedef char U8;
typedef unsigned short U16;
typedef unsigned int U32;
typedef signed char S8;
typedef short S16;
typedef int S32;
typedef void (*VFUNC) (void *);	//函数指针
typedef void (*VFUNC1) (U8);	//函数指针

#ifndef NULL
#define NULL                ((void*)0)	//可以理解成空指针
#endif

//返回值
#define RTN_SUCD            0	//创建成功
#define RTN_FAIL            1	//创建失败

//USR工作模式掩码
#define MODE_USR            0x01000000

//字节对齐掩码
#define ALIGN4MASK          0xFFFFFFFC	//4字节对齐
#define ALIGN8MASK          0xFFFFFFF8	//8字节对齐

//获取指定地址的数据
#define ADDRVAL(addr)      (*((volatile U32*)(addr)))

//优先级选择
#define PRIORITY256          256
#define PRIORITY128          128
#define PRIORITY64           64
#define PRIORITY32           32
#define PRIORITY16           16
#define PRIORITY8            8

//相应的优先级配置
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

//TCB中备份寄存器组的结构体
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

typedef struct nix_list		//链表结构
{
	struct nix_list *pstrHead;	//头指针
	struct nix_list *pstrTail;	//尾指针
} NIX_LIST;

typedef struct nix_tcbque	//TCB队列
{
	NIX_LIST strQueHead;	//指向优先级队列
	struct nix_tcb *pstrTcb;	//TCB指针
} NIX_TCBQUE;

typedef struct nix_taskopt	//任务参数
{
	U8 ucTaskSta;		//任务运行状态
	U32 uiDelayTick;	//延迟时间
} NIX_TASKOPT;

typedef struct nix_prioflag	//优先级标志表
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

typedef struct nix_taskschedtab	//优先级调度表
{
	NIX_LIST astrList[PRIORITYNUM];	//各个优先级的根节点
	NIX_PRIOFLAG strFlag;	//优先级标志
} NIX_TASKSCHEDTAB;

typedef struct nix_sem		//信号量结构体
{
	NIX_TASKSCHEDTAB strSemtab;	//信号量调度表
	U32 uiCounter;		//信号量计数值
	U32 uiSemOpt;		//信号量的调度与种类
	U8 *pucSemMem;		//创建信号量时的内存地址
	struct nix_tcb *pstrSemTask;	//获取到互斥信号量的任务
} NIX_SEM;

#ifdef NIX_DEBUGCPUSHARE

typedef struct nix_cpushare {
	U32 uiSysTickVal;	//记录任务切换进来的时钟值
	U32 uiCounter;		//CPU占有的计数值
	U32 uiCPUShare;		//CPU占有率
} NIX_CPUSHARE;

#endif

typedef struct nix_tcb		//TCB结构体
{
	STACKREG strStackReg;	//需要保存的寄存器组
	NIX_TCBQUE strTaskQue;	//任务链表
	NIX_TCBQUE strTcbQue;	//TCB结构队列,挂接到ready以及delay表上
	NIX_TCBQUE strSemQue;	//sem表队列，挂接到阻塞的信号量上
	NIX_SEM *pstrSem;	//阻塞任务的信号量指针
	U8 *pucTaskName;	//任务名称指针
	U8 *pucTaskStack;	//创建任务的栈指针
	U32 uiTaskFlag;		//任务标志
	U8 ucTaskPrio;		//任务优先级
#ifdef NIX_TASKPRIOINHER
	U8 ucTaskPrioBackup;	//备份原有的优先级
#endif
	NIX_TASKOPT strTaskOpt;	//任务参数
	U32 uiStillTick;	//延迟结束时间
#ifdef NIX_DEBUGCPUSHARE
	NIX_CPUSHARE strCpushare;	//CPU占有率结构
#endif
} NIX_TCB;

#endif
