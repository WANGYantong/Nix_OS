#include "nix_inner.h"

/**********************************************/
//函数功能:初始化链表
//输入参数:pstrList-链表根节点指针
//返回值  :none
/**********************************************/
void NIX_ListInit(NIX_LIST* pstrList)
{
    pstrList->pstrHead = (NIX_LIST*)NULL;
    pstrList->pstrTail = (NIX_LIST*)NULL;
}

/**********************************************/
//函数功能:从链表尾部加入节点
//输入参数:pstrList-链表根节点指针
//         pstrNode-加入的节点指针
//返回值  :none
/**********************************************/
void NIX_ListNodeAdd(NIX_LIST* pstrList, NIX_LIST* pstrNode)
{
    //链表非空
    if(pstrList->pstrTail != NULL)
        {
            pstrNode->pstrHead = pstrList->pstrHead;
            pstrNode->pstrTail = pstrList;

            pstrList->pstrHead->pstrTail = pstrNode;
			pstrList->pstrHead = pstrNode;
    }
    //链表为空
    else
        {
            pstrNode->pstrHead = pstrList;
            pstrNode->pstrTail = pstrList;
            pstrList->pstrHead = pstrNode;
            pstrList->pstrTail = pstrNode;
    }
}

/**********************************************/
//函数功能:从链表头部删除一个节点
//输入参数:pstrList-链表根节点指针
//返回值  :删除的节点指针，链表为空则返回NULL
/**********************************************/
NIX_LIST* NIX_ListNodeDelete(NIX_LIST* pstrList)
{
    NIX_LIST* pstrTempNode;
    pstrTempNode = pstrList->pstrTail;
    //链表非空
    if(pstrTempNode != NULL)
        {
            //链表有多个节点
            if(pstrList->pstrHead != pstrList->pstrTail)
                {
                    pstrList->pstrTail = pstrTempNode->pstrTail;
                    pstrTempNode->pstrTail->pstrHead = pstrList;
            }
            //链表只有一个节点
            else
                {
                    pstrList->pstrHead = (NIX_LIST*)NULL;
                    pstrList->pstrTail = (NIX_LIST*)NULL;
            }
            return pstrTempNode;
    }
    //链表为空
    else
        {
            return (NIX_LIST*)NULL;
    }
}

/**********************************************/
//函数功能:向链表指定的节点前插入一个节点
//输入参数:pstrList-链表根节点指针
//         pstrNode-基准节点指针，将新节点插入到此节点之前
//         pstrNewNode-新插入的节点指针
//返回值  :none
/**********************************************/
void NIX_ListNodeInsert(NIX_LIST* pstrList, NIX_LIST* pstrNode, NIX_LIST* pstrNewNode)
{
    //基准节点不是根节点
    if(pstrList != pstrNode)
        {
            pstrNode->pstrHead->pstrTail = pstrNewNode;
            pstrNewNode->pstrHead = pstrNode->pstrHead;
            pstrNewNode->pstrTail = pstrNode;
            pstrNode->pstrHead = pstrNewNode;
    }
    //基准节点是根节点
    else
        {
            NIX_ListNodeAdd(pstrList, pstrNewNode);
    }
}

/**********************************************/
//函数功能:从链表删除指定节点
//输入参数:pstrList-链表根节点指针
//         pstrNode-要删除的节点指针
//返回值  :删除节点的下个节点指针，若为空则返回NULL
/**********************************************/
NIX_LIST* NIX_ListCurNodeDelete(NIX_LIST* pstrList, NIX_LIST* pstrNode)
{
    //待删除的节点不是根节点
    if(pstrList != pstrNode)
        {
            //链表有多个节点
            if(pstrList->pstrHead != pstrList->pstrTail)
                {
                    pstrNode->pstrHead->pstrTail = pstrNode->pstrTail;
                    pstrNode->pstrTail->pstrHead = pstrNode->pstrHead;
                    return pstrNode->pstrTail;
            }
            //链表只有一个节点
            else
                {
                    NIX_ListNodeDelete(pstrList);
                    return (NIX_LIST*)NULL;
                }
    }
    //删除的节点是根节点
    else
        {
            return (NIX_LIST*)NULL;
    }
}

/**********************************************/
//函数功能:查询链表是否为空
//输入参数:pstrList-链表根节点指针
//返回值  :若非空则返回第一个节点的指针，为空则返回NULL
/**********************************************/
NIX_LIST* NIX_ListEmpInq(NIX_LIST* pstrList)
{
    return pstrList->pstrTail;
}

/**********************************************/
//函数功能:查询链表中指定节点的下一个节点是否为空
//输入参数:pstrList-链表根节点指针
//         pstrNode-基准节点指针
//返回值  :若非空则返回下一个节点的指针，为空则返回NULL
/**********************************************/
NIX_LIST* NIX_ListNextNodeEmpInq(NIX_LIST* pstrList, NIX_LIST* pstrNode)
{
    //基准节点是最后一个节点
    if(pstrList == pstrNode->pstrTail)
        {
            return (NIX_LIST*)NULL;
    }
    else
        {
            return pstrNode->pstrTail;
    }
}


