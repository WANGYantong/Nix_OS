#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "nix_inner.h"

#ifdef NIX_DEBUGCONTEXT
NIX_CONTHEAD *gpstrContext;
#endif

#ifdef NIX_DEBUGCPUSHARE
U32 guiCpuSharePeriod;
#endif
/**********************************************/
//函数功能:将切换前的任务信息保存到内存
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_SaveTaskContext(void)
{
#ifdef NIX_DEBUGCONTEXT

	U32 uiStack;
	U32 uiTcbAddr;
	U32 uiSaveLen;

	if (gpstrContext->uiAbsAddr == 0) {
		return;
	}

	if (gpstrCurTaskReg == gpstrNextTaskReg) {
		return;
	}

	uiStack = gpstrCurTaskReg->uiR13;
	uiTcbAddr = (U32) gpstrCurTaskReg;
	uiSaveLen = sizeof(NIX_CONTMSG) + (uiTcbAddr - uiStack);

	if (uiSaveLen > gpstrContext->uiLen) {
		gpstrContext->uiAbsAddr = 0;
		return;
	}

	while (1) {
		if (uiSaveLen > gpstrContext->uiRemLen) {
			NIX_CoverFisrtContext();
		} else {
			break;
		}
	}

	NIX_SaveLastContext(uiTcbAddr - uiStack);

#endif
}

/**********************************************/
//函数功能:硬件故障中断服务程序中打印保存的栈数据的函数
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_FaultIsrPrint(void)
{
#ifdef NIX_DEBUGCONTEXT

	gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
	gpstrNextTaskReg = (STACKREG *) NULL;	//表明发生异常中断

	NIX_SaveTaskContext();
	NIX_PrintTaskContext();

#endif
}

#ifdef NIX_DEBUGCONTEXT

