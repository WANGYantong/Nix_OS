#include <stdlib.h>
#include "nix_inner.h"

#ifdef NIX_INCLUDETASKHOOK
VFHCRT gvfTaskCreateHook;	//���񴴽����ӱ���
VFHSWT gvfTaskSwitchHook;	//�����л����ӱ���
VFHDLT gvfTaskDeleteHook;	//����ɾ�����ӱ���
#endif

/**********************************************/
//��������:����������
//�������:pucTaskName:ָ���������Ƶ�ָ��
//         vfFuncPointer:  �������ִ�к���
//         pvPara: ��ڲ���
//         pucTaskStack: ����ջ�������ʼ��ַ
//         uiStackSize: ջ��С����λ�ֽ�
//         ucTaskPrio: �������ȼ�
//         pstrTaskOpt:�������ָ��
//����ֵ  :NULL: ���񴴽�ʧ��
//         ����: ����TCBָ��
/**********************************************/
NIX_TCB *NIX_TaskCreat(U8 * pucTaskName, VFUNC vfFuncPointer, void *pvPara,
		       U8 * pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt)
{
	NIX_TCB *pstrTcb;

	if ((vfFuncPointer == NULL) || (uiStackSize == 0)) {
		return (NIX_TCB *) NULL;
	}

	if (pstrTaskOpt != NULL) {
		if (!((pstrTaskOpt->ucTaskSta == TASKREADY)
		      || (pstrTaskOpt->ucTaskSta == TASKDELAY))) {
			return (NIX_TCB *) NULL;
		}
	}

	if (NIX_GetUser() == USERROOT) {
		if (ucTaskPrio > LOWESTPRIO) {
			return (NIX_TCB *) NULL;
		}
	} else {
		if ((ucTaskPrio < USERHIGHESTPRIO)
		    || (ucTaskPrio > USERLOWESTPRIO)) {
			return (NIX_TCB *) NULL;
		}
	}

	pstrTcb =
	    NIX_TaskTcbInit(pucTaskName, vfFuncPointer, pvPara, pucTaskStack, uiStackSize, ucTaskPrio, pstrTaskOpt);

	if (pstrTcb == NULL) {
		return NULL;
	}

	if (guiSystemStatus == SYSTEMSCHEDULE) {
#ifdef NIX_INCLUDETASKHOOK
		if (gvfTaskCreateHook != (VFHCRT) NULL) {
			gvfTaskCreateHook(pstrTcb);
		}
#endif
		NIX_TaskSwiSched();
	}

	return pstrTcb;
}

/**********************************************/
//��������:ɾ��һ������
//�������:pstrTcb:��Ҫɾ���������TCBָ��
//����ֵ  :RTN_SUCD: ����ɾ���ɹ�
//         RTN_FAIL: ����ɾ��ʧ��
/**********************************************/
U32 NIX_TaskDelete(NIX_TCB * pstrTcb)
{
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;
	U8 ucTaskSta;

	if ((pstrTcb == (NIX_TCB *) NULL) || (pstrTcb == gpstrIdleTaskTcb)) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

#ifdef NIX_INCLUDETASKHOOK

	if (gvfTaskDeleteHook != (VFHDLT) NULL) {
		gvfTaskDeleteHook(pstrTcb);
	}
#endif

	ucTaskSta = pstrTcb->strTaskOpt.ucTaskSta;

	if ((ucTaskSta & TASKREADY) == TASKREADY) {
		ucTaskPrio = pstrTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrPrioFlag = &gstrReadyTab.strFlag;

		(void) NIX_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
	}

	if ((pstrTcb->uiTaskFlag & DELAYQUEFLAG) == DELAYQUEFLAG) {
		pstrNode = &pstrTcb->strTcbQue.strQueHead;

		(void) NIX_ListCurNodeDelete(pstrList, pstrNode);
	}

	if ((ucTaskSta & TASKPEND) == TASKPEND) {
		(void) NIX_TaskDelFromSemTab(pstrTcb);
	}

	if ((pstrTcb->uiTaskFlag & TASKSTACKFLAG) == TASKSTACKFLAG) {
		free(pstrTcb->pucTaskStack);
	}

	if (pstrTcb == gpstrCurTcb) {
		gpstrCurTcb = NULL;
	}

	(void) NIX_IntUnLock();

	NIX_TaskSwiSched();

	return RTN_SUCD;

}

/**********************************************/
//��������:ɾ����������
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskSelfDelete(void)
{
	(void) NIX_TaskDelete(gpstrCurTcb);
}

