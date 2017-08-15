// senderTestDlg.h : header file

#include "sender.h"
#include "afxcmn.h"
#include "afxwin.h"
//
#pragma once


// CsenderTestDlg dialog
class CsenderTestDlg : public CDialogEx
{
// Construction
public:
	CsenderTestDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	enum { IDD = IDD_SENDERTEST_DIALOG };

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support


// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
    afx_msg void OnCancel();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

// init, read .ini
public:
	void GetConfig();

public:
	afx_msg void OnBnClickedStartSendButton();
	CString m_cstrMediaFile;
	CString m_cstrMeidaLenFile;
	CString m_mediaType;
	u8 m_frameRate;
	CString m_srcPort;
	DWORD m_dwDstIP;
	CString m_dstPort;
	int m_isRepeat;
	afx_msg void OnBnClickedStopSendButton();
	CString m_printInfo;
	CString m_cbmediaType;
	u32 m_NetBand;
    int m_nSendPathNum;
    CEdit m_wndEdit;
    u32 m_dwBitRate;
    u32 m_dwSafeFlag;
private:
    void ActiveX2Snd(int nNum);

public:
    CSliderCtrl m_slider;
    int m_nOutputLines;
    CComboBox m_cbLines;
};
