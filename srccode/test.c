#include "test.h"

NIX_QUE *gpstrSerialMsgQue;	/* ���ڴ�ӡ��Ϣ����ָ�� */
NIX_TCB *gpstrSerialTaskTcb;	/* ���ڴ�ӡ����TCBָ�� */


/**********************************************/
//��������:��������1
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void
TEST_TestTask1 (void *pvPara)
{
  while (1)
    {
      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask1 will run 2s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* ��������2�� */
      TEST_TaskRun (2000);

      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask1 will delay 5s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* �����ӳ�5�� */
      (void) NIX_TaskDelay (500);

      /* ����70����˳������� */
      if (NIX_GetSystemTick () > 7000)
	{
	  break;
	}
    }
}

/**********************************************/
//��������:��������2
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void
TEST_TestTask2 (void *pvPara)
{
  while (1)
    {
      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask2 will run 0.5s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* ��������0.5�� */
      TEST_TaskRun (500);

      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask2 will delay 0.5s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* �����ӳ�2�� */
      (void) NIX_TaskDelay (200);

      /* ����50����˳������� */
      if (NIX_GetSystemTick () > 5000)
	{
	  break;
	}
    }
}

/**********************************************/
//��������:��������3
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void
TEST_TestTask3 (void *pvPara)
{
  while (1)
    {
      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask3 will run 0.5s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* ��������0.2�� */
      TEST_TaskRun (200);

      /* �����ӡ */
      DEV_PutStrToMem ((U8 *) "\r\nTask3 will delay 0.5s! Tick is: %d",
		       NIX_GetSystemTick ());

      /* �����ӳ�0.4�� */
      (void) NIX_TaskDelay (40);

      /* ����30����˳������� */
      if (NIX_GetSystemTick () > 3000)
	{
	  break;
	}
    }
}

/**********************************************/
//��������:���ڴ�ӡ���񣬴Ӷ����л�ȡ��Ҫ��ӡ����Ϣ���壬�������е����ݴ�ӡ������
//�������:pvPara:�������ָ��
//����ֵ  :none
/**********************************************/
void
TEST_SerialPrintTask (void *pvPara)
{
  NIX_LIST *pstrMsgQueNode;
  MSGBUF *pstrMsgBuf;

  /* �Ӷ���ѭ����ȡ��Ϣ */
  while (1)
    {
      /* �Ӷ����л�ȡ��һ����Ҫ��ӡ����Ϣ, �򴮿ڴ�ӡ��Ϣ���� */
      if (NIX_QueGet (gpstrSerialMsgQue, &pstrMsgQueNode, QUEWAITFEV) ==
	  RTN_SUCD)
	{
	  pstrMsgBuf = (MSGBUF *) pstrMsgQueNode;

	  /* �������е����ݴ�ӡ������ */
	  DEV_PrintMsg (pstrMsgBuf->aucBuf, pstrMsgBuf->ucLength);

	  /* ������Ϣ�е����ݷ������, �ͷŻ��� */
	  DEV_BufferFree (&gstrBufPool, pstrMsgQueNode);
	}
    }

}

/**********************************************/
//��������:ģ���������к���
//�������:uiMs:Ҫ�ӳٵ�ʱ�䣬��λms
//����ֵ  :none
/**********************************************/
void
TEST_TaskRun (U32 uiMs)
{
  DEV_DelayMs (uiMs);
}

/**********************************************/
//��������:�����񴴽����̴�ӡ���ڴ���
//�������:pstrTcb:�´����������TCBָ��
//����ֵ  :none
/**********************************************/
void
TEST_TaskCreatePrint (NIX_TCB * pstrTcb)
{
  if (pstrTcb != (NIX_TCB *) NULL)
    {
      DEV_PutStrToMem ((U8 *)
		       "\r\nTask %s 0x%08X is created! Tick is: %d",
		       pstrTcb->pucTaskName, pstrTcb, NIX_GetSystemTick ());
    }
  else
    {
      DEV_PutStrToMem ((U8 *)
		       "\r\nFail to create task! Tick is: %d",
		       NIX_GetSystemTick ());
    }
}

/**********************************************/
//��������:�������л����̴�ӡ���ڴ���
//�������:pstrOldTcb:�л�ǰ������TCBָ��
//         pstrNewTcb:�л��������TCBָ��
//����ֵ  :none
/**********************************************/
void
TEST_TaskSwitchPrint (NIX_TCB * pstrOldTcb, NIX_TCB * pstrNewTcb)
{
  //��ӡCPUռ����
  TEST_PrintCpuUtilize ();
  //����ӡ���ڴ�ӡ������л�����,����������Ϊ��������
  if (pstrOldTcb == gpstrSerialTaskTcb)
    {
      pstrOldTcb = NIX_GetIdleTcb ();
    }

  if (pstrNewTcb == gpstrSerialTaskTcb)
    {
      pstrNewTcb = NIX_GetIdleTcb ();
    }
  //ͬһ�������л�����ӡ��Ϣ
  if (pstrNewTcb == pstrOldTcb)
    {
      return;
    }

  if (pstrOldTcb != (NIX_TCB *) NULL)
    {
      DEV_PutStrToMem ((U8 *)
		       "\r\nTask %s ----> Task %s ! Tick is: %d",
		       pstrOldTcb->pucTaskName, pstrNewTcb->pucTaskName,
		       NIX_GetSystemTick ());
    }
  else
    {
      DEV_PutStrToMem ((U8 *)
		       "\r\nTask NULL ----> Task %s ! Tick is: %d",
		       pstrNewTcb->pucTaskName, NIX_GetSystemTick ());
    }

}

/**********************************************/
//��������:������ɾ�����̴�ӡ���ڴ���
//�������:pstrTcb:��ɾ���������TCBָ��
//����ֵ  :none
/**********************************************/
void
TEST_TaskDeletePrint (NIX_TCB * pstrTcb)
{
  DEV_PutStrToMem ((U8 *) "\r\nTask %s is deleted! Tick is: %d",
		   pstrTcb->pucTaskName, NIX_GetSystemTick ());
}

/**********************************************/
//��������:������������CPUռ����
//�������:none
//����ֵ  :none
/**********************************************/
void
TEST_PrintCpuUtilize (void)
{
  static U32 suiLastTime = 0;
  U32 uiTime;
  NIX_LIST *pstrTaskList;
  NIX_TCB *pstrTcb;

  /* ��ȡ��ǰʱ�� */
  uiTime = NIX_GetSystemTick () / 100;

  /* ����1��ʱ�� */
  if (uiTime != suiLastTime)
    {
      pstrTaskList = NIX_GetTaskLinkRoot ();

      while (NULL !=
	     (pstrTaskList =
	      NIX_ListNextNodeEmpInq (NIX_GetTaskLinkRoot (), pstrTaskList)))
	{
	  pstrTcb = ((NIX_TCBQUE *) pstrTaskList)->pstrTcb;
	  DEV_PutStrToMem ((U8 *) "\r\nTask %s CPU utilization is %d%%",
			   pstrTcb->pucTaskName, NIX_GetCpuShare (pstrTcb));
	}

      /* ����ʱ�� */
      suiLastTime = uiTime;
    }
}
