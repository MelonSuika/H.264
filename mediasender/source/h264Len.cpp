#include "h264Len.h"

/* 
    �������ܣ�ָ�����ײ����룬��ȡue(v)��ֵ������Ϊ��ȡfirst_mb_in_slice��ֵ

*/
u32 GolombCode(u8 *pbyData, s32 nBytePosition, BOOL32 bIsZero)
{
    s32 nLeadingZeroBits = -1;  // ǰ�����λ��
    u32 dwCodeNum = 0;          // CodeNumֵ
    u32 dwTail = 0;             // �����׺
    u32 dwOffset = 0;           // ��λƫ����
    u8  byPos = 0x80;           // & ����
    u32 dwOffsetByte  = 0;      // ƫ���ֽ���

    if (bIsZero)
    {
        return !(pbyData[nBytePosition+5] & byPos);
    }
    /* �õ�ǰ��������� */
    for(s32 b = 0; !b; nLeadingZeroBits++)
    {
        if (!((nLeadingZeroBits + 1) % 8) && (nLeadingZeroBits + 1))
        {
            dwOffsetByte++;
            byPos = 0x80;
        }
        b = pbyData[nBytePosition + 5 + dwOffsetByte] & byPos;
        byPos >>= 1;
    }

    byPos = 0x80;

    /* ��׺���� */
    for(s32 k = nLeadingZeroBits; k >= 0; k--)
    {
        dwOffset = 2 * nLeadingZeroBits - k + 1;
        dwTail += (u32)pow(2.0, k - 1) * (pbyData[nBytePosition + 5 + dwOffset / 8] & (byPos >> ((dwOffset % 8) ? 1 : 0)));
    }

    /* ue(v)��ֵ���� */
    dwCodeNum = (u32)pow(2.0, (s32)nLeadingZeroBits) - 1 + dwTail;

    return dwCodeNum;
}

/* 
  �������ܣ�����֡�����ļ�������֡����(������h264) 

*/
BOOL32 CreateFrameLenFile(s8 const* pchFileName)
{
    BOOL32 bRet         = FALSE; // ����֡���ȴ���������ɹ����
    FILE *fp            = NULL; 
    u8 *pbyData         = NULL; // �����ý���ļ�����
    s8 *pchLenFileName  = NULL; // ý�峤���ļ���
    do
    {
        /* ��h264�ļ�����ȡ�ļ����� */
        fp = fopen(pchFileName, "rb");
        if (!fp)
        {
            printf("open fail\n");
            break;
        }
        fseek(fp, 0, SEEK_END);
        s32 nFileLen = ftell(fp);
        fseek(fp, 0, SEEK_SET);
        pbyData = new u8[nFileLen];
        if (!pbyData)
        {
            printf("new fail\n");
            break;
        }
        fread(pbyData, 1, nFileLen, fp);
        SAFE_FCLOSE(fp);

        s32 nCurNalType = 0;
        s32 nPreNalType = 0;
        s32 nPreFrameStartPos = 0;
        s32 nFrameLen = 0;

        pchLenFileName  = new s8[MAX_FILENAME];
        g_snprintf(pchLenFileName, MAX_FILENAME, "%s.len", pchFileName);
    
        fp = fopen(pchLenFileName, "a");
        if (!fp)
        {
            printf("open lenFile fail\n");
            break;
        }

        /* ������������ */
        for(s32 i = 0; i < nFileLen - 5; i++)
        {
            /* ��ѯstart_code_prefix ��ʼ�� */
            if (pbyData[i] == 0 && pbyData[i+1] == 0 && pbyData[i+2] == 0 && pbyData[i+3] == 1)
            {
                /* �жϵ�ǰNalu���� */
                nCurNalType = pbyData[i+4] & 0x1f;
                switch(nCurNalType)
                {
                    /* ��δ�������ͣ�����澯 */
                    case NALU_UNDEFINED:
                        printf("%d data is undefined\n", i);
                        continue;
                    /* ����Ƭslice */
                    case NALU_TYPE_SLICE:
                        if (nCurNalType != nPreNalType)
                        {
                            if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                            {
                                printf("%d\n", i - nPreFrameStartPos);
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
                                nPreFrameStartPos = i;
                            }
                        }
                        else
                        {
                        
                            u32 dwCodeNum = GolombCode(pbyData, i, GOLOMB_IS_ZERO);

                            printf("slice nalu, fmis is %d\n", dwCodeNum);
                            if (!dwCodeNum)
                            {
                                printf("%d\n", i - nPreFrameStartPos);
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
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
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
                                nPreFrameStartPos = i;
                            }
                        }
                        else
                        {
                            u32 dwCodeNum = GolombCode(pbyData, i, GOLOMB_IS_ZERO);
                            printf("idr nalu, fmis is %d\n", dwCodeNum);
                            if (!dwCodeNum)
                            {
                                printf("%d\n", i - nPreFrameStartPos);
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
                                nPreFrameStartPos = i;

                            }
                        }
                        break;
                    /* slice���ݷָ�B */
                    case NALU_TYPE_DPB:
                        break;
                    /* slice���ݷָ�C */
                    case NALU_TYPE_DPC:
                        break;
                    /* IDR���� */
                    case NALU_TYPE_IDR:
                        if (nCurNalType != nPreNalType)
                        {
                            if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                            {
                                printf("%d\n", i - nPreFrameStartPos);
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
                                nPreFrameStartPos = i;
                            }
                        }
                        else
                        {
                            u32 dwCodeNum = GolombCode(pbyData, i, GOLOMB_IS_ZERO);
                            printf("idr nalu, fmis is %d\n", dwCodeNum);
                            if (!dwCodeNum)
                            {
                                printf("%d\n", i - nPreFrameStartPos);
                                fprintf(fp, "%d\n", i - nPreFrameStartPos);
                                nPreFrameStartPos = i;

                            }
                        }
                        break;
                    case NALU_TYPE_SEI:
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            fprintf(fp, "%d\n", i - nPreFrameStartPos);
                            nPreFrameStartPos = i;
                        }
                        break;
                    case NALU_TYPE_SPS:
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            fprintf(fp, "%d\n", i - nPreFrameStartPos);
                            nPreFrameStartPos = i;
                        }
                        break;
                    case NALU_TYPE_PPS:
                        if (nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            fprintf(fp, "%d\n", i - nPreFrameStartPos);
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
        fprintf(fp, "%d\n", nFileLen - nPreFrameStartPos);
        bRet = TRUE;
    }while(0);
    SAFE_DELETEA(pbyData);
    SAFE_DELETEA(pchLenFileName);
    SAFE_FCLOSE(fp);
    return bRet;
}
