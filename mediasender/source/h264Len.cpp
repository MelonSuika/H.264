#include "h264Len.h"

/* 
    �������ܣ�ָ�����ײ����룬��ȡue(v)��ֵ������Ϊ��ȡfirst_mb_in_slice��ֵ

*/
u32 GolombCode(u8 *pchData, s32 i, BOOL32 bIsZero)
{
    s32 nLeadingZeroBits = -1;  // ǰ�����λ��
    u32 dwCodeNum = 0;          // CodeNumֵ
    u32 dwTail = 0;             // �����׺
    u32 dwOffset = 0;           // ��λƫ����
    u8 wPos = 0x80;
    u32 dwOffsetByte  = 0;

    if (bIsZero)
    {
        return !(pchData[i+5] & wPos);
    }
    /* �õ�ǰ��������� */
    for(s32 b = 0; !b; nLeadingZeroBits++)
    {
        if (!((nLeadingZeroBits + 1) % 8) && (nLeadingZeroBits + 1))
        {
            dwOffsetByte++;
            wPos = 0x80;
        }
        b = pchData[i + 5 + dwOffsetByte] & wPos;
        wPos >>= 1;
    }

    wPos = 0x80;

    /* ��׺���� */
    for(s32 k = nLeadingZeroBits; k >= 0; k--)
    {
        dwOffset = 2 * nLeadingZeroBits - k + 1;
        dwTail += (u32)pow(2.0, k - 1) * (pchData[i + 5 + dwOffset / 8] & (wPos >> ((dwOffset % 8) ? 1 : 0)));
    }

    /* ue(v)��ֵ���� */
    dwCodeNum = (u32)pow(2.0, (s32)nLeadingZeroBits) - 1 + dwTail;

    return dwCodeNum;
}

/* 
  �������ܣ�����֡�����ļ�������֡����(������h264) 

*/
void CreateFrameLen(const s8 * pchFileName)
{
    /* ��h264�ļ�����ȡ�ļ����� */
    FILE *fp = fopen(pchFileName, "rb");
    if (!fp)
    {
        printf("open fail\n");
        return ;
    }
    fseek(fp, 0, SEEK_END);
    s32 nFileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    u8 *pchData = new u8[nFileLen];
    if (!pchData)
    {
        printf("new fail\n");
        return ;
    }
    fread(pchData, 1, nFileLen, fp);
    SAFE_FCLOSE(fp);

    s32 nCurNalType = 0;
    s32 nPreNalType = 0;
    s32 nPreFrameStartPos = 0;
    s32 nFrameLen = 0;

    s8 *pchLenFileName = NULL;
    pchLenFileName  = new s8[strlen(pchFileName) + 10];
    strcpy(pchLenFileName, pchFileName);
    strcat(pchLenFileName, ".len");
    
    fp = fopen(pchLenFileName, "a");
    if (!fp)
    {
        printf("open lenFile fail\n");
        return ;
    }

    s8 *pchFrameLen = new s8[MAX_FRAME_LEN];

    /* ������������ */
    for(s32 i = 0; i < nFileLen - 5; i++)
    {
        /* ��ѯstart_code_prefix ��ʼ�� */
        if (pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            /* �жϵ�ǰNalu���� */
            nCurNalType = pchData[i+4] & 0x1f;
            switch(nCurNalType)
            {
                /* ��δ�������ͣ�����澯 */
                case NALU_UNDEFINED:
                    printf("%d ~ %d data is undefined\n", i, i);
                    continue;
                /* ����Ƭslice */
                case NALU_TYPE_SLICE:
                    if (nCurNalType != nPreNalType)
                    {
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;
                        }
                    }
                    else
                    {
                        
                        u32 dwCodeNum = GolombCode(pchData, i, GOLOMB_IS_ZERO);

                        printf("slice nalu, fmis is %d\n", dwCodeNum);
                        if (!dwCodeNum)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }
                    }
                    break;
                /* slice���ݷָ�A */
                case NALU_TYPE_DPA:
                    if (nCurNalType != nPreNalType)
                    {
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }
                    }
                    else
                    {
                        u32 dwCodeNum = GolombCode(pchData, i, GOLOMB_IS_ZERO);
                        printf("idr nalu, fmis is %d\n", dwCodeNum);
                        if (!dwCodeNum)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }
                    }
                    break;
                /* slice���ݷָ�B */
                case NALU_TYPE_DPB:
                    //printf("DPB sliceid = %d\n", GolombCode(pchData, i));
                    break;
                /* slice���ݷָ�C */
                case NALU_TYPE_DPC:
                    //printf("DPC sliceid = %d\n", GolombCode(pchData, i));
                    break;
                /* IDR���� */
                case NALU_TYPE_IDR:
                    if (nCurNalType != nPreNalType)
                    {
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }

                    }
                    else
                    {
                        u32 dwCodeNum = GolombCode(pchData, i, GOLOMB_IS_ZERO);
                        printf("idr nalu, fmis is %d\n", dwCodeNum);
                        if (!dwCodeNum)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }
                    }
                    break;
                case NALU_TYPE_SEI:
                    if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if (1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
                        {
                            printf("write error\n");
                        }

                        nPreFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_SPS:
                    if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if (1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
                        {
                            printf("write error\n");
                        }

                        nPreFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_PPS:
                    if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if (1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
                        {
                            printf("write error\n");
                        }
                        
                        nPreFrameStartPos = i;
                    }
                    break;
                default:
                    break;

            }
            nPreNalType = nCurNalType;
        }
    }
    printf("%d\n", nFileLen - nPreFrameStartPos);
    sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", nFileLen - nPreFrameStartPos);
    fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
    SAFE_DELETEA(pchLenFileName);
    SAFE_DELETEA(pchFrameLen);
    SAFE_FCLOSE(fp);
    return ;
}
