#include <stdlib.h>
#include "nix_inner.h"

/**********************************************/
//��������:����һ������
//�������:pstrQue:��Ҫ�����Ķ��е�ָ�룬��ΪNULL���ɸú������������ڴ�
//����ֵ  :NULL:��������ʧ��
//         ����:�������гɹ�������ֵΪ�����Ķ���ָ��
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
//��������:��һ���ڵ�������
//�������:pstrQue:����ָ��
//         pstrQueNode:Ҫ����Ķ��нڵ�ָ��
//����ֵ  :RTN_SUCD:�������гɹ�
//         RTN_FAIL:��������ʧ��
//         RTN_QUPTOV: ���ڵ������е��¶����ź������.
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
//��������:�Ӷ�����ȡ��һ���ڵ�
//�������:pstrQue:����ָ��
//         ppstrQueNode:��Ŷ��нڵ�ָ���ָ��
//�� �� ֵ: RTN_SUCD: �Ӷ���ȡ���ڵ�ɹ�.
//          RTN_FAIL: �Ӷ���ȡ���ڵ�ʧ��.
//          RTN_QUGTTO: �ȴ�������Ϣ��ʱ��ľ�, ��ʱ����.
//          RTN_QUGTRT: ʹ�ò��ȴ�ʱ�����û�л�ȡ��������Ϣ, ֱ�ӷ���.
//          RTN_QUGTDL: ���б�ɾ��.
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

	//����ȡ���ź�����˵��pstrQueNodeһ����ΪNULL
	*ppstrQueNode = pstrQueNode;

	return RTN_SUCD;
}

/**********************************************/
//��������:ɾ��һ������
//�������:pstrQue:��ɾ���Ķ���ָ��
//����ֵ  :RTN_SUCD:ɾ�����гɹ�
//         RTN_FAIL:ɾ������ʧ��
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
