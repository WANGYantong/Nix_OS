#ifndef NIX_USERDEF_H
#define NIX_USERDEF_H

#include "stm32f10x.h"

//���޸ĵĺ궨��
#define NIX_TASKPRIOINHER	//�������ȼ��̳к궨��
#define NIX_INCLUDETASKHOOK	//�����Ӻ����궨��
#define NIX_TASKROUNDROBIN	//ͬ�����ȼ�������ת����

#define TICK                10	//����ϵͳ�������ڣ���λ:ms
#define CORECLOCKPMS        72000000	//оƬ�ں���Ƶ�ʣ���λ:Hz

#define ROOTTASKNAME        "Root"	//����������
#define IDLETASKNAME        "Idle"	//������������

#define ROOTTASKSTACK       600	//������ջ��С����λ:�ֽ�
#define IDLETASKSTACK       600	//��������ջ��С����λ:�ֽ�

#define STACKALIGNMASK      ALIGN8MASK	//ջ��������
#define PRIORITYNUM         PRIORITY8	//֧�ֵ����ȼ���Ŀ

#endif
