// SelectPageDlg.cpp : implementation file
//

#include "stdafx.h"
#include "honeyview.h"
#include "SelectPageDlg.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CSelectPageDlg dialog


CSelectPageDlg::CSelectPageDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CSelectPageDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(CSelectPageDlg)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
	m_pFiles = 0;
	m_nItemPos = 0;
}


void CSelectPageDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CSelectPageDlg)
	DDX_Control(pDX, IDC_LIST_FILES, m_listFiles);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CSelectPageDlg, CDialog)
	//{{AFX_MSG_MAP(CSelectPageDlg)
	ON_NOTIFY(NM_DBLCLK, IDC_LIST_FILES, OnDblclkListFiles)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSelectPageDlg message handlers

BOOL CSelectPageDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
	SetColumn();

	Strings::iterator	i;

	for(i=(*m_pFiles).begin(); i<(*m_pFiles).end(); i++)
	{
		m_listFiles.InsertItem(m_listFiles.GetItemCount(), *i);
	}

	m_listFiles.SetExtendedStyle(LVS_EX_FULLROWSELECT );
	m_listFiles.SetItemState(m_nItemPos, LVIS_SELECTED|LVIS_FOCUSED, LVIS_SELECTED|LVIS_FOCUSED);
	m_listFiles.EnsureVisible(m_nItemPos, TRUE);
	
	return TRUE; 
}


// 리스트 콘트롤의 컬럼을 세팅한다.
void CSelectPageDlg::SetColumn()
{
	LVCOLUMN	lvcolumn;
	lvcolumn.mask = LVCF_FMT | LVCF_SUBITEM | LVCF_TEXT | LVCF_WIDTH;
	lvcolumn.fmt = LVCFMT_LEFT;
	lvcolumn.iSubItem = 0;

	CRect r;
	m_listFiles.GetWindowRect(&r);
	lvcolumn.cx = r.Width() - GetSystemMetrics(SM_CXHSCROLL) - 5;
	lvcolumn.pszText = "파일 목록";
	m_listFiles.InsertColumn(0,&lvcolumn);
}

void CSelectPageDlg::OnOK() 
{
	POSITION p;
	p = m_listFiles.GetFirstSelectedItemPosition();
	if(p==NULL) return;

	m_nItemPos = m_listFiles.GetNextSelectedItem(p);
	
	CDialog::OnOK();
}

void CSelectPageDlg::OnDblclkListFiles(NMHDR* pNMHDR, LRESULT* pResult) 
{
	*pResult = 0;
	OnOK();
}
