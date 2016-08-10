#include "global.h"

/**********************************************/
//函数功能:延时函数，延迟时间与编译选项和芯片时钟有关
//输入参数:uiMs:延迟的时间，单位Ms
//返回值  :none
/**********************************************/
void DEV_DelayMs(U32 uiMs)
{
    U32 i;
    U32 j;

    j = 5540*uiMs;

    for(i = 0; ; i++)
    {
        if(i == j)
        {
            break;
        }
    }
}
