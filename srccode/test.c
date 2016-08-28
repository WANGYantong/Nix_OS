#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* ���ڴ�ӡ��Ϣ����ָ�� */
NIX_TCB *gpstrSerialTaskTcb;	/* ���ڴ�ӡ����TCBָ�� */

NIX_SEM *gpstrSemMut;

/**********************************************/
//��������:��������1
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	while (1) {
		/* �����ӡ */
		DEV_PutStrToMem((U8 *) "\r\nTask1 is running! Tick is: %d", NIX_GetSystemTick());

		DEV_PutStrToMem((U8 *) "\r\nTask1 is running! Tick is: %d", NIX_GetSystemTick());

		/* ��������0.5�� */
		TEST_TaskRun(500);

		/* �����ӳ�2�� */
		(void) NIX_TaskDelay(200);
	}

}


/**********************************************/
//��������:��������2
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	while (1) {
		/* �����ӡ */
		DEV_PutStrToMem((U8 *) "\r\nTask2 is running! Tick is: %d", NIX_GetSystemTick());

		DEV_PutStrToMem((U8 *) "\r\nTask2 is running! Tick is: %d", NIX_GetSystemTick());

		DEV_PutStrToMem((U8 *) "\r\nTask2 is running! Tick is: %d", NIX_GetSystemTick());

		/* ��������2�� */
		TEST_TaskRun(2000);

		/* �����ӳ�3�� */
		(void) NIX_TaskDelay(300);
	}

}

/**********************************************/
//��������:���ڴ�ӡ���񣬴Ӷ����л�ȡ��Ҫ��ӡ����Ϣ���壬�������е����ݴ�ӡ������
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_SerialPrintTask(void *pvPara)
{
	NIX_LIST *pstrMsgQueNode;
	MSGBUF *pstrMsgBuf;

	/* �Ӷ���ѭ����ȡ��Ϣ */
	while (1) {
		/* �Ӷ����л�ȡ��һ����Ҫ��ӡ����Ϣ, �򴮿ڴ�ӡ��Ϣ���� */
		if (NIX_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV) == RTN_SUCD) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			/* �������е����ݴ�ӡ������ */
			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			/* ������Ϣ�е����ݷ������, �ͷŻ��� */
			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
		}
	}

}

/**********************************************/
//��������:ģ���������к���
//�������:uiMs:Ҫ�ӳٵ�ʱ�䣬��λms
//����ֵ  :none
/**********************************************/
void TEST_TaskRun(U32 uiMs)
{
	DEV_DelayMs(uiMs);
}

/**********************************************/
//��������:�����񴴽����̴�ӡ���ڴ���
//�������:pstrTcb:�´����������TCBָ��
//����ֵ  :none
/**********************************************/
void TEST_TaskCreatePrint(NIX_TCB * pstrTcb)
{
	if (pstrTcb != (NIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s is created! Tick is: %d", pstrTcb->pucTaskName, NIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nFail to create task! Tick is: %d", NIX_GetSystemTick());
	}
}

/**********************************************/
//��������:�������л����̴�ӡ���ڴ���
//�������:pstrOldTcb:�л�ǰ������TCBָ��
//         pstrNewTcb:�л��������TCBָ��
//����ֵ  :none
/**********************************************/
void TEST_TaskSwitchPrint(NIX_TCB * pstrOldTcb, NIX_TCB * pstrNewTcb)
{
	//����ӡ���ڴ�ӡ������л�����,����������Ϊ��������
	if (pstrOldTcb == gpstrSerialTaskTcb) {
		pstrOldTcb = NIX_GetIdleTcb();
	}

	if (pstrNewTcb == gpstrSerialTaskTcb) {
		pstrNewTcb = NIX_GetIdleTcb();
	}
	//ͬһ�������л�����ӡ��Ϣ
	if (pstrNewTcb == pstrOldTcb) {
		return;
	}

	if (pstrOldTcb != (NIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s ----> Task %s ! Tick is: %d",
				pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName, NIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nTask NULL ----> Task %s ! Tick is: %d",
				pstrNewTcb->pucTaskName, NIX_GetSystemTick());
	}

}

/**********************************************/
//��������:������ɾ�����̴�ӡ���ڴ���
//�������:pstrTcb:��ɾ���������TCBָ��
//����ֵ  :none
/**********************************************/
void TEST_TaskDeletePrint(NIX_TCB * pstrTcb)
{
	DEV_PutStrToMem((U8 *) "\r\nTask %s is deleted! Tick is: %d", pstrTcb->pucTaskName, NIX_GetSystemTick());
}
