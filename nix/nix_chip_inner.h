#ifndef NIX_CHIP_INNER_H
#define NIX_CHIP_INNER_H

//�жϷ��ص�ַ
#define RTN_HANDLER             0xFFFFFFF1	//����handlerģʽ
#define RTN_THREAD_MSP          0xFFFFFFF9	//����threadģʽ����ʹ��MSP
#define RTN_THREAD_PSP          0xFFFFFFFD	//����threadģʽ����ʹ��PSP

//XPSR�ж�����λ
#define XPSR_EXTMASK            0x1FF	//XPSR�е�9λΪIPSR�������жϺ�

//��ջָ��ѡ��
#define MSP_STACK               0	//����ջָ��
#define PSP_STACK               1	//���̶�ջָ��

//ϵͳ����ģʽ
#define PRIVILEGED              0	//��Ȩ��ģʽ
#define UNPRIVILEGED            1	//�û���ģʽ

extern U32 guiIntLockCounter;

extern void NIX_TaskStackInit(NIX_TCB * pstrTcb, VFUNC vfFuncPointer, void *pvPara);
extern void NIX_SystemHardwareInit(void);
extern void NIX_TickTimerInit(void);
extern void NIX_PendSvIsrInit(void);
extern void NIX_SetChipWorkMode(U32 uiMode);
extern void NIX_TaskSwiSched(void);
extern void NIX_TaskOccurSwi(U32 uiSwiNo);

#endif
