#include "stdafx.h"
#include "GMessageBoxWnd.h"
#include "Gutil.h"
#include "GDib.h"
#include "GDocker.h"
#include "resource.h"

#define _IDC_STATIC_FRM		25
#define _IDC_STATIC_ICN		26
#define _IDC_STATIC_MSG		27
#define _IDC_STATIC_TTL		28

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

#define	DEFAULT_TITLEFONT_SIZE		12

#define TIMERID_TIMER				1						// timer 정의
#define	TIMERGAP_TIMER				1000

UINT WM_GMESSAGEBOX_CLOSE = ::RegisterWindowMessage("WM_GMESSAGEBOX_CLOSE");

int GAlaramBox(CWnd* pParentWnd, CString sMsg, CString sCaption, UINT uType, int nDuration)
{
	return GMessageBox(pParentWnd, sMsg, sCaption, uType, GMB_ALARM|GMB_TIMER, nDuration);
}

int GMessageBox(CWnd* pParentWnd, CString sMsg, CString sCaption, UINT uType, UINT uTypeEx, int nExParam)
{
	GMessageBoxWnd* pWnd;
	BOOL bRet;
	int	nRet;

	if(sCaption=="~~")
		sCaption = GMessageBoxWnd::m_sDefaultCaption;

	pWnd = new GMessageBoxWnd;
	bRet = pWnd->Create(pParentWnd, sMsg, sCaption, uType, uTypeEx,nExParam);
	if(bRet==FALSE)
	{
		ASSERT(0);
		delete pWnd;
		return 0;
	}
	nRet = pWnd->DoModal();
	delete pWnd;
	return nRet;
}

struct SAlarmBox
{
	GMessageBoxWnd*	pWnd;
	CString			sMsg;
	CString			sCaption;
	CWnd*			pParentWnd;
	bool			bCreated;
	int				nDuration;
};

UINT AlarmBoxThreadProc(void* pParam)
{
	SAlarmBox*		pSAB = (SAlarmBox*)pParam;
	BOOL			bRet;

	if(pSAB->nDuration)
		bRet = pSAB->pWnd->Create(pSAB->pParentWnd, pSAB->sMsg, pSAB->sCaption, MB_OK, GMB_ALARM|GMB_TIMER, pSAB->nDuration);
	else
		bRet = pSAB->pWnd->Create(pSAB->pParentWnd, pSAB->sMsg, pSAB->sCaption, MB_OK, GMB_ALARM);
	pSAB->bCreated = true;
	if(bRet==FALSE)
	{
		ASSERT(0);
		goto END;
	}

	pSAB->pWnd->DoModal();

END :
	pSAB->pWnd->DestroyWindow();
	delete pSAB->pWnd;
	delete pSAB;

	return 0;
}

HWND 	GShowAlarmBox(CWnd* pParentWnd, CString sMsg, CString sCaption, int nDuration)
{
	SAlarmBox	*pSAB;
	pSAB = new SAlarmBox;

	pSAB->pParentWnd = pParentWnd;
	pSAB->sMsg = (const char*) sMsg;
	pSAB->sCaption = (const char*) sCaption;
	pSAB->pWnd = new GMessageBoxWnd;
	pSAB->bCreated = false;
	pSAB->nDuration = nDuration;

	AfxBeginThread(AlarmBoxThreadProc, (void*)pSAB, 0, 0, 0, NULL);

	MSG msg;
	// 윈도우가 완성될때까지 메시지 펌핑..
	while(!(pSAB->pWnd->m_nFlags & WF_MODALLOOP))
	{
		if(::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			::TranslateMessage(&msg);
			::DispatchMessage(&msg);
		}
	}

	return pSAB->pWnd->m_hWnd;
}

void GHideAlarmBox(HWND hWnd)
{
	if(::IsWindow(hWnd))
		::PostMessage(hWnd, WM_GMESSAGEBOX_CLOSE, 0,0);
}


