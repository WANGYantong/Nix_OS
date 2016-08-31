#ifndef GLOBAL_H
#define GLOBAL_H

#include "nix.h"

//�汾��:���汾��.�Ӱ汾��.�����汾��.����汾��
#define SOFTWARE_VER                "002.007.001.001"

#define BUFPOOLNUM                  20	//BUFPOOL����
#define MSGBUFLEN                   100	//ÿ��BUF�ĳ��ȣ���λ:�ֽ�

#define TASKSTACK                   1024	//����ջ��С����λ:�ֽ�

typedef struct msgbuf		//��Ϣ����ṹ
{
	NIX_LIST strList;	//��������
	U8 ucLength;		//��Ϣ����
	U8 aucBuf[MSGBUFLEN];	//��Ϣ����
} MSGBUF;

typedef struct bufpool		//��Ϣ����ؽṹ
{
	NIX_LIST strFreeList;	//����ؿ�������
	MSGBUF astrBufPool[BUFPOOLNUM];	//�����
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
