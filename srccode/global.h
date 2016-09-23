#ifndef GLOBAL_H
#define GLOBAL_H

#include "nix.h"

//版本号:主版本号.子版本号.修正版本号.编译版本号
#define SOFTWARE_VER                "003.004.001.001"

#define BUF_MAX_LEN                 256

#define BUFPOOLNUM                  40	//BUFPOOL数量
#define MSGBUFLEN                   100	//每个BUF的长度，单位:字节

#define TASKSTACK                   1024	//任务栈大小，单位:字节

typedef struct slist		//单向链表结构
{
	struct slist *next;
} SLIST;

typedef struct buf_pool		//缓冲池结构
{
	SLIST list;		//缓冲池链表根指针
	NIX_SEM *psem;		//缓冲池互斥操作的二进制信号量
	U32 len;		//缓冲池中每个缓冲的长度
	U32 s_num;		//从堆中静态申请的缓冲的数量
	U32 d_num;		//从缓冲池中动态申请的缓冲的数量
} BUF_POOL;

typedef struct buf_node		//缓冲节点结构
{
	NIX_LIST *list;		//缓冲节点相连的链表指针，在缓冲区单向链表，在消息队列中双向链表
	BUF_POOL *phead;	//指向缓冲池
} BUF_NODE;

typedef struct msgbuf		//消息缓冲结构
{
	NIX_LIST strList;	//缓冲链表
	U8 ucLength;		//消息长度
	U8 aucBuf[MSGBUFLEN];	//消息缓冲
} MSGBUF;

typedef struct bufpool		//消息缓冲池结构
{
	NIX_LIST strFreeList;	//缓冲池空闲链表
	MSGBUF astrBufPool[BUFPOOLNUM];	//缓冲池
} BUFPOOL;

typedef struct task_str     //任务结构
{
    SLIST list;             //任务链表节点
    NIX_QUE* pque;          //任务消息传递队列
    NIX_TCB* ptcb;          //任务TCB
}TASK_STR;

typedef struct task_pool    //任务池结构
{
    SLIST list;             //任务池链表根节点
    NIX_SEM* psem;          //任务池信号量
    U32 s_num;              //静态创建任务数量
    U32 d_num;              //动态创建任务数量
}TASK_POOL;

/************************************************************************/
extern BUFPOOL gstrBufPool;
extern NIX_QUE *gpstrSerialMsgQue;
extern NIX_TCB *gpstrSerialTaskTcb;
//extern NIX_SEM *gpstrSemMut;

/************************************************************************/
extern void DEV_SoftwareInit(void);
extern void DEV_HardwareInit(void);
extern void DEV_PutStrToMem(U8 * pvStringPt, ...);
extern void DEV_PrintMsg(U8 * pucBuf, U32 uiLen);
extern MSGBUF *DEV_BufferAlloc(BUFPOOL * pstrBufPool);
extern void DEV_BufferFree(BUFPOOL * pstrBufPool, NIX_LIST * pstrQueNode);
extern void DEV_DelayMs(U32 uiMs);
extern void TEST_TestTask1(void *pvPara);
extern void TEST_TestTask2(void *pvPara);
extern void TEST_TestTask3(void *pvPara);
extern void TEST_SerialPrintTask(void *pvPara);
extern void TEST_TaskCreatePrint(NIX_TCB * pstrTcb);
extern void TEST_TaskSwitchPrint(NIX_TCB * pstrOldTcb, NIX_TCB * pstrNewTcb);
extern void TEST_TaskDeletePrint(NIX_TCB * pstrTcb);
#endif
