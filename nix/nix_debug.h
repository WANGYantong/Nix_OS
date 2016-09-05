
#ifndef NIX_DEBUG_H
#define NIX_DEBUG_H

#ifdef NIX_DEBUGCONTEXT
extern void NIX_SendCharFuncInit(VFUNC1 vfFuncPointer);
#endif

#ifdef NIX_DEBUGSTACKCHECK
extern U32 NIX_TaskStackCheck(NIX_TCB * pstrTcb);
#endif

#ifdef NIX_DEBUGCPUSHARE
extern U32 NIX_GetCpuShare(NIX_TCB * pstrTcb);
#endif

#endif
