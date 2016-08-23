#include "nix_inner.h"

/**********************************************/
//��������:��ʼ������
//�������:pstrList-������ڵ�ָ��
//����ֵ  :none
/**********************************************/
void NIX_ListInit(NIX_LIST * pstrList)
{
	pstrList->pstrHead = (NIX_LIST *) NULL;
	pstrList->pstrTail = (NIX_LIST *) NULL;
}

/**********************************************/
//��������:������β������ڵ�
//�������:pstrList-������ڵ�ָ��
//         pstrNode-����Ľڵ�ָ��
//����ֵ  :none
/**********************************************/
void NIX_ListNodeAdd(NIX_LIST * pstrList, NIX_LIST * pstrNode)
{
	//����ǿ�
	if (pstrList->pstrTail != NULL) {
		pstrNode->pstrHead = pstrList->pstrHead;
		pstrNode->pstrTail = pstrList;

		pstrList->pstrHead->pstrTail = pstrNode;
		pstrList->pstrHead = pstrNode;
	}
	//����Ϊ��
	else {
		pstrNode->pstrHead = pstrList;
		pstrNode->pstrTail = pstrList;
		pstrList->pstrHead = pstrNode;
		pstrList->pstrTail = pstrNode;
	}
}

/**********************************************/
//��������:������ͷ��ɾ��һ���ڵ�
//�������:pstrList-������ڵ�ָ��
//����ֵ  :ɾ���Ľڵ�ָ�룬����Ϊ���򷵻�NULL
/**********************************************/
NIX_LIST *NIX_ListNodeDelete(NIX_LIST * pstrList)
{
	NIX_LIST *pstrTempNode;
	pstrTempNode = pstrList->pstrTail;
	//����ǿ�
	if (pstrTempNode != NULL) {
		//�����ж���ڵ�
		if (pstrList->pstrHead != pstrList->pstrTail) {
			pstrList->pstrTail = pstrTempNode->pstrTail;
			pstrTempNode->pstrTail->pstrHead = pstrList;
		}
		//����ֻ��һ���ڵ�
		else {
			pstrList->pstrHead = (NIX_LIST *) NULL;
			pstrList->pstrTail = (NIX_LIST *) NULL;
		}
		return pstrTempNode;
	}
	//����Ϊ��
	else {
		return (NIX_LIST *) NULL;
	}
}

/**********************************************/
//��������:������ָ���Ľڵ�ǰ����һ���ڵ�
//�������:pstrList-������ڵ�ָ��
//         pstrNode-��׼�ڵ�ָ�룬���½ڵ���뵽�˽ڵ�֮ǰ
//         pstrNewNode-�²���Ľڵ�ָ��
//����ֵ  :none
/**********************************************/
void NIX_ListNodeInsert(NIX_LIST * pstrList, NIX_LIST * pstrNode, NIX_LIST * pstrNewNode)
{
	//��׼�ڵ㲻�Ǹ��ڵ�
	if (pstrList != pstrNode) {
		pstrNode->pstrHead->pstrTail = pstrNewNode;
		pstrNewNode->pstrHead = pstrNode->pstrHead;
		pstrNewNode->pstrTail = pstrNode;
		pstrNode->pstrHead = pstrNewNode;
	}
	//��׼�ڵ��Ǹ��ڵ�
	else {
		NIX_ListNodeAdd(pstrList, pstrNewNode);
	}
}

/**********************************************/
//��������:������ɾ��ָ���ڵ�
//�������:pstrList-������ڵ�ָ��
//         pstrNode-Ҫɾ���Ľڵ�ָ��
//����ֵ  :ɾ���ڵ���¸��ڵ�ָ�룬��Ϊ���򷵻�NULL
/**********************************************/
NIX_LIST *NIX_ListCurNodeDelete(NIX_LIST * pstrList, NIX_LIST * pstrNode)
{
	//��ɾ���Ľڵ㲻�Ǹ��ڵ�
	if (pstrList != pstrNode) {
		//�����ж���ڵ�
		if (pstrList->pstrHead != pstrList->pstrTail) {
			pstrNode->pstrHead->pstrTail = pstrNode->pstrTail;
			pstrNode->pstrTail->pstrHead = pstrNode->pstrHead;
			return pstrNode->pstrTail;
		}
		//����ֻ��һ���ڵ�
		else {
			NIX_ListNodeDelete(pstrList);
			return (NIX_LIST *) NULL;
		}
	}
	//ɾ���Ľڵ��Ǹ��ڵ�
	else {
		return (NIX_LIST *) NULL;
	}
}

/**********************************************/
//��������:��ѯ�����Ƿ�Ϊ��
//�������:pstrList-������ڵ�ָ��
//����ֵ  :���ǿ��򷵻ص�һ���ڵ��ָ�룬Ϊ���򷵻�NULL
/**********************************************/
NIX_LIST *NIX_ListEmpInq(NIX_LIST * pstrList)
{
	return pstrList->pstrTail;
}

/**********************************************/
//��������:��ѯ������ָ���ڵ����һ���ڵ��Ƿ�Ϊ��
//�������:pstrList-������ڵ�ָ��
//         pstrNode-��׼�ڵ�ָ��
//����ֵ  :���ǿ��򷵻���һ���ڵ��ָ�룬Ϊ���򷵻�NULL
/**********************************************/
NIX_LIST *NIX_ListNextNodeEmpInq(NIX_LIST * pstrList, NIX_LIST * pstrNode)
{
	//��׼�ڵ������һ���ڵ�
	if (pstrList == pstrNode->pstrTail) {
		return (NIX_LIST *) NULL;
	} else {
		return pstrNode->pstrTail;
	}
}
