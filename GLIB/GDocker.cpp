#include "stdafx.h"
#include "GDocker.h"
#include "Gutil.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif


#define		DEFAULT_DOCKING_GAP			15					// 들러 붙을 거리.
#define		TIMERID_DOCKER				9999				// 타이머 처리용.
#define		TIMERELAPSE_DOCKER			10

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

GDocker::GDocker()
{
	m_bCaptured = false;
	m_pWndToDock = NULL;
	m_pWnd = NULL;
	m_bDockedToWindow = false;
	m_nDockingGap = DEFAULT_DOCKING_GAP;
	m_bInit = false;
}

GDocker::~GDocker()
{

}

//////////////////////////////////////////////////////////////////////
//
//	PUBLIC CLASSES
//
//////////////////////////////////////////////////////////////////////

/* 초기화 */
void GDocker::Create(CWnd *pWnd, CWnd *pWndToDock)
{
	m_pWndToDock = pWndToDock;
	m_pWnd = pWnd;
	m_bInit = true;

	if(m_pWndToDock) 
	{
		pWnd->SetTimer(TIMERID_DOCKER,TIMERELAPSE_DOCKER,NULL);
		CheckDockAtStart();
	}
}

/* 클라이언트 영역 마우스 처리용 */
void GDocker::OnLButtonDown()
{
	if(m_bInit==false) ASSERT(0);
	CPoint	point;
	GetCursorPos(&point);
	m_bCaptured = true;
	m_pntOld = point;
	m_pWnd->GetWindowRect(m_rWindow);
	m_pWnd->SetCapture();
}


bool IsMaxmized(HWND hWnd)
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	if(GetWindowPlacement(hWnd, &wndpl)==FALSE) return false;
	if(wndpl.showCmd==SW_SHOWMAXIMIZED) return true;
	return false;
}

/* 캡션 영역 마우스 클릭시 처리용 */
void GDocker::OnNcLButtonDown(UINT nHitTest)
{
	if(m_bInit==false) ASSERT(0);

	if(IsMaxmized(m_pWnd->m_hWnd)) return;
	
	if(nHitTest!=HTCAPTION) return;
	CPoint	point;
	GetCursorPos(&point);
	m_bCaptured = true;
	m_pntOld = point;
	m_pWnd->GetWindowRect(m_rWindow);
	m_pWnd->SetCapture();
}

/* 마우스 움직일때 사용 */
void GDocker::OnMouseMove()
{
	if(m_bInit==false) ASSERT(0);
	if(m_bCaptured==false) return;
	CPoint	point;
	CPoint	pntNew;
	GetCursorPos(&point);

	m_rWindow = m_rWindow + (point - m_pntOld);
	DockMove();
	m_pntOld = point;
}

/* 떨구기 */
void GDocker::OnLButtonUp()
{
	if(m_bInit==false) ASSERT(0);
	m_bCaptured = false;
	ReleaseCapture();
}

/* 다른 윈도우에 붙을경우 이 윈도우가 움직이면 따라 다닌다. */
void GDocker::OnTimer(UINT nIDEvent) 
{
	if(m_bInit==false) ASSERT(0);
	if(nIDEvent!=TIMERID_DOCKER) return;
	if(m_pWndToDock==NULL) return;
	if(m_bDockedToWindow==false) return;
	if(m_bCaptured) return;


	CRect r;

	m_pWnd->GetWindowRect(r);
	// 현재 윈도우의 사이즈에 변화가 있었을 경우 - 들러붙은걸 떼낸다 //
	if(r.Width()!=m_rWindow.Width() || r.Height()!=m_rWindow.Height())
	{
		m_bDockedToWindow = false;
		return;
	}

	if(m_pWndToDock->IsIconic()) return;								// 미니마이즈되었을 경우....
	m_pWndToDock->GetWindowRect(r);

	// 들러붙을 윈도우가 움직였을 경우 - 쫗아서 움직인다 //
	if(r!=m_rWindowToDock)
	{
		CPoint pntNew=CPoint(0,0);											// 변화량
		// (이전꺼 - 새거) 변화량을 빼준다..
		if(m_bDockToLeftOfWindow && m_rWindowToDock.left!=r.left) 
		{
			pntNew.x = -( m_rWindowToDock.left - r.left) ;
		}
		if(m_bDockToTopOfWindow && m_rWindowToDock.top!=r.top)
		{
			pntNew.y = -( m_rWindowToDock.top  - r.top ) ;
		}
		if(m_bDockToLeftOfWindow==false && m_rWindowToDock.right!=r.right)	// 우측에 붙어 있으면서 우측에 변화가 오면
		{
			pntNew.x =  - (m_rWindowToDock.right-r.right) ;
		}
		if(m_bDockToTopOfWindow==false && m_rWindowToDock.bottom!=r.bottom)
		{
			pntNew.y =  - (m_rWindowToDock.bottom-r.bottom);
		}

		m_pWnd->GetWindowRect(m_rWindow);
		m_rWindow = m_rWindow + pntNew;
		m_pWnd->MoveWindow(m_rWindow);
		DockMove();															// 여전히 붙어 있는지를 체크한다.

		m_rWindowToDock = r;
	}
}

/* app 시작시 윈도우가 parent 에 붙어 있는지 체크해 준다. */
void GDocker::CheckDockAtStart()
{
	if(m_pWndToDock==NULL) return;

	m_pWnd->GetWindowRect(m_rWindow);
	DockMove();
}

