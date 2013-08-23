#include "stdafx.h"
#include "HoneyView.h"
#include "MainFrm.h"
#include "OptionDlg.h"
#include "GUtil.h"
#include "GMessageBoxWnd.h"
#include "Option.h"
#include "global.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CMainFrame

IMPLEMENT_DYNAMIC(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	//{{AFX_MSG_MAP(CMainFrame)
	ON_WM_CREATE()
	ON_WM_SETFOCUS()
	ON_COMMAND(IDM_SETTING_OPTION, OnSettingOption)
	ON_WM_SIZE()
	ON_COMMAND(IDM_VIEW_FULLSCREEN, OnViewFullscreen)
	ON_UPDATE_COMMAND_UI(IDM_VIEW_FULLSCREEN, OnUpdateViewFullscreen)
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()


static UINT indicators[] =
{
	ID_SEPARATOR,           // status line indicator
	ID_INDICATOR_PAGEINFO, 
	ID_INDICATOR_PROGRESS,
//	ID_INDICATOR_CAPS,
//	ID_INDICATOR_NUM,
//	ID_INDICATOR_SCRL,
};

/////////////////////////////////////////////////////////////////////////////
// CMainFrame construction/destruction

CMainFrame::CMainFrame()
{
	m_bFullScreen = false;
}

CMainFrame::~CMainFrame()
{
}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct)
{
	if (CFrameWnd::OnCreate(lpCreateStruct) == -1)
		return -1;
	// create a view to occupy the client area of the frame
	if (!m_wndView.Create(NULL, NULL, AFX_WS_DEFAULT_VIEW,
		CRect(0, 0, 0, 0), this, AFX_IDW_PANE_FIRST, NULL))
	{
		TRACE0("Failed to create view window\n");
		return -1;
	}
	
	if (!m_wndToolBar.CreateEx(this, TBSTYLE_FLAT, WS_CHILD | WS_VISIBLE | CBRS_TOP
		| CBRS_GRIPPER | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_DYNAMIC) ||
		!m_wndToolBar.LoadToolBar(IDR_MAINFRAME))
	{
		TRACE0("Failed to create toolbar\n");
		return -1;      // fail to create
	}

	if (!m_wndStatusBar.Create(this) ||
		!m_wndStatusBar.SetIndicators(indicators,
		  sizeof(indicators)/sizeof(UINT)))
	{
		TRACE0("Failed to create status bar\n");
		return -1;      // fail to create
	}

	// PANE 속성 바꾸기
	UINT nID, nStyle;
	int cxWidth;
	m_wndStatusBar.GetPaneInfo(2, nID, nStyle, cxWidth);
	m_wndStatusBar.SetPaneInfo(2, nID, nStyle/*|SBPS_STRETCH*/, 200);


	// meter 생성
	m_meter.Create(WS_CHILD| WS_VISIBLE, CRect(0,0,0,0), &m_wndStatusBar, 101);
	m_meter.SetRange(0, 100);
	m_meter.SetPos(0);
	m_meter.SetStep(1);

	COLORREF clr, clrBk;
	int		r,g,b;
	clr = GetSysColor(COLOR_3DLIGHT);
	r = max(0,GetRValue(clr)-15);
	g = max(0,GetRValue(clr)-15);
	b = max(0,GetRValue(clr)+15);
	clr = RGB(r,g,b);
	clrBk = GetSysColor(COLOR_MENU);
	m_meter.SendMessage(PBM_SETBARCOLOR, 0, (LPARAM)clr);
	m_meter.SendMessage(PBM_SETBKCOLOR, 0, (LPARAM)clrBk);


	m_wndToolBar.EnableDocking(CBRS_ALIGN_ANY);
	EnableDocking(CBRS_ALIGN_ANY);
	DockControlBar(&m_wndToolBar);

	// 커맨드 라인 분석 타이머
	SetTimer(1,10,0);

	if(g_Option.bIsMaximized)
	{
		MoveWindow(CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT, CW_USEDEFAULT);
		ShowWindow(SW_MAXIMIZE);
	}

	return 0;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs)
{
	if( !CFrameWnd::PreCreateWindow(cs) )
		return FALSE;
	cs.dwExStyle &= ~WS_EX_CLIENTEDGE;
//	cs.lpszClass = AfxRegisterWndClass(0);
	cs.hMenu = NULL;								// 메뉴 없애기..
	return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
// CMainFrame diagnostics

#ifdef _DEBUG
void CMainFrame::AssertValid() const
{
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const
{
	CFrameWnd::Dump(dc);
}

#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CMainFrame message handlers
void CMainFrame::OnSetFocus(CWnd* pOldWnd)
{
	// forward focus to the view window
	m_wndView.SetFocus();
}

BOOL CMainFrame::OnCmdMsg(UINT nID, int nCode, void* pExtra, AFX_CMDHANDLERINFO* pHandlerInfo)
{
	// let the view have first crack at the command
	if (m_wndView.OnCmdMsg(nID, nCode, pExtra, pHandlerInfo))
		return TRUE;

	// otherwise, do default handling
	return CFrameWnd::OnCmdMsg(nID, nCode, pExtra, pHandlerInfo);
}


void CMainFrame::OnSettingOption() 
{
	COptionDlg dlg;
	if(dlg.DoModal()!=IDOK) return;

	m_wndView.Refresh();
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) 
{
	CFrameWnd::OnSize(nType, cx, cy);

	// 메터 위치 옮겨주기..
	CRect r;
	m_wndStatusBar.GetItemRect(2, &r);
	m_meter.MoveWindow(r, TRUE);
}

void CMainFrame::OnViewFullscreen() 
{
	if(m_bFullScreen)
	{
		m_bFullScreen = false;
		this->ModifyStyle(NULL, WS_THICKFRAME|WS_DLGFRAME|WS_BORDER|WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION);
		PostMessage(WM_COMMAND, ID_VIEW_TOOLBAR);
		PostMessage(WM_COMMAND, ID_VIEW_STATUS_BAR);
		this->RecalcLayout();

		MoveWindow(&m_rPrev);
//		m_wndView.Refresh();
	}
	else
	{
		m_bFullScreen = true;
		this->ModifyStyle(WS_THICKFRAME|WS_DLGFRAME|WS_BORDER|WS_OVERLAPPED|WS_SYSMENU|WS_CAPTION, NULL);
		PostMessage(WM_COMMAND, ID_VIEW_TOOLBAR);
		PostMessage(WM_COMMAND, ID_VIEW_STATUS_BAR);
		this->RecalcLayout();

		GetWindowRect(&m_rPrev);

		SetWindowPos(NULL , 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW|SWP_FRAMECHANGED);

		// 풀스크린을 만들기 위한 더미 코드
		GMessageBoxWnd	wnd;
		wnd.Create(this, "", "");

//		m_wndView.Refresh();
	}
}

void CMainFrame::OnUpdateViewFullscreen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(m_bFullScreen);
	
}

void CMainFrame::SetTitle(CString sTitle)
{
	SetWindowText(sTitle);

	int nIndex = m_wndStatusBar.CommandToIndex(ID_INDICATOR_PAGEINFO);

//	int nWidth;
//	unsigned int nID ,nStyle ;
//	m_wndStatusBar.GetPaneInfo(nIndex, nID, nStyle, nWidth);
//	m_wndStatusBar.SetPaneInfo(nIndex, nID, nStyle, 200);

	m_wndStatusBar.SetPaneText(nIndex, m_wndView.GetFilePosInfo());
}


void CMainFrame::SetProgress(int nPos)
{
	m_meter.SetPos(nPos);
}


void CMainFrame::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==1)
	{
		KillTimer(1);

		// 커맨드 라인 분석..
		CCommandLineInfo rCmdInfo;
		AfxGetApp()->ParseCommandLine(rCmdInfo);

		if(rCmdInfo.m_strFileName!="")
		{
			// 폴더 열기
			if(IsFolder(rCmdInfo.m_strFileName))
			{
				m_wndView.OpenFolder(rCmdInfo.m_strFileName);
			}
			// 파일 열기
			else
			{
				CString sFileName = rCmdInfo.m_strFileName;
				CString sExt = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.'));
				sExt.MakeLower();
				if(sExt==".pco")
				{
					m_wndView.OpenPCO(sFileName);
				}
				if(sExt==".zip")
				{
					m_wndView.OpenZIP(sFileName);
				}
			}
		}
	}
}

BOOL CMainFrame::DestroyWindow() 
{
	UINT nState;
	nState = GetWindowShowState(m_hWnd);
	if(nState==SW_MAXIMIZE)
		g_Option.bIsMaximized=TRUE;
	else
		g_Option.bIsMaximized=FALSE;
	
	return CFrameWnd::DestroyWindow();
}
