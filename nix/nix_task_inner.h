#ifndef NIX_TASK_INNER_H
#define NIX_TASK_INNER_H

//ϵͳ���ȼ�
#define HIGHESTPRIO             0	//ϵͳ������ȼ�
#define LOWESTPRIO              (PRIORITYNUM - 1)	//ϵͳ������ȼ�

//�����־
#define DELAYQUEFLAG            0x00000001	//������Delay���еı�־��0:���ڣ�1:�ڡ�
#define TASKSTACKFLAG           0x00000002	//����Ӷ�����������ջ��ǣ�0:�����룻1:����

extern NIX_TCB *NIX_TaskTcbInit(U8 * pucTaskName, VFUNC vfFuncPointer,
				void *pvPara, U8 * pucTaskStack,
				U32 uiStackSize, U8 ucTaskPrio, NIX_TASKOPT * pstrTaskOpt);
extern U32 NIX_TaskPend(NIX_SEM * pstrSem, U32 uiDelayTick);

#ifdef NIX_INCLUDETASKHOOK
extern VFHCRT gvfTaskCreateHook;
extern VFHSWT gvfTaskSwitchHook;
extern VFHDLT gvfTaskDeleteHook;
extern void NIX_TaskHookInit(void);
#endif

#endif
