#include "sender.h"
#ifdef WIN32
#include "resource.h"
#endif


#ifdef WIN32
#define g_debug         printf
#define g_warning       printf
#define g_malloc0       malloc
#define g_usleep        Sleep
#define gpointer        void*
#define g_free          free
#define g_snprintf      sprintf_s
#define LINUX_TIME      1
#else
#define LINUX_TIME      1000
#endif
#define SAFE_FCLOSE(p)          if(p){fclose(p),p=NULL;}
#define SAFE_DELETEA(p)         if(p){delete []p,p=NULL;}
#define SAFE_DELETE(p)          if(p){delete p,p=NULL;}



const s8* g_pchFileConfig   = "./config.ini";
static u32 g_dwLines = 0;
#ifdef WIN32
extern HANDLE g_hMuxtex;
#endif

CSender::CSender(void)
{
    m_pcSender  = NULL;
    m_pchMediaDatName = NULL;
    m_pchMediaLenName = NULL;
    m_nWidth    = 1680;
    m_nHeight   = 1050;
    m_nMediaType = 106;
    m_pchMediaTypeName = NULL;
    m_bIsVid    = TRUE;
    m_bIsRepeat = FALSE;
    m_nFramerate = 30;
    m_pchSendDstIP = NULL;
    m_pchSendSrcIP = NULL;
    m_nSendDstPort = 7200;
    m_nSendSrcPort = 6500;
    m_dwLoopTime = 100;
    m_dwFrameSize = 1024 * 1024;
    m_dwNetBand = 4096000;
    m_pInDat    = NULL;
    m_pInTxt    = NULL;
    m_pchData   = NULL;
    m_pchLen    = NULL;
    m_bIsStop   = FALSE;
    m_dwOffset  = 0;
#ifdef WIN32
    m_pWndEdit  = NULL;
    m_dwLines    = 500;
#endif

}

CSender::~CSender(void)
{
    SAFE_FCLOSE(m_pInDat);
    SAFE_FCLOSE(m_pInTxt);
    SAFE_DELETEA(m_pchMediaDatName);
    SAFE_DELETEA(m_pchMediaLenName);
    SAFE_DELETEA(m_pchSendDstIP);
    SAFE_DELETEA(m_pchSendSrcIP);
    SAFE_DELETEA(m_pchMediaTypeName);
    m_pcSender->RemoveNetSndLocalParam();
    CKdvMediaSnd::PutMediaSnd(m_pcSender);
}


u16 CSender::Init(void)
{
#ifdef WIN32
    m_pWndEdit = (CEdit*)AfxGetApp()->m_pMainWnd->GetDlgItem(IDC_EDITPRINT);
    if (NULL == m_pWndEdit)
    {
        PrintLCommon("获取edit控件失败");
        return GET_EDIT_ERROR;
    }
#endif
    /* 指针成员初始化，分配空间 */
    m_pchMediaDatName = new s8[MAX_FILENAME];
    m_pchMediaLenName = new s8[MAX_FILENAME];
    m_pchSendDstIP    = new s8[MAX_IP];
    m_pchSendSrcIP    = new s8[MAX_IP];
    m_pchMediaTypeName= new s8[MAX_MEDIATYPENAME];

    /* 内部获取对象 */
    m_pcSender = CKdvMediaSnd::GetMediaSnd();
    if (NULL == m_pcSender)
    {
        PrintLCommon("GetMediaSnd fail");
        return GET_SND_ERROR;
    }

    /* 读取配置文件 */
    GetConfig();

    /* 长度文件处理 */
    g_snprintf(m_pchMediaLenName, MAX_FILENAME, "%s.len", m_pchMediaDatName);
    SetMediaType(m_nMediaType);

    /* 延时和帧大小计算 */
    m_dwLoopTime = 1000 * LINUX_TIME / m_nFramerate;
    m_dwFrameSize = 1024 * 1024;
    if (!m_bIsVid)
    {
        m_dwFrameSize = 5 * 1024;
    }
    /* 创建发送模块 */
    u16 wRet = m_pcSender->Create(m_dwFrameSize, 1024 * 1024, m_nFramerate, m_nMediaType);
    if (wRet != MEDIANET_NO_ERROR)
    {
        PrintLCommon("create send module fail");
        return CREATE_SND_ERROR;

    }
    return SND_NO_ERROR;
}

