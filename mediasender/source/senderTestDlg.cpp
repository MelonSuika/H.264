
// senderTestDlg.cpp : implementation file
//
//lint -e1046   error,��Ա�ھ�̬���������ɶ���������,����û���ھ�̬������
//lint -e1039   error,�������Ա����ʵ���������Ա
//lint -e522    warning
//lint -e110    error,win32�¾�����;���void*
//lint -e1032   error,�ú���Ϊ��Ա�����������ڲ�����
//lint -e1018   error,DWORDΪwin32�¶��������
//lint -e48     error,�ýӿڷ���ֵΪָ�������
#include "stdafx.h"
#include "senderTest.h"
#include "senderTestDlg.h"
#include "afxdialogex.h"
#include "sender.h"
#include "mediatype.h"


#ifdef _DEBUG
#define new DEBUG_NEW
#endif



static void CreateSnd(s32);
static void SndInit(s32, u32, u8, u32);
static CSender *g_pSender = NULL;
static DWORD *g_pdwThreadId = NULL;
static u32 g_dwSafeFlag;
HANDLE g_hMuxtex;



// CsenderTestDlg dialog

CsenderTestDlg::CsenderTestDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(CsenderTestDlg::IDD, pParent)
	, m_cstrMediaFile(_T(""))
	, m_cstrMeidaLenFile(_T(""))
	, m_frameRate(0)
	, m_srcPort(_T(""))
	, m_dwDstIP(0)
	, m_dstPort(_T(""))
	, m_isRepeat(0)
	, m_printInfo(_T(""))
	, m_cbmediaType(_T(""))
	, m_NetBand(0)
    , m_nSendPathNum(0)
    , m_dwBitRate(4096000)
    , m_nOutputLines(2)
    , m_dwSafeFlag(0)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CsenderTestDlg::DoDataExchange(CDataExchange* pDX)
{
    CDialogEx::DoDataExchange(pDX);
    DDX_Text(pDX, IDC_MFCEDITBROWSE1, m_cstrMediaFile);
    DDX_Text(pDX, IDC_MFCEDITBROWSE2, m_cstrMeidaLenFile);
    DDX_Text(pDX, IDC_EDIT4, m_frameRate);
    DDX_Text(pDX, IDC_EDIT1, m_srcPort);
    DDX_IPAddress(pDX, IDC_IPADDRESS2, m_dwDstIP);
    DDX_Text(pDX, IDC_EDIT2, m_dstPort);
    DDX_Text(pDX, IDC_EDIT6, m_isRepeat);
    DDX_CBString(pDX, IDC_COMBO1, m_cbmediaType);
    DDX_Text(pDX, IDC_EDIT5, m_NetBand);
    DDX_Text(pDX, IDC_EDIT7, m_nSendPathNum);
    DDX_CBIndex(pDX, IDC_COMBO2, m_nOutputLines);
    DDX_Control(pDX, IDC_COMBO2, m_cbLines);
}

BEGIN_MESSAGE_MAP(CsenderTestDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_BUTTON1, &CsenderTestDlg::OnBnClickedStartSendButton)
	ON_BN_CLICKED(IDC_BUTTON2, &CsenderTestDlg::OnBnClickedStopSendButton)
END_MESSAGE_MAP()


// CsenderTestDlg message handlers

BOOL CsenderTestDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();
	
	// Set the icon for this dialog.  The framework does this automatically
	//  when the application's main window is not a dialog
	SetIcon(m_hIcon, TRUE);			// Set big icon
	SetIcon(m_hIcon, FALSE);		// Set small icon

	// TODO: Add extra initialization here
    g_hMuxtex = CreateMutex(NULL, FALSE, NULL);
	// ��ȡ�����ļ�
	GetConfig();

	return TRUE;  // return TRUE  unless you set the focus to a control
}

void CsenderTestDlg::OnSysCommand(UINT nID, LPARAM lParam)
{

	CDialogEx::OnSysCommand(nID, lParam);
	return ;
}

// If you add a minimize button to your dialog, you will need the code below
//  to draw the icon.  For MFC applications using the document/view model,
//  this is automatically done for you by the framework.

void CsenderTestDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // device context for painting

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// Center icon in client rectangle
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// Draw the icon
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}


