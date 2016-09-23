#ifndef GLOBAL_H
#define GLOBAL_H

#include "nix.h"

//�汾��:���汾��.�Ӱ汾��.�����汾��.����汾��
#define SOFTWARE_VER                "003.004.001.001"

#define BUF_MAX_LEN                 256

#define BUFPOOLNUM                  40	//BUFPOOL����
#define MSGBUFLEN                   100	//ÿ��BUF�ĳ��ȣ���λ:�ֽ�

#define TASKSTACK                   1024	//����ջ��С����λ:�ֽ�

typedef struct slist		//��������ṹ
{
	struct slist *next;
} SLIST;

typedef struct buf_pool		//����ؽṹ
{
	SLIST list;		//����������ָ��
	NIX_SEM *psem;		//����ػ�������Ķ������ź���
	U32 len;		//�������ÿ������ĳ���
	U32 s_num;		//�Ӷ��о�̬����Ļ��������
	U32 d_num;		//�ӻ�����ж�̬����Ļ��������
} BUF_POOL;

typedef struct buf_node		//����ڵ�ṹ
{
	NIX_LIST *list;		//����ڵ�����������ָ�룬�ڻ�����������������Ϣ������˫������
	BUF_POOL *phead;	//ָ�򻺳��
} BUF_NODE;

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

typedef struct task_str     //����ṹ
{
    SLIST list;             //��������ڵ�
    NIX_QUE* pque;          //������Ϣ���ݶ���
    NIX_TCB* ptcb;          //����TCB
}TASK_STR;

typedef struct task_pool    //����ؽṹ
{
    SLIST list;             //�����������ڵ�
    NIX_SEM* psem;          //������ź���
    U32 s_num;              //��̬������������
    U32 d_num;              //��̬������������
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
