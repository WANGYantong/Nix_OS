#include "nix_inner.h"

U32 guiSystemStatus;		//ϵͳ״̬

NIX_TASKSCHEDTAB gstrReadyTab;	//���������
NIX_LIST gstrDelayTab;		//�����ӳٱ�

U32 guiTick;			//����ϵͳtick����
U8 gucTickSched;		//tick���ȱ�־

STACKREG *gpstrCurTaskReg;	//��ǰ����ļĴ�����ָ��
STACKREG *gpstrNextTaskReg;	//��һ������ļĴ�����ָ��

NIX_TCB *gpstrCurTcb;		//��ǰ�����TCBָ��
NIX_TCB *gpstrRootTaskTcb;	//ǰ�������TCBָ��
NIX_TCB *gpstrIdleTaskTcb;	//Idle�����TCBָ��

U32 guiUser;			//����ϵͳ��ʹ�õ��û�״̬

//���ȼ�������ұ�
const U8 caucTaskPrioUnmapTab[256] = {
	0, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	7, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	6, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	5, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0,
	4, 0, 1, 0, 2, 0, 1, 0, 3, 0, 1, 0, 2, 0, 1, 0
};

/**********************************************/
//��������:����������OS̬�л���OS̬ROOT��������
//�������:none
//����ֵ  :�������е�����
/**********************************************/
S32 main(void)
{
	//��ʼ��ϵͳ��������������ϵͳ��������Ҫ�Ļ���
	NIX_SystemVarInit();
	//���и�����
	NIX_TaskStart(gpstrRootTaskTcb);
	return 0;
}

/**********************************************/
//��������:��ʼ��ϵͳ����
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_SystemVarInit(void)
{
	NIX_SetUser(USERROOT);	//����Ϊ�߼��û�Ȩ��
	guiSystemStatus = SYSTEMNOTSCHEDULE;	//��ʼ��Ϊδ�������ϵͳ״̬

	//����ǰ����root�����idle�����TCB��ʼ��ΪNULL
	gpstrCurTcb = (NIX_TCB *) NULL;
	gpstrRootTaskTcb = (NIX_TCB *) NULL;
	gpstrIdleTaskTcb = (NIX_TCB *) NULL;

	//tick������ʼ��
	guiTick = 0;

	//��ʼ��Ϊ��tick�жϵ���״̬
	gucTickSched = TICKSCHEDCLR;

	//��ʼ�����жϼ���Ϊ0
	guiIntLockCounter = 0;

#ifdef NIX_INCLUDETASKHOOK
	NIX_TaskHookInit();
#endif

	//��ʼ������ready���ȱ�
	NIX_TaskSchedTabInit(&gstrReadyTab);

	//��ʼ������delay���ȱ�
	NIX_ListInit(&gstrDelayTab);

	//����ǰ������
	gpstrRootTaskTcb =
	    NIX_TaskCreat(ROOTTASKNAME, NIX_BeforeRootTask, NULL, NULL, ROOTTASKSTACK, USERHIGHESTPRIO, NULL);

	//����Idle����
	gpstrIdleTaskTcb = NIX_TaskCreat(IDLETASKNAME, NIX_IdleTask, NULL, NULL, IDLETASKSTACK, LOWESTPRIO, NULL);

}

/**********************************************/
//��������:ǰ�����񣬳�ʼ��Ӳ���������û�Ȩ�ޣ����ø�����
//�������:pvPara:��ڲ���
//����ֵ  :none
/**********************************************/
void NIX_BeforeRootTask(void *pvPara)
{
	NIX_SystemHardwareInit();
	guiSystemStatus = SYSTEMSCHEDULE;
	NIX_SetUser(USERGUEST);
	NIX_RootTask();
}

/*************************************************************************************/
/**************************************��ȡϵͳ״̬***********************************/
/*************************************************************************************/

/**********************************************/
//��������:��ȡ��ǰ��tick
//�������:none
//����ֵ  :ϵͳ��ǰ��tick
/**********************************************/
U32 NIX_GetSystemTick(void)
{
	return guiTick;
}