// 配置文件参数获取
void CSender::GetConfig(void)
{

    /* 文件信息 */
    if (!GetRegKeyInt(g_pchFileConfig, "FILTERINFO", "WIDTH", 1920, &m_nWidth))
    {
        PrintLCommon("Read Filter WIDTH error!");
    }

    if (!GetRegKeyInt(g_pchFileConfig, "FILTERINFO", "HEIGHT", 1080, &m_nHeight))
    {
        PrintLCommon("Read Filter HEIGHT error!");
    }

    if (!GetRegKeyInt(g_pchFileConfig, "FILTERINFO", "TYPE", 106, &m_nMediaType))
    {
        PrintLCommon("Read Filter TYPE error!");
    }

    if (!GetRegKeyInt(g_pchFileConfig, "FILTERINFO", "IS_VID", 1, &m_bIsVid))
    {
        PrintLCommon("Read Filter IS_VID error!");
    }

    if (!GetRegKeyString(g_pchFileConfig, "FILTERINFO", "FILENAME", "test", m_pchMediaDatName, MAX_FILENAME - 1))
    {
        PrintLCommon("Read Filter IS_VID error!");
    }

    /* 发送信息 */
    if (!GetRegKeyString(g_pchFileConfig, "SENDINFO", "DST_IP", "127.0.0.1", m_pchSendDstIP, MAX_IP))
    {
        PrintLCommon("Read Send DST_IP error!");
    }

    if (!GetRegKeyInt(g_pchFileConfig, "SENDINFO", "DST_PORT", 0, &m_nSendDstPort))
    {
        PrintLCommon("Read Send DST_PORT error!");
    } 

    if (!GetRegKeyString(g_pchFileConfig, "SENDINFO", "SRC_IP", "127.0.0.1", m_pchSendSrcIP, MAX_IP))
    {
        s8 *pchInfo = new s8[MAX_OUTINFO];
        g_snprintf(pchInfo, MAX_OUTINFO, "use default local send addr %s", m_pchSendSrcIP);
        PrintLCommon(pchInfo);
        SAFE_DELETEA(pchInfo);
    }

    if (!GetRegKeyInt(g_pchFileConfig, "SENDINFO", "SRC_PORT", 0, &m_nSendSrcPort))
    {
        s8 *pchInfo = new s8[MAX_OUTINFO];
        g_snprintf(pchInfo, MAX_OUTINFO, "use default local send port %d", m_nSendSrcPort);
        PrintLCommon(pchInfo);
        SAFE_DELETEA(pchInfo);
    }

    if (!GetRegKeyInt(g_pchFileConfig, "SENDINFO", "REPEAT", 0, &m_bIsRepeat))
    {
        m_bIsRepeat = 0;
        s8 *pchInfo = new s8[MAX_OUTINFO];
        g_snprintf(pchInfo, MAX_OUTINFO, "use default repeat value %d\n", m_bIsRepeat);
        PrintLCommon(pchInfo);
        SAFE_DELETEA(pchInfo);
    }

    if (!GetRegKeyInt(g_pchFileConfig, "SENDINFO", "FRAMERATE", 0, &m_nFramerate))
    {
        m_nFramerate = 25;
        s8 *pchInfo = new s8[MAX_OUTINFO];
        g_snprintf(pchInfo, MAX_OUTINFO, "use default framerate value %d\n", m_nFramerate);
        PrintLCommon(pchInfo);
        SAFE_DELETEA(pchInfo);
    }



}

