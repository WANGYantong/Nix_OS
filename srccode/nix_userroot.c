#include "nix_userroot.h"

/**********************************************/
//函数功能:根任务，用户代码从此执行
//输入参数:none
//返回值  :none
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
//函数功能:空闲任务
//输入参数:pvPara:任务入口参数指针
//返回值  :none
/**********************************************/
void NIX_IdleTask(void *pvPara)
{
	while (1) {
		;
	}
}
