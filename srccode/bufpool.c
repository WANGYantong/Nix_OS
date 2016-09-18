#include <stdlib.h>
#include "bufpool.h"

U32 gabuf_len[] = BUF_CONFIG_VAL;
BUF_POOL gabuf_pool[BUF_TYPE_NUM];

/***********************************************************************************/
//函数功能: 初始化消息缓冲池.
//入口参数: none.
//返 回 值: none.
/***********************************************************************************/
void buf_pool_init(void)
{
	U32 i;

	if (BUF_TYPE_NUM > 1) {
		for (i = 0; i < BUF_TYPE_NUM; i++) {
			if (gabuf_len[i] > gabuf_len[i + 1]) {
				return;
			}
		}
	}

	for (i = 0; i < BUF_TYPE_NUM; i++) {
		slist_int(&gabuf_pool[i].list);
		gabuf_pool[i].psem = NIX_SemCreat(NULL, SEMBIN | SEMPRIO, SEMFULL);
		gabuf_pool[i].len = 0;
		gabuf_pool[i].s_num = 0;
		gabuf_pool[i].d_num = 0;
	}

}

/***********************************************************************************/
//函数功能: 从消息缓冲池申请缓冲
//入口参数: len: 申请的缓冲长度，单位：字节，不包含BUF_NODE部分的长度
//返 回 值: 申请的缓冲指针，如果申请不到返回NULL
/***********************************************************************************/
BUF_NODE *buf_malloc(U32 len)
{
    BUF_POOL* pbuf_pool;
    BUF_NODE* pnode;
    U32 i,j;

    for(i=0;i<BUF_TYPE_NUM;i++)
    {
       if(len<=gabuf_len[i])
       {
            pbuf_pool=&gabuf_pool[i];
            break;
       }
    }

    if(i>=BUF_TYPE_NUM){
        print_msg()
    }
}

/***********************************************************************************/
//函数功能: 释放缓冲到消息缓冲池
//入口参数: len: 申请的缓冲长度，单位：字节，不包含BUF_NODE部分的长度
//返 回 值: 申请的缓冲指针，如果申请不到返回NULL
/***********************************************************************************/

/***********************************************************************************/
//函数功能: 初始化单向链表
//入口参数: plist: 链表根节点指针
//返 回 值: none.
/***********************************************************************************/
void slist_int(SLIST * plist)
{
	plist->next = (SLIST *) NULL;
}

/***********************************************************************************/
//函数功能: 向单向链表增加一个节点
//入口参数: plist: 链表根节点指针
//          pnode: 加入的节点指针
//返 回 值: none.
/***********************************************************************************/
void slist_node_add(SLIST * plist, SLIST * pnode)
{
	pnode->next = plist->next;
	plist->next = pnode;
}

/***********************************************************************************/
//函数功能: 从单向链表删除一个节点
//入口参数: plist: 链表根节点指针
//返 回 值: 删除的节点指针，如果链表为空则返回NULL
/***********************************************************************************/
SLIST *slist_node_delete(SLIST * plist)
{
	SLIST *ptemp;
	ptemp = plist->next;

	if (plist->next != NULL) {
		plist->next = plist->next->next;
	}

	return ptemp;
}
