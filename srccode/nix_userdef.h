#ifndef NIX_USERDEF_H
#define NIX_USERDEF_H

#include "stm32f10x.h"

//可修改的宏定义
#define NIX_TASKPRIOINHER	//任务优先级继承宏定义
#define NIX_INCLUDETASKHOOK	//任务钩子函数宏定义
#define NIX_TASKROUNDROBIN	//同等优先级任务轮转调度

#define TICK                10	//操作系统调度周期，单位:ms
#define CORECLOCKPMS        72000000	//芯片内核振荡频率，单位:Hz

#define ROOTTASKNAME        "Root"	//根任务名称
#define IDLETASKNAME        "Idle"	//空闲任务名称

#define ROOTTASKSTACK       600	//根任务栈大小，单位:字节
#define IDLETASKSTACK       600	//空闲任务栈大小，单位:字节

#define STACKALIGNMASK      ALIGN8MASK	//栈对齐掩码
#define PRIORITYNUM         PRIORITY8	//支持的优先级数目

#endif
