#include "nix_userroot.h"

/**********************************************/
//��������:�������û�����Ӵ�ִ��
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_RootTask(void)
{
	NIX_TASKOPT strTaskOpt;

	DEV_SoftwareInit();

	DEV_HardwareInit();

	/* ��������1 */
	(void) NIX_TaskCreat("Test1", TEST_TestTask1, NULL, NULL, TASKSTACK, 4, NULL);

	strTaskOpt.ucTaskSta = TASKDELAY;
	strTaskOpt.uiDelayTick = 1000;

	/* ��������2, �ӳ�10������� */
	(void) NIX_TaskCreat("Test2", TEST_TestTask2, NULL, NULL, TASKSTACK, 3, &strTaskOpt);

	strTaskOpt.ucTaskSta = TASKDELAY;
	strTaskOpt.uiDelayTick = 200;

	/* ��������3, �ӳ�2������� */
	(void) NIX_TaskCreat("Test3", TEST_TestTask3, NULL, NULL, TASKSTACK, 2, &strTaskOpt);

	gpstrSerialTaskTcb = NIX_TaskCreat("SrlPrt", TEST_SerialPrintTask, NULL, NULL, TASKSTACK, 6, NULL);

	DEV_PutStrToMem((U8 *) "\r\nNIX is running! Tick is: %d", NIX_GetSystemTick());

}


/**********************************************/
//��������:��������
//�������:pvPara:������ڲ���ָ��
//����ֵ  :none
/**********************************************/
void NIX_IdleTask(void *pvPara)
{
	while (1) {
		;
	}
}
