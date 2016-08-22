#ifndef NIX_QUEUE_H
#define NIX_QUEUE_H

#define RTN_NULL                2	//队列为空

typedef struct nix_que		//队列结构
{
	NIX_LIST strList;	//队列链表
	U8 *pucQueMem;		//创建队列时的内存地址
} NIX_QUE;

extern NIX_QUE *NIX_QueCreate(NIX_QUE * pstrQue);
extern U32 NIX_QuePut(NIX_QUE * pstrQue, NIX_LIST * pstrQueNode);
extern U32 NIX_QueGet(NIX_QUE * pstrQue, NIX_LIST * *ppstrQueNode);
extern U32 NIX_QueDelete(NIX_QUE * pstrQue);

#endif