// 유틸함수 - 그라데이션을 그린다.
void _Gradation(GDib* pDib)
{
	int w = pDib->GetSize().cx;
	int h = pDib->GetSize().cy;
	int x;

	int r, g, b;
	int rs, gs, bs;
	int rd, gd, bd;

	CDC* pDC;
	pDC = pDib->GetMemDC();
	rs = GetRValue(GMessageBoxWnd::m_clrCaptionStart);
	gs = GetGValue(GMessageBoxWnd::m_clrCaptionStart);
	bs = GetBValue(GMessageBoxWnd::m_clrCaptionStart);

	rd = GetRValue(GMessageBoxWnd::m_clrCaptionEnd);
	gd = GetGValue(GMessageBoxWnd::m_clrCaptionEnd);
	bd = GetBValue(GMessageBoxWnd::m_clrCaptionEnd);

	for(x=0;x<w;x++)
	{
		r = rs - (rs-rd)*x/w;
		g = gs - (gs-gd)*x/w;
		b = bs - (bs-bd)*x/w;

		pDC->FillSolidRect(x,0, 1, h, RGB(r,g,b));
	}
}


// static 속성 변수 
int			GMessageBoxWnd::m_nIconResourceID = IDR_MAINFRAME;
COLORREF	GMessageBoxWnd::m_clrCaption		= RGB(255,255,255);
COLORREF	GMessageBoxWnd::m_clrCaptionStart	= RGB(0,0,200);
COLORREF	GMessageBoxWnd::m_clrCaptionEnd		= RGB(140,161,255);
COLORREF	GMessageBoxWnd::m_clrCaption_p		= RGB(0,0,0);
COLORREF	GMessageBoxWnd::m_clrCaptionStart_p	= RGB(255,27,252);			// 분홍색 그라데이션용..
COLORREF	GMessageBoxWnd::m_clrCaptionEnd_p	= RGB(255,189,254);
CString		GMessageBoxWnd::m_sDefaultCaption	= "";

GMessageBoxWnd::GMessageBoxWnd()
{
	m_pDocker = new GDocker;
	m_pDibTitle = new GDib;
	m_uType = MB_OK;
	m_uTypeEx = 0;
	m_hWndParent = NULL;
}

GMessageBoxWnd::~GMessageBoxWnd()
{
	if(m_pDocker) delete m_pDocker;
	if(m_pDibTitle) delete m_pDibTitle;
}

// 캡션색 바꾸기
void GMessageBoxWnd::ChangeCaptionColorToPurple()
{
	m_clrCaptionStart = m_clrCaptionStart_p;
	m_clrCaptionEnd = m_clrCaptionEnd_p;
	m_clrCaption = m_clrCaption_p;
}

BEGIN_MESSAGE_MAP(GMessageBoxWnd, CWnd)
	//{{AFX_MSG_MAP(GMessageBoxWnd)
	ON_WM_CLOSE()
	ON_WM_LBUTTONDOWN()
	ON_WM_LBUTTONUP()
	ON_WM_MOUSEMOVE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	//}}AFX_MSG_MAP
	ON_REGISTERED_MESSAGE(WM_GMESSAGEBOX_CLOSE, OnGMessageBoxClose)
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// GMessageBoxWnd message handlers

BOOL GMessageBoxWnd::Create(CWnd* pParentWnd, CString sMsg, CString sCaption, UINT uType, UINT uTypeEx, int nExParam)
{
	BOOL bRet;

	m_hWndParent = pParentWnd->GetSafeHwnd();
	m_sMsg = sMsg;
	m_sCaption = sCaption;
	m_uType = uType;
	m_uTypeEx = uTypeEx;

	LPCTSTR lpszWindowName = "GMessageBoxWnd";
	LPCTSTR	lpszClassName;

	lpszClassName = AfxRegisterWndClass(NULL,
		LoadCursor(NULL,IDC_ARROW),	(HBRUSH)(COLOR_BTNFACE+1),NULL);

	bRet = this->CreateEx(WS_EX_TOOLWINDOW, lpszClassName, lpszWindowName, 
		WS_POPUP | WS_DLGFRAME, 
		CRect(0,0,0,0), pParentWnd, NULL, NULL);

	if(bRet)
	{
		// 카운트 다이알로그 박스 이면..
		if(uTypeEx & GMB_TIMER)	m_nCount = nExParam;

		m_font.CreateFont(-DEFAULT_TITLEFONT_SIZE,0,0,0,FW_NORMAL,0,0,0,129,3,2,1,49,"굴림");
		SetFont(&m_font, FALSE);
		CreateControl();
		ResizeWnd();
		m_pDocker->Create(this);
	}

	return bRet;
}


