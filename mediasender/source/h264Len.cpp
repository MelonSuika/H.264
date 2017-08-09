#include "h264Len.h"


void CreateFrameLen(const s8 * pchFileName)
{
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
    fclose(fp);



}
