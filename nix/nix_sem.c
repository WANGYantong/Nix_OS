#include <stdlib.h>
#include "nix_inner.h"

/**********************************************/
//��������:�����ź���
//�������:pstrSem:������ź������ڴ��ַ
//         uiSemOpt:�ź������ȷ�ʽ:FIFO or PRIO
//         uiInitVal:�ź�����ʼֵ
//����ֵ  :�������ź���ָ��
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
//��������:�ͷ��ź���
//�������:pstrSem:���ͷŵ��ź���ָ��
//����ֵ  :RTN_SUCD:�ͷųɹ�       RTN_FAIL:�ͷ�ʧ��
/**********************************************/
U32 NIX_SemGive(NIX_SEM * pstrSem)
{
#define NOTCHECKPENDTASK        0	//������������
#define CHECKPENDTASK           1	//����������
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
//��������:�ͷű��ź�����������������
//�������:pstrSem:����������ź���ָ��
//         uiRtnValue:ָ������������ķ���ֵ
//����ֵ  :RTN_SUCD:�ͷųɹ�       RTN_FAIL:�ͷ�ʧ��
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

    //�������ź��������������ܱ������ͷ�
    //��Ϊ�����ź������������
    //ֻ�ܱ�pstrSem->pstrSemTask�����������ͨ��SemGive�����ͷ�
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
