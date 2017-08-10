#include "h264Len.h"

/* 
    �������ܣ����ײ����룬��ȡue(v)��ֵ������Ϊ��ȡfirst_mb_in_slice��ֵ

*/
u32 GolombCode(u8 *pchData, s32 i)
{
    u32 dwLeadingZeroBits = -1; // ǰ�����λ��
    u32 dwCodeNum = 0;          // CodeNumֵ
    u32 dwTail = 0;             // �����׺
    u32 dwOffset = 0;           // ��λƫ����
    u8 wPos = 0x80;
    u32 j  = 0;
    /* �õ�ǰ��������� */
    for(s32 b = 0; !b; dwLeadingZeroBits++)
    {
        if(!((dwLeadingZeroBits + 1) % 8) && (dwLeadingZeroBits + 1))
        {
            j++;
            wPos = 0x80;
        }
        b = pchData[i + 5 + j] & wPos;
        wPos >>= 1;
    }

    wPos = 0x80;

    /* ��׺���� */
    for(s32 k = dwLeadingZeroBits; k >= 0; k--)
    {
        dwOffset = 2 * dwLeadingZeroBits - k + 1;
        dwTail += (u32)pow(2.0, k - 1) * (pchData[i + 5 + dwOffset / 8] & (wPos >> ((dwOffset % 8)?1:0)));
    }

    /* ue(v)��ֵ���� */
    dwCodeNum = (u32)pow(2.0, (s32)dwLeadingZeroBits) - 1 + dwTail;

    return dwCodeNum;
}

/* 
  �������ܣ�����֡�����ļ�������֡����(������h264) 

*/
void CreateFrameLen(const s8 * pchFileName)
{
    /* ��h264�ļ�����ȡ�ļ����� */
    FILE *fp = fopen(pchFileName, "rb");
    if(NULL == fp)
    {
        printf("open fail\n");
        return ;
    }
    fseek(fp, 0, SEEK_END);
    s32 nFileLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    u8 *pchData = new u8[nFileLen];
    if(NULL == pchData)
    {
        printf("new fail\n");
        return ;
    }
    fread(pchData, 1, nFileLen, fp);
    if(fp)
    {
        printf("close success\n");
        fclose(fp);
        fp = NULL;
    }

    s32 nCurNalType = 0;
    s32 nPreNalType = 0;
    s32 nPreFrameStartPos = 0;
    s32 nFrameLen = 0;

    s8 *pchLenFileName = NULL;
    pchLenFileName  = new s8[strlen(pchFileName) + 10];
    strcpy(pchLenFileName, pchFileName);
    strcat(pchLenFileName, ".len");
    
    fp = fopen(pchLenFileName, "a");
    if(!fp)
    {
        printf("open lenFile fail\n");
    }


    s8 *pchFrameLen = new s8[MAX_FRAME_LEN];

    /* ������������ */
    for(s32 i = 0; i < nFileLen - 5; i++)
    {
        /* ��ѯstart_code_prefix ��ʼ�� */
        if(pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            /* �жϵ�ǰNalu���� */
            nCurNalType = pchData[i+4] & 0x1f;
            switch(nCurNalType)
            {
                /* ��δ�������ͣ�����澯 */
                case NALU_UNDEFINED:
                    printf("%d ~ %d data is undefined\n", i, i);
                    continue;
                    break;
                /* ����Ƭslice */
                case NALU_TYPE_SLICE:
                    if(nCurNalType != nPreNalType)
                    {
                        if(nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
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
                        
                        u32 dwCodeNum = GolombCode(pchData, i);

                        printf("slice nalu, fmis is %d\n", dwCodeNum);
                        if(!dwCodeNum)
                        {
                            printf("%d\n", i - nPreFrameStartPos);
                            sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                            fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp);
                            printf("str = %s\n", pchFrameLen);
                            nPreFrameStartPos = i;

                        }
                    }
                    break;
                /* IDR���� */
                case NALU_TYPE_IDR:
                    if(nCurNalType != nPreNalType)
                    {
                        if(nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
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
                        u32 dwCodeNum = GolombCode(pchData, i);
                        printf("idr nalu, fmis is %d\n", dwCodeNum);
                        if(!dwCodeNum)
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
                    if(nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if(1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
                        {
                            printf("write error\n");
                        }

                        nPreFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_SPS:
                    if(nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if(1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
                        {
                            printf("write error\n");
                        }

                        nPreFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_PPS:
                    if(nPreNalType >= NALU_TYPE_SLICE && nPreNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPreFrameStartPos);
                        sprintf_s(pchFrameLen, MAX_FRAME_LEN, "%d\n", i - nPreFrameStartPos);
                        printf("strlen : %d\n", strlen(pchFrameLen));
                        printf("str = %s\n", pchFrameLen);
                        if(1 != fwrite(pchFrameLen, strlen(pchFrameLen), 1, fp))
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
    delete []pchFrameLen;
    if(fp)
    {
        fclose(fp);
    }
    return ;
}
