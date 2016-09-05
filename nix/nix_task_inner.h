#ifndef NIX_TASK_INNER_H
#define NIX_TASK_INNER_H

//ϵͳ���ȼ�
#define HIGHESTPRIO             0	//ϵͳ������ȼ�
#define LOWESTPRIO              (PRIORITYNUM - 1)	//ϵͳ������ȼ�

//�����־
#define DELAYQUEFLAG            0x00000001	//������Delay���еı�־��0:���ڣ�1:�ڡ�
#define TASKSTACKFLAG           0x00000002	//����Ӷ�����������ջ��ǣ�0:�����룻1:����
#define TASKPRIINHFLAG          0x00000004	//�������ȼ��̳б�־��0:���̳У�1:�̳�

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