int GMessageBoxWnd::DoModal()
{
	ASSERT(m_hWndParent);

	int nRet;
	::EnableWindow(m_hWndParent, FALSE);
	this->CenterWindow(NULL);
	this->ShowWindow(SW_SHOW);

	if(m_uTypeEx & GMB_TIMER)					// 타이머 다이알로그 박스 처리 
	{
		SetTimer(TIMERID_TIMER, TIMERGAP_TIMER, NULL);
	}

//	nRet = CWnd::RunModalLoop();
	nRet = this->RunModalLoop();
	::EnableWindow(m_hWndParent, TRUE);

	return nRet;
}

void GMessageBoxWnd::OnClose() 
{
//	this->EndModalLoop(IDOK);
//	CWnd::OnClose();
}

void GMessageBoxWnd::CreateTltleBar()
{
	CDC*	pDC;
	int		w, h;
	CFont	font;
	CFont*	pOldFont;

	if(m_sCaption=="")
		m_sCaption = "팝 폴더";

	m_sCaption = " " + m_sCaption;

	pDC = m_pDibTitle->GetMemDC();
	w = m_pDibTitle->GetSize().cx;
	h = m_pDibTitle->GetSize().cy;
	//pDC->FillSolidRect(0, 0, w, h, RGB(0,0,200));
	_Gradation(m_pDibTitle);

	font.CreateFont(-DEFAULT_TITLEFONT_SIZE,0,0,0,FW_NORMAL,0,0,0,129,3,2,1,49,"굴림");
	pOldFont = pDC->SelectObject(&font);

	pDC->SetBkMode(TRANSPARENT);
	pDC->SetTextColor(m_clrCaption);
	pDC->DrawText(m_sCaption, CRect(0,0,w,h), DT_LEFT | DT_VCENTER | DT_SINGLELINE);
	pDC->SelectObject(pOldFont);
}

