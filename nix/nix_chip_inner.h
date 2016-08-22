#ifndef NIX_CHIP_INNER_H
#define NIX_CHIP_INNER_H

//中断返回地址
#define RTN_HANDLER             0xFFFFFFF1	//返回handler模式
#define RTN_THREAD_MSP          0xFFFFFFF9	//返回thread模式，并使用MSP
#define RTN_THREAD_PSP          0xFFFFFFFD	//返回thread模式，并使用PSP

//XPSR中断掩码位
#define XPSR_EXTMASK            0x1FF	//XPSR中低9位为IPSR，保存中断号

//堆栈指针选择
#define MSP_STACK               0	//主堆栈指针
#define PSP_STACK               1	//进程堆栈指针

//系统工作模式
#define PRIVILEGED              0	//特权级模式
#define UNPRIVILEGED            1	//用户级模式

extern U32 guiIntLockCounter;

extern void NIX_TaskStackInit(NIX_TCB * pstrTcb, VFUNC vfFuncPointer,
			      void *pvPara);
extern void NIX_SystemHardwareInit(void);
extern void NIX_TickTimerInit(void);
extern void NIX_PendSvIsrInit(void);
extern void NIX_SetChipWorkMode(U32 uiMode);
extern void NIX_TaskSwiSched(void);
extern void NIX_TaskOccurSwi(U32 uiSwiNo);

#endif
