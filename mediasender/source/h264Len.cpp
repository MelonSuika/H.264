#include "h264Len.h"

/* 
    函数功能：指数哥伦布编码，获取ue(v)的值，此例为获取first_mb_in_slice的值

*/
u32 GolombCode(u8 *pchData, s32 i, BOOL32 bIsZero)
{
    s32 nLeadingZeroBits = -1;  // 前导零的位数
    u32 dwCodeNum = 0;          // CodeNum值
    u32 dwTail = 0;             // 编码后缀
    u32 dwOffset = 0;           // 移位偏移量
    u8 wPos = 0x80;
    u32 dwOffsetByte  = 0;

    if (bIsZero)
    {
        return !(pchData[i+5] & wPos);
    }
    /* 得到前导零的数量 */
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

    /* 后缀计算 */
    for(s32 k = nLeadingZeroBits; k >= 0; k--)
    {
        dwOffset = 2 * nLeadingZeroBits - k + 1;
        dwTail += (u32)pow(2.0, k - 1) * (pchData[i + 5 + dwOffset / 8] & (wPos >> ((dwOffset % 8) ? 1 : 0)));
    }

    /* ue(v)的值计算 */
    dwCodeNum = (u32)pow(2.0, (s32)nLeadingZeroBits) - 1 + dwTail;

    return dwCodeNum;
}

/* 
  函数功能：创建帧长度文件并解析帧长度(适用于h264) 

*/
void CreateFrameLen(const s8 * pchFileName)
{
    /* 打开h264文件并获取文件长度 */
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

    /* 遍历分析码流 */
    for(s32 i = 0; i < nFileLen - 5; i++)
    {
        /* 查询start_code_prefix 开始码 */
        if (pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            /* 判断当前Nalu类型 */
            nCurNalType = pchData[i+4] & 0x1f;
            switch(nCurNalType)
            {
                /* 若未定义类型，输出告警 */
                case NALU_UNDEFINED:
                    printf("%d ~ %d data is undefined\n", i, i);
                    continue;
                /* 不分片slice */
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
                /* slice数据分割A */
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
                /* slice数据分割B */
                case NALU_TYPE_DPB:
                    //printf("DPB sliceid = %d\n", GolombCode(pchData, i));
                    break;
                /* slice数据分割C */
                case NALU_TYPE_DPC:
                    //printf("DPC sliceid = %d\n", GolombCode(pchData, i));
                    break;
                /* IDR类型 */
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