// 网络参数相关配置
void CSender::SetMnetNetParam(void)
{
    TMnetNetParam tNetParam = {0};
    tNetParam.m_byRemoteNum = 1;

    OSP_SET_NETADDR_PORT(&(tNetParam.m_tLocalNet.tRTPAddr), AF_INET, m_nSendSrcPort);
    OSP_SET_NETADDR_ADDR(&(tNetParam.m_tLocalNet.tRTPAddr), AF_INET, 0);
    OSP_SET_NETADDR_ADDR(&(tNetParam.m_tLocalNet.tRTCPAddr), AF_INET, 0);
    OSP_SET_NETADDR_PORT(&(tNetParam.m_tLocalNet.tRTCPAddr), AF_INET, (m_nSendSrcPort + 1));

    OSP_SET_NETADDR_PORT(&(tNetParam.m_tRemoteNet[0].tRTPAddr), AF_INET, m_nSendDstPort);
    OSP_SET_NETADDR_ADDR(&(tNetParam.m_tRemoteNet[0].tRTPAddr), AF_INET, inet_addr(m_pchSendDstIP));
    OSP_SET_NETADDR_ADDR(&(tNetParam.m_tRemoteNet[0].tRTCPAddr), AF_INET, inet_addr(m_pchSendDstIP));
    OSP_SET_NETADDR_PORT(&(tNetParam.m_tRemoteNet[0].tRTCPAddr), AF_INET, (m_nSendDstPort + 1));

    m_pcSender->SetNetSndParam(tNetParam);
    return ;
}

/* 发送媒体文件 */
void CSender::Send()
{

    m_bIsStop = FALSE;
    /* 打开文件 */
    m_pInDat = fopen(m_pchMediaDatName, "rb");
    m_pInTxt = fopen(m_pchMediaLenName, "r");
    s8 *pchInfo = new s8[MAX_OUTINFO];
    if (NULL == m_pInDat || NULL == m_pInTxt)
    {
        // error
        PrintLCommon("open file fail");
        return ;
    }
    else
    {
        PrintLCommon("open test file succeed");
    }

    /* 分配内存 */
    m_pchData = new s8[DATA_LEN];
    m_pchLen = new s8[LEN_LEN];

    if (NULL == m_pchData || NULL == m_pchLen)
    {
        //error
        printf("分配内存失败\n");
        return ;
    }
    TKdvSndStatistics tKdvSndStatistics;
    

    /* 读取并发送 */
    s32 nFrmId = 0;
    BOOL32 bFile2End = FALSE;
    u32 dwFrameId = 0;
    do
    {
        do
        {
            nFrmId++;
            s8* pbyRet = fgets(m_pchLen, LEN_LEN, m_pInTxt);
            if (!pbyRet)
            {
                bFile2End = TRUE;
                break;
            }
            s32 nFrmLen = atoi(m_pchLen);
            if (nFrmLen <= 0 || nFrmLen >= DATA_LEN)
            {
                g_snprintf(pchInfo, MAX_OUTINFO, "frame %d length %d %d\n", nFrmId, nFrmLen, DATA_LEN);
                PrintLCommon(pchInfo);
                SAFE_DELETEA(pchInfo);
                bFile2End = TRUE;
                break;
            }

            if ((u32)nFrmLen != fread(m_pchData, 1, nFrmLen, m_pInDat))
            {
                g_warning("frame %d get fail %d\n", nFrmId);
                bFile2End = TRUE;
                break;
            }
            FRAMEHDR    tFrmHdr     = {0};
            tFrmHdr.m_dwSSRC        = 1;
            tFrmHdr.m_dwDataSize    = nFrmLen;
            tFrmHdr.m_byMediaType   = m_nMediaType;
            tFrmHdr.m_pData         = (u8*)m_pchData;
            tFrmHdr.m_dwFrameID     = dwFrameId++;
            tFrmHdr.m_tVideoParam.m_wVideoWidth     = m_nWidth;
            tFrmHdr.m_tVideoParam.m_wVideoHeight    = m_nHeight;
            tFrmHdr.m_tVideoParam.m_bHighProfile    = TRUE;


            if (1 == nFrmId)
            {
                tFrmHdr.m_tVideoParam.m_bKeyFrame   = TRUE;
            }
            else
            {
                tFrmHdr.m_tVideoParam.m_bKeyFrame   = FALSE;
            }

            u16 wRet = m_pcSender->Send(&tFrmHdr);
           
            if (wRet != MEDIANET_NO_ERROR)
            {
                SAFE_DELETEA(pchInfo);
                printf("medianet send fail %d\n", wRet);
                return ;
            }
            if(nFrmId % 12 == 1)
            {
                m_pcSender->GetStatistics(tKdvSndStatistics);
                g_snprintf(pchInfo, MAX_FILENAME, "线路:%d, 已发送包数:%d,已发送帧数:%d,丢帧数:%d",
                    ((m_nSendDstPort - SRC_PORT) + PORT_ADD_NUM) / 2,
                    tKdvSndStatistics.m_dwPackSendNum, 
                    tKdvSndStatistics.m_dwFrameNum, 
                    tKdvSndStatistics.m_dwFrameLoseNum);
                PrintLCommon(pchInfo);
            }
            g_usleep(m_dwLoopTime);

        } while(!bFile2End && !m_bIsStop);

        if (TRUE == bFile2End)
        {
            bFile2End = FALSE;
            PrintLCommon("file to end, re initial");
            nFrmId = 0;
            fseek(m_pInDat, 0, SEEK_SET);
            fseek(m_pInTxt, 0, SEEK_SET);
        }
        else if (TRUE == m_bIsStop)
        {
            PrintLCommon("Stop");
            break;
        }

        g_usleep(m_dwLoopTime);	

    }while (m_bIsRepeat && !m_bIsStop);

    SAFE_DELETEA(m_pchData);
    SAFE_DELETEA(m_pchLen);

    SAFE_FCLOSE(m_pInDat);
    SAFE_FCLOSE(m_pInTxt);
    SAFE_DELETEA(pchInfo);
    return ;
}

