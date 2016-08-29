#include <stdlib.h>
#include "nix_inner.h"

/**********************************************/
//函数功能:创建信号量
//输入参数:pstrSem:分配给信号量的内存地址
//         uiSemOpt:信号量调度方式:FIFO or PRIO
//         uiInitVal:信号量初始值
//返回值  :创建的信号量指针
/**********************************************/
NIX_SEM *NIX_SemCreat(NIX_SEM * pstrSem, U32 uiSemOpt, U32 uiInitVal)
{
	U8 *pucSemMemAddr;

	if ((((uiSemOpt & SEMSCHEDOPTMASK) != SEMFIFO)
	     && ((uiSemOpt & SEMSCHEDOPTMASK) != SEMPRIO))
	    || (((uiSemOpt & SEMTYPEMASK) != SEMBIN)
		&& ((uiSemOpt & SEMTYPEMASK) != SEMCNT)
		&& ((uiSemOpt & SEMTYPEMASK) != SEMMUT))) {
		return (NIX_SEM *) NULL;
	}

	if ((uiSemOpt & SEMTYPEMASK) == SEMBIN) {
		if ((uiInitVal != SEMEMPTY) && (uiInitVal != SEMFULL)) {
			return (NIX_SEM *) NULL;
		}
	} else if ((uiSemOpt & SEMTYPEMASK) == SEMMUT) {
		if (uiInitVal != SEMFULL) {
			return (NIX_SEM *) NULL;
		}
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
	pstrSem->pstrSemTask = (NIX_TCB *) NULL;

	return pstrSem;
}

/**********************************************/
//函数功能:获取信号量
//输入参数:pstrSem:待获取的信号量指针
//         uiDelayTick:等待时间
//返回值  :RTN_SUCD: 在延迟时间内获取到信号量.
//         RTN_FAIL: 获取信号量失败.
//         RTN_SMTKTO: 等待信号量的时间耗尽, 超时返回.
//         RTN_SMTKRT: 使用不等待时间参数没有获取到信号量, 函数立刻返回.
//         RTN_SMTKDL: 信号量被删除.
/**********************************************/
U32 NIX_SemTake(NIX_SEM * pstrSem, U32 uiDelayTick)
{
	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	if (NIX_RunInInt() == RTN_SUCD) {
		if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMMUT) {
			return RTN_FAIL;
		} else {
			if (uiDelayTick != SEMNOWAIT) {
				return RTN_FAIL;
			}
		}
	}

	(void) NIX_IntLock();

	if (uiDelayTick == SEMNOWAIT) {
		if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMBIN) {
			if (pstrSem->uiCounter == SEMFULL) {
				pstrSem->uiCounter = SEMEMPTY;
				(void) NIX_IntUnLock();
				return RTN_SUCD;
			} else {
				(void) NIX_IntUnLock();
				return RTN_SMTKRT;
			}
		} else if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMCNT) {
			if (pstrSem->uiCounter != SEMEMPTY) {
				pstrSem->uiCounter--;
				(void) NIX_IntUnLock();
				return RTN_SUCD;
			} else {
				(void) NIX_IntUnLock();
				return RTN_SMTKRT;
			}
		} else {
			if (pstrSem->uiCounter == SEMFULL) {
				pstrSem->uiCounter--;
				pstrSem->pstrSemTask = gpstrCurTcb;
				(void) NIX_IntUnLock();
				return RTN_SUCD;
			} else {
				if (pstrSem->pstrSemTask == gpstrCurTcb) {
					if (pstrSem->uiCounter != SEMEMPTY) {
						pstrSem->uiCounter--;
						(void) NIX_IntUnLock();
						return RTN_SUCD;
					} else {
						(void) NIX_IntUnLock();
						return RTN_SMTKOV;
					}
				} else {
					(void) NIX_IntUnLock();
					return RTN_SMTKRT;
				}
			}
		}
	} else {
		if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMBIN) {
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
		} else if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMCNT) {
			if (pstrSem->uiCounter != SEMEMPTY) {
				pstrSem->uiCounter--;
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
		} else {
			if (pstrSem->uiCounter == SEMFULL) {
				pstrSem->uiCounter--;
				pstrSem->pstrSemTask = gpstrCurTcb;
				(void) NIX_IntUnLock();
				return RTN_SUCD;
			} else {
				if (pstrSem->pstrSemTask == gpstrCurTcb) {
					if (pstrSem->uiCounter != SEMEMPTY) {
						pstrSem->uiCounter--;
						(void) NIX_IntUnLock();
						return RTN_SUCD;
					} else {
						(void) NIX_IntUnLock();
						return RTN_SMTKOV;
					}
				} else {
					if (NIX_TaskPend(pstrSem, uiDelayTick) == RTN_FAIL) {
						(void) NIX_IntUnLock();
						return RTN_FAIL;
					}

					(void) NIX_IntUnLock();

					NIX_TaskSwiSched();

					return gpstrCurTcb->strTaskOpt.uiDelayTick;
				}
			}
		}
	}
}


