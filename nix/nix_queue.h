#ifndef NIX_QUEUE_H
#define NIX_QUEUE_H

/* �ڵ������еķ���ֵ */
#define RTN_QUPTOV      RTN_SMGVOV	/* ���ڵ������е��¶����ź������ */

/* ����ȡ���ڵ�ķ���ֵ */
#define RTN_QUGTTO      RTN_SMTKTO	/* �ȴ�������Ϣ��ʱ��ľ�, ��ʱ���� */
#define RTN_QUGTRT      RTN_SMTKRT	/* û�л�ȡ��������Ϣ, ֱ�ӷ��� */
#define RTN_QUGTDL      RTN_SMTKDL	/* ���б�ɾ�� */

/* ���е��ȷ�ʽ */
#define QUEFIFO         SEMFIFO	/* ���в����Ƚ��ȳ����� */
#define QUEPRIO         SEMPRIO	/* ���в������ȼ����� */

/* ���г�ʼֵ */
#define QUEEMPTY        SEMEMPTY	/* ����Ϊ��״̬ */
#define QUEFULL         SEMFULL	/* ����Ϊ��״̬ */

/* �ӳٵȴ���ʱ�� */
#define QUENOWAIT       SEMNOWAIT	/* ���ȴ� */
#define QUEWAITFEV      SEMWAITFEV	/* ���õȴ� */

typedef struct nix_que		//���нṹ
{
	NIX_LIST strList;	//��������
	NIX_SEM strSem;		//�����ź���
	U8 *pucQueMem;		//��������ʱ���ڴ��ַ
} NIX_QUE;

extern NIX_QUE *NIX_QueCreate(NIX_QUE * pstrQue, U32 uiQueOpt);
extern U32 NIX_QuePut(NIX_QUE * pstrQue, NIX_LIST * pstrQueNode);
extern U32 NIX_QueGet(NIX_QUE * pstrQue, NIX_LIST * *ppstrQueNode, U32 uiDelayTick);
extern U32 NIX_QueDelete(NIX_QUE * pstrQue);

#endif
