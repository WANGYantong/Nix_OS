#ifndef NIX_TASK_H
#define NIX_TASK_H

typedef void (*VFHCRT) (NIX_TCB *);	//���񴴽����Ӻ�������
typedef void (*VFHSWT) (NIX_TCB *, NIX_TCB *);	//�����л����Ӻ�������
typedef void (*VFHDLT) (NIX_TCB *);	//����ɾ�����Ӻ�������

//�û����ȼ�
#define USERHIGHESTPRIO             (HIGHESTPRIO + 1)	//�û�������ȼ�
#define USERLOWESTPRIO              (LOWESTPRIO - 1)	//�û�������ȼ�

//����״̬
#define TASKRUNNING                 0x01	//����̬
#define TASKREADY                   0x02	//����̬
#define TASKDELAY                   0x04	//�ӳ�̬
#define TASKPEND                    0x08	//����̬
#define TASKSUSPEND                 0x10	//����̬

//�ӳ����񷵻�ֵ
#define RTN_TKDLTO                  2	//�ӳ�ʱ��ľ�����ʱ����
#define RTN_TKDLBK                  3	//�ӳ�״̬�жϣ����񷵻�

//�ӳ�ʱ��
#define DELAYNOWAIT                 0	//���ȴ�
#define DELAYWAITFEV                0xFFFFFFFF	//���õȴ�


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
