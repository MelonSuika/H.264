#ifndef _SENDER_H
#define _SENDER_H

#include "kdvsys.h"
#include "kdvmedianet.h"
#include "kdvtype.h"
#include "kdvdef.h"
#include "mediatype.h"
#include "osp.h"
#include "h264Len.h"
#include "common.h"
#ifdef WIN32
#include <atlconv.h>
#include "io.h"
#else
#include "glib.h"
#include "unistd.h"
#endif

#define INIT_NO_ERROR                (u16)(0)
#define GET_EDIT_ERROR          (u16)(1)
#define GET_SND_ERROR           (u16)(2)
#define CREATE_SND_ERROR        (u16)(3)
#define NO_LENFILE_ERROR        (u16)(4)
#define CREATE_LEN_FILE_ERROR   (u16)(5)
#define NO_DATFILE_ERROR        (u16)(6)

#define DATA_LEN                (512 * 1024)                  // 一帧数据最大长度
#define LEN_LEN                 (32)                          // 媒体文件长度长度
#define MAX_FILENAME            (256)                         // 文件名最大长度
#define MAX_OUTINFO             (256)                         // 一包打印信息
#define MAX_IP                  (64)                          // IP长度
#define MAX_MEDIATYPENAME       (64)                          // 最大媒体类型名
#define SRC_PORT                (u16)(7200)                   // 原始端口
#define PORT_ADD_NUM            (2)                           // 多路发送时，端口间隔
#define MAX_LINE                (500)                         // 控件最大显示行数

#define F_OK        0
#define X_OK        1
#define W_OK        2
#define R_OK        4  
#define WR_OK       6

class CSender
{
public:
    CKdvMediaSnd *m_pcSender;       // 媒体发送对象
private:
    s8 *m_pchMediaDatName;          // 媒体文件名
    s8 *m_pchMediaLenName;          // 媒体长度文件名

    s32 m_nWidth;                   // 画面宽
    s32 m_nHeight;                  // 画面高
    s32 m_nMediaType;               // 媒体类型编号
    s8 *m_pchMediaTypeName;         // 媒体类型名称
    BOOL32 m_bIsVid;                // 是否视频文件
    BOOL32 m_bIsRepeat;             // 是否重复发送
    s32 m_nFramerate;               // 帧率

    s8 *m_pchSendDstIP;             // 目的端IP
    s8 *m_pchSendSrcIP;             // 发送端IP
    s32 m_nSendDstPort;             // 目的端端口
    s32 m_nSendSrcPort;             // 发送端端口

    u32 m_dwLoopTime;               // 发送延时
    u32 m_dwFrameSize;              // 帧大小
    u32 m_dwNetBand;                // 带宽,bps

    FILE* m_pInDat;                 // 数据文件指针
    FILE* m_pInTxt;                 // 长度文件指针

    s8* m_pchData;                  // 发送数据包
    s8* m_pchLen;                   // 发送长度包

    u32 m_dwOffset;                 // 偏移量

    BOOL32 m_bIsStop;               // 停止发送标志位

#ifdef WIN32
    CEdit *m_pWndEdit;              // 对话框窗体信息编辑栏控制
    u32 m_dwLines;                  // 刷新行数
#endif

public:
    CSender(void);
    ~CSender();

public:
    u16 Init(void);                     // 对象初始化
    void GetConfig(void);               // 获取默认配置参数
    void SetMnetNetParam(void);         // 设置网参
    void Send(void);                    // 发送包
    void SendAudio(void);               // 发送音频包
    void Stop(void);                    // 停止发送
    void SetDstIpPort(s32 nPort);       // 设置远端端口
    s32  GetDstIpPort(void);            // 获取远端端口
    void SetSrcIpPort(s32 nPort);       // 设置本地端口
    s32  GetSrcIpPort(void);            // 获取本地端口
    void SetMediaDatName(const s8* pchFilename); // 设置媒体文件名
    s8 * GetMediaDatName(void);         // 获取媒体文件名
    s8 * GetMediaLenName(void);         // 获取媒体长度文件名
    void SetSrcIp(const s8* pchSrcIp);        // 设置发送端ip
    s8 * GetSrcIp(void);                // 获取发送端ip
    void SetDstIp(const s8* pchDstIp);        // 设置目的端(接收方)ip
    s8 * GetDstIp(void);                // 获取目的端(接收方)ip
    void SetFrameRate(s32 nFramerate);  // 设置发送帧率
    s32  GetFrameRate(void);            // 获取发送帧率
    void SetRepeat(BOOL32 bIsRepeat);   // 设置重复发送
    BOOL32 GetRepeat(void);             // 获取重复发送
    void SetMediaType(s32 nMediaType);  // 设置媒体类型
    s8*  GetMediaTypeName(void);        // 获取媒体类型名
    s32  GetMeidaTypeNum(void);            // 获取媒体类型号
    void SetIsStop(BOOL32 bIsStop);     // 设置标志位
    void SetNetBand(u32 dwNetBand);     // 设置带宽
    u32  GetNetBand(void);              // 获取带宽
    void PrintInfo(void);               // 打印发送对象信息
    s8*  GetPrintInfo(void);            // 获取需要打印的信息
    void PrintLCommon(const s8 *pchInfo);     // 打印一行常规信息
#ifdef WIN32
    void SetWndEdit(CEdit *pWndEdit);   // 获取绑定的控件变量
    void SetOutLines(s32 nLines);       // 设置统计信息刷新行数
#endif
    
};
#endif