//////////////////////////////////////////////////////////////////////
//
//	PRIVATE CLASSES
//
//////////////////////////////////////////////////////////////////////
void GDocker::DockMove()
{
	CPoint	pntNew;

	int x,y;
	int x1, y1, x2, y2;
	bool	bDock1, bDock2;
	int w = m_rWindow.Width();							// 현재 윈도우의 폭과 높이
	int h = m_rWindow.Height();			

	x = m_rWindow.left;									// 현재 윈도우의 좌/상단 좌표 (화면상의 좌표를 의미하지는 않는다.)
	y = m_rWindow.top;

	x2 = x1 = x;
	y2 = y1 = y;

	m_bDockedToWindow = false;

	// 벽에 붙나 체크하기 //
	bDock1 = CheckDockToScreen(x1,y1,w,h);

	// 윈도우에 붙나 체크하기 //
	bDock2 = CheckDockToWindow(x2,y2,w,h);				// 다른 윈도우에 붙여 주기.


	if(bDock1 && bDock2)								// 벽과 윈도우 둘다 붙을수 있을 경우
	{
		int tempx, tempy;
		tempx = x2;										// 다른 윈도우에 붙는 좌표
		tempy = y2;
		if(CheckDockToScreen(tempx, tempy, w, h))		// 벽에도 붙고 윈도우에도 붙은 합의점이 있을경우.
		{
			x = x2;
			y = y2;
			m_bDockedToWindow = true;
		}
		else
		{
			int d1, d2;
			/* 벽과 윈도우중 가까운 거리를 찾는다. */
			d1 = (x-x1)*(x-x1) + (y-y1)*(y-y1);			// 벽과의 거리
			d2 = (x-x2)*(x-x2) + (y-y2)*(y-y2);			// 윈도우 와의 거리

			/* 둘중 가까운 위치로 이동한다 */
			if(d1 < d2 )			
			{
				x = x1;
				y = y1;
				m_bDockedToWindow = false;
			}
			else
			{
				x = x2;
				y = y2;
				m_bDockedToWindow = true;
			}
		}
	}
	else if(bDock1)										// 벽에만 붙을경우
	{
		x = x1;
		y = y1;
		m_bDockedToWindow = false;
	}
	else if(bDock2)										// 윈도우에만 붙을 경우.
	{
		x = x2;
		y = y2;
		m_bDockedToWindow = true;
	}

	// 새로운 위치로 옮기기 //
	m_pWnd->MoveWindow(x, y, m_rWindow.Width(), m_rWindow.Height());
}


/* 바탕 화면 작업 영역 얻어 오기 */
void _GetWorkArea(CRect& r)
{
	SystemParametersInfo(SPI_GETWORKAREA,NULL,&r,0);
}

/* 벽에 붙나 체크하기 */
bool GDocker::CheckDockToScreen(int &x, int &y, int w, int h)
{
	CRect r;
	_GetWorkArea(r);
	int sx = r.left;
	int sy = r.top;
	int sr = r.right;
	int sb = r.bottom;
	bool bRet=false;

	if(abs(x-sx)    < m_nDockingGap){x=sx  ;bRet=true;}			// 화면 좌단
	if(abs(y-sy)    < m_nDockingGap){y=sy  ;bRet=true;}			// 화면 상단
	if(abs(x+w - sr)< m_nDockingGap){x=sr-w;bRet=true;}			// 화면 우측
	if(abs(y+h - sb)< m_nDockingGap){y=sb-h;bRet=true;}			// 화면 하단.

	return bRet;
}

/* 다른 윈도우에 붙을경우 처리하기 */
bool GDocker::CheckDockToWindow(int &x, int &y, int w, int h)
{
	bool bRet=false;
	if(m_pWndToDock==NULL)
	{
		m_bDockedToWindow = false;
		return bRet;
	}

	CRect	rDockWnd;

	m_pWndToDock->GetWindowRect(rDockWnd);

	int ox, oy;
	int dx = rDockWnd.left;
	int dy = rDockWnd.top;
	int dw = rDockWnd.Width();
	int dh = rDockWnd.Height();

	ox = x;
	oy = y;

	m_bDockToLeftOfWindow = true;
	m_bDockToTopOfWindow = true;

	if((dy-m_nDockingGap<y+h && y<dy+dh+m_nDockingGap) && abs(dx+dw-x ) < m_nDockingGap) {x= dx+dw;bRet=true;m_bDockToLeftOfWindow=false;}	// 우측 붙이기
	if((dy-m_nDockingGap<y+h && y<dy+dh+m_nDockingGap) && abs(dx-(x+w)) < m_nDockingGap) {x= dx- w;bRet=true;m_bDockToLeftOfWindow=true; }	// 좌측 붙이기
 	if((dx-m_nDockingGap<x+w && x<dx+dw+m_nDockingGap) && abs(dy+dh-y ) < m_nDockingGap) {y= dy+dh;bRet=true;m_bDockToTopOfWindow=false;}	// 하단 붙이기
	if((dx-m_nDockingGap<x+w && x<dx+dw+m_nDockingGap) && abs(dy-(y+h)) < m_nDockingGap) {y= dy- h;bRet=true;m_bDockToTopOfWindow=true; }	// 상단 붙이기

	if(bRet)				// 만일 좌/우/상/하 어디든지 붙어 있는 상황이라면..
	{
		x = ( abs(x+w-(dx+dw)) < m_nDockingGap ) ? dx+dw-w : x;		// 우측선 정렬
		x = ( abs(x - dx     ) < m_nDockingGap ) ? dx      : x;		// 좌측선 정렬
		y = ( abs(y+h-(dy+dh)) < m_nDockingGap ) ? dy+dh-h : y;		// 하단선 정렬
		y = ( abs(y - dy     ) < m_nDockingGap ) ? dy      : y;		// 상단선 정렬
	}

	m_rWindowToDock = rDockWnd;

	return bRet;
}

