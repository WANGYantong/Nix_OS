#ifndef NIX_CORE_C_H
#define NIX_CORE_C_H

extern void NIX_TaskTick(void);
extern U32 NIX_GetSystemTick(void);
extern NIX_TCB *NIX_GetCurrentTcb(void);
extern NIX_TCB *NIX_GetRootTcb(void);
extern NIX_TCB *NIX_GetIdleTcb(void);

#endif
