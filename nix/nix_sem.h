
#ifndef MDS_SEM_H
#define MDS_SEM_H


/* 获取信号量的返回值 */
#define RTN_SMTKTO          2	/* 等待信号量的时间耗尽, 超时返回 */
#define RTN_SMTKRT          3	/* 没有获取到信号量, 直接返回 */
#define RTN_SMTKDL          4	/* 信号量被删除 */


/* 信号量参数偏移量 */
#define SEMSCHOF            0	/* 信号量参数中调度方式的偏移量 */

/* 信号量调度方式 */
#define SEMFIFO             (1 << SEMSCHOF)	/* 信号量采用先进先出排列 */
#define SEMPRIO             (2 << SEMSCHOF)	/* 信号量采用优先级排列 */

/* 信号量初始值 */
#define SEMEMPTY            0	/* 信号量为空状态 */
#define SEMFULL             0xFFFFFFFF	/* 信号量为满状态 */


/* 延迟等待的时间 */
#define SEMNOWAIT           0	/* 不等待 */
#define SEMWAITFEV          0xFFFFFFFF	/* 永久等待 */

/****************************************************************************/
extern NIX_SEM *NIX_SemCreat(NIX_SEM * pstrSem, U32 uiSemOpt, U32 uiInitVal);
extern U32 NIX_SemDelete(NIX_SEM * pstrSem);
extern U32 NIX_SemFlush(NIX_SEM * pstrSem);
extern U32 NIX_SemTake(NIX_SEM * pstrSem, U32 uiDelayTick);
extern U32 NIX_SemGive(NIX_SEM * pstrSem);


#endif
