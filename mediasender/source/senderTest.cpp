
// senderTest.cpp : Defines the class behaviors for the application.
//

#include "stdafx.h"
#include "sender.h"
#ifdef WIN32
#include "senderTest.h"
#include "senderTestDlg.h"
#else

#include <pthread.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#endif
#ifdef _DEBUG
#define new DEBUG_NEW
#endif

#ifdef WIN32
// CsenderTestApp

BEGIN_MESSAGE_MAP(CsenderTestApp, CWinApp)
	ON_COMMAND(ID_HELP, &CWinApp::OnHelp)
END_MESSAGE_MAP()


// CsenderTestApp construction

CsenderTestApp::CsenderTestApp()
{
	// support Restart Manager
	m_dwRestartManagerSupportFlags = AFX_RESTART_MANAGER_SUPPORT_RESTART;

	// TODO: add construction code here,
	// Place all significant initialization in InitInstance
}


// The one and only CsenderTestApp object

CsenderTestApp theApp;


// CsenderTestApp initialization

BOOL CsenderTestApp::InitInstance()
{
	// InitCommonControlsEx() is required on Windows XP if an application
	// manifest specifies use of ComCtl32.dll version 6 or later to enable
	// visual styles.  Otherwise, any window creation will fail.
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	// Set this to include all the common control classes you want to use
	// in your application.
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();


	AfxEnableControlContainer();

	// Create the shell manager, in case the dialog contains
	// any shell tree view or shell list view controls.
	CShellManager *pShellManager = new CShellManager;

	// Standard initialization
	// If you are not using these features and wish to reduce the size
	// of your final executable, you should remove from the following
	// the specific initialization routines you do not need
	// Change the registry key under which our settings are stored
	// TODO: You should modify this string to be something appropriate
	// such as the name of your company or organization
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));

	CsenderTestDlg dlg;
	m_pMainWnd = &dlg;
	INT_PTR nResponse = dlg.DoModal();
	if (nResponse == IDOK)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with OK
	}
	else if (nResponse == IDCANCEL)
	{
		// TODO: Place code here to handle when the dialog is
		//  dismissed with Cancel
	}

	// Delete the shell manager created above.
	if (pShellManager != NULL)
	{
		delete pShellManager;
	}

	// Since the dialog has been closed, return FALSE so that we exit the
	//  application, rather than start the application's message pump.
	return FALSE;
}
#else
typedef struct TagPthreadParam
{
    s32 m_nNum;
    CSender *m_pSender;
}TPthreadParam, *PTPthreadParam;

void *threadFuc(void *pthPrm)
{

    printf("this is pthread %d\n", getpid());
    ((PTPthreadParam)pthPrm)->m_pSender = new CSender();
    ((PTPthreadParam)pthPrm)->m_pSender->Init();

    s32 nDstPort = ((PTPthreadParam)pthPrm)->m_pSender->GetDstIpPort();
    s32 nSrcPort = ((PTPthreadParam)pthPrm)->m_pSender->GetSrcIpPort();
    ((PTPthreadParam)pthPrm)->m_pSender->SetDstIpPort(nDstPort + ((PTPthreadParam)pthPrm)->m_nNum * PORT_ADD_NUM);
    ((PTPthreadParam)pthPrm)->m_pSender->SetSrcIpPort(nSrcPort + ((PTPthreadParam)pthPrm)->m_nNum * PORT_ADD_NUM);
    ((PTPthreadParam)pthPrm)->m_pSender->SetMnetNetParam();

    // set ssrc
    ((PTPthreadParam)pthPrm)->m_pSender->m_pcSender->ResetSSRC();
    ((PTPthreadParam)pthPrm)->m_pSender->m_pcSender->ResetRSFlag(FALSE);
    ((PTPthreadParam)pthPrm)->m_pSender->m_pcSender->ResetFrameId();

    ((PTPthreadParam)pthPrm)->m_pSender->m_pcSender->SetSndInfo(4096000, 30, 4096000);
    ((PTPthreadParam)pthPrm)->m_pSender->PrintInfo();
    ((PTPthreadParam)pthPrm)->m_pSender->Send();

    return (void *)0;
}

s32 main(s32 argc, const s8* argv[])
{
    s32 nPathNum = 1;
    if (argc > 1)
    {
        nPathNum = atoi(argv[1]);
    }
    printf("path : %d\n",  nPathNum);
    /* 根据路数创建对象和线程 */
    pthread_t *pnThreadId = NULL;
    pnThreadId = new pthread_t[nPathNum];

    PTPthreadParam paThPrm = NULL;
    paThPrm = new TPthreadParam[nPathNum];

    for(s32 nNum = 0; nNum < nPathNum; nNum++)
    {
        paThPrm[nNum].m_nNum = nNum;
        paThPrm[nNum].m_pSender = NULL;
        s32 nRet = pthread_create(pnThreadId + nNum, NULL, threadFuc, (void *)(paThPrm + nNum));
        if (nRet)
        {
            printf("create pthread %d error!\n", nNum + 1);
            return 1;
        }
    }

    while (1)
    {
        sleep(0);
    }

    return 0;
}
#endif
