#include "device.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

BUFPOOL gstrBufPool;		//消息缓冲池

/**********************************************/
//函数功能:初始化软件
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_SoftwareInit(void)
{
	DEV_BufferInit(&gstrBufPool);	//初始化消息缓冲池

	gpstrSerialMsgQue = NIX_QueCreate((NIX_QUE *) NULL, QUEPRIO);	//创建打印消息的队列

	gpstrSemMut = NIX_SemCreat((NIX_SEM *) NULL, SEMPRIINH | SEMMUT | SEMPRIO, SEMFULL);
#ifdef NIX_INCLUDETASKHOOK
	NIX_TaskCreateHookAdd(TEST_TaskCreatePrint);
	NIX_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
	NIX_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/**********************************************/
//函数功能:初始化硬件
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_HardwareInit(void)
{
	DEV_UartInit();
}

/**********************************************/
//函数功能:初始化串口
//输入参数:none
//返回值  :none
/**********************************************/
void DEV_UartInit(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	USART_InitTypeDef USART_InitStructure;

	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_USART1 | RCC_APB2Periph_AFIO, ENABLE);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);

	USART_StructInit(&USART_InitStructure);
	USART_Init(USART1, &USART_InitStructure);
	USART_Cmd(USART1, ENABLE);

}

/**********************************************/
//函数功能:向串口发送一个字符
//输入参数:ucChar:待输出的字符
//返回值  :none
/**********************************************/
void DEV_PutChar(U8 ucChar)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ucChar);
}

/**********************************************/
//函数功能:将字符串打印到内存，该函数入口参数与printf参数一致
//输入参数:pvStringPt:第一个参数的指针，必须为字符串指针
//         ...:其他参数
//返回值  :none
/**********************************************/
void DEV_PutStrToMem(U8 * pvStringPt, ...)
{
	MSGBUF *pstrMsgBuf;
	va_list args;

	/* 申请buf, 用来存放需要打印的字符 */
	pstrMsgBuf = (MSGBUF *) DEV_BufferAlloc(&gstrBufPool);
	if (NULL == pstrMsgBuf) {
		return;
	}

	/* 将字符串打印到内存 */
	va_start(args, pvStringPt);
	(void) vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
	va_end(args);

	/* 填充buf参数 */
	pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

	/* 将buf挂入队列 */
	(void) NIX_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);


}

/**********************************************/
//函数功能:从内存中指定位置向串口打印数据
//输入参数:pucBuf:需要打印数据存放的缓冲地址
//         uiLen:需要打印的数据长度，单位:字节
//返回值  :none
/**********************************************/
void DEV_PrintMsg(U8 * pucBuf, U32 uiLen)
{
	U32 uiCounter;
	uiCounter = 0;

	while (uiLen > uiCounter) {
		(void) DEV_PutChar(pucBuf[uiCounter++]);
	}
}

/**********************************************/
//函数功能:初始化消息缓冲池
//输入参数:pstrBufPool:消息缓冲池指针
//返回值  :none
/**********************************************/
void DEV_BufferInit(BUFPOOL * pstrBufPool)
{
	U32 i;

	NIX_ListInit(&pstrBufPool->strFreeList);

	for (i = 0; i < BUFPOOLNUM; i++) {
		NIX_ListNodeAdd(&pstrBufPool->strFreeList, &pstrBufPool->astrBufPool[i].strList);
	}
}


/**********************************************/
//函数功能:从消息缓冲池中申请消息缓冲
//输入参数:pstrBufPool:消息缓冲池指针
//返回值  :分配的消息缓冲指针
/**********************************************/
MSGBUF *DEV_BufferAlloc(BUFPOOL * pstrBufPool)
{
	MSGBUF *pstrBuf;

	(void) NIX_IntLock();

	pstrBuf = (MSGBUF *) NIX_ListNodeDelete(&pstrBufPool->strFreeList);

	(void) NIX_IntUnLock();

	return pstrBuf;
}


/**********************************************/
//函数功能:将消息缓冲释放回消息缓冲池
//输入参数:pstrBufPool:消息缓冲池指针
//         pstrQueNode:释放的缓冲节点指针
//返回值  :none
/**********************************************/
void DEV_BufferFree(BUFPOOL * pstrBufPool, NIX_LIST * pstrQueNode)
{
	(void) NIX_IntLock();

	NIX_ListNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

	(void) NIX_IntUnLock();
}