void GMessageBoxWnd::ResizeWnd()
{
	CDC*	pDC;
	int		nMsgH;
	int		nMsgW;
	int		nWndW, nWndH;

	int		nFrmL, nFrmT, nFrmW, nFrmH;
	int		nIcnL, nIcnT;
	int		nMsgL, nMsgT;
	int		nBtnL1, nBtnT1;
	int		nBtnL2, nBtnT2;

	int		nGapW, nGapH;
	int		nIcnW, nIcnH;
	int		nBtnW, nBtnH;
	int		nTtlL, nTtlT, nTtlW, nTtlH;

	pDC = GetDC();

	int		PointSize = 9;
	int		nHeight = -MulDiv(PointSize, GetDeviceCaps(pDC->m_hDC, LOGPIXELSY), 72);
	CFont	font;
	CFont*	pOldFont;

	font.CreateFont(nHeight,0,0,0,FW_NORMAL,0,0,0,129,3,2,1,49,"굴림");
	pOldFont = pDC->SelectObject(&font);

	nMsgH = ParagraphHeight(pDC->m_hDC, FALSE, m_sMsg);
	nMsgW = max(ParagraphWidth(pDC->m_hDC, m_sMsg), 50);

	pDC->SelectObject(pOldFont);

	ReleaseDC(pDC);

	nIcnW = 32;
	nIcnH = 32;
	nGapW = 5;
	nGapH = 5;
	nBtnW = 70;
	nBtnH = 25;

	nFrmW = nIcnW + nMsgW + nGapW*6;
	nFrmH = nGapH*4+ max(nMsgH,nIcnH) + nGapH*4;

	nWndW = nFrmW + nGapW*4;

	nTtlH = 20;
	nTtlW = nWndW - nGapW*3;
	nTtlL = nGapW;
	nTtlT = nGapH;

	nWndH = nFrmH + nBtnH + nGapH*4 + nTtlH + nGapH;

	nFrmL = nGapW + nGapW/2;
	nFrmT = nGapH*2 + nTtlH ;

	nIcnL = nFrmL + nGapW*2;
	nIcnT = nFrmT + (nFrmH-nIcnH)/2; 

	nMsgL = nIcnL + nIcnW + nGapW*2;
	nMsgT = nFrmT + (nFrmH-nMsgH)/2;

	if(m_uTypeEx&GMB_ALARM)	nWndH -= nBtnH;							// 알람박스일경우 - 버튼높이를 빼준다.

	SetWindowPos(NULL, 0, 0, nWndW, nWndH, SWP_NOMOVE);

	//CenterWindow();
	::MoveWindow(GetDlgItem(_IDC_STATIC_FRM)->m_hWnd, nFrmL, nFrmT, nFrmW, nFrmH, TRUE);
	::MoveWindow(GetDlgItem(_IDC_STATIC_ICN)->m_hWnd, nIcnL, nIcnT, nIcnW, nIcnH, TRUE);
	::MoveWindow(GetDlgItem(_IDC_STATIC_MSG)->m_hWnd, nMsgL, nMsgT, nMsgW, nMsgH, TRUE);
	::MoveWindow(GetDlgItem(_IDC_STATIC_TTL)->m_hWnd, nTtlL, nTtlT, nTtlW, nTtlH, TRUE);

	// 버튼
	nBtnL2 = nFrmL + nFrmW - nBtnW - nGapW;
	nBtnT2 = nFrmT + nFrmH + nGapH;
	nBtnL1 = nBtnL2 - nBtnW - nGapW;
	nBtnT1 = nBtnT2;
	if(m_uTypeEx&GMB_ALARM)
	{
		::EnableWindow(GetDlgItem(IDOK)->m_hWnd, FALSE);
		::EnableWindow(GetDlgItem(IDCANCEL)->m_hWnd, FALSE);
	}
	else if(m_uType==MB_OK)
	{
		::ShowWindow(GetDlgItem(IDOK)->m_hWnd, SW_SHOW);
		::MoveWindow(GetDlgItem(IDOK)->m_hWnd          , nBtnL2, nBtnT2, nBtnW, nBtnH, TRUE);
	}
	else if(m_uType==MB_OKCANCEL)
	{
		::ShowWindow(GetDlgItem(IDOK)->m_hWnd, SW_SHOW);
		::ShowWindow(GetDlgItem(IDCANCEL)->m_hWnd, SW_SHOW);
		::MoveWindow(GetDlgItem(IDOK)->m_hWnd          , nBtnL1, nBtnT1, nBtnW, nBtnH, TRUE);
		::MoveWindow(GetDlgItem(IDCANCEL)->m_hWnd      , nBtnL2, nBtnT2, nBtnW, nBtnH, TRUE);
	}
	else if(m_uType==MB_YESNO)
	{
		::ShowWindow(GetDlgItem(IDOK)->m_hWnd, SW_SHOW);
		::ShowWindow(GetDlgItem(IDCANCEL)->m_hWnd, SW_SHOW);
		::MoveWindow(GetDlgItem(IDOK)->m_hWnd          , nBtnL1, nBtnT1, nBtnW, nBtnH, TRUE);
		::MoveWindow(GetDlgItem(IDCANCEL)->m_hWnd      , nBtnL2, nBtnT2, nBtnW, nBtnH, TRUE);
		GetDlgItem(IDOK)->SetWindowText("예");
		GetDlgItem(IDCANCEL)->SetWindowText("아니오");
	}
	else
		ASSERT(0);							// 미구현..

	SetMessageText();

	m_pDibTitle->CreateNullImage(nTtlW, nTtlH);
	CreateTltleBar();
}

// 콘트롤 생성하기 
void GMessageBoxWnd::CreateControl()
{
	m_btnOK.Create("확인", WS_CHILD | WS_GROUP | WS_TABSTOP , CRect (0,0,0,0), this, IDOK);
	m_btnCancel.Create("취소", WS_CHILD | WS_GROUP | WS_TABSTOP , CRect (0,0,0,0), this, IDCANCEL);
	m_btnOK.ModifyStyleEx(NULL, WS_EX_STATICEDGE, 0);
	m_btnCancel.ModifyStyleEx(NULL, WS_EX_STATICEDGE, 0);

	m_btnOK.SetFocus();

	m_stFrm.Create("", WS_CHILD | WS_VISIBLE | BS_GROUPBOX , CRect(0,0,0,0), this, _IDC_STATIC_FRM);
	m_stIcn.Create("", WS_CHILD | WS_VISIBLE | SS_ICON |SS_CENTERIMAGE, CRect(0,0,0,0), this, _IDC_STATIC_ICN);
	m_stMsg.Create("", WS_CHILD | WS_VISIBLE  , CRect(0,0,100,100), this, _IDC_STATIC_MSG);
	m_stTtl.Create("", WS_CHILD | WS_BORDER , CRect(0,0,0,0), this, _IDC_STATIC_TTL);

	m_stIcn.SetIcon(::LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(m_nIconResourceID)));

	m_btnOK.SetFont(&m_font);
	m_btnCancel.SetFont(&m_font);
	m_stMsg.SetFont(&m_font);
}

