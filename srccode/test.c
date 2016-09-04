#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* ���ڴ�ӡ��Ϣ����ָ�� */
NIX_TCB *gpstrSerialTaskTcb;	/* ���ڴ�ӡ����TCBָ�� */


/**********************************************/
//��������:��������1
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* ��ӡջ��Ϣ */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask1 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* ��������0.5�� */
		TEST_TaskRun(500);

		/* ��ȡϵͳʱ�� */
		uiTick = NIX_GetSystemTick();

		/* ϵͳ����ʱ����10~20��֮�� */
		if ((uiTick >= 1000) && (uiTick < 2000)) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}
		/* ϵͳ����ʱ����20~30��֮�� */
		else if ((uiTick >= 2000) && (uiTick < 3000)) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());
		}
		/* ϵͳ����ʱ�䳬��30�� */
		else if (uiTick >= 3000) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());
		}

		/* �����ӳ�2�� */
		(void) NIX_TaskDelay(200);

		/* ��ӡջ��Ϣ */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask1 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}


}


/**********************************************/
//��������:��������2
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* ��ӡջ��Ϣ */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask2 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* ��������0.5�� */
		TEST_TaskRun(500);

		/* ��ȡϵͳʱ�� */
		uiTick = NIX_GetSystemTick();

		/* ϵͳ����ʱ����10~20��֮�� */
		if ((uiTick >= 1000) && (uiTick < 2000)) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());
		}
		/* ϵͳ����ʱ����20~30��֮�� */
		else if ((uiTick >= 2000) && (uiTick < 3000)) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());
		}
		/* ϵͳ����ʱ�䳬��30�� */
		else if (uiTick >= 3000) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}

		/* �����ӳ�2�� */
		(void) NIX_TaskDelay(200);

		/* ��ӡջ��Ϣ */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask2 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}
}

/**********************************************/
//��������:��������3
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void TEST_TestTask3(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* ��ӡջ��Ϣ */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask3 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* ��������0.5�� */
		TEST_TaskRun(500);

		/* ��ȡϵͳʱ�� */
		uiTick = NIX_GetSystemTick();

		/* ϵͳ����ʱ�䳬��10�� */
		if (uiTick >= 1000) {
			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());

			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());

			/* ��ӡ������ֵ */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}

		/* �����ӳ�2�� */
		(void) NIX_TaskDelay(200);

		/* ��ӡջ��Ϣ */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask3 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}

}

/***********************************************************************************
��������: ���Ժ���, ���õݹ���ü����ۼӺ�.
��ڲ���: none.
�� �� ֵ: ������.
***********************************************************************************/
U32 TEST_Add(U32 uiNum)
{
	if (1 == uiNum) {
		return 1;
	}

	return uiNum + TEST_Add(uiNum - 1);
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
				"\r\nTask %s 0x%08X is created! Tick is: %d", pstrTcb->pucTaskName, pstrTcb,
				NIX_GetSystemTick());
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
