#include "nix_inner.h"

U32 guiIntLockCounter;		//���жϼ���ֵ

/**********************************************/
//��������:��ʼ������ջ����
//�������:pstrTcb: ��Ҫ��ʼ��������TCB
//         vfFuncPointer:�������к���ָ��
//         pvPara:��ڲ���ָ��
//����ֵ  :none
/**********************************************/
void NIX_TaskStackInit(NIX_TCB * pstrTcb, VFUNC vfFuncPointer, void *pvPara)
{
	STACKREG *pstrRegSp;
	U32 *puiStack;

	pstrRegSp = &pstrTcb->strStackReg;

	pstrRegSp->uiR0 = (U32) pvPara;
	pstrRegSp->uiR1 = 0;
	pstrRegSp->uiR2 = 0;
	pstrRegSp->uiR3 = 0;
	pstrRegSp->uiR4 = 0;
	pstrRegSp->uiR5 = 0;
	pstrRegSp->uiR6 = 0;
	pstrRegSp->uiR7 = 0;
	pstrRegSp->uiR8 = 0;
	pstrRegSp->uiR9 = 0;
	pstrRegSp->uiR10 = 0;
	pstrRegSp->uiR11 = 0;
	pstrRegSp->uiR12 = 0;
	pstrRegSp->uiR13 = (U32) pstrTcb - 32;
	pstrRegSp->uiR14 = (U32) NIX_TaskSelfDelete;
	pstrRegSp->uiR15 = (U32) vfFuncPointer;
	pstrRegSp->uiXpsr = MODE_USR;
	pstrRegSp->uiExc_Rtn = RTN_THREAD_MSP;

	//���������ʼ����ʱ��ջ������������ʱ��Ӳ���Զ�ȡ��
	puiStack = (U32 *) pstrTcb;
	*(--puiStack) = pstrRegSp->uiXpsr;
	*(--puiStack) = pstrRegSp->uiR15;
	*(--puiStack) = pstrRegSp->uiR14;
	*(--puiStack) = pstrRegSp->uiR12;
	*(--puiStack) = pstrRegSp->uiR3;
	*(--puiStack) = pstrRegSp->uiR2;
	*(--puiStack) = pstrRegSp->uiR1;
	*(--puiStack) = pstrRegSp->uiR0;
}

/**********************************************/
//��������:��ʼ��Ӳ��
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_SystemHardwareInit(void)
{
	NIX_TickTimerInit();
	NIX_PendSvIsrInit();
	NIX_SetChipWorkMode(UNPRIVILEGED);
}

/**********************************************/
//��������:��ʼ��tick��ʱ��
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TickTimerInit(void)
{
	//�����ж϶�ʱ���ڣ��жϼ��TICK��ms
	(void) SysTick_Config(SystemCoreClock / (1000 / TICK));
	//�����ж����ȼ�,6
	NVIC_SetPriority(SysTick_IRQn, 6 << 1);
}

/**********************************************/
//��������:��ʼ��pendSV�ж�
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_PendSvIsrInit(void)
{
	//�����ж����ȼ�,7
	NVIC_SetPriority(PendSV_IRQn, 7 << 1);
}

/**********************************************/
//��������:���ô���������ģʽ
//�������:uiMode:-Privileged ��Ȩ��   -Unprivileged �û���
//����ֵ  :none
/**********************************************/
void NIX_SetChipWorkMode(U32 uiMode)
{
	U32 uiCtrl;
	uiCtrl = __get_CONTROL();

	uiCtrl &= 0xFFFFFFFE;
	uiCtrl |= uiMode;

	__set_CONTROL(uiCtrl);
}

/**********************************************/
//��������:����SWI���ж�
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_TaskSwiSched(void)
{
	if (NIX_RunInInt() != RTN_SUCD) {
		NIX_TaskOccurSwi(SWI_TASKSCHED);
	} else {
		NIX_IntPendSvSet();
	}
}

/**********************************************/
//��������:����PendSV���ж�
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_IntPendSvSet(void)
{
#define HWREG(x)                (*((volatile unsigned long *)(x)))
#define NVIC_INT_CTRL           0xE000ED04
#define NVIC_INT_CTRL_PEND_SV   0x10000000

	HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

/**********************************************/
//��������:�жϵ�ǰ�����Ƿ����ж�������
//�������:none
//����ֵ  :RTN_SUCD:���ж�������
//         RTN_FAIL:�����ж�������
/**********************************************/
U32 NIX_RunInInt(void)
{
	if ((NIX_GetXpsr() & XPSR_EXTMASK) != 0) {
		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}

/**********************************************/
//��������:���ж�
//�������:none
//����ֵ  :RTN_SUCD:���жϳɹ�; RTN_FAIL:���ж�ʧ��
/**********************************************/
U32 NIX_IntLock(void)
{
	if (NIX_RunInInt() == RTN_SUCD) {
		return RTN_FAIL;
	}
	if (guiIntLockCounter == 0) {
		__disable_irq();
		guiIntLockCounter++;
		return RTN_SUCD;
	} else if (guiIntLockCounter < 0xFFFFFFFF) {
		guiIntLockCounter++;
		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}

/**********************************************/
//��������:�������ж�
//�������:none
//����ֵ  :RTN_SUCD:�������жϳɹ�; RTN_FAIL:�������ж�ʧ��
/**********************************************/
U32 NIX_IntUnLock(void)
{
	if (NIX_RunInInt() == RTN_SUCD) {
		return RTN_FAIL;
	}
	if (guiIntLockCounter > 1) {
		guiIntLockCounter--;
		return RTN_SUCD;
	} else if (guiIntLockCounter == 1) {
		guiIntLockCounter--;
		__enable_irq();
		return RTN_SUCD;
	} else {
		return RTN_FAIL;
	}
}
