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
    �������ܣ�����֡�����ļ�������֡����
    ������
        pchFileName,ý���ļ���
*/
void CreateFrameLen(const s8* pchFileName);

/* 
    �������ܣ�ָ�����ײ����룬��ȡue(v)��ֵ������Ϊ��ȡfirst_mb_in_slice��ֵ
    ������
        pchData, H264��Ƶ�ļ���
        i, �ֽ����е�λ��
        bIsZero, �Ƿ�ֻ�ж�first_mb_in_sliceΪ0��trueΪֻ�ж��Ƿ�Ϊ0��false�������ֵ
    ����ֵ��
        Golomb�������ֵ
*/
u32 GolombCode(u8 *pchData, s32 i, BOOL32 bIsZero);






#endif
