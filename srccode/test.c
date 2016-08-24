#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* 串口打印消息队列指针 */
NIX_TCB *gpstrSerialTaskTcb;	/* 串口打印任务TCB指针 */

NIX_SEM *gpstrSemSync;		/*同步信号量 */
NIX_SEM *gpstrSemMute;		/*互斥信号量 */

/**********************************************/
//函数功能:测试任务1
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* 任务打印 */
		DEV_PutStrToMem((U8 *) "\r\nTask1 is running ! Tick is: %d", NIX_GetSystemTick());

		/* 任务运行1.5秒 */
		TEST_TaskRun(1500);

		/* 任务延迟2秒 */
		(void) NIX_TaskDelay(200);

		/* 前10次, 每次运行释放一次gpstrSemSync信号量 */
		if (i < 10) {
			i++;

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 give gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 同步其它任务 */
			(void) NIX_SemGive(gpstrSemSync);
		}
		/* 接下来5次, 每次运行flush一次gpstrSemSync信号量 */
		else if (i < 15) {
			i++;

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 flush gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 释放所有被gpstrSemSync阻塞的任务 */
			(void) NIX_SemFlush(gpstrSemSync);
		}
		/* 删除gpstrSemSync信号量 */
		else if (15 == i) {
			i++;

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 delete gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 删除gpstrSemSync信号量 */
			(void) NIX_SemDelete(gpstrSemSync);
		}
	}
}



/**********************************************/
//函数功能:测试任务2
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* 前3次获取gpstrSemMute信号量, 与TEST_TestTask3任务互锁 */
		if (i < 3) {
			i++;

			/* 获取到信号量才运行 */
			(void) NIX_SemTake(gpstrSemMute, SEMWAITFEV);

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask2 take gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 任务运行0.5秒 */
			TEST_TaskRun(500);

			/* 任务延迟2秒 */
			(void) NIX_TaskDelay(200);

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask2 give gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 释放信号量, 以便其它任务可以获得该信号量 */
			(void) NIX_SemGive(gpstrSemMute);
		} else {	/* 接下来获取gpstrSemSync信号量, 由TEST_TestTask1任务激活 */

			i++;

			/* 信号量被删除, 任务返回 */
			if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
				/* 任务打印 */
				DEV_PutStrToMem((U8 *) "\r\nTask2 gpstrSemSync deleted! Tick is: %d",
						NIX_GetSystemTick());

				return;
			}
			/* 获取到gpstrSemSync信号量才运行 */
			else {
				/* 任务打印 */
				DEV_PutStrToMem((U8 *) "\r\nTask2 take gpstrSemSync %d! Tick is: %d",
						i, NIX_GetSystemTick());

				/* 任务运行0.5秒 */
				TEST_TaskRun(500);
			}
		}
	}

}


/**********************************************/
//函数功能:测试任务3
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask3(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* 前3次获取gpstrSemMute信号量, 与TEST_TestTask2任务互锁 */
		if (i < 3) {
			i++;

			/* 获取到信号量才运行 */
			(void) NIX_SemTake(gpstrSemMute, SEMWAITFEV);

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask3 take gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 任务运行0.5秒 */
			TEST_TaskRun(500);

			/* 任务延迟1.5秒 */
			(void) NIX_TaskDelay(150);

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask3 give gpstrSemMute %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 释放信号量, 以便其它任务可以获得该信号量 */
			(void) NIX_SemGive(gpstrSemMute);
		} else {	/* 接下来获取gpstrSemSync信号量, 由TEST_TestTask1任务激活 */

			i++;

			/* 信号量被删除, 任务返回 */
			if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
				/* 任务打印 */
				DEV_PutStrToMem((U8 *) "\r\nTask3 gpstrSemSync deleted! Tick is: %d",
						NIX_GetSystemTick());

				return;
			} else {	/* 获取到gpstrSemSync信号量才运行 */

				/* 任务打印 */
				DEV_PutStrToMem((U8 *) "\r\nTask3 take gpstrSemSync %d! Tick is: %d",
						i, NIX_GetSystemTick());

				/* 任务运行0.5秒 */
				TEST_TaskRun(500);
			}
		}
	}


}

