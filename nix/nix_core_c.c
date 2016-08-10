#include "nix_inner.h"

U32 guiSystemStatus;                //ÏµÍ³×´Ì¬

NIX_TASKSCHEDTAB gstrReadyTab;      //ÈÎÎñ¾ÍĞ÷±í
NIX_LIST gstrDelayTab;             //ÈÎÎñÑÓ³Ù±í

U32 guiTick;                        //²Ù×÷ÏµÍ³tick¼ÆÊı
U8  gucTickSched;                   //tickµ÷¶È±êÖ¾

STACKREG* gpstrCurTaskReg;          //µ±Ç°ÈÎÎñµÄ¼Ä´æÆ÷×éÖ¸Õë
STACKREG* gpstrNextTaskReg;         //ÏÂÒ»¸öÈÎÎñµÄ¼Ä´æÆ÷×éÖ¸Õë

NIX_TCB* gpstrCurTcb;               //µ±Ç°ÈÎÎñµÄTCBÖ¸Õë
NIX_TCB* gpstrRootTaskTcb;          //Ç°¸ùÈÎÎñµÄTCBÖ¸Õë
NIX_TCB* gpstrIdleTaskTcb;          //IdleÈÎÎñµÄTCBÖ¸Õë

U32 guiUser;                        //²Ù×÷ÏµÍ³ËùÊ¹ÓÃµÄÓÃ»§×´Ì¬

