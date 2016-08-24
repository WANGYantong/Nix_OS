#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* ���ڴ�ӡ��Ϣ����ָ�� */
NIX_TCB *gpstrSerialTaskTcb;	/* ���ڴ�ӡ����TCBָ�� */

NIX_SEM *gpstrSemSync;		/*ͬ���ź��� */
NIX_SEM *gpstrSemMute;		/*�����ź��� */

/**********************************************/
//��������:��������1
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* �����ӡ */
		DEV_PutStrToMem((U8 *) "\r\nTask1 is running ! Tick is: %d", NIX_GetSystemTick());

		/* ��������1.5�� */
		TEST_TaskRun(1500);

		/* �����ӳ�2�� */
		(void) NIX_TaskDelay(200);

		/* ǰ10��, ÿ�������ͷ�һ��gpstrSemSync�ź��� */
		if (i < 10) {
			i++;

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 give gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* ͬ���������� */
			(void) NIX_SemGive(gpstrSemSync);
		}
		/* ������5��, ÿ������flushһ��gpstrSemSync�ź��� */
		else if (i < 15) {
			i++;

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 flush gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* �ͷ����б�gpstrSemSync���������� */
			(void) NIX_SemFlush(gpstrSemSync);
		}
		/* ɾ��gpstrSemSync�ź��� */
		else if (15 == i) {
			i++;

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 delete gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* ɾ��gpstrSemSync�ź��� */
			(void) NIX_SemDelete(gpstrSemSync);
		}
	}
}



/**********************************************/
//��������:��������2
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* ǰ3�λ�ȡgpstrSemMute�ź���, ��TEST_TestTask3������ */
		if (i < 3) {
			i++;

			/* ��ȡ���ź��������� */
			(void) NIX_SemTake(gpstrSemMute, SEMWAITFEV);

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask2 take gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* ��������0.5�� */
			TEST_TaskRun(500);

			/* �����ӳ�2�� */
			(void) NIX_TaskDelay(200);

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask2 give gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* �ͷ��ź���, �Ա�����������Ի�ø��ź��� */
			(void) NIX_SemGive(gpstrSemMute);
		} else {	/* ��������ȡgpstrSemSync�ź���, ��TEST_TestTask1���񼤻� */

			i++;

			/* �ź�����ɾ��, ���񷵻� */
			if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
				/* �����ӡ */
				DEV_PutStrToMem((U8 *) "\r\nTask2 gpstrSemSync deleted! Tick is: %d",
						NIX_GetSystemTick());

				return;
			}
			/* ��ȡ��gpstrSemSync�ź��������� */
			else {
				/* �����ӡ */
				DEV_PutStrToMem((U8 *) "\r\nTask2 take gpstrSemSync %d! Tick is: %d",
						i, NIX_GetSystemTick());

				/* ��������0.5�� */
				TEST_TaskRun(500);
			}
		}
	}

}


/**********************************************/
//��������:��������3
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask3(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* ǰ3�λ�ȡgpstrSemMute�ź���, ��TEST_TestTask2������ */
		if (i < 3) {
			i++;

			/* ��ȡ���ź��������� */
			(void) NIX_SemTake(gpstrSemMute, SEMWAITFEV);

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask3 take gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* ��������0.5�� */
			TEST_TaskRun(500);

			/* �����ӳ�1.5�� */
			(void) NIX_TaskDelay(150);

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask3 give gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* �ͷ��ź���, �Ա�����������Ի�ø��ź��� */
			(void) NIX_SemGive(gpstrSemMute);
		} else {	/* ��������ȡgpstrSemSync�ź���, ��TEST_TestTask1���񼤻� */

			i++;

			/* �ź�����ɾ��, ���񷵻� */
			if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
				/* �����ӡ */
				DEV_PutStrToMem((U8 *) "\r\nTask3 gpstrSemSync deleted! Tick is: %d",
						NIX_GetSystemTick());

				return;
			} else {	/* ��ȡ��gpstrSemSync�ź��������� */

				/* �����ӡ */
				DEV_PutStrToMem((U8 *) "\r\nTask3 take gpstrSemSync %d! Tick is: %d",
						i, NIX_GetSystemTick());

				/* ��������0.5�� */
				TEST_TaskRun(500);
			}
		}
	}


}

/**********************************************/
//��������:��������4
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask4(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* �ź�����ɾ��, ���񷵻� */
		if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask4 gpstrSemSync deleted! Tick is: %d", NIX_GetSystemTick());

			return;
		} else {	/* ��ȡ��gpstrSemSync�ź��������� */

			i++;

			/* �����ӡ */
			DEV_PutStrToMem((U8 *) "\r\nTask4 take gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* ��������0.5�� */
			TEST_TaskRun(500);
		}
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
		if (RTN_SUCD == NIX_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode)) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			/* �������е����ݴ�ӡ������ */
			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			/* ������Ϣ�е����ݷ������, �ͷŻ��� */
			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
		} else {	/* û�л�ȡ����Ϣ, �ӳ�һ��ʱ����ٲ�ѯ���� */

			(void) NIX_TaskDelay(100);
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