/* 发送音频包 */
void CSender::SendAudio(void)
{





}

/* 停止发送 */
void CSender::Stop(void)
{
    if (NULL == m_pInTxt || NULL == m_pInDat)
    {
        PrintLCommon("未执行发送");
    }
    else
    {
        m_bIsStop = true;
    }
    return ;
}

/* set dst ip port */
void CSender::SetDstIpPort(s32 nPort)
{
    m_nSendDstPort = nPort;
    return;
}

/* get dst ip port */
s32 CSender::GetDstIpPort(void)
{

    return m_nSendDstPort;
}

/* set src ip port */
void CSender::SetSrcIpPort(s32 nPort)
{
    m_nSendSrcPort = nPort;
    return;
}

/* get src ip port */
s32 CSender::GetSrcIpPort()
{
    return m_nSendSrcPort;  
}

/* set mediafilename */
void CSender::SetMediaDatName(const s8* filename)
{
    g_snprintf(m_pchMediaDatName, MAX_FILENAME, "%s", filename);
    g_snprintf(m_pchMediaLenName, MAX_FILENAME, "%s.len", m_pchMediaDatName);
    return;
}

/* get mediafilename */
s8 * CSender::GetMediaDatName(void)
{
    return m_pchMediaDatName;
}

/* set send srcip */
void CSender::SetSrcIp(const s8 *pchSrcIp)
{
    if(m_pchSendSrcIP)
    {
        memset(m_pchSendSrcIP, 0,  MAX_IP - 1);
    }
    else
    {
        return ;
    }
    strncpy(m_pchSendSrcIP, pchSrcIp, MAX_IP - 1);
    return ;
}

/* get send srcip */
s8 * CSender::GetSrcIp()
{
    return m_pchSendSrcIP;
}

/* set dstip */
void CSender::SetDstIp(const s8 *pchDstIp)
{
    if(m_pchSendDstIP)
    {
        memset(m_pchSendDstIP, 0, MAX_IP - 1);
    }
    else
    {
        return ;
    }
    strncpy(m_pchSendDstIP, pchDstIp, MAX_IP - 1);
    return ;
}

/* get dstip */
s8 * CSender::GetDstIp(void)
{
    return m_pchSendDstIP;
}
/* set framerate */
void CSender::SetFrameRate(s32 nFramerate)
{
    m_nFramerate = nFramerate;
    return ;
}

/* get framerate */
s32 CSender::GetFrameRate(void)
{
    return m_nFramerate;
}