/**********************************************/
//函数功能:释放信号量
//输入参数:pstrSem:待释放的信号量指针
//返回值  :RTN_SUCD:释放成功       RTN_FAIL:释放失败
/**********************************************/
U32 NIX_SemGive(NIX_SEM * pstrSem)
{
#define NOTCHECKPENDTASK        0	//不检查挂起任务
#define CHECKPENDTASK           1	//检查挂起任务
	NIX_TCB *pstrTcb;
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U32 uiRtn;
	U8 ucTaskPrio;
	U8 ucTaskPendFlag;

	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

	if (NIX_RunInInt() == RTN_SUCD) {
		if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMMUT) {
			return RTN_FAIL;
		}
	}

	uiRtn = RTN_SUCD;

	(void) NIX_IntLock();

	if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMBIN) {
		if (pstrSem->uiCounter == SEMEMPTY) {
			ucTaskPendFlag = CHECKPENDTASK;
		} else {
			ucTaskPendFlag = NOTCHECKPENDTASK;
		}
	} else if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMCNT) {
		if (pstrSem->uiCounter == SEMEMPTY) {
			ucTaskPendFlag = CHECKPENDTASK;
		} else {
			ucTaskPendFlag = NOTCHECKPENDTASK;
			if (pstrSem->uiCounter != SEMFULL) {
				pstrSem->uiCounter++;
			} else {
				uiRtn = RTN_SMTKOV;
			}
		}
	} else {
		if (pstrSem->pstrSemTask != gpstrCurTcb) {
			(void) NIX_IntUnLock();
			return RTN_FAIL;
		}
		pstrSem->uiCounter++;
		if (pstrSem->uiCounter != SEMFULL) {
			ucTaskPendFlag = NOTCHECKPENDTASK;
		} else {
			ucTaskPendFlag = CHECKPENDTASK;
		}
	}
	if (ucTaskPendFlag == CHECKPENDTASK) {
		pstrTcb = NIX_SemGetActiveTask(pstrSem);

		if (pstrTcb != NULL) {
			(void) NIX_TaskDelFromSemTab(pstrTcb);

			if ((pstrTcb->uiTaskFlag & DELAYQUEFLAG) == DELAYQUEFLAG) {
				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				(void) NIX_ListCurNodeDelete(&gstrDelayTab, pstrNode);
				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
			}

			pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
			pstrTcb->strTaskOpt.uiDelayTick = RTN_SUCD;

			pstrNode = &pstrTcb->strTcbQue.strQueHead;
			ucTaskPrio = pstrTcb->ucTaskPrio;
			pstrList = &gstrReadyTab.astrList[ucTaskPrio];
			pstrPrioFlag = &gstrReadyTab.strFlag;

			NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

			pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

			if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMMUT) {
				pstrSem->pstrSemTask = pstrTcb;
				pstrSem->uiCounter--;
			}

			(void) NIX_IntUnLock();

			NIX_TaskSwiSched();

			return uiRtn;
		} else {
			if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMBIN) {
				pstrSem->uiCounter = SEMFULL;
			} else if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMCNT) {
				pstrSem->uiCounter++;
			} else {
				pstrSem->pstrSemTask = (NIX_TCB *) NULL;
			}
		}
	}

	(void) NIX_IntUnLock();

	return uiRtn;
}

/**********************************************/
//函数功能:释放被信号量阻塞的所有任务
//输入参数:pstrSem:阻塞任务的信号量指针
//         uiRtnValue:指定被阻塞任务的返回值
//返回值  :RTN_SUCD:释放成功       RTN_FAIL:释放失败
/**********************************************/
U32 NIX_SemFlushValue(NIX_SEM * pstrSem, U32 uiRtnValue)
{
	NIX_TCB *pstrTcb;
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

    //被互斥信号量阻塞的任务不能被批量释放
    //因为互斥信号量和任务相关
    //只能被pstrSem->pstrSemTask相关联的任务通过SemGive函数释放
	if ((pstrSem->uiSemOpt & SEMTYPEMASK) == SEMMUT) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

	while (1) {
		pstrTcb = NIX_SemGetActiveTask(pstrSem);

		if (pstrTcb != NULL) {
			(void) NIX_TaskDelFromSemTab(pstrTcb);

			if ((pstrTcb->uiTaskFlag & DELAYQUEFLAG) == DELAYQUEFLAG) {
				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				(void) NIX_ListCurNodeDelete(&gstrDelayTab, pstrNode);
				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));
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
	//信号量暂时用不了了，除非被谁释放掉
	pstrSem->uiCounter = SEMEMPTY;

	(void) NIX_IntUnLock();

	NIX_TaskSwiSched();

	return RTN_SUCD;
}


/**********************************************/
//函数功能:释放被信号量阻塞的所有任务
//输入参数:pstrSem:阻塞任务的信号量指针
//返回值  :RTN_SUCD:释放成功       RTN_FAIL:释放失败
/**********************************************/
U32 NIX_SemFlush(NIX_SEM * pstrSem)
{
	return NIX_SemFlushValue(pstrSem, RTN_SUCD);
}

/**********************************************/
//函数功能:释放信号量
//输入参数:pstrSem:待删除的信号量指针
//返回值  :RTN_SUCD:删除成功       RTN_FAIL删除失败
/**********************************************/
U32 NIX_SemDelete(NIX_SEM * pstrSem)
{
	if (pstrSem == NULL) {
		return RTN_FAIL;
	}

    if((pstrSem->uiSemOpt & SEMTYPEMASK) != SEMMUT) {
        if (NIX_SemFlushValue(pstrSem, RTN_SMTKDL) != RTN_SUCD) {
            return RTN_FAIL;
        }
    }

	if (pstrSem->pucSemMem != NULL) {
		(void) NIX_IntLock();

		free(pstrSem->pucSemMem);

		(void) NIX_IntUnLock();
	}

	return RTN_SUCD;
}