BOOL GMessageBoxWnd::OnCommand(WPARAM wParam, LPARAM lParam) 
{
	if(wParam==IDOK)
	{
		OnOK();
		return TRUE;
	}
	else if(wParam==IDCANCEL)
	{
		OnCancel();
		return TRUE;
	}
	return CWnd::OnCommand(wParam, lParam);
}

void GMessageBoxWnd::OnOK()
{
	if(m_uType==MB_YESNO)
		this->EndModalLoop(IDYES);
	else
		this->EndModalLoop(IDOK);
}

void GMessageBoxWnd::OnCancel()
{
	if(m_uType==MB_YESNO)
		this->EndModalLoop(IDNO);
	else
		this->EndModalLoop(IDCANCEL);
}

void GMessageBoxWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_pDocker->OnLButtonDown();
}

void GMessageBoxWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_pDocker->OnLButtonUp();
}

void GMessageBoxWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_pDocker->OnMouseMove();
}

// 페인트 메시지 처리 - 캡션바 그리기
void GMessageBoxWnd::OnPaint() 
{
	CPaintDC dc(this); 
	CRect r;

	GetDlgItem(_IDC_STATIC_TTL)->GetWindowRect(&r);
	ScreenToClient(&r);
	m_pDibTitle->DrawTo(&dc, r.left, r.top);
}

BOOL GMessageBoxWnd::PreTranslateMessage(MSG* pMsg) 
{
	if (pMsg->message == WM_KEYDOWN ||   pMsg->message == WM_KEYUP)
	{
		// ENTER 키를 SPACE 와 동일하게 처리한다. 
		if(pMsg->wParam == VK_RETURN)
		{
			pMsg->wParam = VK_SPACE;
		}
	}

	if(pMsg->message==WM_KEYDOWN)
	{
//		if((m_uTypeEx & GMB_ALARAM)==FALSE)
		{
			// ESC 키는 취소용..
			if(pMsg->wParam==VK_ESCAPE)
			{
				if( (m_uType&MB_YESNO) || (m_uType&MB_OKCANCEL))
				{
					PostMessage(WM_COMMAND, IDCANCEL, 0);
					return TRUE;
				}
			}
			// TAB 키 처리
			if(pMsg->wParam==VK_TAB)
			{
				CWnd* pWndNext;
				pWndNext = this->GetNextDlgTabItem(this->GetFocus());
				if(pWndNext)
					pWndNext->SetFocus();
			}
		}
	}
	return CWnd::PreTranslateMessage(pMsg);
}

int GMessageBoxWnd::RunModalLoop()
{
	m_nFlags |= (WF_MODALLOOP|WF_CONTINUEMODAL);

	for(;;)
	{
/*				- 작동하지 않는다.. 
		MSG msg;
		if(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
		{
			if(msg.message==WM_GETFOCUS)
			{
				::GetMessage(&msg, m_hWndParent, 0, 0);
				SetFocus();
			}
		}
*/
		if (!AfxGetThread()->PumpMessage())
		{
			AfxPostQuitMessage(0);
			return -1;
		}

		if(!ContinueModal())
			break;
	}
	return m_nModalResult;
}

// 타이머 처리
void GMessageBoxWnd::OnTimer(UINT nIDEvent) 
{
	if(nIDEvent==TIMERID_TIMER)
	{
		m_nCount --;

		if(m_nCount <=0)
		{
			KillTimer(nIDEvent);
			OnOK();
		}
		else
			SetMessageText();
	}
}

// 메시지 지정하기
void GMessageBoxWnd::SetMessageText()
{
	if(m_uTypeEx & GMB_TIMER)
	{
		CString s;
		s.Format(m_sMsg, m_nCount);
		SetDlgItemText(_IDC_STATIC_MSG, s);
	}
	else
		SetDlgItemText(_IDC_STATIC_MSG, m_sMsg);
}


// close 메시지 왔을때 
LRESULT GMessageBoxWnd::OnGMessageBoxClose(WPARAM wParam, LPARAM lParam)
{
	OnOK();
	return 0;
}