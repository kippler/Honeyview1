#if !defined(AFX_OPTIONDLG_H__A8F74B60_FAC4_11D4_A480_00105A84B4E0__INCLUDED_)
#define AFX_OPTIONDLG_H__A8F74B60_FAC4_11D4_A480_00105A84B4E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// OptionDlg.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog

class COptionDlg : public CDialog
{
// Construction
public:
	COptionDlg(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(COptionDlg)
	enum { IDD = IDD_OPTION };
	BOOL	m_bFlipPage;
	BOOL	m_bInterpolation;
	BOOL	m_bStretech;
	BOOL	m_bView2Page;
	BOOL	m_bCacheNextFile;
	BOOL	m_bAutoDetectWidth;
	BOOL	m_bSharpen;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(COptionDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(COptionDlg)
	virtual void OnOK();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_OPTIONDLG_H__A8F74B60_FAC4_11D4_A480_00105A84B4E0__INCLUDED_)
