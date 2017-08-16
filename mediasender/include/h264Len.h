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
    ��������CreateFrameLen
    �������ܣ�����֡�����ļ�������֡����
    ������
        pchFileName,ý���ļ���
    ����ֵ��
        �ɹ�����ture,ʧ�ܷ���false
    ע��
        slice���ݷָ�����δ���ԣ�ֻ�����۴���
        �ú���������H264
*/
BOOL32 CreateFrameLenFile(const s8 *pchFileName);

/* 
    ��������GolombCode
    �������ܣ�ָ�����ײ����룬��ȡue(v)��ֵ������Ϊ��ȡfirst_mb_in_slice��ֵ
    ������
        pchData, H264��Ƶ�ļ���
        nBytePosition, �ֽ����е�λ��
        bIsZero, �Ƿ�ֻ�ж�first_mb_in_sliceΪ0��trueΪֻ�ж��Ƿ�Ϊ0��false�������ֵ
    ����ֵ��
        Golomb�������ֵ
*/
u32 GolombCode(u8 *pchData, s32 nBytePosition, BOOL32 bIsZero);






#endif
