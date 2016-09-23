#include <stdlib.h>
#include "bufpool.h"

U32 gabuf_len[] = BUF_CONFIG_VAL;
BUF_POOL gabuf_pool[BUF_TYPE_NUM];

/***********************************************************************************/
//��������: ��ʼ����Ϣ�����.
//��ڲ���: none.
//�� �� ֵ: none.
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
		slist_init(&gabuf_pool[i].list);
		gabuf_pool[i].psem = NIX_SemCreat(NULL, SEMBIN | SEMPRIO, SEMFULL);
		gabuf_pool[i].len = 0;
		gabuf_pool[i].s_num = 0;
		gabuf_pool[i].d_num = 0;
	}

}

/***********************************************************************************/
//��������: ����Ϣ��������뻺��
//��ڲ���: len: ����Ļ��峤�ȣ���λ���ֽڣ�������BUF_NODE���ֵĳ���
//�� �� ֵ: ����Ļ���ָ�룬������벻������NULL
/***********************************************************************************/
BUF_NODE *buf_malloc(U32 len)
{
	BUF_POOL *pbuf_pool;
	BUF_NODE *pnode;
	U32 i, j;

	for (i = 0; i < BUF_TYPE_NUM; i++) {
		if (len <= gabuf_len[i]) {
			pbuf_pool = &gabuf_pool[i];
			break;
		}
	}

	if (i >= BUF_TYPE_NUM) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\ncan't support length of %d buf, max length is %d, (%d, %s)",
			  len, BUF_MAX_LEN, __LINE__, __FILE__);
		return NULL;
	}

	if (NIX_SemTake(pbuf_pool->psem, SEMWAITFEV) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\nbuf length %d wait sem failed! buf pool is 0x%x, (%d,%s)",
			  len, pbuf_pool, __LINE__, __FILE__);
		return NULL;
	}

	pnode = (BUF_NODE *) slist_node_delete(&pbuf_pool->list);

	if (pnode != NULL) {
		pbuf_pool->d_num++;
		print_msg(PRINT_SUGGEST, PRINT_BUF, "\r\nmalloc %d bytes 0x%x from buf pool successfully. (%d,%s)",
			  len, pnode, __LINE__, __FILE__);
	} else {
		if ((pnode = (BUF_NODE *) malloc(gabuf_len[i] + sizeof(BUF_NODE))) != NULL) {
			pbuf_pool->s_num++;
			pbuf_pool->d_num++;
			pnode->phead = pbuf_pool;
			print_msg(PRINT_SUGGEST, PRINT_BUF, "\r\nmalloc %d(+%d) bytes 0x%x from system successfully."
				  "(%d,%s)", gabuf_len[i], sizeof(BUF_NODE), pnode, __LINE__, __FILE__);
		} else {
			print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\nmalloc %d(+%d) bytes from system failed. (%d,%s)",
				  gabuf_len[i], sizeof(BUF_NODE), __LINE__, __FILE__);
			for (j = 0; j < BUF_TYPE_NUM; j++) {
				print_msg(PRINT_IMPORTANT, PRINT_BUF,
					  "\r\nalready malloc %d buf: length is %d(+%d).(%s,%d)", gabuf_pool[j].s_num,
					  gabuf_pool[j].len, sizeof(BUF_NODE), __FILE__, __LINE__);
			}
		}
	}

	if (NIX_SemGive(pbuf_pool->psem) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\nbuf length %d post sem failed! buf pool is 0x%x.(%d,%s)",
			  len, pbuf_pool, __LINE__, __FILE__);
		return NULL;
	}

	return pnode;
}

/***********************************************************************************/
//��������: �ͷŻ��嵽��Ϣ�����
//��ڲ���: pbuf:��Ҫ�ͷŵĻ���ָ��
//�� �� ֵ: none.
/***********************************************************************************/
void buf_free(BUF_NODE * pbuf)
{
	U32 len;
	U32 i;

	if (pbuf == NULL) {
		return;
	}

	len = pbuf->phead->len;

	for (i = 0; i < BUF_TYPE_NUM; i++) {
		if (gabuf_len[i] == len) {
			break;
		}
	}

	if (i >= BUF_TYPE_NUM) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\ncan't free the buf of %d length.(%d,%s)",
			  len, __LINE__, __FILE__);
		return;
	}

	if (NIX_SemTake(gabuf_pool[i].psem, SEMWAITFEV) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\nbuf length %d wait sem failed.(%d,%s)",
			  len, __LINE__, __FILE__);
		return;
	}

	slist_node_add((SLIST *) & gabuf_pool[i].list, (SLIST *) & pbuf->list);
	gabuf_pool[i].d_num--;

	if (NIX_SemGive(gabuf_pool[i].psem) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_BUF, "\r\nbuf length %d post sem failed! error is 0x%x.(%d,%s)",
			  len, __LINE__, __FILE__);

		//�費��Ҫ��һ�䣿
		//return;
	}

	print_msg(PRINT_SUGGEST, PRINT_BUF, "\r\nbuf length %d free successfully.(%d,%s)", len, __LINE__, __FILE__);
}

/***********************************************************************************/
//��������: ��ʼ����������
//��ڲ���: plist: ������ڵ�ָ��
//�� �� ֵ: none.
/***********************************************************************************/
void slist_init(SLIST * plist)
{
	plist->next = (SLIST *) NULL;
}

/***********************************************************************************/
//��������: ������������һ���ڵ�
//��ڲ���: plist: ������ڵ�ָ��
//          pnode: ����Ľڵ�ָ��
//�� �� ֵ: none.
/***********************************************************************************/
void slist_node_add(SLIST * plist, SLIST * pnode)
{
	pnode->next = plist->next;
	plist->next = pnode;
}

/***********************************************************************************/
//��������: �ӵ�������ɾ��һ���ڵ�
//��ڲ���: plist: ������ڵ�ָ��
//�� �� ֵ: ɾ���Ľڵ�ָ�룬�������Ϊ���򷵻�NULL
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
