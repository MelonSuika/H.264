#include "h264Len.h"

/* 
    函数功能：指数哥伦布编码，获取ue(v)的值，此例为获取first_mb_in_slice的值

*/
u32 GolombCode(u8 *pbyData, s32 nBytePosition, BOOL32 bIsZero)
{
    s32 nLeadingZeroBits = -1;  // 前导零的位数
    u32 dwCodeNum = 0;          // CodeNum值
    u32 dwTail = 0;             // 编码后缀
    u32 dwOffset = 0;           // 移位偏移量
    u8  byPos = 0x80;           // & 变量
    u32 dwOffsetByte  = 0;      // 偏移字节数

    if (bIsZero)
    {
        return !(pbyData[nBytePosition+5] & byPos);
    }
    /* 得到前导零的数量 */
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

    /* 后缀计算 */
    for(s32 k = nLeadingZeroBits; k >= 0; k--)
    {
        dwOffset = 2 * nLeadingZeroBits - k + 1;
        dwTail += (u32)pow(2.0, k - 1) * (pbyData[nBytePosition + 5 + dwOffset / 8] & (byPos >> ((dwOffset % 8) ? 1 : 0)));
    }

    /* ue(v)的值计算 */
    dwCodeNum = (u32)pow(2.0, (s32)nLeadingZeroBits) - 1 + dwTail;

    return dwCodeNum;
}

/* 
  函数功能：创建帧长度文件并解析帧长度(适用于h264) 

*/
BOOL32 CreateFrameLenFile(s8 const* pchFileName)
{
    BOOL32 bRet         = FALSE; // 整个帧长度创建与解析成功与否
    FILE *fp            = NULL; 
    u8 *pbyData         = NULL; // 读入的媒体文件数据
    s8 *pchLenFileName  = NULL; // 媒体长度文件名
    do
    {
        /* 打开h264文件并获取文件长度 */
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

        /* 遍历分析码流 */
        for(s32 i = 0; i < nFileLen - 5; i++)
        {
            /* 查询start_code_prefix 开始码 */
            if (pbyData[i] == 0 && pbyData[i+1] == 0 && pbyData[i+2] == 0 && pbyData[i+3] == 1)
            {
                /* 判断当前Nalu类型 */
                nCurNalType = pbyData[i+4] & 0x1f;
                switch(nCurNalType)
                {
                    /* 若未定义类型，输出告警 */
                    case NALU_UNDEFINED:
                        printf("%d data is undefined\n", i);
                        continue;
                    /* 不分片slice */
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
                    /* slice数据分割A */
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
                    /* slice数据分割B */
                    case NALU_TYPE_DPB:
                        break;
                    /* slice数据分割C */
                    case NALU_TYPE_DPC:
                        break;
                    /* IDR类型 */
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
