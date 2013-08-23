#include "stdafx.h"
#include "honeyview.h"
#include "OptionDlg.h"
#include "Global.h"
#include "Option.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// COptionDlg dialog


COptionDlg::COptionDlg(CWnd* pParent /*=NULL*/)
	: CDialog(COptionDlg::IDD, pParent)
{
	//{{AFX_DATA_INIT(COptionDlg)
	m_bFlipPage = FALSE;
	m_bInterpolation = FALSE;
	m_bStretech = FALSE;
	m_bView2Page = FALSE;
	m_bCacheNextFile = FALSE;
	m_bAutoDetectWidth = FALSE;
	m_bSharpen = FALSE;
	//}}AFX_DATA_INIT

	m_bFlipPage = g_Option.bFlipPage;
	m_bInterpolation = g_Option.bInterpolation;
	m_bSharpen = g_Option.bSharpen;
	m_bStretech = g_Option.bStretech;
	m_bView2Page = g_Option.bView2Page;
	m_bCacheNextFile = g_Option.bCacheNextFile;
	m_bAutoDetectWidth = g_Option.bAutoDetectWidth;
}


void COptionDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(COptionDlg)
	DDX_Check(pDX, IDC_CHECK_FLIPPAGE, m_bFlipPage);
	DDX_Check(pDX, IDC_CHECK_INTERPOLATION, m_bInterpolation);
	DDX_Check(pDX, IDC_CHECK_STRETCH, m_bStretech);
	DDX_Check(pDX, IDC_CHECK_VIEW2PAGE, m_bView2Page);
	DDX_Check(pDX, IDC_CHECK_CACHENEXTPAGE, m_bCacheNextFile);
	DDX_Check(pDX, IDC_CHECK_AUTODETECTWIDTH, m_bAutoDetectWidth);
	DDX_Check(pDX, IDC_CHECK_SHARPEN, m_bSharpen);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(COptionDlg, CDialog)
	//{{AFX_MSG_MAP(COptionDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// COptionDlg message handlers

void COptionDlg::OnOK() 
{
	UpdateData();

	g_Option.bFlipPage = m_bFlipPage;
	g_Option.bInterpolation = m_bInterpolation;
	g_Option.bSharpen = m_bSharpen;
	g_Option.bStretech = m_bStretech;
	g_Option.bView2Page = m_bView2Page;
	g_Option.bCacheNextFile = m_bCacheNextFile;
	g_Option.bAutoDetectWidth = m_bAutoDetectWidth;

	g_Option.SaveOption();

	CDialog::OnOK();
}

void COptionDlg::OnCancel() 
{
	CDialog::OnCancel();
}
