
#ifndef NIX_DEBUG_INNER_H
#define NIX_DEBUG_INNER_H

#ifdef NIX_DEBUGCONTEXT

#define VALIDTASKNAMEMAXLEN     20	//任务名称的最大长度

//定义程序空间范围，MDK软件中可以在project options的Target栏中查看
#define ROMVALIDADDRSTART       0x08000000	//程序空间起始地址
#define ROMVALIDADDRLEN         0x40000	//程序空间的大小
#define ROMVALIDADDREND         (ROMVALIDADDRSTART+ROMVALIDADDRLEN-1)	//程序空间结束地址

//环形缓冲的结构：NIX_CONTHEAD+(NIX_CONTMSG1+MSG1)+(NIX_CONTMSG2+MSG2)······
//维护环形缓冲中的记录信息，位于缓冲结构的最低端
typedef struct nix_conthead {
	U32 uiAbsAddr;		//记录信息的起始地址，绝对地址
	U32 uiRelAddr;		//记录信息的当前地址，相对地址
	U32 uiLen;		//记录信息的长度
	U32 uiRemLen;		//记录信息的剩余长度
	VFUNC1 vfSendChar;	//发送字符的钩子变量
} NIX_CONTHEAD;

//信息头结构体
typedef struct nix_contmsg {
	U32 uiLen;		//保存数据的长度
	U32 uiPreAddr;		//上个记录地址
	U32 uiPreTcb;		//前一个任务的TCB
	U32 uiNextTcb;		//后一个任务的TCB
	STACKREG strReg;	//保存的寄存器组
} NIX_CONTMSG;

#endif

//栈检查填充的数据
#ifdef NIX_DEBUGSTACKCHECK
#define STACKCHECKVALUE     0xCCCCCCCC
#endif

/****************************************************************/
#ifdef NIX_DEBUGCONTEXT
extern NIX_CONTHEAD *gpstrContext;
#endif

/****************************************************************/
extern void NIX_SaveTaskContext(void);
extern void NIX_FaultIsrPrint(void);

#ifdef NIX_DEBUGCONTEXT
extern void NIX_TaskContextInit(void);
extern void NIX_PrintTaskContext(void);
extern void NIX_CoverFisrtContext(void);
extern void NIX_SaveLastContext(U32 uiStackLen);
extern U32 NIX_SaveOneContextData(U32 uiAddr, U32 uiData);
extern U32 NIX_GetOneContextData(U32 uiAddr, U32 * puiData);
extern void NIX_PrintContext(void);
extern void NIX_SendChar(U8 ucChar);
extern void NIX_SendString(U8 * pvStringPt, ...);
extern void NIX_IfValidString(U8 * *pucString);
#endif

#ifdef NIX_DEBUGSTACKCHECK
extern void NIX_TaskStackCheckInit(NIX_TCB* pstrTcb);
#endif

#endif
