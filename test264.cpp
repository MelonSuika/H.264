// test264.cpp : Defines the entry point for the console application.
//
#include "stdafx.h"
#include "string.h"
#include "stdlib.h"


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
    FILE *fp = fopen("./1024x768.h264", "rb");
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
    /*
    for(; i < 256; i++)
    {
        printf("%3x ", pchData[i]);
        if(!((i+1) % 16))
        {
            printf("\n");
        }
    }
    */

    int nNalType = 0, nPrevFrameStartPos = 0, nCurNalType = 0;
    fp = fopen("len.txt", "w");
    for(; i < nLen - 5; i++)
    {
        if(pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            nNalType = pchData[i+4] & 0x1f;
            if(nNalType == NALU_TYPE_SPS || nNalType == NALU_TYPE_PPS)
            {
                printf("pos:%d is %s\n", i, (nNalType == NALU_TYPE_SPS)?"sps":"pps");
                nPrevFrameStartPos = i;    
            }
            else
            {
                break;
            }
        } 
    }
    for(int cnt = 0; i < nLen - 5; i++)
    {
        if(pchData[i] == 0 && pchData[i+1] == 0 && pchData[i+2] == 0 && pchData[i+3] == 1)
        {
            nNalType = pchData[i+4] & 0x1f;
            if(nNalType >= NALU_TYPE_SLICE && nNalType <= NALU_TYPE_IDR)
            {
                if(pchData[i+5] & 0x)
            }
        } 
    }
    fclose(fp);
    getchar();
	return 0;
}

