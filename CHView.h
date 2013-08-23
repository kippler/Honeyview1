#pragma once


class GDib;
class CMainFrame;

typedef vector<CString>	 Strings;

enum FILE_TYPE
{
	FT_NORMAL,
	FT_PCO,
	FT_ZIP,
};


#include "PCO.h"



class CHView : public CWnd
{
// Construction
public:
	CHView();


private :
	Strings				m_files;
	Strings::iterator	m_curfile;

	CRect				m_rWindow;

	GDib*				m_pCurDib;
	GDib*				m_pCurDib2;
	GDib*				m_pCurDibStretch;

	CString				m_sCachedFileName;
	GDib*				m_pCachedDib;
	GDib*				m_pCachedDib2;
	GDib*				m_pCachedDibStretch;
	bool				m_bCaching;
	FILE_TYPE			m_nFileType;
	CMainFrame*			m_pParent;

private :
	CString				m_sPCOFileName;
	CPco				m_pco;

private :
	CString				m_sZIPFileName;


// Attributes
public:
	bool				Refresh();
	CString				GetFilePosInfo();

public :
	void				OpenFolder(CString sFolder);
	void				OpenPCO(CString sFileName);
	void				OpenZIP(CString sFileName);

private :
	bool				Refresh1();
	bool				Refresh2();
	bool				ShowNextFile(bool bNext);
	bool				ShowNext2File(bool bNext);
	void				CacheNextFile(bool bNext);
	void				Cache2NextFile(bool bNext);
	void				SelectPage();
	void				SortFiles();
	bool				LoadFile(GDib* pDib, CString sFileName);
	
private :
	bool				LoadImageFile();
	bool				LoadImageFile2();
	bool				ProcessImage(GDib* pSrc, GDib* pDest,bool bDrawToScreen=false);
	bool				ProcessImage2(GDib* pSrc, GDib* pSrc2, GDib* pDest,bool bDrawToScreen=false);
	bool				AddFileToList(CString sFileName);

// Operations
public:

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHView)
	public:
	virtual BOOL Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext = NULL);
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL
	void	OnDropFiles(HDROP hDropInfo) ;

// Implementation
public:
	virtual ~CHView();

	// Generated message map functions
protected:
	//{{AFX_MSG(CHView)
	afx_msg void OnPaint();
	afx_msg void OnFileNext();
	afx_msg void OnFilePrev();
	afx_msg void OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags);
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnMButtonDown(UINT nFlags, CPoint point);
	afx_msg BOOL OnMouseWheel(UINT nFlags, short zDelta, CPoint pt);
	afx_msg void OnOpenFolder();
	afx_msg void OnOpenFile();
	afx_msg void OnPrev10();
	afx_msg void OnPrev();
	afx_msg void OnNext();
	afx_msg void OnNext10();
	afx_msg void OnFlippage();
	afx_msg void OnUpdateFlippage(CCmdUI* pCmdUI);
	afx_msg void OnShow2page();
	afx_msg void OnUpdateShow2page(CCmdUI* pCmdUI);
	afx_msg void OnInterpolation();
	afx_msg void OnUpdateInterpolation(CCmdUI* pCmdUI);
	afx_msg void OnStretching();
	afx_msg void OnUpdateStretching(CCmdUI* pCmdUI);
	afx_msg void OnSelectpage();
	afx_msg void OnOpenPco();
	afx_msg void OnSharpen();
	afx_msg void OnUpdateSharpen(CCmdUI* pCmdUI);
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnRotate();
	afx_msg void OnUpdateRotate(CCmdUI* pCmdUI);
	afx_msg void OnLButtonUp(UINT nFlags, CPoint point);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

