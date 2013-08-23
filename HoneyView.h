// HoneyView.h : main header file for the HONEYVIEW application
//

#if !defined(AFX_HONEYVIEW_H__C95F48C9_9A2B_4A9E_BDE0_C34DFB9EE593__INCLUDED_)
#define AFX_HONEYVIEW_H__C95F48C9_9A2B_4A9E_BDE0_C34DFB9EE593__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#ifndef __AFXWIN_H__
	#error include 'stdafx.h' before including this file for PCH
#endif

#include "resource.h"       // main symbols

/////////////////////////////////////////////////////////////////////////////
// CHoneyViewApp:
// See HoneyView.cpp for the implementation of this class
//

class CHoneyViewApp : public CWinApp
{
public:
	CHoneyViewApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CHoneyViewApp)
	public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();
	//}}AFX_VIRTUAL

// Implementation

public:
	//{{AFX_MSG(CHoneyViewApp)
	afx_msg void OnAppAbout();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};


/////////////////////////////////////////////////////////////////////////////

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_HONEYVIEW_H__C95F48C9_9A2B_4A9E_BDE0_C34DFB9EE593__INCLUDED_)
