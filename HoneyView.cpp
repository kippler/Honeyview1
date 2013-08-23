#include "stdafx.h"
#include "HoneyView.h"
#include "MainFrm.h"
#include "GMessageBoxWnd.h"
#include "Global.h"
#include "Option.h"
#include "AboutDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp

BEGIN_MESSAGE_MAP(CHoneyViewApp, CWinApp)
	//{{AFX_MSG_MAP(CHoneyViewApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp construction

CHoneyViewApp::CHoneyViewApp()
{
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CHoneyViewApp object

CHoneyViewApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp initialization

BOOL CHoneyViewApp::InitInstance()
{
	AfxEnableControlContainer();

	// Standard initialization

#ifdef _AFXDLL
//	Enable3dControls();			// Call this when using MFC in a shared DLL
#else
//	Enable3dControlsStatic();	// Call this when linking to MFC statically
#endif

	// Change the registry key under which our settings are stored.
	SetRegistryKey(_T("Local AppWizard-Generated Applications"));



	GMessageBoxWnd::m_sDefaultCaption = APP_NAME;
	g_Option.LoadOption();


	CMainFrame* pFrame = new CMainFrame;
	m_pMainWnd = pFrame;

	// create and load the frame with its resources

	pFrame->LoadFrame(IDR_MAINFRAME,
		WS_OVERLAPPEDWINDOW | FWS_ADDTOTITLE, NULL,
		NULL);
	pFrame->ShowWindow(SW_SHOW);
	pFrame->UpdateWindow();

	return TRUE;
}


int CHoneyViewApp::ExitInstance() 
{
	g_Option.SaveOption();
	return CWinApp::ExitInstance();
}

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp message handlers






// App command to run the dialog
void CHoneyViewApp::OnAppAbout()
{
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp message handlers


