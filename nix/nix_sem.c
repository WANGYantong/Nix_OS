#include <stdlib.h>
#include "nix_inner.h"

/**********************************************/
//��������:�����ź���
//�������:pstrSem:������ź������ڴ��ַ
//         uiSemOpt:�ź������ȷ�ʽ:FIFO or PRIO
//         uiInitVal:�ź�����ʼֵ:FULL or EMPTY
//����ֵ  :�������ź���ָ��
/**********************************************/
NIX_SEM *NIX_SemCreat(NIX_SEM * pstrSem, U32 uiSemOpt, U32 uiInitVal)
{
	U8 *pucSemMemAddr;

	if (((uiSemOpt & SEMSCHEDOPTMASK) != SEMFIFO)
	    && ((uiSemOpt & SEMSCHEDOPTMASK) != SEMPRIO)) {
		return (NIX_SEM *) NULL;
	}

	if ((uiInitVal != SEMEMPTY) && (uiInitVal != SEMFULL)) {
		return (NIX_SEM *) NULL;
	}

	if (pstrSem == NULL) {
		(void) NIX_IntLock();

		pucSemMemAddr = malloc(sizeof(NIX_SEM));

		if (pucSemMemAddr == NULL) {
			(void) NIX_IntUnLock();

			return (NIX_SEM *) NULL;
		}

		(void) NIX_IntUnLock();

		pstrSem = (NIX_SEM *) pucSemMemAddr;
	} else {
		pucSemMemAddr = (U8 *) NULL;
	}

	NIX_TaskSchedTabInit(&pstrSem->strSemtab);
	pstrSem->uiCounter = uiInitVal;
	pstrSem->uiSemOpt = uiSemOpt;
	pstrSem->pucSemMem = pucSemMemAddr;

	return pstrSem;
}

/**********************************************/
//��������:��ȡ�ź���
//�������:pstrSem:����ȡ���ź���ָ��
//         uiDelayTick:�ȴ�ʱ��
//����ֵ  :RTN_SUCD: ���ӳ�ʱ���ڻ�ȡ���ź���.
//         RTN_FAIL: ��ȡ�ź���ʧ��.
//         RTN_SMTKTO: �ȴ��ź�����ʱ��ľ�, ��ʱ����.
//         RTN_SMTKRT: ʹ�ò��ȴ�ʱ�����û�л�ȡ���ź���, �������̷���.
//         RTN_SMTKDL: �ź�����ɾ��.
/**********************************************/
U32 NIX_SemTake(NIX_SEM * pstrSem, U32 uiDelayTick)
{
	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	if (NIX_RunInInt() == RTN_SUCD) {
		if (uiDelayTick != SEMNOWAIT) {
			return RTN_FAIL;
		}
	}

	(void) NIX_IntLock();

	if (uiDelayTick == SEMNOWAIT) {
		if (pstrSem->uiCounter == SEMFULL) {
			pstrSem->uiCounter == SEMEMPTY;
			(void) NIX_IntUnLock();
			return RTN_SUCD;
		} else {
			(void) NIX_IntUnLock();
			return RTN_SMTKRT;
		}
	} else {
		if (pstrSem->uiCounter == SEMFULL) {
			pstrSem->uiCounter = SEMEMPTY;
			(void) NIX_IntUnLock();
			return RTN_SUCD;
		} else {
			if (NIX_TaskPend(pstrSem, uiDelayTick) == RTN_FAIL) {
				(void) NIX_IntUnLock();
				return RTN_FAIL;
			}

			(void) NIX_IntLock();

			NIX_TaskSwiSched();

			return gpstrCurTcb->strTaskOpt.uiDelayTick;
		}
	}
}


