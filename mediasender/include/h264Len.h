#ifndef _H264LEN_H
#define _H264LEN_H

#include <cstring>
#include <stdlib.h>
#include <math.h>
#include "kdvsys.h"
#include "common.h"

#ifndef WIN32
#include "glib.h"
#endif


#define NALU_UNDEFINED      0
#define NALU_TYPE_SLICE     1
#define NALU_TYPE_DPA       2
#define NALU_TYPE_DPB       3
#define NALU_TYPE_DPC       4
#define NALU_TYPE_IDR       5
#define NALU_TYPE_SEI       6
#define NALU_TYPE_SPS       7
#define NALU_TYPE_PPS       8
#define NALU_TYPE_AUD       9
#define NALU_TYPE_EOSEQ     10
#define NALU_TYPE_EOSTREAM  11
#define NALU_TYPE_FILL      12



#define MAX_FRAME_LEN       10
#define GOLOMB_IS_ZERO      TRUE


/*
    函数名：CreateFrameLen
    函数功能：创建帧长度文件并填入帧长度
    参数：
        pchFileName,媒体文件名
    返回值：
        成功返回ture,失败返回false
    注：
        slice数据分割的情况未测试，只有理论代码
        该函数适用于H264
*/
BOOL32 CreateFrameLenFile(const s8 *pchFileName);

/* 
    函数名：GolombCode
    函数功能：指数哥伦布编码，获取ue(v)的值，此例为获取first_mb_in_slice的值
    参数：
        pchData, H264视频文件名
        nBytePosition, 字节流中的位置
        bIsZero, 是否只判断first_mb_in_slice为0，true为只判断是否为0，false计算具体值
    返回值：
        Golomb编码的数值
*/
u32 GolombCode(u8 *pchData, s32 nBytePosition, BOOL32 bIsZero);






#endif