//ÓÅÏÈ¼¶·´Ïò²éÕÒ±í
const U8 caucTaskPrioUnmapTab[256] =
{
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
//º¯Êı¹¦ÄÜ:Ö÷º¯Êı£¬·ÇOSÌ¬ÇĞ»»µ½OSÌ¬ROOTÈÎÎñÔËĞĞ
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :²»»áÔËĞĞµ½·µ»Ø
/**********************************************/
S32 main(void)
{
    //³õÊ¼»¯ÏµÍ³±äÁ¿£¬½¨Á¢²Ù×÷ÏµÍ³Æô¶¯ËùĞèÒªµÄ»·¾³
    NIX_SystemVarInit();
    //ÔËĞĞ¸ùÈÎÎñ
    NIX_TaskStart(gpstrRootTaskTcb);
    return 0;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:³õÊ¼»¯ÏµÍ³±äÁ¿
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_SystemVarInit(void)
{
    NIX_SetUser(USERROOT);              //ÉèÖÃÎª¸ß¼¶ÓÃ»§È¨ÏŞ
    guiSystemStatus = SYSTEMNOTSCHEDULE;//³õÊ¼»¯ÎªÎ´½øÈë²Ù×÷ÏµÍ³×´Ì¬

    //½«µ±Ç°ÈÎÎñ¡¢rootÈÎÎñºÍidleÈÎÎñµÄTCB³õÊ¼»¯ÎªNULL
    gpstrCurTcb = (NIX_TCB*)NULL;
    gpstrRootTaskTcb = (NIX_TCB*)NULL;
    gpstrIdleTaskTcb = (NIX_TCB*)NULL;

    //tick¼ÆÊı³õÊ¼»¯
    guiTick = 0;

    //³õÊ¼»¯Îª·ÇtickÖĞ¶Ïµ÷¶È×´Ì¬
    gucTickSched = TICKSCHEDCLR;

    //³õÊ¼»¯ËøÖĞ¶Ï¼ÆÊıÎª0
    guiIntLockCounter = 0;

#ifdef NIX_INCLUDETASKHOOK
    NIX_TaskHookInit();
#endif

    //³õÊ¼»¯ÈÎÎñreadyµ÷¶È±í
    NIX_TaskSchedTabInit(&gstrReadyTab);

    //³õÊ¼»¯ÈÎÎñdelayµ÷¶È±í
    NIX_ListInit(&gstrDelayTab);

    //´´½¨Ç°¸ùÈÎÎñ
    gpstrRootTaskTcb = NIX_TaskCreat(ROOTTASKNAME,NIX_BeforeRootTask, NULL, NULL,
                                        ROOTTASKSTACK, USERHIGHESTPRIO, NULL);

    //´´½¨IdleÈÎÎñ
    gpstrIdleTaskTcb = NIX_TaskCreat(IDLETASKNAME,NIX_IdleTask, NULL, NULL,
                                        IDLETASKSTACK, LOWESTPRIO, NULL);

}

/**********************************************/
//º¯Êı¹¦ÄÜ:Ç°¸ùÈÎÎñ£¬³õÊ¼»¯Ó²¼ş£¬ÉèÖÃÓÃ»§È¨ÏŞ£¬µ÷ÓÃ¸ùÈÎÎñ
//ÊäÈë²ÎÊı:pvPara:Èë¿Ú²ÎÊı
//·µ»ØÖµ  :none
/**********************************************/
void NIX_BeforeRootTask(void* pvPara)
{
    NIX_SystemHardwareInit();
    guiSystemStatus = SYSTEMSCHEDULE;
    NIX_SetUser(USERGUEST);
    NIX_RootTask();
}

/**********************************************/
//º¯Êı¹¦ÄÜ:tickÖĞ¶Ïµ÷¶ÈÈÎÎñº¯Êı
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskTick(void)
{
    guiTick++;
    gucTickSched = TICKSCHEDSET;
    NIX_IntPendSvSet();
}

/**********************************************/
//º¯Êı¹¦ÄÜ:Îª·¢ÉúµÄÈÎÎñÇĞ»»×ö×¼±¸£¬¸üĞÂ»ã±àº¯ÊıËùÊ¹ÓÃµÄ±äÁ¿
//ÊäÈë²ÎÊı:pstrTcb: ¼´½«ÔËĞĞµÄÈÎÎñTCBÖ¸Õë
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskSwitch(NIX_TCB* pstrTcb)
{
    if(gpstrCurTcb != NULL)
        {
            gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
    }
    else
        {
            gpstrCurTaskReg = NULL;
    }
    gpstrNextTaskReg = &pstrTcb->strStackReg;
    gpstrCurTcb = pstrTcb;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:´Ó·Ç²Ù×÷ÏµÍ³×´Ì¬ÇĞ»»µ½²Ù×÷ÏµÍ³×´Ì¬
//ÊäÈë²ÎÊı:pstrTcb: ¼´½«ÔËĞĞµÄÈÎÎñTCBÖ¸Õë
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskStart(NIX_TCB* pstrTcb)
{
    gpstrNextTaskReg = &pstrTcb->strStackReg;
    gpstrCurTcb = pstrTcb;
    NIX_SwitchToTask();
}

/**********************************************/
//º¯Êı¹¦ÄÜ:³õÊ¼»¯ÈÎÎñµ÷¶È±í
//ÊäÈë²ÎÊı:pstrSchedTab:ÈÎÎñµ÷¶È±íÖ¸Õë
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskSchedTabInit(NIX_TASKSCHEDTAB* pstrSchedTab)
{
    U32 i;
    for(i=0;i < PRIORITYNUM;i++)
        {
            NIX_ListInit(&pstrSchedTab->astrList[i]);
    }
    #if PRIORITYNUM >= PRIORITY128
        for(i=0;i < PRIOFLAGGRP1; i++)
            {
                pstrSchedTab->strFlag.aucPrioFlagGRP1[i] = 0;
        }
        for(i=0;i < PRIOFLAGGRP2; i++)
            {
                pstrSchedTab->strFlag.aucPrioFlagGRP2[i] = 0;
        }
        pstrSchedTab->strFlag.ucPrioFlagGrp3 = 0;
    #elif PRIORITYNUM >= PRIORITY16
        for(i=0;i < PRIOFLAGGRP1; i++)
            {
                pstrSchedTab->strFlag.aucPrioFlagGRP1[i] = 0;
        }
        pstrSchedTab->strFlag.ucPrioFlagGRP2 = 0;
    #else
        pstrSchedTab->strFlag.ucPrioFlagGRP1 = 0;
    #endif
}

/**********************************************/
//º¯Êı¹¦ÄÜ:½«ÈÎÎñ°´ÕÕÓÅÏÈ¼¶Ìí¼Óµ½ÈÎÎñµ÷¶È±í
//ÊäÈë²ÎÊı:pstrList:µ÷¶È±íÖ¸Õë
//         pstrNode:´ıÌí¼ÓµÄ½ÚµãÖ¸Õë
//         pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//         ucTaskPrio:´ıÌí¼ÓµÄ½ÚµãµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskAddToSchedTab(NIX_LIST* pstrList, NIX_LIST* pstrNode,
                           NIX_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
    NIX_ListNodeAdd(pstrList, pstrNode);
    NIX_TaskSetPrioFlag(pstrPrioFlag, ucTaskPrio);
}

/**********************************************/
//º¯Êı¹¦ÄÜ:½«ÈÎÎñ°´ÕÕÓÅÏÈ¼¶Ìí¼Óµ½Delay±í
//ÊäÈë²ÎÊı:pstrNode:´ıÌí¼ÓµÄ½ÚµãÖ¸Õë
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskAddToDelayTab(NIX_LIST* pstrNode)
{
    NIX_LIST* pstrTempNode;
    NIX_LIST* pstrNextNode;
    NIX_TCBQUE* pstrTcbQue;
    U32 uiStillTick;
    U32 uiTempStillTick;

    pstrTempNode = NIX_ListEmpInq(&gstrDelayTab);

    if(pstrTempNode != NULL)
        {
            pstrTcbQue = (NIX_TCBQUE*)pstrNode;
            uiStillTick = pstrTcbQue->pstrTcb->uiStillTick;

            while(1)
                {
                    pstrTcbQue = (NIX_TCBQUE*)pstrTempNode;
                    uiTempStillTick = pstrTcbQue->pstrTcb->uiStillTick;
                    //½ÓÏÂÀ´µÄÂß¼­ÔİÊ±Ã»ÓĞÍ¨¹ıÑéÖ¤
                    //ÑéÖ¤Ê±Çë³ä·Ö¿¼ÂÇÊı¾İÒç³öµÄÇé¿ö£
                    //Î´À´¿¼ÂÇ²Î¿¼freertosµÄÊµÏÖ
                    if(uiStillTick < uiTempStillTick)
                               {
                                   if(uiStillTick > guiTick)
                                   {
                                       NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode, pstrNode);

                                       return;
                                   }
                                   else if(uiStillTick < guiTick)
                                   {
                                       if(guiTick > uiTempStillTick)
                                       {
                                           NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode,
                                                                  pstrNode);

                                           return;
                                       }
                                   }
                               }
                               else if(uiStillTick > uiTempStillTick)
                               {
                                   if(uiStillTick > guiTick)
                                   {
                                       if(guiTick > uiTempStillTick)
                                       {
                                           NIX_ListNodeInsert(&gstrDelayTab, pstrTempNode,
                                                                  pstrNode);

                                           return;
                                       }
                                   }
                               }


                    pstrNextNode = NIX_ListNextNodeEmpInq(&gstrDelayTab,pstrTempNode);

                    if(pstrNextNode == NULL)
                        {
                            NIX_ListNodeAdd(&gstrDelayTab, pstrNode);
                            return;
                    }
                    else
                        {
                            pstrTempNode = pstrNextNode;
                    }

								}
				}
    else
        {
            NIX_ListNodeAdd(&gstrDelayTab, pstrNode);
            return;
    }


}

