#include "stdafx.h"
#include "resource.h"
#include "aboutdlg.h"


/////////////////////////////////////////////////////////////////////////////
// CAboutDlg dialog used for App About


CAboutDlg::CAboutDlg() : CDialog(CAboutDlg::IDD)
{
	//{{AFX_DATA_INIT(CAboutDlg)
	//}}AFX_DATA_INIT
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CAboutDlg)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialog)
	//{{AFX_MSG_MAP(CAboutDlg)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

BOOL CAboutDlg::OnInitDialog() 
{
	CDialog::OnInitDialog();

	CString sVersion;
	CString sAppName;

	sVersion.LoadString(IDS_VERSION);
	sVersion = sVersion.Right(sVersion.GetLength() - sVersion.ReverseFind('.') - 1);

	sAppName.Format("%s [ºôµå%s]", APP_NAME, sVersion);

	GetDlgItem(IDC_APPNAME)->SetWindowText(sAppName);

	Help();

	return TRUE; 
}

bool CAboutDlg::Help()
{
	HRSRC		hResInfo;
	HANDLE		hRes;
	char*		pResource=NULL;
	int			nBufSize;
	HMODULE		hInst = AfxGetInstanceHandle();

	char		szName[] = "IDR_TXT_ABOUT";
	char		szType[] = "TXT";
	char*		szRes;


	hResInfo = FindResource(hInst, szName, szType);
	if (hResInfo == NULL) 
		return false;

	// Load the resource
	hRes = LoadResource(hInst, hResInfo);

	// Lock the resource
	szRes = (char*)LockResource(hRes);

	if (szRes != NULL)
	{ 
		nBufSize = SizeofResource(hInst, hResInfo);
		pResource = (char*)malloc(nBufSize+1);
		memset(pResource, 0, nBufSize+1);
		memcpy(pResource, szRes, nBufSize);
		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);


	if(pResource)
	{
		GetDlgItem(IDC_EDIT_HELP)->SetWindowText(pResource);
		free(pResource);
	}

	return true;
}