#include "nix_inner.h"

U32 guiSystemStatus;		//ÏµÍ³×´Ì¬

NIX_TASKSCHEDTAB gstrReadyTab;	//ÈÎÎñ¾ÍÐ÷±í
NIX_LIST gstrDelayTab;		//ÈÎÎñÑÓ³Ù±í

U32 guiTick;			//²Ù×÷ÏµÍ³tick¼ÆÊý
U8 gucTickSched;		//tickµ÷¶È±êÖ¾

STACKREG *gpstrCurTaskReg;	//µ±Ç°ÈÎÎñµÄ¼Ä´æÆ÷×éÖ¸Õë
STACKREG *gpstrNextTaskReg;	//ÏÂÒ»¸öÈÎÎñµÄ¼Ä´æÆ÷×éÖ¸Õë

NIX_TCB *gpstrCurTcb;		//µ±Ç°ÈÎÎñµÄTCBÖ¸Õë
NIX_TCB *gpstrRootTaskTcb;	//Ç°¸ùÈÎÎñµÄTCBÖ¸Õë
NIX_TCB *gpstrIdleTaskTcb;	//IdleÈÎÎñµÄTCBÖ¸Õë

U32 guiUser;			//²Ù×÷ÏµÍ³ËùÊ¹ÓÃµÄÓÃ»§×´Ì¬

