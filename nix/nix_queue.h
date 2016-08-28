#ifndef NIX_QUEUE_H
#define NIX_QUEUE_H

/* 节点加入队列的返回值 */
#define RTN_QUPTOV      RTN_SMGVOV	/* 将节点加入队列导致队列信号量溢出 */

/* 从列取出节点的返回值 */
#define RTN_QUGTTO      RTN_SMTKTO	/* 等待队列消息的时间耗尽, 超时返回 */
#define RTN_QUGTRT      RTN_SMTKRT	/* 没有获取到队列消息, 直接返回 */
#define RTN_QUGTDL      RTN_SMTKDL	/* 队列被删除 */

/* 队列调度方式 */
#define QUEFIFO         SEMFIFO	/* 队列采用先进先出排列 */
#define QUEPRIO         SEMPRIO	/* 队列采用优先级排列 */

/* 队列初始值 */
#define QUEEMPTY        SEMEMPTY	/* 队列为空状态 */
#define QUEFULL         SEMFULL	/* 队列为满状态 */

/* 延迟等待的时间 */
#define QUENOWAIT       SEMNOWAIT	/* 不等待 */
#define QUEWAITFEV      SEMWAITFEV	/* 永久等待 */

typedef struct nix_que		//队列结构
{
	NIX_LIST strList;	//队列链表
	NIX_SEM strSem;		//队列信号量
	U8 *pucQueMem;		//创建队列时的内存地址
} NIX_QUE;

extern NIX_QUE *NIX_QueCreate(NIX_QUE * pstrQue, U32 uiQueOpt);
extern U32 NIX_QuePut(NIX_QUE * pstrQue, NIX_LIST * pstrQueNode);
extern U32 NIX_QueGet(NIX_QUE * pstrQue, NIX_LIST * *ppstrQueNode, U32 uiDelayTick);
extern U32 NIX_QueDelete(NIX_QUE * pstrQue);

#endif