/**********************************************/
//��������:��ȡ��ǰ�����TCBָ��
//�������:none
//����ֵ  :ϵͳ��ǰ��TCBָ��
/**********************************************/
NIX_TCB *NIX_GetCurrentTcb(void)
{
	return gpstrCurTcb;
}

/**********************************************/
//��������:��ȡ�������TCBָ��
//�������:none
//����ֵ  :�������TCBָ��
/**********************************************/
NIX_TCB *NIX_GetRootTcb(void)
{
	return gpstrRootTaskTcb;
}

/**********************************************/
//��������:��ȡ���������TCBָ��
//�������:none
//����ֵ  :���������TCBָ��
/**********************************************/
NIX_TCB *NIX_GetIdleTcb(void)
{
	return gpstrIdleTaskTcb;
}

/**********************************************/
//��������:�����û�
//�������:uiUser: ��Ҫ���õ��û�
//����ֵ  :none
/**********************************************/
void NIX_SetUser(U32 uiUser)
{
	guiUser = uiUser;
}

/**********************************************/
//��������:��ȡ�û�
//�������:none
//����ֵ  :��ȡ�����û�
/**********************************************/
U32 NIX_GetUser(void)
{
	return guiUser;
}

/*************************************************************************************/
/**************************************�жϵ������***********************************/
/*************************************************************************************/

/**********************************************/
//��������:tick�жϵ���������
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskTick(void)
{
	guiTick++;
	gucTickSched = TICKSCHEDSET;
	NIX_IntPendSvSet();
}

/**********************************************/
//��������:Ϊ�����������л���׼�������»�ຯ����ʹ�õı���
//�������:pstrTcb: �������е�����TCBָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskSwitch(NIX_TCB * pstrTcb)
{
	if (gpstrCurTcb != NULL) {
		gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
	} else {
		gpstrCurTaskReg = NULL;
	}
	gpstrNextTaskReg = &pstrTcb->strStackReg;
	gpstrCurTcb = pstrTcb;
}

/**********************************************/
//��������:�ӷǲ���ϵͳ״̬�л�������ϵͳ״̬
//�������:pstrTcb: �������е�����TCBָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskStart(NIX_TCB * pstrTcb)
{
	gpstrNextTaskReg = &pstrTcb->strStackReg;
	gpstrCurTcb = pstrTcb;
	NIX_SwitchToTask();
}

/**********************************************/
//��������:������ȣ�ʵ��״̬�л�
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskSched(void)
{
	NIX_TCB *pstrTcb;

	if (gucTickSched == TICKSCHEDSET) {
		gucTickSched = TICKSCHEDCLR;
		NIX_TaskDelayTabSched();
	}
	pstrTcb = NIX_TaskReadyTabSched();

#ifdef NIX_INCLUDETASKHOOK
	if (gvfTaskSwitchHook != (VFHSWT) NULL) {
		gvfTaskSwitchHook(gpstrCurTcb, pstrTcb);
	}
#endif

	NIX_TaskSwitch(pstrTcb);
}

/*************************************************************************************/
/**************************************ready�����************************************/
/*************************************************************************************/

/**********************************************/
//��������:��ʼ�����ȱ�
//�������:pstrSchedTab:���ȱ�ָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskSchedTabInit(NIX_TASKSCHEDTAB * pstrSchedTab)
{
	U32 i;
	for (i = 0; i < PRIORITYNUM; i++) {
		NIX_ListInit(&pstrSchedTab->astrList[i]);
	}
#if PRIORITYNUM >= PRIORITY128
	for (i = 0; i < PRIOFLAGGRP1; i++) {
		pstrSchedTab->strFlag.aucPrioFlagGRP1[i] = 0;
	}
	for (i = 0; i < PRIOFLAGGRP2; i++) {
		pstrSchedTab->strFlag.aucPrioFlagGRP2[i] = 0;
	}
	pstrSchedTab->strFlag.ucPrioFlagGrp3 = 0;
#elif PRIORITYNUM >= PRIORITY16
	for (i = 0; i < PRIOFLAGGRP1; i++) {
		pstrSchedTab->strFlag.aucPrioFlagGRP1[i] = 0;
	}
	pstrSchedTab->strFlag.ucPrioFlagGRP2 = 0;
#else
	pstrSchedTab->strFlag.ucPrioFlagGRP1 = 0;
#endif
}

