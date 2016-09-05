#ifndef NIX_TASK_INNER_H
#define NIX_TASK_INNER_H

//系统优先级
#define HIGHESTPRIO             0	//系统最高优先级
#define LOWESTPRIO              (PRIORITYNUM - 1)	//系统最低优先级

//任务标志
#define DELAYQUEFLAG            0x00000001	//任务在Delay表中的标志，0:不在；1:在。
#define TASKSTACKFLAG           0x00000002	//任务从堆中申请任务栈标记，0:不申请；1:申请
#define TASKPRIINHFLAG          0x00000004	//任务优先级继承标志，0:不继承；1:继承

/******************************************************************/
extern NIX_LIST gstrTaskList;

#ifdef NIX_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
#endif

#ifdef NIX_TASKROUNDROBIN
extern U32 guiTimeSlice;
extern U32 gauiSliceCnt[PRIORITYNUM];
#endif

/******************************************************************/
extern NIX_TCB *NIX_TaskTcbInit(U8 * pucTaskName, VFUNC vfFuncPointer,
				void *pvPara, U8 * pucTaskStack,
				U32 uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt);
extern U32 NIX_TaskPend(NIX_SEM * pstrSem, U32 uiDelayTick);

#ifdef NIX_INCLUDETASKHOOK
extern void NIX_TaskHookInit(void);
#endif

#ifdef NIX_TASKPRIOINHER
extern void NIX_TaskPrioInheritance(NIX_TCB * pstrTcb, U8 ucTaskPrio);
extern void NIX_TaskPrioResume(NIX_TCB * pstrTcb);
#endif

#endif
