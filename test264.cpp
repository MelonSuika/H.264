// test264.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "string.h"
#include "stdlib.h"
#include "math.h"

#define NALU_TYPE_SLICE 1
#define NALU_TYPE_DPA 2
#define NALU_TYPE_DPB 3
#define NALU_TYPE_DPC 4
#define NALU_TYPE_IDR 5
#define NALU_TYPE_SEI 6
#define NALU_TYPE_SPS 7
#define NALU_TYPE_PPS 8
#define NALU_TYPE_AUD 9
#define NALU_TYPE_EOSEQ 10
#define NALU_TYPE_EOSTREAM 11
#define NALU_TYPE_FILL 12



typedef unsigned char u8;


int _tmain(int argc, _TCHAR* argv[])
{

    FILE *fp = fopen("./4cif-h264.data", "rb");
    if(NULL == fp)
    {
        printf("open fail\n");
        return 0;
    }
    fseek(fp, 0, SEEK_END);
    int nLen = ftell(fp);
    fseek(fp, 0, SEEK_SET);
    u8 *pchData = new u8[nLen];
    fread(pchData, 1, nLen, fp);
    fclose(fp);

    int i = 0;
    int nNalType = 0, nPrevFrameStartPos = 0, nCurNalType = 0, nFrameLen = 0, flag = 0, cnt = 0;
    fp = fopen("len.txt", "w");
    for(; i < nLen - 5; i++)
    {
        if(pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            nNalType = pchData[i+4] & 0x1f;
            switch(nNalType)
            {
                case NALU_TYPE_SLICE:
                    if(nNalType != nCurNalType)
                    {
                        if(nCurNalType >= NALU_TYPE_SLICE && nCurNalType <= NALU_TYPE_IDR)
                        {
                            nCurNalType = nNalType;
                            printf("%d\n", i - nPrevFrameStartPos);
                            nPrevFrameStartPos = i;
                        }

                    }
                    else
                    {
                        cnt++;
                        //first_mb_in_slice, ue(v)
                        nCurNalType = nNalType;
                        int leadingZeroBits = -1, codeNum = 0, tail = 0;
                        unsigned int nPos = 0x80000000;
                        unsigned int *dwFmis = NULL;
                        dwFmis = new unsigned int;
                        memcpy(dwFmis, pchData + i + 5, sizeof(unsigned int));
                        for(int b = 0; !b; leadingZeroBits++)
                        {
                            b = *dwFmis & nPos;
                            nPos >>= 1;
                        }
                        for(int j = leadingZeroBits; j >= 0; j--)
                        {
                            tail += (int)pow(2.0, j);
                        }
                        codeNum = (int)pow((float)2, leadingZeroBits) - 1 + tail;
                        if(cnt < 50)
                        printf("first_mb_in_slice:%d\n", codeNum);
                        if(!codeNum)
                        {
                            printf("%d\n", i - nPrevFrameStartPos);
                            nPrevFrameStartPos = i;
                        }

                    }
                    break;
                case NALU_TYPE_IDR:
                    if(nNalType != nCurNalType)
                    {
                        if(nCurNalType >= NALU_TYPE_SLICE && nCurNalType <= NALU_TYPE_IDR)
                        {
                            nCurNalType = nNalType;
                            printf("%d\n", i - nPrevFrameStartPos);
                            nPrevFrameStartPos = i;
                        }

                    }
                    else
                    {
                        cnt++;
                        //first_mb_in_slice, ue(v)
                        nCurNalType = nNalType;
                        int leadingZeroBits = -1, codeNum = 0, tail = 0;
                        unsigned int nPos = 0x80000000;
                        unsigned int *dwFmis = NULL;
                        dwFmis = new unsigned int;
                        memcpy(dwFmis, pchData + i + 5, sizeof(unsigned int));
                        for(int b = 0; !b; leadingZeroBits++)
                        {
                            b = *dwFmis & nPos;
                            nPos >>= 1;
                        }
                        for(int j = leadingZeroBits; j >= 0; j--)
                        {
                            tail += (int)pow(2.0, j);
                        }
                        codeNum = (int)pow((float)2, leadingZeroBits) - 1 + tail;
                        if(cnt < 50)
                        printf("first_mb_in_slice:%d\n", codeNum);
                        if(!codeNum)
                        {
                            printf("%d\n", i - nPrevFrameStartPos);
                            nPrevFrameStartPos = i;
                        }
                    }
                    break;
                case NALU_TYPE_SEI:
                    if(nCurNalType >= NALU_TYPE_SLICE && nCurNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPrevFrameStartPos);
                        nPrevFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_SPS:
                    if(nCurNalType >= NALU_TYPE_SLICE && nCurNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPrevFrameStartPos);
                        nPrevFrameStartPos = i;
                    }
                    break;
                case NALU_TYPE_PPS:
                    if(nCurNalType >= NALU_TYPE_SLICE && nCurNalType <= NALU_TYPE_IDR)
                    {
                        printf("%d\n", i - nPrevFrameStartPos);
                        nPrevFrameStartPos = i;
                    }
                    break;
                default:
                    break;
            }
            
        } 
    }
    printf("%d\n", nLen - nFrameLen);
    fclose(fp);
    getchar();
	return 0;
}