/**********************************************/
//��������:��ʼ������TCB
//�������:pucTaskName: ָ���������Ƶ�ָ��
//         vfFuncPointer:  �������ִ�к���
//         pvPara: ��ڲ���
//         pucTaskStack: ����ջ�������ʼ��ַ
//         uiStackSize: ջ��С����λ�ֽ�
//         ucTaskPrio: �������ȼ�
//         pstrTaskOpt:�������ָ��
//����ֵ  :NULL: ���񴴽�ʧ��
//         ����: ����TCBָ��
/**********************************************/
NIX_TCB *NIX_TaskTcbInit(U8 * pucTaskName, VFUNC vfFuncPointer,
			 void *pvPara, U8 * pucTaskStack, U32 uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt)
{
	NIX_TCB *pstrTcb;
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 *pucStackBy4;
	U32 uiTaskFlag;

	(void) NIX_IntLock();

	if (pucTaskStack == NULL) {
		pucTaskStack = malloc(uiStackSize);
		if (pucTaskStack == NULL) {
			(void) NIX_IntUnLock();
			return (NIX_TCB *) NULL;
		}

		uiTaskFlag = TASKSTACKFLAG;
	} else {
		uiTaskFlag = 0;
	}

	pucStackBy4 = (U8 *) (((U32) pucTaskStack + uiStackSize) & ALIGN4MASK);
	pstrTcb = (NIX_TCB *) (((U32) pucStackBy4 - sizeof(NIX_TCB)) & STACKALIGNMASK);
	pstrTcb->pucTaskStack = pucTaskStack;

	NIX_TaskStackInit(pstrTcb, vfFuncPointer, pvPara);

	pstrTcb->uiTaskFlag = 0;
	pstrTcb->uiTaskFlag |= uiTaskFlag;
	pstrTcb->strTcbQue.pstrTcb = pstrTcb;
	pstrTcb->strSemQue.pstrTcb = pstrTcb;
	pstrTcb->ucTaskPrio = ucTaskPrio;
	pstrTcb->pucTaskName = pucTaskName;

	if (pstrTaskOpt == NULL) {
		pstrTcb->strTaskOpt.ucTaskSta = TASKREADY;
	} else {
		pstrTcb->strTaskOpt.ucTaskSta = pstrTaskOpt->ucTaskSta;
		pstrTcb->strTaskOpt.uiDelayTick = pstrTaskOpt->uiDelayTick;
	}

	if ((pstrTcb->strTaskOpt.ucTaskSta & TASKREADY) == TASKREADY) {
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrNode = &pstrTcb->strTcbQue.strQueHead;
		pstrPrioFlag = &gstrReadyTab.strFlag;

		NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
	}

	if ((pstrTcb->strTaskOpt.ucTaskSta & TASKDELAY) == TASKDELAY) {
		if (pstrTaskOpt->uiDelayTick != DELAYWAITFEV) {
			pstrTcb->uiStillTick = guiTick + pstrTaskOpt->uiDelayTick;
			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			NIX_TaskAddToDelayTab(pstrNode);
			pstrTcb->uiTaskFlag |= DELAYQUEFLAG;
		}
	}

	(void) NIX_IntUnLock();

	return pstrTcb;
}


/**********************************************/
//��������:�������delay̬���ӳ�ָ����ʱ�䡣���ô˺�������������л�
//�������:uiDelayTick:������Ҫ�ӳٵ�ʱ�䣬��λ:tick.
//                     DELAYNOWAIT:���ȴ��������������л�
//                     DELAYWAITFEV:����һֱ�ȴ�
//                     ����:������Ҫ�ӳٵ�tick��Ŀ
//����ֵ  :RTN_SUCD:�����ӳٳɹ�
//         RTN_FAIL:�����ӳ�ʧ��
//         RTN_TKDLTO:�����ӳ�ʱ��ľ�����ʱ����
//         RTN_TKDLBK:�����ӳ�״̬���жϣ����񷵻�
/**********************************************/
U32 NIX_TaskDelay(U32 uiDelayTick)
{
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (uiDelayTick != DELAYNOWAIT) {
		if (gpstrCurTcb == gpstrIdleTaskTcb) {
			return RTN_FAIL;
		}

		ucTaskPrio = gpstrCurTcb->ucTaskPrio;
		pstrList = &gstrReadyTab.astrList[ucTaskPrio];
		pstrPrioFlag = &gstrReadyTab.strFlag;

		(void) NIX_IntLock();

		pstrNode = NIX_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);

		gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKREADY);
		gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

		if (uiDelayTick != DELAYWAITFEV) {
			gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;
			NIX_TaskAddToDelayTab(pstrNode);
			gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
		}

		gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKDELAY;

		(void) NIX_IntUnLock();
	} else {
		gpstrCurTcb->strTaskOpt.uiDelayTick = RTN_SUCD;
	}

	NIX_TaskSwiSched();

	return gpstrCurTcb->strTaskOpt.uiDelayTick;
}

