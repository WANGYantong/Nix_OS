#include "device.h"
#include <stdio.h>
#include <stdarg.h>
#include <string.h>

BUFPOOL gstrBufPool;		//��Ϣ�����

/**********************************************/
//��������:��ʼ�����
//�������:none
//����ֵ  :none
/**********************************************/
void DEV_SoftwareInit(void)
{
	DEV_BufferInit(&gstrBufPool);	//��ʼ����Ϣ�����

	gpstrSerialMsgQue = NIX_QueCreate((NIX_QUE *) NULL, QUEPRIO);	//������ӡ��Ϣ�Ķ���

	gpstrSemMut = NIX_SemCreat((NIX_SEM *) NULL, SEMPRIINH | SEMMUT | SEMPRIO, SEMFULL);
#ifdef NIX_INCLUDETASKHOOK
	NIX_TaskCreateHookAdd(TEST_TaskCreatePrint);
	NIX_TaskSwitchHookAdd(TEST_TaskSwitchPrint);
	NIX_TaskDeleteHookAdd(TEST_TaskDeletePrint);

#endif
}

/**********************************************/
//��������:��ʼ��Ӳ��
//�������:none
//����ֵ  :none
/**********************************************/
void DEV_HardwareInit(void)
{
	DEV_UartInit();
}

/**********************************************/
//��������:��ʼ������
//�������:none
//����ֵ  :none
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
//��������:�򴮿ڷ���һ���ַ�
//�������:ucChar:��������ַ�
//����ֵ  :none
/**********************************************/
void DEV_PutChar(U8 ucChar)
{
	while (USART_GetFlagStatus(USART1, USART_FLAG_TXE) == RESET);
	USART_SendData(USART1, ucChar);
}

/**********************************************/
//��������:���ַ�����ӡ���ڴ棬�ú�����ڲ�����printf����һ��
//�������:pvStringPt:��һ��������ָ�룬����Ϊ�ַ���ָ��
//         ...:��������
//����ֵ  :none
/**********************************************/
void DEV_PutStrToMem(U8 * pvStringPt, ...)
{
	MSGBUF *pstrMsgBuf;
	va_list args;

	/* ����buf, ���������Ҫ��ӡ���ַ� */
	pstrMsgBuf = (MSGBUF *) DEV_BufferAlloc(&gstrBufPool);
	if (NULL == pstrMsgBuf) {
		return;
	}

	/* ���ַ�����ӡ���ڴ� */
	va_start(args, pvStringPt);
	(void) vsprintf(pstrMsgBuf->aucBuf, pvStringPt, args);
	va_end(args);

	/* ���buf���� */
	pstrMsgBuf->ucLength = strlen(pstrMsgBuf->aucBuf);

	/* ��buf������� */
	(void) NIX_QuePut(gpstrSerialMsgQue, &pstrMsgBuf->strList);


}

/**********************************************/
//��������:���ڴ���ָ��λ���򴮿ڴ�ӡ����
//�������:pucBuf:��Ҫ��ӡ���ݴ�ŵĻ����ַ
//         uiLen:��Ҫ��ӡ�����ݳ��ȣ���λ:�ֽ�
//����ֵ  :none
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
//��������:��ʼ����Ϣ�����
//�������:pstrBufPool:��Ϣ�����ָ��
//����ֵ  :none
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
//��������:����Ϣ�������������Ϣ����
//�������:pstrBufPool:��Ϣ�����ָ��
//����ֵ  :�������Ϣ����ָ��
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
//��������:����Ϣ�����ͷŻ���Ϣ�����
//�������:pstrBufPool:��Ϣ�����ָ��
//         pstrQueNode:�ͷŵĻ���ڵ�ָ��
//����ֵ  :none
/**********************************************/
void DEV_BufferFree(BUFPOOL * pstrBufPool, NIX_LIST * pstrQueNode)
{
	(void) NIX_IntLock();

	NIX_ListNodeAdd(&pstrBufPool->strFreeList, pstrQueNode);

	(void) NIX_IntUnLock();
}
