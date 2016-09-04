
#ifndef NIX_DEBUG_INNER_H
#define NIX_DEBUG_INNER_H

#ifdef NIX_DEBUGCONTEXT

#define VALIDTASKNAMEMAXLEN     20	//�������Ƶ���󳤶�

//�������ռ䷶Χ��MDK����п�����project options��Target���в鿴
#define ROMVALIDADDRSTART       0x08000000	//����ռ���ʼ��ַ
#define ROMVALIDADDRLEN         0x40000	//����ռ�Ĵ�С
#define ROMVALIDADDREND         (ROMVALIDADDRSTART+ROMVALIDADDRLEN-1)	//����ռ������ַ

//���λ���Ľṹ��NIX_CONTHEAD+(NIX_CONTMSG1+MSG1)+(NIX_CONTMSG2+MSG2)������������
//ά�����λ����еļ�¼��Ϣ��λ�ڻ���ṹ����Ͷ�
typedef struct nix_conthead {
	U32 uiAbsAddr;		//��¼��Ϣ����ʼ��ַ�����Ե�ַ
	U32 uiRelAddr;		//��¼��Ϣ�ĵ�ǰ��ַ����Ե�ַ
	U32 uiLen;		//��¼��Ϣ�ĳ���
	U32 uiRemLen;		//��¼��Ϣ��ʣ�೤��
	VFUNC1 vfSendChar;	//�����ַ��Ĺ��ӱ���
} NIX_CONTHEAD;

//��Ϣͷ�ṹ��
typedef struct nix_contmsg {
	U32 uiLen;		//�������ݵĳ���
	U32 uiPreAddr;		//�ϸ���¼��ַ
	U32 uiPreTcb;		//ǰһ�������TCB
	U32 uiNextTcb;		//��һ�������TCB
	STACKREG strReg;	//����ļĴ�����
} NIX_CONTMSG;

#endif

//ջ�����������
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