/**********************************************/
//��������:���Ѵ���delay״̬������
//�������:pstrTcb: �����������TCBָ��
//����ֵ  :RTN_SUCD:�����ӳٳɹ�
//         RTN_FAIL:�����ӳ�ʧ��
//         RTN_TKDLTO:�����ӳ�ʱ��ľ�����ʱ����
//         RTN_TKDLBK:�����ӳ�״̬���жϣ����񷵻�
/**********************************************/
U32 NIX_TaskWake(NIX_TCB * pstrTcb)
{
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (pstrTcb == NULL) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

	if ((pstrTcb->strTaskOpt.ucTaskSta & TASKDELAY) != TASKDELAY) {
		(void) NIX_IntUnLock();
		return RTN_FAIL;
	}



	pstrNode = &pstrTcb->strTcbQue.strQueHead;

	if (pstrTcb->strTaskOpt.uiDelayTick != DELAYWAITFEV) {
		(void) NIX_ListCurNodeDelete(&gstrDelayTab, pstrNode);
		pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
	}

	pstrTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKDELAY);

	pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLBK;

	ucTaskPrio = pstrTcb->ucTaskPrio;
	pstrList = &gstrReadyTab.astrList[ucTaskPrio];
	pstrPrioFlag = &gstrReadyTab.strFlag;

	NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

	pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

	(void) NIX_IntUnLock();

	NIX_TaskSwiSched();

	return RTN_SUCD;

}

/**********************************************/
//��������:�������pend̬
//�������:pstrSem:����������ź���ָ��
//         uiDelayTick:�ӳ�ʱ��
//����ֵ  :SUCD:����ɹ�pend    FAIL:ʧ��
/**********************************************/
U32 NIX_TaskPend(NIX_SEM * pstrSem, U32 uiDelayTick)
{
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (gpstrCurTcb == gpstrIdleTaskTcb) {
		return RTN_FAIL;
	}

	ucTaskPrio = gpstrCurTcb->ucTaskPrio;
	pstrList = &gstrReadyTab.astrList[ucTaskPrio];
	pstrPrioFlag = &gstrReadyTab.strFlag;

	pstrNode = NIX_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
	gpstrCurTcb->strTaskOpt.ucTaskSta &= ~((U8) TASKREADY);

	gpstrCurTcb->strTaskOpt.uiDelayTick = uiDelayTick;

	if (uiDelayTick != SEMWAITFEV) {
		gpstrCurTcb->uiStillTick = guiTick + uiDelayTick;
		NIX_TaskAddToDelayTab(pstrNode);
		gpstrCurTcb->uiTaskFlag |= DELAYQUEFLAG;
	}

	NIX_TaskAddToSemTab(gpstrCurTcb, pstrSem);

	gpstrCurTcb->strTaskOpt.ucTaskSta |= TASKPEND;

	return RTN_SUCD;
}


#ifdef NIX_INCLUDETASKHOOK

/**********************************************/
//��������:��ʼ�����Ӻ���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskHookInit(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
	gvfTaskSwitchHook = (VFHSWT) NULL;
	gvfTaskDeleteHook = (VFHDLT) NULL;
}

/**********************************************/
//��������:������񴴽����Ӻ���
//�������:vfFuncPointer: ��ӵĹ��Ӻ���
//����ֵ  :none
/**********************************************/
void NIX_TaskCreateHookAdd(VFHCRT vfFuncPointer)
{
	gvfTaskCreateHook = vfFuncPointer;
}

/**********************************************/
//��������:ɾ�����񴴽����Ӻ���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskCreateHookDel(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
}

/**********************************************/
//��������:��������л����Ӻ���
//�������:vfFuncPointer: ��ӵĹ��Ӻ���
//����ֵ  :none
/**********************************************/
void NIX_TaskSwitchHookAdd(VFHSWT vfFuncPointer)
{
	gvfTaskSwitchHook = vfFuncPointer;
}

/**********************************************/
//��������:ɾ�������л����Ӻ���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskSwitchHookDel(void)
{
	gvfTaskSwitchHook = (VFHSWT) NULL;
}

/**********************************************/
//��������:�������ɾ�����Ӻ���
//�������:vfFuncPointer: ��ӵĹ��Ӻ���
//����ֵ  :none
/**********************************************/
void NIX_TaskDeleteHookAdd(VFHDLT vfFuncPointer)
{
	gvfTaskDeleteHook = vfFuncPointer;
}

/**********************************************/
//��������:ɾ�������л����Ӻ���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskDeleteHookDel(void)
{
	gvfTaskDeleteHook = (VFHDLT) NULL;
}



#endif
