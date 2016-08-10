#ifndef NIX_CHIP_H
#define NIX_CHIP_H

//软中断服务号，0x00000000~0x1FFFFFFF为操作系统保留
#define SWI_TASKSCHED               0x10001001      //任务调度

extern void NIX_IntPendSvSet(void);
extern U32  NIX_RunInInt(void);
extern U32  NIX_IntLock(void);
extern U32  NIX_IntUnLock(void);

#endif
