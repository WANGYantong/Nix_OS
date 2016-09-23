#include <stdlib.h>
#include "task.h"

TASK_POOL gtask_pool;

/**********************************************/
//函数功能:创建任务平台所需要的任务
//输入参数:none
//返回值  :none
/**********************************************/
void task_create(void)
{
	process_task_pool_init();
}

/**********************************************/
//函数功能:初始化任务池
//输入参数:none
//返回值  :none
/**********************************************/
void process_task_pool_init(void)
{
	if ((gtask_pool.psem = NIX_SemCreat(NULL, SEMBIN | SEMPRIO, SEMFULL)) == NULL) {
		print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\ntask_pool sem init failed.(%s,%d)", __FILE__, __LINE__);
	}

	slist_init(&gtask_pool.list);

	gtask_pool.d_num = 0;
	gtask_pool.s_num = 0;
}

/**********************************************/
//函数功能:从任务池中申请任务
//输入参数:none
//返回值  :申请到的任务结构指针，否则返回NULL
/**********************************************/
TASK_STR *process_task_pool_malloc(void)
{
	TASK_STR *ptask;

	if (NIX_SemTake(gtask_pool.psem, SEMWAITFEV) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\nprocess_task wait sem failed. (%d, %s)",
			  __LINE__, __FILE__);
		return NULL;
	}

	ptask = (TASK_STR *) slist_node_delete(&gtask_pool.list);

	if (ptask != NULL) {
		gtask_pool.d_num++;
		print_msg(PRINT_SUGGEST, PRINT_TASK, "\r\nmalloc 0x%x process_task successfully. (%s, %d)",
			  ptask, __FILE__, __LINE__);
	} else {
		ptask = malloc(sizeof(TASK_STR));

		if (ptask != NULL) {
			print_msg(PRINT_SUGGEST, PRINT_TASK, "\r\nmalloc 0x%x process_task from system successfully."
				  " (%s, %d)", ptask, __FILE__, __LINE__);
		} else {
			print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\nmalloc %d bytes from system failed. (%s, %d)",
				  sizeof(TASK_STR), __FILE__, __LINE__);

			goto RTN;
		}

		if (create_process_task(ptask) != RTN_SUCD) {
			free(ptask);
			goto RTN;
		} else {
			gtask_pool.s_num++;
			gtask_pool.d_num++;
		}
	}
      RTN:
	if (NIX_SemGive(gtask_pool.psem) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\nprocess_task post sem failed. (%d, %s)",
			  __LINE__, __FILE__);
		return NULL;
	}

	return ptask;
}

/**********************************************/
//函数功能:释放任务到任务池
//输入参数:ptask:待释放的任务结构指针
//返回值  :none
/**********************************************/
void process_task_free(TASK_STR * ptask)
{
	if (NIX_SemTake(gtask_pool.psem, SEMWAITFEV) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\nprocess_task wait sem failed. (%d, %s)",
			  __LINE__, __FILE__);
		return;
	}

	slist_node_add(&gtask_pool.list, &ptask->list);
	gtask_pool.d_num--;

	if (NIX_SemGive(gtask_pool.psem) != RTN_SUCD) {
		print_msg(PRINT_IMPORTANT, PRINT_TASK, "\r\nprocess_task post sem failed. (%d, %s)",
			  __LINE__, __FILE__);

		return;
	}

	print_msg(PRINT_SUGGEST, PRINT_TASK, "\r\nfree process_task 0x%x to taskpool successfully. (%s, %d)",
		  ptask, __FILE__, __LINE__);
}