/**********************************************/
//��������:�����������ȼ���ӵ����ȱ�
//�������:pstrList:���ȱ�ָ��
//         pstrNode:����ӵĽڵ�ָ��
//         pstrPrioFlag:���ȼ���ʶ��
//         ucTaskPrio:����ӵĽڵ�����ȼ�
//����ֵ  :none
/**********************************************/
void NIX_TaskAddToSchedTab(NIX_LIST * pstrList, NIX_LIST * pstrNode, NIX_PRIOFLAG * pstrPrioFlag, U8 ucTaskPrio)
{
	NIX_ListNodeAdd(pstrList, pstrNode);
	NIX_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/**********************************************/
//��������:�ӵ��ȱ���ɾ������
//�������:pstrList:���ȱ�ָ��
//         pstrPrioFlag:���ȱ��Ӧ�����ȼ���ʶָ��
//         ucTaskPrio:Ҫɾ����������ȼ�
//����ֵ  :��ɾ��������Ľڵ�ָ��
/**********************************************/
NIX_LIST *NIX_TaskDelFromSchedTab(NIX_LIST * pstrList, NIX_PRIOFLAG * pstrPrioFlag, U8 ucTaskPrio)
{
	NIX_LIST *pstrDelNode;
	pstrDelNode = NIX_ListNodeDelete(pstrList);
	if (NIX_ListEmpInq(pstrList) == NULL) {
		NIX_TaskClrPrioFlag(pstrPrioFlag, ucTaskPrio);
	}
	return pstrDelNode;
}

/**********************************************/
//��������:��ready����е���
//�������:none
//����ֵ  :�������е�����Tcbָ��
/**********************************************/
NIX_TCB *NIX_TaskReadyTabSched(void)
{
	NIX_TCB *pstrTcb;
	NIX_TCBQUE *pstrTaskQue;
	U8 ucTaskPrio;

	ucTaskPrio = NIX_TaskGetHighestPrio(&gstrReadyTab.strFlag);
	pstrTaskQue = (NIX_TCBQUE *)
	    NIX_ListEmpInq(&gstrReadyTab.astrList[ucTaskPrio]);
	pstrTcb = pstrTaskQue->pstrTcb;

	return pstrTcb;
}

/*************************************************************************************/
/**************************************delay�����************************************/
/*************************************************************************************/
//delay���ǰ��յȴ�ʱ�����еĵ�һ����
//���delay��Ĵ�����delay��ڵ��ɾ����Ϊlist�ĳ�ʼ���ͽڵ�ɾ��
/**********************************************/
//��������:�����������ȼ���ӵ�Delay��
//�������:pstrNode:����ӵĽڵ�ָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskAddToDelayTab(NIX_LIST * pstrNode)
{
	NIX_LIST *pstrTempNode;
	NIX_LIST *pstrNextNode;
	NIX_TCBQUE *pstrTcbQue;
	U32 uiStillTick;
	U32 uiTempStillTick;

	pstrTempNode = NIX_ListEmpInq(&gstrDelayTab);

	if (pstrTempNode != NULL) {
		pstrTcbQue = (NIX_TCBQUE *) pstrNode;
		uiStillTick = pstrTcbQue->pstrTcb->uiStillTick;

		while (1) {
			pstrTcbQue = (NIX_TCBQUE *) pstrTempNode;
			uiTempStillTick = pstrTcbQue->pstrTcb->uiStillTick;
			//���������߼���ʱû��ͨ����֤
			//��֤ʱ���ֿ����������������
			//δ�����ǲο�freertos��ʵ��
			if (uiStillTick < uiTempStillTick) {
				if (uiStillTick > guiTick) {
					NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

					return;
				} else if (uiStillTick < guiTick) {
					if (guiTick > uiTempStillTick) {
						NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

						return;
					}
				}
			} else if (uiStillTick > uiTempStillTick) {
				if (uiStillTick > guiTick) {
					if (guiTick > uiTempStillTick) {
						NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

						return;
					}
				}
			}


			pstrNextNode = NIX_ListNextNodeEmpInq(&gstrDelayTab, pstrTempNode);

			if (pstrNextNode == NULL) {
				NIX_ListNodeAdd(&gstrDelayTab, pstrNode);
				return;
			} else {
				pstrTempNode = pstrNextNode;
			}

		}
	} else {
		NIX_ListNodeAdd(&gstrDelayTab, pstrNode);
		return;
	}
}

/**********************************************/
//��������:��delay����е���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskDelayTabSched(void)
{
	NIX_TCB *pstrTcb;
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_LIST *pstrDelayNode;
	NIX_LIST *pstrNextNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	NIX_TCBQUE *pstrTcbQue;
	U32 uiTick;
	U8 ucTaskPrio;

	pstrDelayNode = NIX_ListEmpInq(&gstrDelayTab);

	if (pstrDelayNode != NULL) {
		while (1) {
			pstrTcbQue = (NIX_TCBQUE *) pstrDelayNode;
			pstrTcb = pstrTcbQue->pstrTcb;
			uiTick = pstrTcb->uiStillTick;

			if (guiTick == uiTick) {
				pstrNextNode = NIX_ListCurNodeDelete(&gstrDelayTab, pstrDelayNode);

				pstrTcb->uiTaskFlag &= (~((U32) DELAYQUEFLAG));

				if ((pstrTcb->strTaskOpt.ucTaskSta & TASKDELAY) == TASKDELAY) {
					pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKDELAY));
					pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLTO;
				} else if ((pstrTcb->strTaskOpt.ucTaskSta & TASKPEND) == TASKPEND) {
					(void)
					    NIX_TaskDelFromSemTab(pstrTcb);
					pstrTcb->strTaskOpt.ucTaskSta &= (~((U8) TASKPEND));
					pstrTcb->strTaskOpt.uiDelayTick = RTN_SMTKTO;
				}

				pstrNode = &pstrTcb->strTcbQue.strQueHead;
				ucTaskPrio = pstrTcb->ucTaskPrio;
				pstrList = &gstrReadyTab.astrList[ucTaskPrio];
				pstrPrioFlag = &gstrReadyTab.strFlag;

				NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

				pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

				if (pstrNextNode == NULL) {
					break;
				} else {
					pstrDelayNode = pstrNextNode;
				}

			} else {
				break;
			}
		}
	}

}

