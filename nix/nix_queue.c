#include <stdlib.h>
#include "nix_inner.h"

/**********************************************/
//函数功能:创建一个队列
//输入参数:pstrQue:需要创建的队列的指针，若为NULL则由该函数自行申请内存
//返回值  :NULL:创建队列失败
//         其他:创建队列成功，返回值为创建的队列指针
/**********************************************/
NIX_QUE *NIX_QueCreate(NIX_QUE * pstrQue, U32 uiQueOpt)
{
	U8 *pucQueMemAddr;

	if (((uiQueOpt & QUESCHEDOPTMASK) != QUEFIFO)
	    && ((uiQueOpt & QUESCHEDOPTMASK) != QUEPRIO)) {
		return (NIX_QUE *) NULL;
	}

	if (pstrQue == NULL) {
		(void) NIX_IntLock();

		pucQueMemAddr = malloc(sizeof(NIX_QUE));
		if (pucQueMemAddr == NULL) {
			(void) NIX_IntUnLock();
			return (NIX_QUE *) NULL;
		}

		(void) NIX_IntUnLock();
		pstrQue = (NIX_QUE *) pucQueMemAddr;

	} else {
		pucQueMemAddr = (U8 *) NULL;
	}

	NIX_ListInit(&pstrQue->strList);

	pstrQue->pucQueMem = pucQueMemAddr;

	if (NIX_SemCreat(&pstrQue->strSem, SEMCNT | uiQueOpt, SEMEMPTY) != NULL) {
		return pstrQue;
	} else {
		if (pucQueMemAddr != NULL) {
			free(pucQueMemAddr);
		}

		return (NIX_QUE *) NULL;
	}
}

/**********************************************/
//函数功能:将一个节点加入队列
//输入参数:pstrQue:队列指针
//         pstrQueNode:要加入的队列节点指针
//返回值  :RTN_SUCD:创建队列成功
//         RTN_FAIL:创建队列失败
//         RTN_QUPTOV: 将节点加入队列导致队列信号量溢出.
/**********************************************/
U32 NIX_QuePut(NIX_QUE * pstrQue, NIX_LIST * pstrQueNode)
{
	if ((pstrQue == NULL) || (pstrQueNode == NULL)) {
		return RTN_FAIL;
	}

	(void) NIX_IntLock();

	NIX_ListNodeAdd(&pstrQue->strList, pstrQueNode);

	(void) NIX_IntUnLock();

	return NIX_SemGive(&pstrQue->strSem);
}

/**********************************************/
//函数功能:从队列中取出一个节点
//输入参数:pstrQue:队列指针
//         ppstrQueNode:存放队列节点指针的指针
//返 回 值: RTN_SUCD: 从队列取出节点成功.
//          RTN_FAIL: 从队列取出节点失败.
//          RTN_QUGTTO: 等待队列消息的时间耗尽, 超时返回.
//          RTN_QUGTRT: 使用不等待时间参数没有获取到队列消息, 直接返回.
//          RTN_QUGTDL: 队列被删除.
/**********************************************/
U32 NIX_QueGet(NIX_QUE * pstrQue, NIX_LIST ** ppstrQueNode, U32 uiDelayTick)
{
	NIX_LIST *pstrQueNode;
	U32 uiRtn;

	if ((NULL == pstrQue) || (NULL == ppstrQueNode)) {
		return RTN_FAIL;
	}

	uiRtn = NIX_SemTake(&pstrQue->strSem, uiDelayTick);
	if (uiRtn != RTN_SUCD) {
		return uiRtn;
	}

	(void) NIX_IntLock();

	pstrQueNode = NIX_ListNodeDelete(&pstrQue->strList);

	(void) NIX_IntUnLock();

	//可以取出信号量，说明pstrQueNode一定不为NULL
	*ppstrQueNode = pstrQueNode;

	return RTN_SUCD;
}

/**********************************************/
//函数功能:删除一个队列
//输入参数:pstrQue:待删除的队列指针
//返回值  :RTN_SUCD:删除队列成功
//         RTN_FAIL:删除队列失败
/**********************************************/
U32 NIX_QueDelete(NIX_QUE * pstrQue)
{
	if (pstrQue == NULL) {
		return RTN_FAIL;
	}

	if (NIX_SemFlushValue(&pstrQue->strSem, RTN_SMTKDL) != RTN_SUCD) {
		return RTN_FAIL;
	}

	if (pstrQue->pucQueMem != NULL) {
		(void) NIX_IntLock();

		free(pstrQue->pucQueMem);

		(void) NIX_IntUnLock();
	}

	return RTN_SUCD;

}
