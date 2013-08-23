#if !defined(AFX_SELECTPAGEDLG_H__AA4BE960_006B_11D5_A480_00105A84B4E0__INCLUDED_)
#define AFX_SELECTPAGEDLG_H__AA4BE960_006B_11D5_A480_00105A84B4E0__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// SelectPageDlg.h : header file
//


#include "CHView.h"

class CSelectPageDlg : public CDialog
{
// Construction
public:
	CSelectPageDlg(CWnd* pParent = NULL);   // standard constructor


	void		SetFiles(Strings* pFiles) { m_pFiles = pFiles; }
	void		SetColumn();
	void		SetItemPos(int nItemPos) { m_nItemPos = nItemPos; }

	Strings*	m_pFiles;
	int			m_nItemPos;


public :

// Dialog Data
	//{{AFX_DATA(CSelectPageDlg)
	enum { IDD = IDD_SELECTPAGE };
	CListCtrl	m_listFiles;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CSelectPageDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CSelectPageDlg)
	virtual BOOL OnInitDialog();
	virtual void OnOK();
	afx_msg void OnDblclkListFiles(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_SELECTPAGEDLG_H__AA4BE960_006B_11D5_A480_00105A84B4E0__INCLUDED_)
