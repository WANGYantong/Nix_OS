#ifndef NIX_USERDEF_H
#define NIX_USERDEF_H

#include "stm32f10x.h"

//���޸ĵĺ궨��
#define NIX_INCLUDETASKHOOK	//�����Ӻ����궨��
//#define NIX_TASKPRIOINHER     //�������ȼ��̳к궨��
//#define NIX_TASKROUNDROBIN    //ͬ�����ȼ�������ת����
#define NIX_DEBUGCONTEXT	//��¼�Ĵ�����ջ��Ϣ
//#define NIX_DEBUGSTACKCHECK   //�������ջ�ռ�ʹ�����
#define NIX_DEBUGCPUSHARE	//CPUʹ����ͳ�ƹ���

#ifdef NIX_DEBUGCONTEXT
#define NIX_CONTEXTADDR     0x2000B000	//��¼�ڴ����ʼ��ַ
#define NIX_CONTEXTLEN      0x1000	//��¼�ĳ��ȣ���λ���ֽ�
#endif

#define TICK                10	//����ϵͳ�������ڣ���λ:ms
#define CORECLOCKPMS        72000000	//оƬ�ں���Ƶ�ʣ���λ:Hz

#define ROOTTASKNAME        "Root"	//����������
#define IDLETASKNAME        "Idle"	//������������

#define ROOTTASKSTACK       600	//������ջ��С����λ:�ֽ�
#define IDLETASKSTACK       600	//��������ջ��С����λ:�ֽ�

#define STACKALIGNMASK      ALIGN8MASK	//ջ��������
#define PRIORITYNUM         PRIORITY8	//֧�ֵ����ȼ���Ŀ

#endif
