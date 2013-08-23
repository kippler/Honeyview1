// Docker.h: interface for the GDocker class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_DOCKER_H__7C0C22A9_4F23_4086_9087_40A26809571F__INCLUDED_)
#define AFX_DOCKER_H__7C0C22A9_4F23_4086_9087_40A26809571F__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000


///////////////////////////////////////////////////////////////////////////////
//
// GDocker 
//	- 윈도우를 들러붙게 만들어주는 클래스.
//	- 최초 작성일 : 1999/12/14 by hong
//	- 수정 노트
//		99/12/16 : 초기화 안했을 경우 ASSERT하기 추가.
//		99/12/27 : 윈도우에 들러붙을때 뻐그 수정, math 함수 빼버림. 
//		00/02/23 : 벽과 윈도우에 동시에 붙는 경우 처리가 제대로 됨. 
//					& 다른 윈도우에 들러 붙을시 제대로 쫒아가지 못하던 문제 해결
//		00/02/26 : 다른 윈도우에 들러붙을시 버그 해결...
//		00/03/15 : CheckDockAtStart() 추가 .
//
//	- 사용법 
//		. CWnd 의 멤버로서 GDocker 를 집어넣는다.
//		. Create를 호출하면서  this 와 들러 붙고 싶은 윈도우의 포인터를 넘긴다.
//		. OnLButtonDown() / OnNcLButtonDown() / OnMouseMove() / 
//		  OnLButtonUp() / OnTimer() 등을 호출한다.
//		. OnLButtonDown() 을 호출하면 클라이언트 영역을 눌렀을때 윈도우 이동이 가능하며.
//		. OnNcLButtonDown() 을 호출하면 캡션 영역을 잡고 이동시에 윈도우 이동이 처리된다.
//		. OnTimer() 는 다른 윈도우에 들러 붙을 경우에만 호출하여 주면 된다.
//
///////////////////////////////////////////////////////////////////////////////

/******************************************************************************
******************************* 사용예 *****************************************

BOOL CDockingWnd::Create(CWnd* pWndToDock)
{
	...
	...
  	m_docker.Create(this,pWndToDock);
	...
	...
}

// 클라이언트 영역 드래그 처리시 //
void CDockingWnd::OnLButtonDown(UINT nFlags, CPoint point) 
{
	m_docker.OnLButtonDown(nFlags, point);
}
void CDockingWnd::OnMouseMove(UINT nFlags, CPoint point) 
{
	m_docker.OnMouseMove();
}
void CDockingWnd::OnLButtonUp(UINT nFlags, CPoint point) 
{
	m_docker.OnLButtonUp();
}
// 캡션 영역 드래그 처리시 //
void CDockingWnd::OnNcLButtonDown(UINT nHitTest, CPoint point) 
{
	m_docker.OnNcLButtonDown(nHitTest);
}
// 다른 윈도우에 들러 붙을 경우 처리 //
void CDockingWnd::OnTimer(UINT nIDEvent) 
{
	m_docker.OnTimer(nIDEvent);
}

*******************************************************************************
*******************************************************************************/


class GDocker  
{
public :
	void				Create(CWnd* pWnd, CWnd* pWndToDock=NULL);
	void				SetDockingGap(int nDockingGap) { m_nDockingGap = nDockingGap; }		// 들러 붙을 거리 지정.

public :
	void				OnLButtonDown();
	void				OnNcLButtonDown(UINT nHitTest);
	void				OnMouseMove();
	void				OnLButtonUp();
	void				OnTimer(UINT nIDEvent);
	void				CheckDockAtStart();									

private :
	bool				m_bCaptured;
	bool				m_bDockedToWindow;
	bool				m_bDockToLeftOfWindow;											// m_bDockedToWindow 가 true 일경우 left 인가 아니면 right 인가?
	bool				m_bDockToTopOfWindow;											// m_bDockedToWindow 가 true 일경우 top  인가 아니면 bottom 인가?
	CPoint				m_pntOld;
	CRect				m_rWindow;
	CRect				m_rWindowToDock;
	CWnd*				m_pWndToDock;
	CWnd*				m_pWnd;
	int					m_nDockingGap;
	bool				m_bInit;

private :
	void				DockMove();
	bool				CheckDockToScreen(int &x, int &y, int w, int h);
	bool				CheckDockToWindow(int &x, int &y, int w, int h);

public:
	GDocker();
	virtual ~GDocker();

};

#endif // !defined(AFX_DOCKER_H__7C0C22A9_4F23_4086_9087_40A26809571F__INCLUDED_)