/**********************************************/
//函数功能:初始化环形缓冲记录功能
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TaskContextInit(void)
{
	NIX_CONTMSG *pstrCurAddr;

	gpstrContext = (NIX_CONTHEAD *) ((NIX_CONTEXTADDR + 3) & ALIGN4MASK);
	gpstrContext->uiAbsAddr = (U32) gpstrContext + sizeof(NIX_CONTHEAD);

	if (gpstrContext->uiAbsAddr == 0) {
		return;
	}

	gpstrContext->uiRelAddr = 0;
	gpstrContext->uiLen = ((NIX_CONTEXTADDR + NIX_CONTEXTLEN) & ALIGN4MASK) - gpstrContext->uiAbsAddr;

	if (gpstrContext->uiLen < sizeof(NIX_CONTMSG)) {
		gpstrContext->uiAbsAddr = 0;
		return;
	}

	if (gpstrContext->uiLen > 0xFFFFFFFF - gpstrContext->uiAbsAddr) {
		gpstrContext->uiAbsAddr = 0;
		return;
	}

	gpstrContext->uiRemLen = gpstrContext->uiLen;

	pstrCurAddr = (NIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
	pstrCurAddr->uiLen = 0;
}

/**********************************************/
//函数功能:将环形缓冲的数据打印到串口
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_PrintTaskContext(void)
{
	if (gpstrContext->uiAbsAddr == 0) {
		return;
	}

	NIX_SendString("\r\n\r\n!!!!!EXCEPTION OCCUR!!!!!\n\r");

	while (gpstrContext->uiLen > gpstrContext->uiRemLen) {
		NIX_PrintContext();
	}

	NIX_SendString("\r\n\r\n!!!!!EXCEPTION PRINT OVER!!!!!\n\r");
}

/**********************************************/
//函数功能:覆盖记录的第一条任务切换消息
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_CoverFisrtContext(void)
{
	NIX_CONTMSG *pstrLstMsg;
	NIX_CONTMSG *pstrFstMsg;
	U32 uiIdleAddr;
	U32 uiFstAddr;

	pstrLstMsg = (NIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
	uiIdleAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

	if (uiIdleAddr >= gpstrContext->uiLen) {
		uiIdleAddr -= gpstrContext->uiLen;
		uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;
		pstrFstMsg = (NIX_CONTMSG *) (gpstrContext->uiAbsAddr + uiFstAddr);
	} else {
		uiFstAddr = uiIdleAddr + gpstrContext->uiRemLen;
		if (uiFstAddr >= gpstrContext->uiLen) {
			uiFstAddr -= gpstrContext->uiLen;
		}
		pstrFstMsg = (NIX_CONTMSG *) (gpstrContext->uiAbsAddr + uiFstAddr);
	}

	gpstrContext->uiRemLen += pstrFstMsg->uiLen;
}

/**********************************************/
//函数功能:保存最后一条任务切换信息
//输入参数:uiStackLen:需要保存的栈的长度
//返回值  :none
/**********************************************/
void NIX_SaveLastContext(U32 uiStackLen)
{
	NIX_CONTMSG *pstrLstMsg;
	U32 *puiDataAddr;
	U32 uiLstAddr;
	U32 uiDataAddr;
	U32 uiTcbAddr;
	U32 i;

	pstrLstMsg = (NIX_CONTMSG *) (gpstrContext->uiAbsAddr + gpstrContext->uiRelAddr);
	uiLstAddr = gpstrContext->uiRelAddr + pstrLstMsg->uiLen;

	if (uiLstAddr >= gpstrContext->uiLen) {
		uiLstAddr -= gpstrContext->uiLen;
	}
	//保存NIX_CONTMSG.uiLen
	uiDataAddr = NIX_SaveOneContextData(uiLstAddr, sizeof(NIX_CONTMSG) + uiStackLen);

	//保存NIX_CONTMSG.uiPreAddr
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, gpstrContext->uiRelAddr);

	//保存NIX_CONTMSG.uiPreTcb
	uiTcbAddr = (U32) gpstrCurTaskReg;
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	//保存NIX_CONTMSG.uiNextTcb
	if (gpstrNextTaskReg != NULL) {
		uiTcbAddr = (U32) gpstrNextTaskReg;
	} else {
		uiTcbAddr = 0;
	}
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	//保存NIX_CONTMSG.strReg
	puiDataAddr = &gpstrCurTaskReg->uiExc_Rtn;
	for (i = 0; i < (sizeof(STACKREG) / sizeof(U32)); i++) {
		uiDataAddr = NIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	//保存栈数据
	for (i = 0; i < (uiStackLen / sizeof(U32)); i++) {
		uiDataAddr = NIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	gpstrContext->uiRelAddr = uiLstAddr;
	gpstrContext->uiRemLen -= (sizeof(NIX_CONTMSG) + uiStackLen);
}

/**********************************************/
//函数功能:保存一个数据
//输入参数:uiAddr:保存数据的地址
//         uiData:保存的数据
//返回值  :下个保存数据的地址
/**********************************************/
U32 NIX_SaveOneContextData(U32 uiAddr, U32 uiData)
{
	ADDRVAL(gpstrContext->uiAbsAddr + uiAddr) = uiData;

	if (uiAddr + 4 != gpstrContext->uiLen) {
		uiAddr += 4;
	} else {
		uiAddr = 0;
	}

	return uiAddr;
}

/**********************************************/
//函数功能:获取一个数据
//输入参数:uiAddr:保存数据的地址
//         puiData: 待保存的数据的指针
//返回值  :下个获取数据的地址
/**********************************************/
U32 NIX_GetOneContextData(U32 uiAddr, U32 * puiData)
{
	*puiData = ADDRVAL(gpstrContext->uiAbsAddr + uiAddr);

	if (uiAddr + 4 != gpstrContext->uiLen) {
		uiAddr += 4;
	} else {
		uiAddr = 0;
	}

	return uiAddr;
}

/**********************************************/
//函数功能:打印任务切换信息
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_PrintContext(void)
{
	U8 *pucTaskName;
	U32 uiData;
	U32 uiLen;
	U32 uiDataAddr;
	U32 uiTcb;
	U32 i;

	uiDataAddr = gpstrContext->uiRelAddr;

	//NIX_CONTMSG.uiLen
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiLen);

	//NIX_CONTMSG.uiPreAddr
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	gpstrContext->uiRelAddr = uiData;

	//NIX_CONTMSG.uiPreTcb
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiTcb);
	pucTaskName = ((NIX_TCB *) uiTcb)->pucTaskName;
	NIX_IfValidString(&pucTaskName);
	NIX_SendString("\r\n\r\nTask %s 0x%08X ", pucTaskName, uiTcb);

	//NIX_CONTMSG.uiNextTcb
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	if (uiData != 0) {
		pucTaskName = ((NIX_TCB *) uiData)->pucTaskName;
		NIX_IfValidString(&pucTaskName);
		NIX_SendString("switch to task %s 0x%08X ", pucTaskName, uiData);
	} else {
		NIX_SendString("occur exception!");
	}

	//NIX_CONTMSG.strReg
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nExc_Rtn = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nXPSR = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nR15 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R14 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R13 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R12 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nR11 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R10 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R9 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R8 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nR7 = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R6  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R5  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R4  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString("\r\nR3  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R2  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R1  = 0x%08X", uiData);
	uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
	NIX_SendString(" R0  = 0x%08X", uiData);

	//其他栈数据
	for (i = 0; i < (uiLen - sizeof(NIX_CONTMSG)) / sizeof(U32); i++) {
		uiTcb -= 4;
		uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
		NIX_SendString("\r\n0x%08X: 0x%08X", uiTcb, uiData);
	}

	gpstrContext->uiRemLen += uiLen;
}

/**********************************************/
//函数功能:挂接发送字符的函数
//输入参数:vfFuncPointer: 需要挂接的发送字符函数，函数原型
//返回值  :none
/**********************************************/
void NIX_SendCharFuncInit(VFUNC1 vfFuncPointer)
{
	gpstrContext->vfSendChar = vfFuncPointer;
}

/**********************************************/
//函数功能:挂接后调用的发送字符函数
//输入参数:ucChar:需要发送的字符
//返回值  :none
/**********************************************/
void NIX_SendChar(U8 ucChar)
{
	if (gpstrContext->vfSendChar == NULL) {
		return;
	}

	gpstrContext->vfSendChar(ucChar);
}

/**********************************************/
//函数功能:打印字符串函数
//输入参数:pvStringPt: 第一个参数指针，必须为字符串指针
//         ...:其他参数
//返回值  :none
/**********************************************/
void NIX_SendString(U8 * pvStringPt, ...)
{
#define BUFLEN      200		//消息缓冲长度

	U8 aucBuf[BUFLEN];
	U32 i;
	va_list args;

	va_start(args, pvStringPt);
	(void) vsprintf(aucBuf, pvStringPt, args);
	va_end(args);

	for (i = 0; aucBuf[i] != NULL; i++) {
		NIX_SendChar(aucBuf[i]);
	}
}

/**********************************************/
//函数功能:判断字符串是否有效
//输入参数:pucString: 存储字符串地址的指针
//返回值  :none
/**********************************************/
void NIX_IfValidString(U8 ** pucString)
{
	U8 *pucStr;
	U32 i;

	if (pucString == NULL) {
		goto BAD_RTN;
	}

	pucStr = *pucString;

	if (((U32) pucStr < ROMVALIDADDRSTART) || ((U32) pucStr > ROMVALIDADDREND) || (pucStr == NULL)) {
		goto BAD_RTN;
	}

	for (i = 0; i < VALIDTASKNAMEMAXLEN; i++) {
		if (pucStr[i] == NULL) {
			return;
		}

		if ((pucStr[i] < ' ') || (pucStr[i] > '~'))	//ASCII码表可打印的字符范围
		{
			goto BAD_RTN;
		}
	}

      BAD_RTN:
	*pucString = "BAD DATA!";
	return;
}

#endif

#ifdef NIX_DEBUGSTACKCHECK

/**********************************************/
//函数功能:检查任务栈功能的初始化函数
//输入参数:pstrTcb:需要检查的任务TCB
//返回值  :none
/**********************************************/
void NIX_TaskStackCheckInit(NIX_TCB * pstrTcb)
{
	U32 uiStackBottom;
	U32 i;

	uiStackBottom = ((U32) pstrTcb->pucTaskStack + 3) & ALIGN4MASK;
	for (i = 0; i < (U32) pstrTcb - uiStackBottom; i += 4) {
		ADDRVAL(uiStackBottom + i) = STACKCHECKVALUE;
	}
}

/**********************************************/
//函数功能:检查任务栈剩余未用的长度
//输入参数:pstrTcb:需要检查的任务TCB
//返回值  :任务栈剩余未用的长度，单位：字节
/**********************************************/
U32 NIX_TaskStackCheck(NIX_TCB * pstrTcb)
{
	U32 uiStackBottom;
	U32 i;

	uiStackBottom = ((U32) pstrTcb->pucTaskStack + 3) & ALIGN4MASK;
	for (i = 0; i < (U32) pstrTcb - uiStackBottom; i += 4) {
		if (ADDRVAL(uiStackBottom + i) != STACKCHECKVALUE) {
			break;
		}
	}

	return i;
}

#endif

#ifdef NIX_DEBUGCPUSHARE

/**********************************************/
//函数功能:初始化任务的CPU占有率结构
//输入参数:pstrTcb:需要初始化的任务TCB
//返回值  :none
/**********************************************/
void NIX_TaskCPUShareInit(NIX_TCB * pstrTcb)
{
	pstrTcb->strCpushare.uiSysTickVal = 0;
	pstrTcb->strCpushare.uiCounter = 0;
	pstrTcb->strCpushare.uiCPUShare = 0;
}

/**********************************************/
//函数功能:对每个任务的CPU占有率进行统计
//输入参数:pstrOldTcb:切换前的任务TCB指针
//         pstrNewTcb:切换后的任务TCB指针
//返回值  :none
/**********************************************/
void NIX_CpuShareStatistic(NIX_TCB * pstrOldTcb, NIX_TCB * pstrNewTcb)
{
	static U32 suiTick = 0;
	NIX_LIST *pstrTaskList;
	NIX_TCB *pstrTaskTcb;
	U32 uiSysTickVal;

	uiSysTickVal = SysTick->VAL;

	if (pstrOldTcb != NULL) {
		if (guiTick == suiTick) {
			pstrOldTcb->strCpushare.uiCounter += pstrOldTcb->strCpushare.uiSysTickVal - uiSysTickVal;
		} else {
			pstrOldTcb->strCpushare.uiCounter +=
			    pstrOldTcb->strCpushare.uiSysTickVal + SYSTICKPERIOD - uiSysTickVal;
		}
	}

	suiTick = guiTick;
	pstrNewTcb->strCpushare.uiSysTickVal = uiSysTickVal;

	if (guiCpuSharePeriod == CPUSHARETIME) {
		pstrTaskList = &gstrTaskList;

		while (NULL != (pstrTaskList = NIX_ListNextNodeEmpInq(&gstrTaskList, pstrTaskList))) {
			pstrTaskTcb = ((NIX_TCBQUE *) pstrTaskList)->pstrTcb;
			pstrTaskTcb->strCpushare.uiCPUShare =
			    (pstrTaskTcb->strCpushare.uiCounter +
			     SYSTICKPERIOD / 2) / SYSTICKPERIOD * 100 / CPUSHARETIME;
			pstrTaskTcb->strCpushare.uiCounter = 0;
		}

		guiCpuSharePeriod = 0;
	}
}

/**********************************************/
//函数功能:获取任务的CPU占有率
//输入参数:pstrTcb:需要获取CPU占有率的任务TCB指针
//返回值  :任务的CPU占有率，单位1%
/**********************************************/
U32 NIX_GetCpuShare(NIX_TCB * pstrTcb)
{
	return pstrTcb->strCpushare.uiCPUShare;
}

#endif
