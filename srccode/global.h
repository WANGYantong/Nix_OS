#ifndef GLOBAL_H
#define GLOBAL_H

#include "nix.h"

//版本号:主版本号.子版本号.修正版本号.编译版本号
#define SOFTWARE_VER                "002.007.001.001"

#define BUFPOOLNUM                  20	//BUFPOOL数量
#define MSGBUFLEN                   100	//每个BUF的长度，单位:字节

#define TASKSTACK                   1024	//任务栈大小，单位:字节

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

/************************************************************************/
extern BUFPOOL gstrBufPool;
extern NIX_QUE *gpstrSerialMsgQue;
extern NIX_TCB *gpstrSerialTaskTcb;
extern NIX_SEM *gpstrSemMut;

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