// The system calls this function to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CsenderTestDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CsenderTestDlg::OnCancel()
{
    if (g_dwSafeFlag)
    {
       AfxMessageBox(_T("���߳�δ�˳�,��ֹͣ����")); 
       return ;
    }
    SAFE_DELETEA(g_pSender);
    CDialog::OnCancel();

}
// read .ini
// �����ļ��������
void CsenderTestDlg::GetConfig()
{
    USES_CONVERSION;
	CFileFind finder;
	BOOL32 ifFind = finder.FindFile(_T("./config.ini"));
	if (!ifFind)
	{
		// �ļ���Ϣ
		::WritePrivateProfileStringW(_T("FILTERINFO"), _T("FILENAME"), _T("4cif-h264.data"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("FILTERINFO"), _T("WIDTH"), _T("1680"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("FILTERINFO"), _T("HEIGHT"), _T("1050"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("FILTERINFO"), _T("IS_VID"), _T("1"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("FILTERINFO"), _T("TYPE"), _T("106"), _T("./config.ini"));

		// ������Ϣ
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("DST_IP"), _T("127.0.0.1"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("DST_PORT"), _T("7200"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("SRC_IP"), _T("127.0.0.1"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("SRC_PORT"), _T("6500"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("REPEAT"), _T("1"), _T("./config.ini"));
		::WritePrivateProfileStringW(_T("SENDINFO"), _T("FRAMERATE"), _T("10"), _T("./config.ini"));
	}

    CreateSnd(1);
    // �ؼ���������
	m_cstrMediaFile.Format(_T("%s"), A2W(g_pSender[0].GetMediaDatName()));
	m_cstrMeidaLenFile = m_cstrMediaFile + _T(".len");

	// Դ��IP��ȡ,�ַ���ת��������������ת��
	m_srcPort.Format(_T("%d"), g_pSender[0].GetSrcIpPort());
		
	// Ŀ�Ķ�
	m_dwDstIP = ntohl(inet_addr(g_pSender[0].GetDstIp()));
	m_dstPort.Format(_T("%d"), g_pSender[0].GetDstIpPort());

	m_frameRate = g_pSender[0].GetFrameRate();
	m_isRepeat = g_pSender[0].GetRepeat();
	m_NetBand = g_pSender[0].GetNetBand();
	
    m_cbmediaType.Format(_T("%s"), A2W(g_pSender[0].GetMediaTypeName()));
    m_nSendPathNum = 1;
    SndInit(1, m_NetBand, m_frameRate, m_dwBitRate);
	UpdateData(false);
	return ;
}

DWORD WINAPI ThreadProcSend (LPVOID pTPrintInfo)
{
    g_dwSafeFlag++;
	((CSender*)pTPrintInfo)->Send();
    g_dwSafeFlag--;
	return 0;
}

/*
    �������ܣ���ʼ������������������
*/
void CsenderTestDlg::OnBnClickedStartSendButton()
{
    SAFE_DELETEA(g_pSender);
    UpdateData(true);
    CreateSnd(m_nSendPathNum);
    ActiveX2Snd(m_nSendPathNum);
    SndInit(m_nSendPathNum, m_NetBand, m_frameRate, m_dwBitRate);
	g_pdwThreadId = new DWORD[m_nSendPathNum];
    for(s32 i = 0; i < m_nSendPathNum; i++)
    {

        HANDLE handle = CreateThread(NULL, 
            0, 
            ThreadProcSend, 
            &g_pSender[i], 
            0, 
            &g_pdwThreadId[i]);
        if(NULL == handle)
        {
            AfxMessageBox(_T("send fail"));
        }
        else
        {
            CString str;
            str.Format(_T("handle is %u\r\n"), g_pdwThreadId[i]);
            m_printInfo += str;
            UpdateData(FALSE);

        }
    }
    return ;
}

/*
    �������ܣ�ֹͣ����
*/
void CsenderTestDlg::OnBnClickedStopSendButton()
{
	
    for(s32 i = 0; i < m_nSendPathNum; i++)
    {
	    g_pSender[i].Stop();
    }
    return ;
}

/*
    �������ܣ���������ʼ��sender
*/
void CreateSnd(s32 nNum)
{

    g_pSender = new CSender[nNum];
    for(s32 i = 0; i < nNum; i++)
    {
        g_pSender[i].Init();
    }
    return ;
}

/*
    �������ܣ����ͳ�ʼ��
*/
void SndInit(s32 nNum, u32 dwNetBand, u8 ucFrameRate, u32 dwBitRate)
{
    for(s32 i = 0; i < nNum; i++)
    {
        g_pSender[i].SetMnetNetParam();
        g_pSender[i].m_pcSender->ResetSSRC();
        g_pSender[i].m_pcSender->ResetRSFlag(FALSE);
        g_pSender[i].m_pcSender->ResetFrameId();
        g_pSender[i].m_pcSender->SetSndInfo(dwNetBand, ucFrameRate, dwBitRate);
    }
    return ;
}

/*
    �������ܣ��ؼ����ݸ�����sender
*/
void CsenderTestDlg::ActiveX2Snd(s32 nNum)
{
    USES_CONVERSION;
    in_addr inaddr = {0};
    CString strCBText("");
    s32 nIndex = 0;
    for(s32 i = 0; i < nNum; i++)
    {
       g_pSender[i].SetMediaDatName(W2A(m_cstrMediaFile));
       m_cstrMeidaLenFile = m_cstrMediaFile + _T(".len");
       UpdateData(FALSE);
       g_pSender[i].SetSrcIpPort(atoi(W2A(m_srcPort)) + PORT_ADD_NUM * i);
       inaddr.s_addr = ntohl(m_dwDstIP);
       g_pSender[i].SetDstIp(inet_ntoa(inaddr));
       g_pSender[i].SetDstIpPort(atoi(W2A(m_dstPort)) + PORT_ADD_NUM * i);
       /* media type */
       g_pSender[i].SetFrameRate(m_frameRate);
       g_pSender[i].SetNetBand(m_NetBand);
       g_pSender[i].SetRepeat(m_isRepeat);

       nIndex = m_cbLines.GetCurSel();
       m_cbLines.GetLBText(nIndex, strCBText);
       g_pSender[i].SetOutLines(atoi(W2A(strCBText)));
       
    }
    s32 nRet = access(g_pSender[0].GetMediaLenName(), F_OK);
    if (F_OK != nRet)
    {
        if (MEDIA_TYPE_H264 != g_pSender[0].GetMeidaTypeNum())
        {
            AfxMessageBox(_T("��H264ý���ļ�����ý�峤���ļ����޷�����"));
            return ;
        }
        else
        {
            CreateFrameLen(g_pSender[0].GetMediaDatName());
        }
    }
    return ;
}
