#ifndef NIX_TASK_INNER_H
#define NIX_TASK_INNER_H

//系统优先级
#define HIGHESTPRIO             0                   //系统最高优先级
#define LOWESTPRIO              (PRIORITYNUM - 1)   //系统最低优先级

//任务标志
#define DELAYQUEFLAG            0x00000001          //任务在Delay表中的标志，0:不在；1:在。
#define TASKSTACKFLAG           0x00000002          //任务从堆中申请任务栈标记，0:不申请；1:申请

extern NIX_TCB* NIX_TaskTcbInit(U8* pucTaskName,VFUNC vfFuncPointer, void * pvPara, U8 * pucTaskStack,
                                U32  uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt);

#ifdef NIX_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
extern void NIX_TaskHookInit(void);
#endif

#endif