/**********************************************/
//º¯Êı¹¦ÄÜ:´ÓÈÎÎñµ÷¶È±íÖĞÉ¾³ıÈÎÎñ
//ÊäÈë²ÎÊı:pstrList:ÈÎÎñµ÷¶È±íÖ¸Õë
//         pstrPrioFlag:µ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶Ö¸Õë
//         ucTaskPrio:ÒªÉ¾³ıÈÎÎñµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :±»É¾³ıµÄÈÎÎñµÄ½ÚµãÖ¸Õë
/**********************************************/
NIX_LIST* NIX_TaskDelFromSchedTab(NIX_LIST* pstrList, NIX_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
{
    NIX_LIST* pstrDelNode;
    pstrDelNode = NIX_ListNodeDelete(pstrList);
    if(NIX_ListEmpInq(pstrList) == NULL)
        {
            NIX_TaskClrPrioFlag(pstrPrioFlag, ucTaskPrio);
    }
    return pstrDelNode;
}


/**********************************************/
//º¯Êı¹¦ÄÜ:ÈÎÎñµ÷¶È£¬ÊµÏÖ×´Ì¬ÇĞ»»
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskSched(void)
{
    NIX_TCB* pstrTcb;

    if(gucTickSched == TICKSCHEDSET)
        {
            gucTickSched = TICKSCHEDCLR;
            NIX_TaskDelayTabSched();
    }
    pstrTcb = NIX_TaskReadyTabSched();

#ifdef NIX_INCLUDETASKHOOK
    if(gvfTaskSwitchHook != (VFHSWT)NULL)
        {
            gvfTaskSwitchHook(gpstrCurTcb, pstrTcb);
    }
#endif

    NIX_TaskSwitch(pstrTcb);
}

/**********************************************/
//º¯Êı¹¦ÄÜ:¶Ôready±í½øĞĞµ÷¶È
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :¼´½«ÔËĞĞµÄÈÎÎñTcbÖ¸Õë
/**********************************************/
NIX_TCB* NIX_TaskReadyTabSched(void)
{
    NIX_TCB* pstrTcb;
    NIX_TCBQUE* pstrTaskQue;
    U8 ucTaskPrio;

    ucTaskPrio = NIX_TaskGetHighestPrio(&gstrReadyTab.strFlag);
    pstrTaskQue = (NIX_TCBQUE*)NIX_ListEmpInq(&gstrReadyTab.astrList[ucTaskPrio]);
    pstrTcb = pstrTaskQue->pstrTcb;

    return pstrTcb;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:¶Ôdelay±í½øĞĞµ÷¶È
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskDelayTabSched(void)
{
    NIX_TCB* pstrTcb;
    NIX_LIST* pstrList;
    NIX_LIST* pstrNode;
    NIX_LIST* pstrDelayNode;
    NIX_LIST* pstrNextNode;
    NIX_PRIOFLAG* pstrPrioFlag;
    NIX_TCBQUE* pstrTcbQue;
    U32 uiTick;
    U8 ucTaskPrio;

    pstrDelayNode = NIX_ListEmpInq(&gstrDelayTab);

    if(pstrDelayNode != NULL)
        {
            while(1)
                {
                    pstrTcbQue = (NIX_TCBQUE*)pstrDelayNode;
                    pstrTcb = pstrTcbQue->pstrTcb;
                    uiTick = pstrTcb->uiStillTick;

                    if(guiTick == uiTick)
                        {
                            pstrNextNode = NIX_ListCurNodeDelete(&gstrDelayTab, pstrDelayNode);

                            pstrTcb->uiTaskFlag &= (~((U32)DELAYQUEFLAG));
                            pstrTcb->strTaskOpt.ucTaskSta &= (~((U8)TASKDELAY));
                            pstrTcb->strTaskOpt.uiDelayTick = RTN_TKDLTO;

                            pstrNode = &pstrTcb->strTcbQue.strQueHead;
                            ucTaskPrio = pstrTcb->ucTaskPrio;
                            pstrList = &gstrReadyTab.astrList[ucTaskPrio];
                            pstrPrioFlag = &gstrReadyTab.strFlag;

                            NIX_TaskAddToSchedTab(pstrList, pstrNode, pstrPrioFlag, ucTaskPrio);

                            pstrTcb->strTaskOpt.ucTaskSta |= TASKREADY;

                            if(pstrNextNode == NULL)
                                {
                                    break;
                            }
                            else
                                {
                                    pstrDelayNode = pstrNextNode;
                            }

                    }
                    else
                        {
                            break;
                    }
            }
    }

}

/**********************************************/
//º¯Êı¹¦ÄÜ:½«ÈÎÎñµÄÓÅÏÈ¼¶Ìí¼Óµ½ÓÅÏÈ¼¶±êÊ¶±í
//ÊäÈë²ÎÊı:pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//         ucTaskPrio:´ıÌí¼ÓµÄ½ÚµãµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskSetPrioFlag(NIX_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
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
        ucPrioFlagGrp1 = ucTaskPrio/8;
        ucPrioFlagGrp2 = ucPrioFlagGrp1/8;
    //bit
        ucPosInGrp1 = ucTaskPrio%8;
        ucPosInGrp2 = ucPrioFlagGrp1%8;
        ucPosInGrp3 = ucPrioFlagGrp2;

        pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1] |= (U8)(1<<ucPosInGrp1);
        pstrPrioFlag->aucPrioFlagGRP2[ucPrioFlagGrp2] |= (U8)(1<<ucPosInGrp2);
        pstrPrioFlag->ucPrioFlagGrp3 |= (U8)(1<<ucPosInGrp3);
     #elif PRIORITYNUM >= PRIORITY16
        ucPrioFlagGrp1 = ucTaskPrio/8;

        ucPosInGrp1 = ucTaskPrio%8;
        ucPosInGrp2 = ucPrioFlagGrp1;

        pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1] |= (U8)(1<<ucPosInGrp1);
        pstrPrioFlag->ucPrioFlagGRP2 |= (U8)(1<<ucPosInGrp2);
     #else
        pstrPrioFlag->ucPrioFlagGRP1 |= (U8)(1<<ucTaskPrio);
     #endif
}

