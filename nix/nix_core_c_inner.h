#ifndef NIX_CORE_C_INNER_H
#define NIX_CORE_C_INNER_H

//系统状态标志
#define SYSTEMSCHEDULE          0	//进入操作系统状态
#define SYSTEMNOTSCHEDULE       1	//未进入操作系统状态

//tick调度标志
#define TICKSCHEDCLR            0	//非tick中断调度状态
#define TICKSCHEDSET            1	//tick中断调度状态

//操作系统所使用的用户
#define USERROOT                0	//高级用户，具有全部权限
#define USERGUEST               1	//访问用户，具有部分权限

/********************************************************************/
extern U32 guiSystemStatus;
extern NIX_TASKSCHEDTAB gstrReadyTab;
extern NIX_LIST gstrDelayTab;
extern U32 guiTick;
extern U8 gucTickSched;
extern STACKREG *gpstrCurTaskReg;
extern STACKREG *gpstrNextTaskReg;
extern NIX_TCB *gpstrCurTcb;
extern NIX_TCB *gpstrRootTaskTcb;
extern NIX_TCB *gpstrIdleTaskTcb;
extern U32 guiUser;
extern const U8 caucTaskPrioUnmapTab[256];

/********************************************************************/
extern void NIX_SystemVarInit(void);
extern void NIX_BeforeRootTask(void *pvPara);
extern void NIX_TaskSwitch(NIX_TCB * pstrTcb);
extern void NIX_TaskStart(NIX_TCB * pstrTcb);
extern void NIX_TaskSchedTabInit(NIX_TASKSCHEDTAB * pstrSchedTab);
extern void NIX_TaskAddToSchedTab(NIX_LIST * pstrList,
				  NIX_LIST * pstrNode,
				  NIX_PRIOFLAG * pstrPrioFlag,
				  U8 ucTaskPrio);
extern void NIX_TaskAddToDelayTab(NIX_LIST * pstrNode);
extern NIX_LIST *NIX_TaskDelFromSchedTab(NIX_LIST * pstrList,
					 NIX_PRIOFLAG * pstrPrioFlag,
					 U8 ucTaskPrio);
extern void NIX_TaskSched(void);
extern NIX_TCB *NIX_TaskReadyTabSched(void);
extern void NIX_TaskDelayTabSched(void);
extern void NIX_TaskAddToSemTab(NIX_TCB * pstrTcb, NIX_SEM * pstrSem);
extern NIX_LIST* NIX_TaskDelFromSemTab(NIX_TCB * pstrTcb);
extern NIX_TCB* NIX_SemGetAcitveTask(NIX_SEM * pstrSem);
extern void NIX_TaskSetPrioFlag(NIX_PRIOFLAG * pstrPrioFlag,
				U8 ucTaskPrio);
extern void NIX_TaskClrPrioFlag(NIX_PRIOFLAG * pstrPrioFlag,
				U8 ucTaskPrio);
extern U8 NIX_TaskGetHighestPrio(NIX_PRIOFLAG * pstrPrioFlag);
extern U32 NIX_GetXpsr(void);
extern void NIX_SwitchToTask(void);
extern void NIX_SetUser(U32 uiUser);
extern U32 NIX_GetUser(void);

#endif