/*************************************************************************************/
/**************************************sem�����************************************/
/*************************************************************************************/
//��ʼ������ӣ�ɾ��������
/**********************************************/
//��������:��������ӵ�sem��
//�������:pstrTcb:����ӵĽڵ�ָ��
//         pstrSem:��ӵ���sem��ָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskAddToSemTab(NIX_TCB * pstrTcb, NIX_SEM * pstrSem)
{
	NIX_LIST *pstrList;
	NIX_LIST *pstrNode;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	if ((pstrSem->uiSemOpt & SEMSCHEDOPTMASK) == SEMPRIO) {
		ucTaskPrio = pstrTcb->ucTaskPrio;
		pstrList = &pstrSem->strSemtab.astrList[ucTaskPrio];
		pstrNode = &pstrTcb->strSemQue.strQueHead;
		pstrPrioFlag = &pstrSem->strSemtab.strFlag;

		NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);
	} else {
		pstrList = &pstrSem->strSemtab.astrList[LOWESTPRIO];
		pstrNode = &pstrTcb->strSemQue.strQueHead;

		NIX_ListNodeAdd(pstrList, pstrNode);
	}

	pstrTcb->pstrSem = pstrSem;

}

/**********************************************/
//��������:�������sem����ɾ��
//�������:pstrTcb:��ɾ���Ľڵ�ָ��
//����ֵ  :ɾ�������TCBָ��
/**********************************************/
NIX_LIST *NIX_TaskDelFromSemTab(NIX_TCB * pstrTcb)
{
	NIX_SEM *pstrSem;
	NIX_LIST *pstrList;
	NIX_PRIOFLAG *pstrPrioFlag;
	U8 ucTaskPrio;

	pstrSem = pstrTcb->pstrSem;

	if ((pstrSem->uiSemOpt & SEMSCHEDOPTMASK) == SEMPRIO) {
		ucTaskPrio = pstrTcb->ucTaskPrio;
		pstrList = &pstrSem->strSemtab.astrList[ucTaskPrio];
		pstrPrioFlag = &pstrSem->strSemtab.strFlag;

		return NIX_TaskDelFromSchedTab(pstrList, pstrPrioFlag, ucTaskPrio);
	} else {
		pstrList = &pstrSem->strSemtab.astrList[LOWESTPRIO];

		return NIX_ListNodeDelete(pstrList);
	}
}