/* set Repeat */
void CSender::SetRepeat(BOOL32 bIsRepeat)
{
    m_bIsRepeat = bIsRepeat;
    return ;
}

/* get Repeat */
BOOL32 CSender::GetRepeat(void)
{
    return m_bIsRepeat;
}

/* set MediaType */
void CSender::SetMediaType(s32 nMediaType)
{
    m_nMediaType = nMediaType;
    switch (m_nMediaType)
    {
    case MEDIA_TYPE_MP4:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "MPEG-4");
        break;
    case MEDIA_TYPE_H261:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.261");
        break;
    case MEDIA_TYPE_H262:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.262(MPEG-2)");
        break;
    case MEDIA_TYPE_H263:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.263");
        break;
    case MEDIA_TYPE_H263PLUS:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.263+");
        break;
    case MEDIA_TYPE_H264:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.264");
        break;
    case MEDIA_TYPE_H264_ForHuawei:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.264(ForHuaWei)");
        break;
    case MEDIA_TYPE_FEC:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "fec custom define");
        break;
    case MEDIA_TYPE_H265:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "H.265");
        break;
    case MEDIA_TYPE_G7221C:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "G722.1.C Siren14");
        break;
    case MEDIA_TYPE_PCMA:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "G.711 Alaw  mode 5");
        break;
    case MEDIA_TYPE_PCMU:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "G.711 ulaw  mode 6");
        break;
    case MEDIA_TYPE_OPUS:
        g_snprintf(m_pchMediaTypeName, MAX_MEDIATYPENAME, "OPUS");
        break;
    default:
        break;
    }
    return ;
}


/* get MediaType */
s8* CSender::GetMediaType(void)
{
    return m_pchMediaTypeName;
}

/* print sender info */
void CSender::PrintInfo(void)
{

    printf("Media File: %s\n", m_pchMediaDatName);
    printf("Media Len File: %s\n", m_pchMediaLenName);
    printf("Width: %d\n", m_nWidth);
    printf("Height: %d\n", m_nHeight);
    printf("IS_VID: %d\n", m_bIsVid);
    printf("Type: %s(%d)\n", m_pchMediaTypeName, m_nMediaType);

    printf("Dst Ip: %s\n", m_pchSendDstIP);
    printf("Dst Port: %d\n", m_nSendDstPort);
    printf("Send Ip: %s\n", m_pchSendSrcIP);
    printf("Send Port: %d\n", m_nSendSrcPort);
    printf("Repeat: %d\n", m_bIsRepeat);
    printf("Framerate %d\n", m_nFramerate);
    return;
}

#ifdef WIN32
void CSender::SetWndEdit(CEdit *p_WndEdit)
{
    m_pWndEdit = p_WndEdit;
    return ;
}
#endif

void CSender::SetNetBand(u32 dwNetBand)
{
    m_dwNetBand = dwNetBand;
    return ;
}

u32 CSender::GetNetBand(void)
{
    return m_dwNetBand;
}

void CSender::PrintLCommon(const s8 *pchInfo)
{

#ifdef WIN32
    WaitForSingleObject(g_hMuxtex, INFINITE);
    USES_CONVERSION;
    g_dwLines++;
    if (g_dwLines >= m_dwLines)
    {   
        s32 nIndex = m_pWndEdit->LineIndex(1);
        m_pWndEdit->SetRedraw(FALSE);  
        m_pWndEdit->SetSel(0, nIndex);
        m_pWndEdit->ReplaceSel(_T(""));
        m_pWndEdit->SetSel(-1);
        m_pWndEdit->SetRedraw(TRUE);
    }
    else
    {
        m_pWndEdit->SetSel(-1);
    }
    CString cstrInfo = A2W(pchInfo);
    CString a;
    a.Format(_T(", 行号:%d"), g_dwLines);
    cstrInfo += a;
    cstrInfo += "\r\n";
    m_pWndEdit->ReplaceSel(cstrInfo);
    ReleaseMutex(g_hMuxtex);

#endif
    printf("%s\n",pchInfo);
    return ;
}

#ifdef WIN32
void CSender::SetOutLines(s32 nLines)
{
    m_dwLines = nLines;
    return ;
}
#endif
