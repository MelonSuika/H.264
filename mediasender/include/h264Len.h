#ifndef _H264LEN_H
#define _H264LEN_H

#include "string.h"
#include "stdlib.h"
#include "math.h"
#include "kdvsys.h"


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

#define SAFE_FCLOSE(p)      if (p){fclose(p), p = NULL;}
#define SAFE_DELETEA(p)     if (p){delete []p, p = NULL;}
#define SAFE_DELETE(p)      if (p){delete p, p = NULL;}

#define MAX_FRAME_LEN       10
#define GOLOMB_IS_ZERO      TRUE


/*
    函数功能：创建帧长度文件并填入帧长度
    参数：
        pchFileName,媒体文件名
*/
void CreateFrameLen(const s8* pchFileName);

/* 
    函数功能：指数哥伦布编码，获取ue(v)的值，此例为获取first_mb_in_slice的值
    参数：
        pchData, H264视频文件名
        i, 字节流中的位置
        bIsZero, 是否只判断first_mb_in_slice为0，true为只判断是否为0，false计算具体值
    返回值：
        Golomb编码的数值
*/
u32 GolombCode(u8 *pchData, s32 i, BOOL32 bIsZero);






#endif