/**********************************************/
//函数功能:测试任务4
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask4(void *pvPara)
{
	U8 i;

	i = 0;

	while (1) {
		/* 信号量被删除, 任务返回 */
		if (RTN_SMTKDL == NIX_SemTake(gpstrSemSync, SEMWAITFEV)) {
			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask4 gpstrSemSync deleted! Tick is: %d", NIX_GetSystemTick());

			return;
		} else {	/* 获取到gpstrSemSync信号量才运行 */

			i++;

			/* 任务打印 */
			DEV_PutStrToMem((U8 *) "\r\nTask4 take gpstrSemSync %d! Tick is: %d", i, NIX_GetSystemTick());

			/* 任务运行0.5秒 */
			TEST_TaskRun(500);
		}
	}


}

/**********************************************/
//函数功能:串口打印任务，从队列中获取需要打印的消息缓冲，将缓冲中的数据打印到串口
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_SerialPrintTask(void *pvPara)
{
	NIX_LIST *pstrMsgQueNode;
	MSGBUF *pstrMsgBuf;

	/* 从队列循环获取消息 */
	while (1) {
		/* 从队列中获取到一条需要打印的消息, 向串口打印消息数据 */
		if (RTN_SUCD == NIX_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode)) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			/* 将缓冲中的数据打印到串口 */
			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			/* 缓冲消息中的数据发送完毕, 释放缓冲 */
			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
		} else {	/* 没有获取到消息, 延迟一段时间后再查询队列 */

			(void) NIX_TaskDelay(100);
		}
	}

}

/**********************************************/
//函数功能:模拟任务运行函数
//输入参数:uiMs:要延迟的时间，单位ms
//返回值  :none
/**********************************************/
void TEST_TaskRun(U32 uiMs)
{
	DEV_DelayMs(uiMs);
}

/**********************************************/
//函数功能:将任务创建过程打印到内存中
//输入参数:pstrTcb:新创建的任务的TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskCreatePrint(NIX_TCB * pstrTcb)
{
	if (pstrTcb != (NIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s is created! Tick is: %d", pstrTcb->pucTaskName, NIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nFail to create task! Tick is: %d", NIX_GetSystemTick());
	}
}

/**********************************************/
//函数功能:将任务切换过程打印到内存中
//输入参数:pstrOldTcb:切换前的任务TCB指针
//         pstrNewTcb:切换后的任务TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskSwitchPrint(NIX_TCB * pstrOldTcb, NIX_TCB * pstrNewTcb)
{
	//不打印串口打印任务的切换过程,将此任务定义为空闲任务
	if (pstrOldTcb == gpstrSerialTaskTcb) {
		pstrOldTcb = NIX_GetIdleTcb();
	}

	if (pstrNewTcb == gpstrSerialTaskTcb) {
		pstrNewTcb = NIX_GetIdleTcb();
	}
	//同一个任务切换不打印信息
	if (pstrNewTcb == pstrOldTcb) {
		return;
	}

	if (pstrOldTcb != (NIX_TCB *) NULL) {
		DEV_PutStrToMem((U8 *)
				"\r\nTask %s ----> Task %s ! Tick is: %d",
				pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName, NIX_GetSystemTick());
	} else {
		DEV_PutStrToMem((U8 *)
				"\r\nTask NULL ----> Task %s ! Tick is: %d",
				pstrNewTcb->pucTaskName, NIX_GetSystemTick());
	}

}

/**********************************************/
//函数功能:将任务删除过程打印到内存中
//输入参数:pstrTcb:被删除的任务的TCB指针
//返回值  :none
/**********************************************/
void TEST_TaskDeletePrint(NIX_TCB * pstrTcb)
{
	DEV_PutStrToMem((U8 *) "\r\nTask %s is deleted! Tick is: %d", pstrTcb->pucTaskName, NIX_GetSystemTick());
}
