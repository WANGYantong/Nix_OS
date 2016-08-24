#include <stdlib.h>
#include "nix_inner.h"

#ifdef NIX_INCLUDETASKHOOK
VFHCRT gvfTaskCreateHook;	//任务创建钩子变量
VFHSWT gvfTaskSwitchHook;	//任务切换钩子变量
VFHDLT gvfTaskDeleteHook;	//任务删除钩子变量
#endif

/**********************************************/
//函数功能:创建新任务
//输入参数:pucTaskName:指向任务名称的指针
//         vfFuncPointer:  新任务的执行函数
//         pvPara: 入口参数
//         pucTaskStack: 任务栈的最低起始地址
//         uiStackSize: 栈大小，单位字节
//         ucTaskPrio: 任务优先级
//         pstrTaskOpt:任务参数指针
//返回值  :NULL: 任务创建失败
//         其他: 任务TCB指针
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
//函数功能:删除一个任务
//输入参数:pstrTcb:需要删除的任务的TCB指针
//返回值  :RTN_SUCD: 任务删除成功
//         RTN_FAIL: 任务删除失败
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
//函数功能:删除任务自身
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskSelfDelete(void)
{
	(void) NIX_TaskDelete(gpstrCurTcb);
}

/**********************************************/
//函数功能:初始化任务TCB
//输入参数:pucTaskName: 指向任务名称的指针
//         vfFuncPointer:  新任务的执行函数
//         pvPara: 入口参数
//         pucTaskStack: 任务栈的最低起始地址
//         uiStackSize: 栈大小，单位字节
//         ucTaskPrio: 任务优先级
//         pstrTaskOpt:任务参数指针
//返回值  :NULL: 任务创建失败
//         其他: 任务TCB指针
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
//函数功能:任务进入delay态，延迟指定的时间。调用此函数后产生任务切换
//输入参数:uiDelayTick:任务需要延迟的时间，单位:tick.
//                     DELAYNOWAIT:不等待，仅发生任务切换
//                     DELAYWAITFEV:任务一直等待
//                     其他:任务需要延迟的tick数目
//返回值  :RTN_SUCD:任务延迟成功
//         RTN_FAIL:任务延迟失败
//         RTN_TKDLTO:任务延迟时间耗尽，超时返回
//         RTN_TKDLBK:任务延迟状态被中断，任务返回
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
//函数功能:唤醒处于delay状态的任务
//输入参数:pstrTcb: 被唤醒任务的TCB指针
//返回值  :RTN_SUCD:任务延迟成功
//         RTN_FAIL:任务延迟失败
//         RTN_TKDLTO:任务延迟时间耗尽，超时返回
//         RTN_TKDLBK:任务延迟状态被中断，任务返回
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
//函数功能:任务进入pend态
//输入参数:pstrSem:阻塞任务的信号量指针
//         uiDelayTick:延迟时间
//返回值  :SUCD:任务成功pend    FAIL:失败
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
//函数功能:初始化钩子函数
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskHookInit(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
	gvfTaskSwitchHook = (VFHSWT) NULL;
	gvfTaskDeleteHook = (VFHDLT) NULL;
}

/**********************************************/
//函数功能:添加任务创建钩子函数
//输入参数:vfFuncPointer: 添加的钩子函数
//返回值  :none
/**********************************************/
void NIX_TaskCreateHookAdd(VFHCRT vfFuncPointer)
{
	gvfTaskCreateHook = vfFuncPointer;
}

/**********************************************/
//函数功能:删除任务创建钩子函数
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskCreateHookDel(void)
{
	gvfTaskCreateHook = (VFHCRT) NULL;
}

/**********************************************/
//函数功能:添加任务切换钩子函数
//输入参数:vfFuncPointer: 添加的钩子函数
//返回值  :none
/**********************************************/
void NIX_TaskSwitchHookAdd(VFHSWT vfFuncPointer)
{
	gvfTaskSwitchHook = vfFuncPointer;
}

/**********************************************/
//函数功能:删除任务切换钩子函数
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskSwitchHookDel(void)
{
	gvfTaskSwitchHook = (VFHSWT) NULL;
}

/**********************************************/
//函数功能:添加任务删除钩子函数
//输入参数:vfFuncPointer: 添加的钩子函数
//返回值  :none
/**********************************************/
void NIX_TaskDeleteHookAdd(VFHDLT vfFuncPointer)
{
	gvfTaskDeleteHook = vfFuncPointer;
}

/**********************************************/
//函数功能:删除任务切换钩子函数
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskDeleteHookDel(void)
{
	gvfTaskDeleteHook = (VFHDLT) NULL;
}



#endif