/**********************************************/
//��������:�ͷ��ź���
//�������:pstrSem:���ͷŵ��ź���ָ��
//����ֵ  :RTN_SUCD:�ͷųɹ�       RTN_FAIL:�ͷ�ʧ��
/**********************************************/
U32 NIX_SemGive(NIX_SEM * pstrSem)
{
	NIX_TCB *pstrTcb;
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

	if (pstrSem->uiCounter == SEMEMPTY) {
		pstrTcb = NIX_SemGetAcitveTask(pstrSem);

		if (pstrTcb != NULL) {
			(void) NIX_TaskDelFromSemTab(pstrTcb);

			if ((pstrTcb->uiTaskFlag & DELAYQUEFLAG) == DELAYQUEFLAG) {
				pstrNode = pstrTcb->strTcbQue.strQueHead;
				(void) NIX_ListCurNodeDelete(&gstrDelayTab, pstrNode);
				pstrTcb->uiTaskFlag$ = (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;

			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			ucTaskPrio = pstrTcb->ucTaskPrio;
			pstrList = &gstrReadyTab.astrList[ucTaskPrio];
			pstrPrioFlag = &gstrReadyTab.strFlag;

			NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

			pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

			(void) NIX_IntUnLock();

			NIX_TaskSwiSched();

			return RTN_SUCD;
		} else {
			pstrSem->uiCounter = SEMFULL;
		}
	}

	(void) NIX_IntUnLock();

	return RTN_SUCD;
}

/**********************************************/
//��������:�ͷű��ź�����������������
//�������:pstrSem:����������ź���ָ��
//         uiRtnValue:ָ������������ķ���ֵ
//����ֵ  :RTN_SUCD:�ͷųɹ�       RTN_FAIL:�ͷ�ʧ��
/**********************************************/
U32 NIX_SemFlushValue(NIX_SEM * pstrSem, U32 uiRtnValue)
{
	M_TCB *pstrTcb;
	M_DLIST *pstrList;
	M_DLIST *pstrNode;
	M_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

	while (1) {
		pstrTcb = NIX_SemGetAcitveTask(pstrSem);

		if (pstrTcb != NULL) {
			(void) NIX_TaskDelFromSemTab(pstrTcb);

			if ((pstrTcb->uiTaskFlag & DELAYQUEFLAG) == DELAYQUEFLAG) {
				pstrNode = pstrTcb->strTcbQue.strQueHead;
				(void) NIX_ListCurNodeDelete(&gstrDelayTab, pstrNode);
				pstrTcb->uiTaskFlag$ = (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = uiRtnValue;

			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			ucTaskPrio = pstrTcb->ucTaskPrio;
			pstrList = &gstrReadyTab.astrList[ucTaskPrio];
			pstrPrioFlag = &gstrReadyTab.strFlag;

			NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

			pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;
		} else {
			break;
		}
	}
	//�ź�����ʱ�ò����ˣ����Ǳ�˭�ͷŵ�
	pstrSem->uiCounter = SEMEMPTY;

	(void) NIX_IntUnLock();

	NIX_TaskSwiSched();

	return RTN_SUCD;
}


/**********************************************/
//��������:�ͷű��ź�����������������
//�������:pstrSem:����������ź���ָ��
//����ֵ  :RTN_SUCD:�ͷųɹ�       RTN_FAIL:�ͷ�ʧ��
/**********************************************/
U32 NIX_SemFlush(NIX_SEM * pstrSem)
{
	return NIX_SemFlushValue(pstrSem, RTN_SUCD);
}

/**********************************************/
//��������:�ͷ��ź���
//�������:pstrSem:��ɾ�����ź���ָ��
//����ֵ  :RTN_SUCD:ɾ���ɹ�       RTN_FAILɾ��ʧ��
/**********************************************/
U32 NIX_SemDelete(NIX_SEM * pstrSem)
{
	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	if (NIX_SemFlushValue(pstrSem, RTN_SMTKDL) != RTN_SUCD) {
		return RTN_FAIL;
	}

	if (pstrSem->pucSemMem != NULL) {
		(void) NIX_IntLock();

		free(pstrSem->pucSemMem);

		(void) NIX_IntUnLock();
	}

	return RTN_SUCD;
}