/**********************************************/
//º¯Êı¹¦ÄÜ:½«¸ÃÈÎÎñ´ÓÈÎÎñµ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶ÖĞÇå³ı
//ÊäÈë²ÎÊı:pstrPrioFlag:µ÷¶È±í¶ÔÓ¦µÄÓÅÏÈ¼¶±êÊ¶Ö¸Õë
//         ucTaskPrio:ÈÎÎñµÄÓÅÏÈ¼¶
//·µ»ØÖµ  :none
/**********************************************/
void NIX_TaskClrPrioFlag(NIX_PRIOFLAG* pstrPrioFlag, U8 ucTaskPrio)
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
        ucPrioFlagGrp1 = ucTaskPrio/8;
        ucPrioFlagGrp2 = ucPrioFlagGrp1/8;
    //bit
        ucPosInGrp1 = ucTaskPrio%8;
        ucPosInGrp2 = ucPrioFlagGrp1%8;
        ucPosInGrp3 = ucPrioFlagGrp2;

        pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
        if(pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] == 0)
        {
            pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] &= ~((U8)(1 << ucPosInGrp2));
            if(pstrPrioFlag->aucPrioFlagGrp2[ucPrioFlagGrp2] == 0)
            {
                pstrPrioFlag->ucPrioFlagGrp3 &= ~((U8)(1 << ucPosInGrp3));
            }
        }

