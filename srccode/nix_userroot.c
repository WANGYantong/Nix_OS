#include "nix_userroot.h"

/**********************************************/
//��������:�������û�����Ӵ�ִ��
//�������:none
//����ֵ  :none
/**********************************************/
void NIX_RootTask(void)
{
	DEV_SoftwareInit();

	DEV_HardwareInit();

	(void) NIX_TaskCreat("Task1", TEST_TestTask1, NULL, NULL, TASKSTACK, 5, NULL);

	(void) NIX_TaskCreat("Task2", TEST_TestTask2, NULL, NULL, TASKSTACK, 4, NULL);

	//(void) NIX_TaskCreat("Task3", TEST_TestTask3, NULL, NULL, TASKSTACK, 3, NULL);

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
