
#ifndef MDS_SEM_INNER_H
#define MDS_SEM_INNER_H


/* 信号量参数掩码 */
#define SEMSCHEDOPTMASK     0x00000003	/* 信号量参数中调度方式的掩码 */


/****************************************************************************/
extern U32 NIX_SemFlushValue(NIX_SEM * pstrSem, U32 uiRtnValue);


#endif