//ÓÅÏÈ¼¶·´Ïò²éÕÒ±í
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
//º¯Êý¹¦ÄÜ:Ö÷º¯Êý£¬·ÇOSÌ¬ÇÐ»»µ½OSÌ¬ROOTÈÎÎñÔËÐÐ
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :²»»áÔËÐÐµ½·µ»Ø
/**********************************************/
S32 main(void)
{
	//³õÊ¼»¯ÏµÍ³±äÁ¿£¬½¨Á¢²Ù×÷ÏµÍ³Æô¶¯ËùÐèÒªµÄ»·¾³
	NIX_SystemVarInit();
	//ÔËÐÐ¸ùÈÎÎñ
	NIX_TaskStart(gpstrRootTaskTcb);
	return 0;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:³õÊ¼»¯ÏµÍ³±äÁ¿
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_SystemVarInit(void)
{
	NIX_SetUser(USERROOT);	//ÉèÖÃÎª¸ß¼¶ÓÃ»§È¨ÏÞ
	guiSystemStatus = SYSTEMNOTSCHEDULE;	//³õÊ¼»¯ÎªÎ´½øÈë²Ù×÷ÏµÍ³×´Ì¬

	//½«µ±Ç°ÈÎÎñ¡¢rootÈÎÎñºÍidleÈÎÎñµÄTCB³õÊ¼»¯ÎªNULL
	gpstrCurTcb = (NIX_TCB *) NULL;
	gpstrRootTaskTcb = (NIX_TCB *) NULL;
	gpstrIdleTaskTcb = (NIX_TCB *) NULL;

	//tick¼ÆÊý³õÊ¼»¯
	guiTick = 0;

	//³õÊ¼»¯Îª·ÇtickÖÐ¶Ïµ÷¶È×´Ì¬
	gucTickSched = TICKSCHEDCLR;

	//³õÊ¼»¯ËøÖÐ¶Ï¼ÆÊýÎª0
	guiIntLockCounter = 0;

#ifdef NIX_INCLUDETASKHOOK
	NIX_TaskHookInit();
#endif

	//³õÊ¼»¯ÈÎÎñreadyµ÷¶È±í
	NIX_TaskSchedTabInit(&gstrReadyTab);

	//³õÊ¼»¯ÈÎÎñdelayµ÷¶È±í
	NIX_ListInit(&gstrDelayTab);

	//´´½¨Ç°¸ùÈÎÎñ
	gpstrRootTaskTcb =
	    NIX_TaskCreat(ROOTTASKNAME, NIX_BeforeRootTask, NULL, NULL, ROOTTASKSTACK, USERHIGHESTPRIO, NULL);

	//´´½¨IdleÈÎÎñ
	gpstrIdleTaskTcb = NIX_TaskCreat(IDLETASKNAME, NIX_IdleTask, NULL, NULL, IDLETASKSTACK, LOWESTPRIO, NULL);

}

/**********************************************/
//º¯Êý¹¦ÄÜ:Ç°¸ùÈÎÎñ£¬³õÊ¼»¯Ó²¼þ£¬ÉèÖÃÓÃ»§È¨ÏÞ£¬µ÷ÓÃ¸ùÈÎÎñ
//ÊäÈë²ÎÊý:pvPara:Èë¿Ú²ÎÊý
//·µ»ØÖµ  :none
/**********************************************/
void NIX_BeforeRootTask(void *pvPara)
{
	NIX_SystemHardwareInit();
	guiSystemStatus = SYSTEMSCHEDULE;
	NIX_SetUser(USERGUEST);
	NIX_RootTask();
}

/*************************************************************************************/
/**************************************»ñÈ¡ÏµÍ³×´Ì¬***********************************/
/*************************************************************************************/

/**********************************************/
//º¯Êý¹¦ÄÜ:»ñÈ¡µ±Ç°µÄtick
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :ÏµÍ³µ±Ç°µÄtick
/**********************************************/
U32 NIX_GetSystemTick(void)
{
	return guiTick;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:»ñÈ¡µ±Ç°ÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :ÏµÍ³µ±Ç°µÄTCBÖ¸Õë
/**********************************************/
NIX_TCB *NIX_GetCurrentTcb(void)
{
	return gpstrCurTcb;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:»ñÈ¡¸ùÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :¸ùÈÎÎñµÄTCBÖ¸Õë
/**********************************************/
NIX_TCB *NIX_GetRootTcb(void)
{
	return gpstrRootTaskTcb;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:»ñÈ¡¿ÕÏÐÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :¿ÕÏÐÈÎÎñµÄTCBÖ¸Õë
/**********************************************/
NIX_TCB *NIX_GetIdleTcb(void)
{
	return gpstrIdleTaskTcb;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:ÉèÖÃÓÃ»§
//ÊäÈë²ÎÊý:uiUser: ÐèÒªÉèÖÃµÄÓÃ»§
//·µ»ØÖµ  :none
/**********************************************/
void NIX_SetUser(U32 uiUser)
{
	guiUser = uiUser;
}

/**********************************************/
//º¯Êý¹¦ÄÜ:»ñÈ¡ÓÃ»§
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :»ñÈ¡µ½µÄÓÃ»§
/**********************************************/
U32 NIX_GetUser(void)
{
	return guiUser;
}

/*************************************************************************************/
/**************************************ÖÐ¶Ïµ÷¶ÈÏà¹Ø***********************************/
/*************************************************************************************/

/**********************************************/
//º¯Êý¹¦ÄÜ:tickÖÐ¶Ïµ÷¶ÈÈÎÎñº¯Êý
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskTick(void)
{
	guiTick++;
	gucTickSched = TICKSCHEDSET;
	NIX_IntPendSvSet();
}

/**********************************************/
//º¯Êý¹¦ÄÜ:Îª·¢ÉúµÄÈÎÎñÇÐ»»×ö×¼±¸£¬¸üÐÂ»ã±àº¯ÊýËùÊ¹ÓÃµÄ±äÁ¿
//ÊäÈë²ÎÊý:pstrTcb: ¼´½«ÔËÐÐµÄÈÎÎñTCBÖ¸Õë
//·µ»ØÖµ  :none
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
//º¯Êý¹¦ÄÜ:´Ó·Ç²Ù×÷ÏµÍ³×´Ì¬ÇÐ»»µ½²Ù×÷ÏµÍ³×´Ì¬
//ÊäÈë²ÎÊý:pstrTcb: ¼´½«ÔËÐÐµÄÈÎÎñTCBÖ¸Õë
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskStart(NIX_TCB * pstrTcb)
{
	gpstrNextTaskReg = &pstrTcb->strStackReg;
	gpstrCurTcb = pstrTcb;
	NIX_SwitchToTask();
}

/**********************************************/
//º¯Êý¹¦ÄÜ:ÈÎÎñµ÷¶È£¬ÊµÏÖ×´Ì¬ÇÐ»»
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :none
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
/**************************************ready±íÏà¹Ø************************************/
/*************************************************************************************/

/**********************************************/
//º¯Êý¹¦ÄÜ:³õÊ¼»¯µ÷¶È±í
//ÊäÈë²ÎÊý:pstrSchedTab:µ÷¶È±íÖ¸Õë
//·µ»ØÖµ  :none
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
//º¯Êý¹¦ÄÜ:½«ÈÎÎñ°´ÕÕÓÅÏÈ¼¶Ìí¼Óµ½µ÷¶È±í
//ÊäÈë²ÎÊý:pstrList:µ÷¶È±íÖ¸Õë
//         pstrNode:´ýÌí¼ÓµÄ½ÚµãÖ¸Õë
//         pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//         ucTaskPrio:´ýÌí¼ÓµÄ½ÚµãµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskAddToSchedTab(NIX_LIST * pstrList, NIX_LIST * pstrNode, NIX_PRIOFLAG * pstrPrioFlag, U8 ucTaskPrio)
{
	NIX_ListNodeAdd(pstrList, pstrNode);
	NIX_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/**********************************************/
//º¯Êý¹¦ÄÜ:´Óµ÷¶È±íÖÐÉ¾³ýÈÎÎñ
//ÊäÈë²ÎÊý:pstrList:µ÷¶È±íÖ¸Õë
//         pstrPrioFlag:µ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶Ö¸Õë
//         ucTaskPrio:ÒªÉ¾³ýÈÎÎñµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :±»É¾³ýµÄÈÎÎñµÄ½ÚµãÖ¸Õë
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
//º¯Êý¹¦ÄÜ:¶Ôready±í½øÐÐµ÷¶È
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :¼´½«ÔËÐÐµÄÈÎÎñTcbÖ¸Õë
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
/**************************************delay±íÏà¹Ø************************************/
/*************************************************************************************/
//delay±íÊÇ°´ÕÕµÈ´ýÊ±¼äÅÅÁÐµÄµ¥Ò»¶ÓÁÐ
//Òò´Ëdelay±íµÄ´´½¨ºÍdelay±í½ÚµãµÄÉ¾³ý¼´ÎªlistµÄ³õÊ¼»¯ºÍ½ÚµãÉ¾³ý
/**********************************************/
//º¯Êý¹¦ÄÜ:½«ÈÎÎñ°´ÕÕÓÅÏÈ¼¶Ìí¼Óµ½Delay±í
//ÊäÈë²ÎÊý:pstrNode:´ýÌí¼ÓµÄ½ÚµãÖ¸Õë
//·µ»ØÖµ  :none
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
			//½ÓÏÂÀ´µÄÂß¼­ÔÝÊ±Ã»ÓÐÍ¨¹ýÑéÖ¤
			//ÑéÖ¤Ê±Çë³ä·Ö¿¼ÂÇÊý¾ÝÒç³öµÄÇé¿ö£
			//Î´À´¿¼ÂÇ²Î¿¼freertosµÄÊµÏÖ
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
//º¯Êý¹¦ÄÜ:¶Ôdelay±í½øÐÐµ÷¶È
//ÊäÈë²ÎÊý:none
//·µ»ØÖµ  :none
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
/**************************************sem±íÏà¹Ø************************************/
/*************************************************************************************/
//³õÊ¼»¯£¬Ìí¼Ó£¬É¾³ý£¬µ÷¶È
/**********************************************/
//º¯Êý¹¦ÄÜ:½«ÈÎÎñÌí¼Óµ½sem±í
//ÊäÈë²ÎÊý:pstrTcb:´ýÌí¼ÓµÄ½ÚµãÖ¸Õë
//         pstrSem:Ìí¼Óµ½µÄsem±íÖ¸Õë
//·µ»ØÖµ  :none
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
//º¯Êý¹¦ÄÜ:½«ÈÎÎñ´Ósem±íÖÐÉ¾³ý
//ÊäÈë²ÎÊý:pstrTcb:´ýÉ¾³ýµÄ½ÚµãÖ¸Õë
//·µ»ØÖµ  :É¾³ýÈÎÎñµÄTCBÖ¸Õë
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
//º¯Êý¹¦ÄÜ:µ÷¶Èsem±í
//ÊäÈë²ÎÊý:pstrSem:´ýµ÷¶ÈµÄsem±íÖ¸Õë
//·µ»ØÖµ  :²éÕÒµ½µÄÈÎÎñTCBÖ¸Õë
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
/**********************************ÓÅÏÈ¼¶±êÊ¶±íÏà¹Ø***********************************/
/*************************************************************************************/

/**********************************************/
//º¯Êý¹¦ÄÜ:½«ÈÎÎñµÄÓÅÏÈ¼¶Ìí¼Óµ½ÓÅÏÈ¼¶±êÊ¶±í
//ÊäÈë²ÎÊý:pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//         ucTaskPrio:´ýÌí¼ÓµÄ½ÚµãµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
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
//º¯Êý¹¦ÄÜ:½«¸ÃÈÎÎñ´ÓÈÎÎñµ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶ÖÐÇå³ý
//ÊäÈë²ÎÊý:pstrPrioFlag:µ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶Ö¸Õë
//         ucTaskPrio:ÈÎÎñµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
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
//º¯Êý¹¦ÄÜ:»ñÈ¡ÓÅÏÈ¼¶±êÊ¶±íÖÐµÄ×î¸ßÓÅÏÈ¼¶
//ÊäÈë²ÎÊý:pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//·µ»ØÖµ  :²éÕÒµ½µÄ×î¸ßÓÅÏÈ¼¶
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
