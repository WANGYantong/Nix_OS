#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* 串口打印消息队列指针 */
NIX_TCB *gpstrSerialTaskTcb;	/* 串口打印任务TCB指针 */


/**********************************************/
//函数功能:测试任务1
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask1(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* 打印栈信息 */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask1 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* 任务运行0.5秒 */
		TEST_TaskRun(500);

		/* 获取系统时间 */
		uiTick = NIX_GetSystemTick();

		/* 系统运行时间在10~20秒之间 */
		if ((uiTick >= 1000) && (uiTick < 2000)) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}
		/* 系统运行时间在20~30秒之间 */
		else if ((uiTick >= 2000) && (uiTick < 3000)) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());
		}
		/* 系统运行时间超过30秒 */
		else if (uiTick >= 3000) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask1 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());
		}

		/* 任务延迟2秒 */
		(void) NIX_TaskDelay(200);

		/* 打印栈信息 */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask1 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}


}


/**********************************************/
//函数功能:测试任务2
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask2(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* 打印栈信息 */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask2 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* 任务运行0.5秒 */
		TEST_TaskRun(500);

		/* 获取系统时间 */
		uiTick = NIX_GetSystemTick();

		/* 系统运行时间在10~20秒之间 */
		if ((uiTick >= 1000) && (uiTick < 2000)) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());
		}
		/* 系统运行时间在20~30秒之间 */
		else if ((uiTick >= 2000) && (uiTick < 3000)) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());
		}
		/* 系统运行时间超过30秒 */
		else if (uiTick >= 3000) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask2 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}

		/* 任务延迟2秒 */
		(void) NIX_TaskDelay(200);

		/* 打印栈信息 */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask2 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}
}

/**********************************************/
//函数功能:测试任务3
//输入参数:pvPara:任务入口指针
//返回值  :none
/**********************************************/
void TEST_TestTask3(void *pvPara)
{
	U32 uiStackRemainLen;
	U32 uiTick;

	/* 打印栈信息 */
	uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
	DEV_PutStrToMem((U8 *) "\r\nTask3 stack remain %d bytes, Tick is: %d", uiStackRemainLen, NIX_GetSystemTick());

	while (1) {
		/* 任务运行0.5秒 */
		TEST_TaskRun(500);

		/* 获取系统时间 */
		uiTick = NIX_GetSystemTick();

		/* 系统运行时间超过10秒 */
		if (uiTick >= 1000) {
			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 60 value is %d, Tick is: %d",
					TEST_Add(60), NIX_GetSystemTick());

			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 80 value is %d, Tick is: %d",
					TEST_Add(80), NIX_GetSystemTick());

			/* 打印计算数值 */
			DEV_PutStrToMem((U8 *) "\r\nTask3 1 add to 40 value is %d, Tick is: %d",
					TEST_Add(40), NIX_GetSystemTick());
		}

		/* 任务延迟2秒 */
		(void) NIX_TaskDelay(200);

		/* 打印栈信息 */
		uiStackRemainLen = NIX_TaskStackCheck(NIX_GetCurrentTcb());
		DEV_PutStrToMem((U8 *) "\r\nTask3 stack remain %d bytes, Tick is: %d",
				uiStackRemainLen, NIX_GetSystemTick());
	}

}

/***********************************************************************************
函数功能: 测试函数, 采用递归调用计算累加和.
入口参数: none.
返 回 值: 计算结果.
***********************************************************************************/
U32 TEST_Add(U32 uiNum)
{
	if (1 == uiNum) {
		return 1;
	}

	return uiNum + TEST_Add(uiNum - 1);
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
		if (NIX_QueGet(gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV) == RTN_SUCD) {
			pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

			/* 将缓冲中的数据打印到串口 */
			DEV_PrintMsg(pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

			/* 缓冲消息中的数据发送完毕, 释放缓冲 */
			DEV_BufferFree(&gstrBufPool, pstrMsgQueNode);
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
				"\r\nTask %s 0x%08X is created! Tick is: %d", pstrTcb->pucTaskName, pstrTcb,
				NIX_GetSystemTick());
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
