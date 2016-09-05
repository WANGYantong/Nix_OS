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
//��������:���л�ǰ��������Ϣ���浽�ڴ�
//�������:none
//����ֵ  :none
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
//��������:Ӳ�������жϷ�������д�ӡ�����ջ���ݵĺ���
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_FaultIsrPrint(void)
{
#ifdef NIX_DEBUGCONTEXT

	gpstrCurTaskReg = &gpstrCurTcb->strStackReg;
	gpstrNextTaskReg = (STACKREG *) NULL;	//���������쳣�ж�

	NIX_SaveTaskContext();
	NIX_PrintTaskContext();

#endif
}

#ifdef NIX_DEBUGCONTEXT

/**********************************************/
//��������:��ʼ�����λ����¼����
//�������:none
//����ֵ  :none
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
//��������:�����λ�������ݴ�ӡ������
//�������:none
//����ֵ  :none
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
//��������:���Ǽ�¼�ĵ�һ�������л���Ϣ
//�������:none
//����ֵ  :none
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
//��������:�������һ�������л���Ϣ
//�������:uiStackLen:��Ҫ�����ջ�ĳ���
//����ֵ  :none
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
	//����NIX_CONTMSG.uiLen
	uiDataAddr = NIX_SaveOneContextData(uiLstAddr, sizeof(NIX_CONTMSG) + uiStackLen);

	//����NIX_CONTMSG.uiPreAddr
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, gpstrContext->uiRelAddr);

	//����NIX_CONTMSG.uiPreTcb
	uiTcbAddr = (U32) gpstrCurTaskReg;
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	//����NIX_CONTMSG.uiNextTcb
	if (gpstrNextTaskReg != NULL) {
		uiTcbAddr = (U32) gpstrNextTaskReg;
	} else {
		uiTcbAddr = 0;
	}
	uiDataAddr = NIX_SaveOneContextData(uiDataAddr, uiTcbAddr);

	//����NIX_CONTMSG.strReg
	puiDataAddr = &gpstrCurTaskReg->uiExc_Rtn;
	for (i = 0; i < (sizeof(STACKREG) / sizeof(U32)); i++) {
		uiDataAddr = NIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	//����ջ����
	for (i = 0; i < (uiStackLen / sizeof(U32)); i++) {
		uiDataAddr = NIX_SaveOneContextData(uiDataAddr, *puiDataAddr--);
	}

	gpstrContext->uiRelAddr = uiLstAddr;
	gpstrContext->uiRemLen -= (sizeof(NIX_CONTMSG) + uiStackLen);
}

/**********************************************/
//��������:����һ������
//�������:uiAddr:�������ݵĵ�ַ
//         uiData:���������
//����ֵ  :�¸��������ݵĵ�ַ
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
//��������:��ȡһ������
//�������:uiAddr:�������ݵĵ�ַ
//         puiData: ����������ݵ�ָ��
//����ֵ  :�¸���ȡ���ݵĵ�ַ
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
//��������:��ӡ�����л���Ϣ
//�������:none
//����ֵ  :none
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

	//����ջ����
	for (i = 0; i < (uiLen - sizeof(NIX_CONTMSG)) / sizeof(U32); i++) {
		uiTcb -= 4;
		uiDataAddr = NIX_GetOneContextData(uiDataAddr, &uiData);
		NIX_SendString("\r\n0x%08X: 0x%08X", uiTcb, uiData);
	}

	gpstrContext->uiRemLen += uiLen;
}

/**********************************************/
//��������:�ҽӷ����ַ��ĺ���
//�������:vfFuncPointer: ��Ҫ�ҽӵķ����ַ�����������ԭ��
//����ֵ  :none
/**********************************************/
void NIX_SendCharFuncInit(VFUNC1 vfFuncPointer)
{
	gpstrContext->vfSendChar = vfFuncPointer;
}

/**********************************************/
//��������:�ҽӺ���õķ����ַ�����
//�������:ucChar:��Ҫ���͵��ַ�
//����ֵ  :none
/**********************************************/
void NIX_SendChar(U8 ucChar)
{
	if (gpstrContext->vfSendChar == NULL) {
		return;
	}

	gpstrContext->vfSendChar(ucChar);
}

/**********************************************/
//��������:��ӡ�ַ�������
//�������:pvStringPt: ��һ������ָ�룬����Ϊ�ַ���ָ��
//         ...:��������
//����ֵ  :none
/**********************************************/
void NIX_SendString(U8 * pvStringPt, ...)
{
#define BUFLEN      200		//��Ϣ���峤��

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
//��������:�ж��ַ����Ƿ���Ч
//�������:pucString: �洢�ַ�����ַ��ָ��
//����ֵ  :none
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

		if ((pucStr[i] < ' ') || (pucStr[i] > '~'))	//ASCII���ɴ�ӡ���ַ���Χ
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
//��������:�������ջ���ܵĳ�ʼ������
//�������:pstrTcb:��Ҫ��������TCB
//����ֵ  :none
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
//��������:�������ջʣ��δ�õĳ���
//�������:pstrTcb:��Ҫ��������TCB
//����ֵ  :����ջʣ��δ�õĳ��ȣ���λ���ֽ�
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
//��������:��ʼ�������CPUռ���ʽṹ
//�������:pstrTcb:��Ҫ��ʼ��������TCB
//����ֵ  :none
/**********************************************/
void NIX_TaskCPUShareInit(NIX_TCB * pstrTcb)
{
	pstrTcb->strCpushare.uiSysTickVal = 0;
	pstrTcb->strCpushare.uiCounter = 0;
	pstrTcb->strCpushare.uiCPUShare = 0;
}

/**********************************************/
//��������:��ÿ�������CPUռ���ʽ���ͳ��
//�������:pstrOldTcb:�л�ǰ������TCBָ��
//         pstrNewTcb:�л��������TCBָ��
//����ֵ  :none
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
//��������:��ȡ�����CPUռ����
//�������:pstrTcb:��Ҫ��ȡCPUռ���ʵ�����TCBָ��
//����ֵ  :�����CPUռ���ʣ���λ1%
/**********************************************/
U32 NIX_GetCpuShare(NIX_TCB * pstrTcb)
{
	return pstrTcb->strCpushare.uiCPUShare;
}

#endif
