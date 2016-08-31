#ifndef NIX_TASK_H
#define NIX_TASK_H

typedef void (*VFHCRT) (NIX_TCB *);	//任务创建钩子函数类型
typedef void (*VFHSWT) (NIX_TCB *, NIX_TCB *);	//任务切换钩子函数类型
typedef void (*VFHDLT) (NIX_TCB *);	//任务删除钩子函数类型

//用户优先级
#define USERHIGHESTPRIO             (HIGHESTPRIO + 1)	//用户最高优先级
#define USERLOWESTPRIO              (LOWESTPRIO - 1)	//用户最低优先级

//任务状态
#define TASKRUNNING                 0x01	//运行态
#define TASKREADY                   0x02	//就绪态
#define TASKDELAY                   0x04	//延迟态
#define TASKPEND                    0x08	//阻塞态
#define TASKSUSPEND                 0x10	//挂起态

//延迟任务返回值
#define RTN_TKDLTO                  2	//延迟时间耗尽，超时返回
#define RTN_TKDLBK                  3	//延迟状态中断，任务返回

//延迟时间
#define DELAYNOWAIT                 0	//不等待
#define DELAYWAITFEV                0xFFFFFFFF	//永久等待


extern NIX_TCB *NIX_TaskCreat(U8 * pucTaskName, VFUNC vfFuncPointer,
			      void *pvPara, U8 * pucTaskStack,
			      U32 uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt);
extern U32 NIX_TaskDelete(NIX_TCB * pstrTcb);
extern void NIX_TaskSelfDelete(void);
extern U32 NIX_TaskDelay(U32 uiDelayTick);
extern U32 NIX_TaskWake(NIX_TCB * pstrTcb);

#ifdef NIX_TASKROUNDROBIN
extern void NIX_TaskTimeSlice(U32 uiTimeSlice);
#endif

#ifdef NIX_INCLUDETASKHOOK
extern void NIX_TaskCreateHookAdd(VFHCRT vfFuncPointer);
extern void NIX_TaskCreateHookDel(void);
extern void NIX_TaskSwitchHookAdd(VFHSWT vfFuncPointer);
extern void NIX_TaskSwitchHookDel(void);
extern void NIX_TaskDeleteHookAdd(VFHDLT vfFuncPointer);
extern void NIX_TaskDeleteHookDel(void);
#endif

#endif
