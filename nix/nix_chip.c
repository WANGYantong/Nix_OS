#include "nix_inner.h"

U32 guiIntLockCounter;		//锁中断计数值

/**********************************************/
//函数功能:初始化任务栈函数
//输入参数:pstrTcb: 需要初始化的任务TCB
//         vfFuncPointer:任务运行函数指针
//         pvPara:入口参数指针
//返回值  :none
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

	//构造任务初始运行时的栈，在任务运行时由硬件自动取出
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
//函数功能:初始化硬件
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_SystemHardwareInit(void)
{
	NIX_TickTimerInit();
	NIX_PendSvIsrInit();
	NIX_SetChipWorkMode(UNPRIVILEGED);
}

/**********************************************/
//函数功能:初始化tick定时器
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_TickTimerInit(void)
{
	//设置中断定时周期，中断间隔TICK个ms
	(void) SysTick_Config(SystemCoreClock / (1000 / TICK));
	//设置中断优先级,6
	NVIC_SetPriority(SysTick_IRQn, 6 << 1);
}

/**********************************************/
//函数功能:初始化pendSV中断
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_PendSvIsrInit(void)
{
	//设置中断优先级,7
	NVIC_SetPriority(PendSV_IRQn, 7 << 1);
}

/**********************************************/
//函数功能:设置处理器工作模式
//输入参数:uiMode:-Privileged 特权级   -Unprivileged 用户级
//返回值  :none
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
//函数功能:触发SWI软中断
//输入参数:none
//返回值  :none
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
//函数功能:触发PendSV软中断
//输入参数:none
//返回值  :none
/**********************************************/
void NIX_IntPendSvSet(void)
{
#define HWREG(x)                (*((volatile unsigned long *)(x)))
#define NVIC_INT_CTRL           0xE000ED04
#define NVIC_INT_CTRL_PEND_SV   0x10000000

	HWREG(NVIC_INT_CTRL) |= NVIC_INT_CTRL_PEND_SV;
}

/**********************************************/
//函数功能:判断当前程序是否在中断中运行
//输入参数:none
//返回值  :RTN_SUCD:在中断中运行
//         RTN_FAIL:不在中断中运行
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
//函数功能:锁中断
//输入参数:none
//返回值  :RTN_SUCD:锁中断成功; RTN_FAIL:锁中断失败
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
//函数功能:开启锁中断
//输入参数:none
//返回值  :RTN_SUCD:开启锁中断成功; RTN_FAIL:开启锁中断失败
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