/**********************************************/
//��������:����sem��
//�������:pstrSem:�����ȵ�sem��ָ��
//����ֵ  :���ҵ�������TCBָ��
/**********************************************/
NIX_TCB *NIX_SemGetActiveTask(NIX_SEM * pstrSem)
{
	NIX_TCBQUE *pstrTaskQue;
	NIX_LIST *pstrNode;
	U8 ucTaskPrio;

	if ((pstrSem->uiSemOpt & SEMSCHEDOPTMASK) == SEMPRIO) {
		ucTaskPrio = NIX_TaskGetHighestPrio(&pstrSem->strSemtab.strFlag);
	} else {
		ucTaskPrio = LOWESTPRIO;
	}

	pstrNode = NIX_ListEmpInq(&pstrSem->strSemtab.astrList[ucTaskPrio]);

	if (pstrNode == NULL) {
		return (NIX_TCB *) NULL;
	} else {
		pstrTaskQue = (NIX_TCBQUE *) pstrNode;

		return pstrTaskQue->pstrTcb;
	}
}

/*************************************************************************************/
/**********************************���ȼ���ʶ�����***********************************/
/*************************************************************************************/

/**********************************************/
//��������:����������ȼ���ӵ����ȼ���ʶ��
//�������:pstrPrioFlag:���ȼ���ʶ��
//         ucTaskPrio:����ӵĽڵ�����ȼ�
//����ֵ  :none
/**********************************************/
void NIX_TaskSetPrioFlag(NIX_PRIOFLAG * pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
	U8 ucPrioFlagGrp1;
	U8 ucPrioFlagGrp2;
	U8 ucPosInGrp1;
	U8 ucPosInGrp2;
	U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
	U8 ucPrioFlagGrp1;
	U8 ucPosInGrp1;
	U8 ucPosInGrp2;
#endif

#if PRIORITYNUM >= PRIORITY128
	//Byte
	ucPrioFlagGrp1 = ucTaskPrio / 8;
	ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;
	//bit
	ucPosInGrp1 = ucTaskPrio % 8;
	ucPosInGrp2 = ucPrioFlagGrp1 % 8;
	ucPosInGrp3 = ucPrioFlagGrp2;

	pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1] |= (U8) (1 << ucPosInGrp1);
	pstrPrioFlag->aucPrioFlagGRP2[ucPrioFlagGrp2] |= (U8) (1 << ucPosInGrp2);
	pstrPrioFlag->ucPrioFlagGrp3 |= (U8) (1 << ucPosInGrp3);