#elif PRIORITYNUM >= PRIORITY16

        ucPrioFlagGrp1 = ucTaskPrio / 8;

        ucPosInGrp1 = ucTaskPrio % 8;
        ucPosInGrp2 = ucPrioFlagGrp1;

        pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] &= ~((U8)(1 << ucPosInGrp1));
        if(pstrPrioFlag->aucPrioFlagGrp1[ucPrioFlagGrp1] == 0)
        {
             pstrPrioFlag->ucPrioFlagGrp2 &= ~((U8)(1 << ucPosInGrp2));
        }

#else
        pstrPrioFlag->ucPrioFlagGRP1 &= ~((U8)(1<<ucTaskPrio));
#endif

}


/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡ÓÅÏÈ¼¶±êÊ¶±íÖĞµÄ×î¸ßÓÅÏÈ¼¶
//ÊäÈë²ÎÊı:pstrPrioFlag:ÓÅÏÈ¼¶±êÊ¶±í
//·µ»ØÖµ  :²éÕÒµ½µÄ×î¸ßÓÅÏÈ¼¶
/**********************************************/
U8 NIX_TaskGetHighestPrio(NIX_PRIOFLAG* pstrPrioFlag)
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
        ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp2*8+ucPrioFlagGrp1]];
        return (U8)((ucPrioFlagGrp2*8+ucPrioFlagGrp1)*8+ucHighestFlagInGrp1);
    #elif PRIORITYNUM >= PRIORITY16
        ucPrioFlagGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGRP2];
        ucHighestFlagInGrp1 = caucTaskPrioUnmapTab[pstrPrioFlag->aucPrioFlagGRP1[ucPrioFlagGrp1]];
        return (U8)(ucPrioFlagGrp1*8+ucHighestFlagInGrp1);
    #else
        return caucTaskPrioUnmapTab[pstrPrioFlag->ucPrioFlagGRP1];
    #endif
}

/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡µ±Ç°µÄtick
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :ÏµÍ³µ±Ç°µÄtick
/**********************************************/
U32 NIX_GetSystemTick(void)
{
    return guiTick;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡µ±Ç°ÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :ÏµÍ³µ±Ç°µÄTCBÖ¸Õë
/**********************************************/
NIX_TCB* NIX_GetCurrentTcb(void)
{
    return gpstrCurTcb;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡¸ùÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :¸ùÈÎÎñµÄTCBÖ¸Õë
/**********************************************/
NIX_TCB* NIX_GetRootTcb(void)
{
    return gpstrRootTaskTcb;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡¿ÕÏĞÈÎÎñµÄTCBÖ¸Õë
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :¿ÕÏĞÈÎÎñµÄTCBÖ¸Õë
/**********************************************/
NIX_TCB* NIX_GetIdleTcb(void)
{
    return gpstrIdleTaskTcb;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:ÉèÖÃÓÃ»§
//ÊäÈë²ÎÊı:uiUser: ĞèÒªÉèÖÃµÄÓÃ»§
//·µ»ØÖµ  :none
/**********************************************/
void NIX_SetUser(U32 uiUser)
{
    guiUser = uiUser;
}

/**********************************************/
//º¯Êı¹¦ÄÜ:»ñÈ¡ÓÃ»§
//ÊäÈë²ÎÊı:none
//·µ»ØÖµ  :»ñÈ¡µ½µÄÓÃ»§
/**********************************************/
U32 NIX_GetUser(void)
{
    return guiUser;
}