#elif PRIORITYNUM >= PRIORITY16
	ucPrioFlagGrp1 = ucTaskPrio / 8;

	ucPosInGrp1 = ucTaskPrio % 8;
	ucPosInGrp2 = ucPrioFlagGrp1;

	pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1] |= (U8) (1 << ucPosInGrp1);
	pstrPrioFlag->ucPrioFlagGRP2 |= (U8) (1 << ucPosInGrp2);
#else
	pstrPrioFlag->ucPrioFlagGRP1 |= (U8) (1 << ucTaskPrio);
#endif
}

/**********************************************/
//��������:���������������ȱ��Ӧ�����ȼ���ʶ�����
//�������:pstrPrioFlag:���ȱ��Ӧ�����ȼ���ʶָ��
//         ucTaskPrio:��������ȼ�
//����ֵ  :none
/**********************************************/
void NIX_TaskClrPrioFlag(NIX_PRIOFLAG * pstrPrioFlag, U8 ucTaskPrio)
{
#if PRIORITYNUM >= PRIORITY128
	U8 ucPrioFlagGrp1;
	U8 ucPrioFlagGrp2;
	U8 ucPosInGrp1;
	U8 ucPosInGrp2;
	U8 ucPosInGrp3;
#elif PRIORITYNUM >= PRIORITY16
	U8 ucPrioFlagGrp1;
	U8 ucPosInGrp1;
	U8 ucPosInGrp2;
#endif

#if PRIORITYNUM >= PRIORITY128
	//Byte
	ucPrioFlagGrp1 = ucTaskPrio / 8;
	ucPrioFlagGrp2 = ucPrioFlagGrp1 / 8;
	//bit
	ucPosInGrp1 = ucTaskPrio % 8;
	ucPosInGrp2 = ucPrioFlagGrp1 % 8;
	ucPosInGrp3 = ucPrioFlagGrp2;

	pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8) (1 << ucPosInGrp1));
	if (pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] == 0) {
		pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] &= ~((U8) (1 << ucPosInGrp2));
		if (pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] == 0) {
			pstrPrioFlag->ucPrioFlagGrp3 &= ~((U8) (1 << ucPosInGrp3));
		}
	}
#elif PRIORITYNUM >= PRIORITY16

	ucPrioFlagGrp1 = ucTaskPrio / 8;

	ucPosInGrp1 = ucTaskPrio % 8;
	ucPosInGrp2 = ucPrioFlagGrp1;

	pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8) (1 << ucPosInGrp1));
	if (pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] == 0) {
		pstrPrioFlag->ucPrioFlagGrp2 &= ~((U8) (1 << ucPosInGrp2));
	}
#else
	pstrPrioFlag->ucPrioFlagGRP1 &= ~((U8) (1 << ucTaskPrio));
#endif

}


/**********************************************/
//��������:��ȡ���ȼ���ʶ���е�������ȼ�
//�������:pstrPrioFlag:���ȼ���ʶ��
//����ֵ  :���ҵ���������ȼ�
/**********************************************/
U8 NIX_TaskGetHighestPrio(NIX_PRIOFLAG * pstrPrioFlag)
{
#if PRIORITYNUM >= PRIORITY128
	U8 ucPrioFlagGrp1;
	U8 ucPrioFlagGrp2;
	U8 ucHighestFlagInGrp1;
#elif PRIORITYNUM >= PRIORITY16
	U8 ucPrioFlagGrp1;
	U8 ucHighestFlagInGrp1;
#endif

#if PRIORITYNUM >= PRIORITY128
	ucPrioFlagGrp2 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGrp3];
	ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGRP2[ucPrioFlagGrp2]];
	ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1]];
	return (U8) ((ucPrioFlagGrp2 * 8 + ucPrioFlagGrp1) * 8 + ucHighestFlagInGrp1);
#elif PRIORITYNUM >= PRIORITY16
	ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGRP2];
	ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1]];
	return (U8) (ucPrioFlagGrp1 * 8 + ucHighestFlagInGrp1);
#else
	return caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGRP1];
#endif
}
