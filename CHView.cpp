#include "stdafx.h"
#include "HoneyView.h"
#include "CHView.h"
#include "Gutil.h"
#include "GDib.h"
#include "GMessageBoxWnd.h"
#include "Global.h"
#include "Option.h"
#include "MainFrm.h"
#include "SelectPageDlg.h"
#include "unzip.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

void ProgressFunc(int nCur, int nRange, void* param)
{
	CMainFrame*	pWnd = (CMainFrame*)param;
	pWnd->SetProgress(nCur *100 / nRange);
}

bool IsImageExt(CString sExt)
{
	sExt.MakeLower();
	if(sExt==".jpg" || sExt==".jpe" || sExt==".jpeg" || sExt==".gif" || sExt==".bmp")
		return true;
	return false;
}

bool IsImageFile(CString sFileName)
{
	CString sExt;
//	sExt = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.'));
	int nPos;
	nPos = sFileName.ReverseFind('.');
	sExt = sFileName.Mid(nPos);

	return IsImageExt(sExt);
}


// UNZIP 에서 사용.
#define CASESENSITIVITY (0)


#define		CLR_BKG		RGB(255,255,255)

#define		TIMERID_CACHE_NEXT	1
#define		TIMERID_CACHE_PREV	2

bool IsHorizontal(CSize s)
{
	return s.cx > s.cy ? true : false;
}
/////////////////////////////////////////////////////////////////////////////
// CHView

CHView::CHView()
{
	m_pCurDib = new GDib();
	m_pCurDibStretch = new GDib();
	m_pCachedDib = new GDib();
	m_pCachedDib2 = new GDib();
	m_pCachedDibStretch = new GDib();
	m_pCurDib2 = new GDib();
	m_bCaching = false;

	m_pCurDib->CreateNullImage(4,4);
	m_pCurDibStretch->CreateNullImage(4,4);
	m_pCachedDib->CreateNullImage(4,4);
	m_pCachedDib2->CreateNullImage(4,4);
	m_pCachedDibStretch->CreateNullImage(4,4);
	m_pCurDib2->CreateNullImage(4,4);

	m_curfile = m_files.end();
	m_nFileType = FT_NORMAL;
	m_pParent = NULL;
}

CHView::~CHView()
{
	if(m_pCurDib) delete m_pCurDib;
	if(m_pCurDib2) delete m_pCurDib2;
	if(m_pCurDibStretch) delete m_pCurDibStretch;
	if(m_pCachedDib) delete m_pCachedDib;
	if(m_pCachedDib2) delete m_pCachedDib2;
	if(m_pCachedDibStretch) delete m_pCachedDibStretch;
}


BEGIN_MESSAGE_MAP(CHView,CWnd )
	//{{AFX_MSG_MAP(CHView)
	ON_WM_PAINT()
	ON_COMMAND(IDM_FILE_NEXT, OnFileNext)
	ON_COMMAND(IDM_FILE_PREV, OnFilePrev)
	ON_WM_KEYDOWN()
	ON_WM_TIMER()
	ON_WM_SIZE()
	ON_WM_MBUTTONDOWN()
	ON_WM_MOUSEWHEEL()
	ON_COMMAND(IDM_OPEN_FOLDER, OnOpenFolder)
	ON_COMMAND(IDM_OPEN_FILE, OnOpenFile)
	ON_COMMAND(IDM_PREV10, OnPrev10)
	ON_COMMAND(IDM_PREV, OnPrev)
	ON_COMMAND(IDM_NEXT, OnNext)
	ON_COMMAND(IDM_NEXT10, OnNext10)
	ON_COMMAND(IDM_FLIPPAGE, OnFlippage)
	ON_UPDATE_COMMAND_UI(IDM_FLIPPAGE, OnUpdateFlippage)
	ON_COMMAND(IDM_SHOW2PAGE, OnShow2page)
	ON_UPDATE_COMMAND_UI(IDM_SHOW2PAGE, OnUpdateShow2page)
	ON_COMMAND(IDM_INTERPOLATION, OnInterpolation)
	ON_UPDATE_COMMAND_UI(IDM_INTERPOLATION, OnUpdateInterpolation)
	ON_COMMAND(IDM_STRETCHING, OnStretching)
	ON_UPDATE_COMMAND_UI(IDM_STRETCHING, OnUpdateStretching)
	ON_COMMAND(IDM_SELECTPAGE, OnSelectpage)
	ON_COMMAND(IDM_OPEN_PCO, OnOpenPco)
	ON_COMMAND(IDM_SHARPEN, OnSharpen)
	ON_UPDATE_COMMAND_UI(IDM_SHARPEN, OnUpdateSharpen)
	ON_WM_LBUTTONDBLCLK()
	ON_COMMAND(IDM_ROTATE, OnRotate)
	ON_UPDATE_COMMAND_UI(IDM_ROTATE, OnUpdateRotate)
	ON_WM_LBUTTONUP()
	//}}AFX_MSG_MAP
	ON_WM_DROPFILES()
END_MESSAGE_MAP()


/////////////////////////////////////////////////////////////////////////////
// CHView message handlers

BOOL CHView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(NULL, IDC_ARROW), HBRUSH(COLOR_WINDOW+1), NULL);

	return TRUE;
}

BOOL CHView::Create(LPCTSTR lpszClassName, LPCTSTR lpszWindowName, DWORD dwStyle, const RECT& rect, CWnd* pParentWnd, UINT nID, CCreateContext* pContext) 
{
	BOOL bRet = CWnd::Create(lpszClassName, lpszWindowName, dwStyle, rect, pParentWnd, nID, pContext);

	m_pParent = (CMainFrame*)pParentWnd;

	DragAcceptFiles();


	return bRet;
}

void CHView::OnPaint() 
{
	CPaintDC dc(this); // device context for painting

	if(m_pCurDibStretch->IsCreated())
		m_pCurDibStretch->DrawTo(&dc);
}

// 파일 드롭.
void CHView::OnDropFiles(HDROP hDropInfo) 
{
	char	szFileName[_MAX_PATH];
	int		nCount;
	int		i;

	nCount = ::DragQueryFile(hDropInfo, -1, NULL, 0);

	m_files.clear();

	for(i=0;i<nCount;i++)
	{
		::DragQueryFile(hDropInfo, i, szFileName, _MAX_PATH);

		if(i==0 && IsFolder(szFileName))								// 첫번째가 폴더이면.?
		{
			m_nFileType = FT_NORMAL;
			::DragFinish(hDropInfo);
			g_Option.sRecentFolder = szFileName;
			OpenFolder(szFileName);
			return;
		}

		if(i==0)
		{
			CString sFileName = szFileName;
			CString sExt = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.'));
			sExt.MakeLower();
			if(sExt==".pco")
			{
				g_Option.sRecentFolder = sFileName.Left(sFileName.ReverseFind('\\'));	// 최근 폴더 바꾸기..;
				OpenPCO(sFileName);
				return;
			}
			if(sExt==".zip")
			{
				g_Option.sRecentFolder = sFileName.Left(sFileName.ReverseFind('\\'));	// 최근 폴더 바꾸기..;
				OpenZIP(sFileName);
				return;
			}
		}
		AddFileToList(szFileName);
	}

	m_nFileType = FT_NORMAL;

	SortFiles();
	m_curfile = m_files.end();

	::DragFinish(hDropInfo);

	if(m_files.size())
		ShowNextFile(true);
}

// 폴더 열기
void CHView::OnOpenFolder() 
{
	CString sFolder;
	sFolder = GetDirectory(*this, g_Option.sRecentFolder , "파일이 있는 폴더를 지정해 주세요.");

	if(sFolder=="") return; 

	m_nFileType = FT_NORMAL;

	OpenFolder(sFolder);
}

void CHView::OpenFolder(CString sFolder)
{

	g_Option.sRecentFolder = sFolder;

	CFileFind	ff;
	BOOL		bResult;

	bResult = ff.FindFile(AddPath(sFolder,"*.*"));
	if(bResult==FALSE)														// 패스가 더이상 존재하지 않을 경우.
	{
		CString sMsg;
		sMsg.Format("폴더[ %s ]를 열 수 없습니다.", sFolder);
		GMessageBox(this, sMsg, APP_NAME);
		return;
	}

	m_files.clear();

	while (bResult)
	{
		bResult = ff.FindNextFile();
		if(ff.IsDots()) continue;
		AddFileToList(AddPath(sFolder, ff.GetFileName()));
	}

	SortFiles();

	m_curfile = m_files.end();
	if(m_files.size())
		ShowNextFile(true);
	else
	{
		CString sMsg;
		sMsg.Format("폴더[ %s ]에 파일이 하나도 없습니다.", sFolder);
		GMessageBox(this, sMsg, APP_NAME);
	}
}

#define		MAX_OPEN_FILE		100
// 파일 열기
void CHView::OnOpenFile() 
{
	CFileDialog	dlg(TRUE, "", "", 
		OFN_ALLOWMULTISELECT|OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_ENABLESIZING, "이미지 파일들(*.bmp;*.gif;*.jpg)|*.bmp; *.gif; *.jpg||");

	dlg.m_ofn.lpstrFile = (char*)malloc(MAX_OPEN_FILE*_MAX_PATH);
	::SetCurrentDirectory(g_Option.sRecentFolder);
	strcpy(dlg.m_ofn.lpstrFile,"");
	dlg.m_ofn.nMaxFile = MAX_OPEN_FILE*_MAX_PATH;

	if(dlg.DoModal()!=IDOK) goto END;

	m_nFileType = FT_NORMAL;

	char buf[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, buf);
	g_Option.sRecentFolder = buf; 

	POSITION p;
	p = dlg.GetStartPosition();
	if(p==NULL) goto END;

	m_files.clear();

	while(p)
	{
		AddFileToList(dlg.GetNextPathName(p));
	}

	SortFiles();

	m_curfile = m_files.end();
	if(m_files.size())
		ShowNextFile(true);

END :
	free(dlg.m_ofn.lpstrFile);

}

// PCO,ZIP 파일 열기
void CHView::OnOpenPco() 
{
	CFileDialog	dlg(TRUE, "", "", 
		OFN_HIDEREADONLY|OFN_FILEMUSTEXIST|OFN_ENABLESIZING, "PCO,ZIP 파일(*.PCO;*.ZIP)|*.PCO; *.ZIP||");
 
	::SetCurrentDirectory(g_Option.sRecentFolder);
	strcpy(dlg.m_ofn.lpstrFile,"");

	if(dlg.DoModal()!=IDOK) return;

	char buf[MAX_PATH];
	::GetCurrentDirectory(MAX_PATH, buf);
	g_Option.sRecentFolder = buf; 

	CString sFileName = dlg.GetPathName();
	CString sExt = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.'));
	sExt.MakeLower();
	if(sExt==".pco")
	{
		OpenPCO(sFileName);
		return;
	}
	if(sExt==".zip")
	{
		OpenZIP(sFileName);
		return;
	}
}

void CHView::OpenPCO(CString sFileName) 
{
	m_sPCOFileName = sFileName;

	if(m_pco.ReadHeader(m_sPCOFileName)==false)
	{
		GMessageBox(this, "PCO 파일을 열 수 없습니다. - 아마도 깨졌을껄?", APP_NAME);
		return;
	}

	m_nFileType = FT_PCO;


	// 목록에 추가하기.
	CString s;
	m_files.clear();

	for(int i=0;i<m_pco.m_head.image_count;i++)
	{
		s.Format("%04d", i);
		m_files.push_back(s);
	}

	m_curfile = m_files.end();
	SortFiles();

	if(m_files.size())
		ShowNextFile(true);
}



void CHView::OpenZIP(CString sFileName) 
{
	m_sZIPFileName = sFileName;

	unzFile				uf;
	uf = unzOpen(m_sZIPFileName);
	if (uf==NULL)
	{
		GMessageBox(this, "ZIP 파일을 열 수 없습니다. - 아마도 깨졌을껄?", APP_NAME);
		return;
	}

	m_nFileType = FT_ZIP;

	// 기존 목록 지우기
	m_files.clear();

	// 파일 목록 구하기
	uLong i;
	unz_global_info gi;
	int err;
	
	err = unzGetGlobalInfo (uf,&gi);
	if (err!=UNZ_OK)
	{
		unzCloseCurrentFile(uf);
		uf = NULL;

		CString s;
		unzClose(uf);
		s.Format("error %d with zipfile in unzGetGlobalInfo \n",err);
		GMessageBox(this, s, APP_NAME);
		return;
	}


	for (i=0;i<gi.number_entry;i++)
	{
		char filename_inzip[256];
		unz_file_info file_info;
		uLong ratio=0;
		const char *string_method;
		CString s;
		err = unzGetCurrentFileInfo(uf,&file_info,filename_inzip,sizeof(filename_inzip),NULL,0,NULL,0);
		if (err!=UNZ_OK)
		{
			//printf("error %d with zipfile in unzGetCurrentFileInfo\n",err);
			break;
		}
		if (file_info.uncompressed_size>0)
			ratio = (file_info.compressed_size*100)/file_info.uncompressed_size;
		
		if (file_info.compression_method==0)
			string_method="Stored";
		else
			if (file_info.compression_method==Z_DEFLATED)
			{
				uInt iLevel=(uInt)((file_info.flag & 0x6)/2);
				if (iLevel==0)
					string_method="Defl:N";
				else if (iLevel==1)
					string_method="Defl:X";
				else if ((iLevel==2) || (iLevel==3))
					string_method="Defl:F"; /* 2:fast , 3 : extra fast*/
			}
			else
				string_method="Unkn. ";

			s = filename_inzip;
			s.MakeLower();

			if(s.Right(1)!="\\"&&s.Right(1)!="/")								// 파일이름의 끝이  \\ 인놈은 폴더 이다.
				if(IsImageFile(s))
					m_files.push_back(s);
/*			
			printf("%7lu  %6s %7lu %3lu%%  %2.2lu-%2.2lu-%2.2lu  %2.2lu:%2.2lu  %8.8lx   %s\n",
				file_info.uncompressed_size,string_method,file_info.compressed_size,
				ratio,
				(uLong)file_info.tmu_date.tm_mon + 1,
                (uLong)file_info.tmu_date.tm_mday,
				(uLong)file_info.tmu_date.tm_year % 100,
				(uLong)file_info.tmu_date.tm_hour,(uLong)file_info.tmu_date.tm_min,
				(uLong)file_info.crc,filename_inzip);
*/
			if ((i+1)<gi.number_entry)
			{
				err = unzGoToNextFile(uf);
				if (err!=UNZ_OK)
				{
//					printf("error %d with zipfile in unzGoToNextFile\n",err);
					break;
				}
			}
	}

	unzClose(uf);
	m_curfile = m_files.end();
	SortFiles();
	
	if(m_files.size())
		ShowNextFile(true);

}

bool CHView::AddFileToList(CString sFileName)
{
	CString	sExt;

	sFileName.MakeLower();

	sExt = sFileName.Right(sFileName.GetLength()-sFileName.ReverseFind('.'));

	// 처리 가능한 파일만 목록에 추가 하기. 
	if(IsImageExt(sExt))
	{
		if(IsFile(sFileName))
		{
			m_files.push_back(sFileName);
			g_Option.sRecentFolder = sFileName.Left(sFileName.ReverseFind('\\'));	// 최근 폴더 바꾸기..
		}
	}
	else
		return false;

	return true;
}

void CHView::SortFiles()
{
	if(m_files.size())
		sort(m_files.begin(), m_files.end());
}


bool CHView::ShowNextFile(bool bNext)
{
	if(m_bCaching) return false;


	if(g_Option.bView2Page)
	{
		return ShowNext2File(bNext);
	}

	if(m_files.size()==0) 
	{
		GMessageBox(this, "목록에 파일이 없습니다.", APP_NAME);
		return false;
	}

	if(m_curfile==m_files.end())
	{
		if(bNext)
			m_curfile = m_files.begin();
		else
			return false;
	}
	else if(m_curfile+1==m_files.end() && bNext)
	{
		GAlaramBox(this, "마지막 파일 입니다.", APP_NAME, 1);
		return false;
	}
	else if(m_curfile==m_files.begin() && bNext==false)
	{
		GAlaramBox(this, "맨 처음 파일 입니다.", APP_NAME, 1);
		return false;
	}
	else 
	{
		if(bNext)
			m_curfile++;
		else
			m_curfile--;
	}


	bool bRet;

	bRet =  Refresh();

	if(bRet==false) 
	{
		return false;
	}

	// 캐슁 
	if(g_Option.bCacheNextFile)
	{
		if(bNext)
		{
			SetTimer(TIMERID_CACHE_NEXT, 100, NULL);
		}
		else
		{
			SetTimer(TIMERID_CACHE_PREV, 100, NULL);
		}
		m_bCaching = true;
	}
	else
	{
		MessageBox("no");
	}

	return true;
}

bool CHView::Refresh()
{
	if(m_curfile==m_files.end()) return false;

	bool bRet;
	
	if(g_Option.bView2Page)
		bRet = Refresh2();
	else
		bRet = Refresh1();

	CString s;
	if(m_nFileType==FT_NORMAL)
		s.Format("%s [%s]", APP_NAME, (const char*)*m_curfile);
	else if(m_nFileType==FT_PCO)
		s.Format("%s [%s  %s]", APP_NAME, m_sPCOFileName, (const char*)*m_curfile);
	else if(m_nFileType==FT_ZIP)
		s.Format("%s [%s  %s]", APP_NAME, m_sZIPFileName, (const char*)*m_curfile);


	m_pParent->SetTitle(s);

	return bRet;
}


bool CHView::Refresh1()
{
	bool bRet=true;

	if(*m_curfile == m_sCachedFileName)									
	{
		swap(m_pCurDib, m_pCachedDib);			// 캐쉬되어 있을경우..
	}
	else
	{
		// 이미지 로딩
		bRet = LoadImageFile();
		//Invalidate(TRUE);
		m_pCurDib->DrawTo(this);
	}
	

	if(*m_curfile == m_sCachedFileName
		&&
		g_Option.bStretech && m_pCachedDibStretch->GetSize() == m_rWindow.Size())
	{
		swap(m_pCurDibStretch, m_pCachedDibStretch);
	}
	else
	{
		// 이미지 처리 
		bRet = ProcessImage(m_pCurDib, m_pCurDibStretch, true);
	}

	Invalidate(FALSE);

	return bRet;
}
bool CHView::Refresh2()
{
	bool bRet=true;

	if(*m_curfile == m_sCachedFileName)									
	{
		swap(m_pCurDib, m_pCachedDib);			// 캐쉬되어 있을경우..
		swap(m_pCurDib2, m_pCachedDib2);			// 캐쉬되어 있을경우..
	}
	else
	{
		// 이미지 로딩
		bRet = LoadImageFile2();
		if(bRet==false) 
			return false;
		//Invalidate(TRUE);
		m_pCurDib->DrawTo(this);
	}

	if(*m_curfile == m_sCachedFileName
		&&
		g_Option.bStretech && m_pCachedDibStretch->GetSize() == m_rWindow.Size())
	{
		swap(m_pCurDibStretch, m_pCachedDibStretch);
	}
	else
	{
		// 이미지 처리 
		bRet = ProcessImage2(m_pCurDib, m_pCurDib2, m_pCurDibStretch, true);
	}


	Invalidate(FALSE);

	return bRet;
}


bool CHView::ShowNext2File(bool bNext)
{
	bool bRet;
	if(m_bCaching) return false;

	if(m_files.size()==0) 
	{
		GAlaramBox(this, "목록에 파일이 없습니다.", APP_NAME, 1);
		return false;
	}

	if(m_curfile==m_files.end())
	{
		if(bNext)
			m_curfile = m_files.begin();
		else
			return false;
	}
	else if(m_curfile+2>=m_files.end() && bNext)
	{
		GAlaramBox(this, "맨 마지막 파일 입니다.", APP_NAME, 1);
		return false;
	}
	else if(m_curfile-1<=m_files.begin() && bNext==false)
	{
		GAlaramBox(this, "맨 처음 파일 입니다.", APP_NAME, 1);
		return false;
	}
	else 
	{
		int nStep = 2;
		if(g_Option.bAutoDetectWidth)
		{
			if(IsHorizontal(m_pCurDib->GetSize()))				// 현재 그림의 폭이 길때 - 1페이지만 넘어가기..
				nStep = 1;
		}

		if(bNext)
			m_curfile+=nStep;
		else
			m_curfile-=nStep;
	}

	bRet =  Refresh();

	if(bRet==false) return false;

	// 캐슁 
	if(g_Option.bCacheNextFile)
	{
		if(bNext)
			SetTimer(TIMERID_CACHE_NEXT, 10, NULL);
		else
			SetTimer(TIMERID_CACHE_PREV, 10, NULL);
		m_bCaching = true;
	}

	return true;

}

bool CHView::LoadImageFile()
{
	if(LoadFile(m_pCurDib,*m_curfile)==false)
	{
		GMessageBox(this, "파일 열기 실패..", APP_NAME);
		return false;
	}


	return true;
}

bool CHView::LoadImageFile2()
{
	if(LoadFile(m_pCurDib,*m_curfile)==false)
	{
		GMessageBox(this, "파일 열기 실패..", APP_NAME);
		return false;
	}

	if(m_curfile+1!=m_files.end())
		LoadFile(m_pCurDib2,*(m_curfile+1));
	else
		m_pCurDib2->Clear(CLR_BKG);

	return true;
}

bool CHView::ProcessImage(GDib* pSrc, GDib* pDest, bool bDrawToScreen)
{
	GetWindowRect(&m_rWindow);

	// stretch
	if(g_Option.bStretech)
	{
		CSize	sizeSrc;
		CSize	sizeDst;
		CSize	sizeDstNew;
		double	ratioSrc;						// 가로에 대한 세로의 비율
		double	ratioDst;						// 가로에 대한 세로의 비율
		double	ratio;							// 확대 비율

		if(!(pDest->IsCreated() && pDest->GetSize()==m_rWindow.Size()))
			pDest->CreateNullImage(m_rWindow.Width(), m_rWindow.Height());

		sizeSrc = pSrc->GetSize();
		sizeDst = pDest->GetSize();

		ratioSrc = double(sizeSrc.cy) / sizeSrc.cx;
		ratioDst = double(sizeDst.cy) / sizeDst.cx;

		if(ratioSrc < ratioDst)					// 화면의 세로가 더 크다.. -> 폭을 맞추어야 한다.
			ratio = double(sizeDst.cx) / sizeSrc.cx;
		else
			ratio = double(sizeDst.cy) / sizeSrc.cy;

		sizeDstNew.cx = (int)(sizeSrc.cx * ratio);
		sizeDstNew.cy = (int)(sizeSrc.cy * ratio);

		pDest->Clear(CLR_BKG);


		// 확대 - 인터폴레이션 처리
		if(g_Option.bInterpolation)
		{
			pSrc->SetProgressFunc(ProgressFunc, m_pParent);
			pSrc->DrawStretchInterpolationTo(pDest, 0, 0, sizeDstNew.cx, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);
			pSrc->SetProgressFunc(NULL);
		}
		// 확대 - 그냥 확대
		else
			pSrc->DrawStretchTo(pDest, 0, 0, sizeDstNew.cx, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);

		if(bDrawToScreen) pDest->DrawTo(this);

		pDest->SetProgressFunc(ProgressFunc, m_pParent);
		if(g_Option.bSharpen)
			pDest->Sharpen();
		pDest->SetProgressFunc(NULL);

	}						
	else
	{
		if(!(pDest->IsCreated() && pDest->GetSize()==m_rWindow.Size()))
			pDest->CreateNullImage(m_rWindow.Width(), m_rWindow.Height());

		pDest->Clear(CLR_BKG);
		pSrc->DrawTo(pDest);
	}

	m_pParent->SetProgress(0);

	return true;
}

bool CHView::ProcessImage2(GDib* pSrc, GDib* pSrc2, GDib* pDest,bool bDrawToScreen)
{
	GetWindowRect(&m_rWindow);

	if(g_Option.bAutoDetectWidth)
	{
		if(IsHorizontal(pSrc->GetSize()))						// 폭이 더 길 경우..
		{
			return ProcessImage(pSrc, pDest);
		}
	}
		

	if(g_Option.bFlipPage)	swap(pSrc, pSrc2);

	// stretch
	if(g_Option.bStretech)
	{
		CSize	sizeSrc;
		CSize	sizeDst;
		CSize	sizeDstNew;
		double	ratioSrc;						// 가로에 대한 세로의 비율
		double	ratioDst;						// 가로에 대한 세로의 비율
		double	ratio;							// 확대 비율

		if(!(pDest->IsCreated() && pDest->GetSize()==m_rWindow.Size()))
			pDest->CreateNullImage(m_rWindow.Width(), m_rWindow.Height());

		// 좌측
		sizeSrc = pSrc->GetSize();
		sizeDst = pDest->GetSize();

		ratioSrc = double(sizeSrc.cy) / sizeSrc.cx;
		ratioDst = double(sizeDst.cy) / (sizeDst.cx/2);

		if(ratioSrc < ratioDst)					// 화면의 세로가 더 크다.. -> 폭을 맞추어야 한다.
			ratio = double(sizeDst.cx/2) / sizeSrc.cx;
		else
			ratio = double(sizeDst.cy) / sizeSrc.cy;

		sizeDstNew.cx = (int)(sizeSrc.cx * ratio);
		sizeDstNew.cy = (int)(sizeSrc.cy * ratio);

		pDest->Clear(CLR_BKG);

		if(g_Option.bInterpolation)
		{
			pSrc->SetProgressFunc(ProgressFunc, m_pParent);
			pSrc->DrawStretchInterpolationTo(pDest, 0, 0, sizeDstNew.cx, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);
			pSrc->SetProgressFunc(NULL);
		}
		else
			pSrc->DrawStretchTo(pDest, 0, 0, sizeDstNew.cx, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);

		if(bDrawToScreen) pDest->DrawTo(this);

		// 우측 
		sizeSrc = pSrc2->GetSize();
		sizeDst = pDest->GetSize();

		ratioSrc = double(sizeSrc.cy) / sizeSrc.cx;
		ratioDst = double(sizeDst.cy) / (sizeDst.cx/2);

		if(ratioSrc < ratioDst)					// 화면의 세로가 더 크다.. -> 폭을 맞추어야 한다.
			ratio = double(sizeDst.cx/2) / sizeSrc.cx;
		else
			ratio = double(sizeDst.cy) / sizeSrc.cy;

		sizeDstNew.cx = (int)(sizeSrc.cx * ratio);
		sizeDstNew.cy = (int)(sizeSrc.cy * ratio);

		if(g_Option.bInterpolation)
		{
			pSrc2->SetProgressFunc(ProgressFunc, m_pParent);
			pSrc2->DrawStretchInterpolationTo(pDest, sizeDst.cx/2, 0, sizeDstNew.cx+sizeDst.cx/2, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);
			pSrc2->SetProgressFunc(NULL);
		}
		else
			pSrc2->DrawStretchTo(pDest, sizeDst.cx/2, 0, sizeDstNew.cx+sizeDst.cx/2, sizeDstNew.cy, 0, 0, sizeSrc.cx, sizeSrc.cy);

		if(bDrawToScreen) pDest->DrawTo(this);

		pDest->SetProgressFunc(ProgressFunc, m_pParent);
		if(g_Option.bSharpen)
			pDest->Sharpen();
		pDest->SetProgressFunc(NULL);
	}						
	else
	{
		CSize	sizeSrc;
		CSize	sizeDst;
		CSize	sizeDstNew;
		double	ratioSrc;						// 가로에 대한 세로의 비율
		double	ratioDst;						// 가로에 대한 세로의 비율
		double	ratio;							// 확대 비율

		if(!(pDest->IsCreated() && pDest->GetSize()==m_rWindow.Size()))
			pDest->CreateNullImage(m_rWindow.Width(), m_rWindow.Height());

		// 좌측
		sizeSrc = pSrc->GetSize();
		sizeDst = pDest->GetSize();

		ratioSrc = double(sizeSrc.cy) / sizeSrc.cx;
		ratioDst = double(sizeDst.cy) / (sizeDst.cx/2);

		if(ratioSrc < ratioDst)					// 화면의 세로가 더 크다.. -> 폭을 맞추어야 한다.
			ratio = double(sizeDst.cx/2) / sizeSrc.cx;
		else
			ratio = double(sizeDst.cy) / sizeSrc.cy;

		sizeDstNew.cx = (int)(sizeSrc.cx * ratio);
		sizeDstNew.cy = (int)(sizeSrc.cy * ratio);

		pDest->Clear(CLR_BKG);

		pSrc->DrawTo(pDest, 0, 0, 0, 0, sizeSrc.cx, sizeSrc.cy);

		// 우측 
		sizeSrc = pSrc2->GetSize();
		sizeDst = pDest->GetSize();

		ratioSrc = double(sizeSrc.cy) / sizeSrc.cx;
		ratioDst = double(sizeDst.cy) / (sizeDst.cx/2);

		if(ratioSrc < ratioDst)					// 화면의 세로가 더 크다.. -> 폭을 맞추어야 한다.
			ratio = double(sizeDst.cx/2) / sizeSrc.cx;
		else
			ratio = double(sizeDst.cy) / sizeSrc.cy;

		sizeDstNew.cx = (int)(sizeSrc.cx * ratio);
		sizeDstNew.cy = (int)(sizeSrc.cy * ratio);

		pSrc2->DrawTo(pDest, sizeDst.cx/2, 0, 0, 0, sizeSrc.cx, sizeSrc.cy);
	}

	if(g_Option.bFlipPage)	swap(pSrc, pSrc2);

	m_pParent->SetProgress(0);

	return true;
}


void CHView::OnFileNext() 
{
	ShowNextFile(true);
}

void CHView::OnFilePrev() 
{
	ShowNextFile(false);
}

void CHView::OnKeyDown(UINT nChar, UINT nRepCnt, UINT nFlags) 
{
	gebug("%x", nChar);
	if(nChar==VK_SPACE || nChar==VK_DOWN || nChar==VK_NEXT 
		|| nChar=='.' || nChar==0x2d)
	{
		ShowNextFile(true);
	}
	else if(nChar==VK_RETURN)
	{
		SelectPage();
	}
	else if(nChar==VK_UP || nChar==VK_PRIOR)
	{
		ShowNextFile(false);
	}
	else if(nChar==VK_TAB)
	{
		OnFlippage();
	}
	else if(nChar==0xc0)	// '`' 
	{
		m_pParent->PostMessage(WM_COMMAND, ID_VIEW_TOOLBAR);
		m_pParent->PostMessage(WM_COMMAND, ID_VIEW_STATUS_BAR);
	}
	else if(nChar==0x31)	/// '1'		// 한페이지 보기
	{
		if(g_Option.bView2Page)
		{
			m_sCachedFileName = "";
			g_Option.bView2Page = false;
			Refresh();
		}
	}
	else if(nChar==0x53)	/// 's'		// sharpen 전환
	{
		PostMessage(WM_COMMAND, IDM_SHARPEN);
	}
	else if(nChar==0x49)	/// 'I'		// INTERPOLATION 전환
	{
		PostMessage(WM_COMMAND, IDM_INTERPOLATION);
	}
	else if(nChar==0x32)	// '2'
	{
		OnShow2page();
	}
	else if(nChar==0x5A)	// 'Z'		// 확대
	{
		PostMessage(WM_COMMAND, IDM_STRETCHING);
	}
	else if(nChar==VK_ESCAPE || nChar==0x46)		// 'f'
	{
		m_pParent->PostMessage(WM_COMMAND, IDM_VIEW_FULLSCREEN);
	}
	else if(nChar==VK_F5)
	{
		Refresh();	
	}
	else if(nChar==VK_HOME)														// 맨앞으로 가기
	{
		if(m_files.size() && m_curfile!=m_files.end())
		{
			m_curfile = m_files.begin();
			Refresh();
		}
	}
	else if(nChar==VK_END)														// 맨앞으로 가기
	{
		if(m_files.size() && m_curfile!=m_files.end())
		{
			m_curfile = m_files.end()-1;
			Refresh();
		}
	}
}

void CHView::CacheNextFile(bool bNext)
{
	if(	(m_curfile==m_files.end() ) ||
		(bNext && (m_curfile+1)==m_files.end()) ||
		(bNext==false && m_curfile==m_files.begin()) )
	{
		m_sCachedFileName = "";
		return;
	}

	if(bNext) m_sCachedFileName = *(m_curfile+1);
	else m_sCachedFileName = *(m_curfile-1);


	gebug("cache.");
	if(LoadFile(m_pCachedDib,m_sCachedFileName)==false)
	{
		m_sCachedFileName = "";
		return ;
	}

	ProcessImage(m_pCachedDib, m_pCachedDibStretch);

	gebug("cache........end");
}

// 2개의 파일 캐쉬하기..
void CHView::Cache2NextFile(bool bNext)
{
	if(	(m_curfile==m_files.end() ) ||
		(bNext && (m_curfile+2)>=m_files.end()) ||
		(bNext==false && m_curfile-2<=m_files.begin()) )
	{
		m_sCachedFileName = "";
		return;
	}

	if(bNext) m_sCachedFileName = *(m_curfile+2);
	else m_sCachedFileName = *(m_curfile-2);


	// 1페이지 
	gebug("cache.");
	if(LoadFile(m_pCachedDib,m_sCachedFileName)==false)
	{
		m_sCachedFileName = "";
		return ;
	}

	if(g_Option.bView2Page==false)
	{
		ProcessImage(m_pCachedDib, m_pCachedDibStretch);
		return;
	}


	// 2페이지 캐쉬..
	if(g_Option.bView2Page)
	{
		if(	(m_curfile==m_files.end() ) ||
			(bNext && (m_curfile+3)>=m_files.end()) ||
			(bNext==false && m_curfile-3<=m_files.begin()) )
		{
			m_sCachedFileName = "";
			return;
		}

		CString sFileName;

		if(bNext) sFileName = *(m_curfile+3);
		else sFileName = *(m_curfile-3);

		if(LoadFile(m_pCachedDib2,sFileName)==false)
		{
			m_sCachedFileName = "";
			return ;
		}
		ProcessImage2(m_pCachedDib, m_pCachedDib2, m_pCachedDibStretch);
	}

	gebug("cache........end");
}

void CHView::OnTimer(UINT nIDEvent) 
{
	m_bCaching = false;
	KillTimer(nIDEvent);
	if(nIDEvent==TIMERID_CACHE_NEXT)
	{
		if(g_Option.bView2Page)
			Cache2NextFile(true);
		else
			CacheNextFile(true);
	}
	else if(nIDEvent==TIMERID_CACHE_PREV)
	{
		if(g_Option.bView2Page)
			Cache2NextFile(false);
		else
			CacheNextFile(false);
	}
}

void CHView::OnSize(UINT nType, int cx, int cy) 
{
//	PumpUntilEmpty();
	Invalidate(FALSE);

	CWnd ::OnSize(nType, cx, cy);
	GetWindowRect(&m_rWindow);

	if(m_curfile!=m_files.end())
	{
		if(m_pCurDibStretch->GetSize()==CSize(cx,cy)) return;
		if(g_Option.bView2Page)
			ProcessImage2(m_pCurDib, m_pCurDib2, m_pCurDibStretch);
		else
			ProcessImage(m_pCurDib, m_pCurDibStretch);
	}
}

// 가운데 버튼 처리..
void CHView::OnMButtonDown(UINT nFlags, CPoint point) 
{
	m_pParent->PostMessage(WM_COMMAND, IDM_VIEW_FULLSCREEN);
}

BOOL CHView::OnMouseWheel(UINT nFlags, short zDelta, CPoint pt) 
{
	gebug("%d", zDelta);

	if(zDelta < 0)
		ShowNextFile(true);
	else
		ShowNextFile(false);

	
	return CWnd ::OnMouseWheel(nFlags, zDelta, pt);
}


void CHView::OnPrev10() 
{
	if(m_curfile==m_files.end()) return;
	if(m_curfile==m_files.begin()) 
	{
		GAlaramBox(this, "맨 처음 파일 입니다.", APP_NAME, 1);
		return;
	}

	m_curfile = max((m_curfile-10), m_files.begin());
	Refresh();
}

void CHView::OnPrev() 
{
	if(m_curfile==m_files.end()) return;
	ShowNextFile(false);
}

void CHView::OnNext() 
{
	if(m_curfile==m_files.end()) return;
	ShowNextFile(true);	
}

void CHView::OnNext10() 
{
	if(m_curfile==m_files.end()) return;
	if(m_curfile==m_files.end()-1)
	{
		GAlaramBox(this, "맨 마지막 파일 입니다.", APP_NAME, 1);
		return;
	}

	m_curfile = min((m_curfile+10), m_files.end()-1);
	Refresh();
}

CString CHView::GetFilePosInfo()
{
	if(m_curfile==m_files.end()) return "";

	CString s;
	s.Format("%d/%d", m_curfile - m_files.begin() + 1, m_files.size());

	return s;
}

void CHView::SelectPage()
{
	if(m_files.size()==0)
	{
		GAlaramBox(this, "파일을 먼저 열어야 합니다.", APP_NAME, 1);
		return;
	}


	CSelectPageDlg	dlg;

	dlg.SetFiles(&m_files);
	dlg.SetItemPos(m_curfile - m_files.begin());

	if(dlg.DoModal()!=IDOK) return;

	int nItemPos;
	nItemPos = dlg.m_nItemPos;

	if(m_curfile - m_files.begin() == nItemPos) return;						// 바뀐것이 없다.

	m_curfile = m_files.begin() + nItemPos;									// 이동..
	Refresh();
//	ShowNextFile(true);
}

// 좌우 바꾸기
void CHView::OnFlippage() 
{
	if(g_Option.bView2Page==false) return;
	g_Option.bFlipPage = !g_Option.bFlipPage;
	Refresh();
	m_sCachedFileName = "";					// 캐쉬도 지운다.
}
void CHView::OnUpdateFlippage(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bFlipPage);
	pCmdUI->Enable(g_Option.bView2Page);
}

// 2페이지씩 보기
void CHView::OnShow2page() 
{
	g_Option.bView2Page = !g_Option.bView2Page;
	m_sCachedFileName = "";
	Refresh();
}
void CHView::OnUpdateShow2page(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bView2Page);
}

// 인터폴레이션
void CHView::OnInterpolation() 
{
	g_Option.bInterpolation = !g_Option.bInterpolation;
	m_sCachedFileName = "";							// 캐쉬된거 날린다.
	Refresh();
}

// 샤픈..
void CHView::OnSharpen() 
{
	g_Option.bSharpen = !g_Option.bSharpen;
	m_sCachedFileName = "";							// 캐쉬된거 날린다.
	Refresh();
}

void CHView::OnUpdateInterpolation(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bInterpolation);
	pCmdUI->Enable(g_Option.bStretech);
}
void CHView::OnUpdateSharpen(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bSharpen);
	pCmdUI->Enable(g_Option.bStretech);
}

// 확대
void CHView::OnStretching() 
{
	g_Option.bStretech = !g_Option.bStretech;
	m_sCachedFileName = "";							// 캐쉬된거 날린다.
	Refresh();
	Invalidate(TRUE);
}
void CHView::OnUpdateStretching(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bStretech);
}

// 페이지 고르기
void CHView::OnSelectpage() 
{
	SelectPage();
}


bool CHView::LoadFile(GDib* pDib, CString sFileName)
{
	bool bRet=false;
	if(m_nFileType==FT_NORMAL)
	{
		bRet = pDib->OLELoadFromFile(sFileName);
	}
	else if(m_nFileType==FT_PCO)
	{
		int			nFilePos;
		PCOFileInfo	info;

		nFilePos = atoi(sFileName);
		if(m_pco.GetFileInfo(nFilePos, info)==false) return false;

		BYTE *p;
		FILE* fp;

		fp = fopen(m_sPCOFileName, "rb");
		if(fp==NULL)
		{
			return false;
		}

		p = (BYTE*)malloc(info.size);
		if(p==0)
		{
			fclose(fp);
			return false;
		}

		fseek(fp, info.pos, SEEK_SET);
		fread(p, 1, info.size,fp);

		bRet = pDib->OLELoadFromBuffer((BYTE*)p, info.size);
		free(p);
		fclose(fp);
	}
	else if(m_nFileType==FT_ZIP)
	{

		unzFile			uf;
		int				err	= UNZ_OK;
		unz_file_info	file_info;
		char			szCurFileName[MAX_PATH];
		int				nUncompressedSize;
		BYTE*			p;

		// 파일 열기
		uf = unzOpen(m_sZIPFileName);
		if (uf==NULL) return false;

		// 파일 찾기
		if (unzLocateFile(uf,sFileName,CASESENSITIVITY)!=UNZ_OK)
		{
			unzCloseCurrentFile(uf);
			unzClose(uf);
			return false;
		}

		// 파일 정보 읽기
		err = unzGetCurrentFileInfo(uf,&file_info,szCurFileName,sizeof(szCurFileName),NULL,0,NULL,0);
		if (err!=UNZ_OK) 
		{
			unzCloseCurrentFile(uf);
			unzClose(uf);
			return false;
		}

		// 현재 파일 열기
		err = unzOpenCurrentFile(uf);
		if (err!=UNZ_OK)
		{
			unzCloseCurrentFile(uf);
			unzClose(uf);
			return false;
		}

		// 메모리 잡기
		nUncompressedSize = file_info.uncompressed_size;
		p = (BYTE*)malloc(nUncompressedSize);
		if(p==NULL)
		{
			unzCloseCurrentFile(uf);
			unzClose(uf);
			return false;
		}

		// 압축 풀기
		err = unzReadCurrentFile(uf,p,nUncompressedSize);
		if (err<0)	
		{
			free(p);
			unzCloseCurrentFile(uf);
			unzClose(uf);
			return false;
		}

		// 이미지 읽기
		bRet = pDib->OLELoadFromBuffer((BYTE*)p, nUncompressedSize);
		free(p);
		unzClose(uf);
	}


	if(bRet && g_Option.bRotate)
	{
		GDib	dibtemp;
		int		x,y;
		int		w,h;
		w = pDib->GetSize().cx;
		h = pDib->GetSize().cy;

		dibtemp = *pDib;						// 복사..
		pDib->CreateNullImage(h,w, PF_24BIT);	// 새로운 이미지 생성..

		for(y=0;y<h;y++)
		{
			for(x=0;x<w;x++)
			{
				pDib->SetPixel(y,w-x-1, dibtemp.GetPixel24Fast(x,y));
			}
		}

	}

	return bRet;
}



void CHView::OnLButtonDblClk(UINT nFlags, CPoint point) 
{
	//m_pParent->PostMessage(WM_COMMAND, IDM_VIEW_FULLSCREEN);
	m_pParent->PostMessage(WM_COMMAND, ID_VIEW_TOOLBAR);
}

void CHView::OnRotate() 
{
	g_Option.bRotate = !g_Option.bRotate;
	m_sCachedFileName = "";							// 캐쉬된거 날린다.
	Refresh();
}

void CHView::OnUpdateRotate(CCmdUI* pCmdUI) 
{
	pCmdUI->SetCheck(g_Option.bRotate);
	pCmdUI->Enable(TRUE);
}

////////////////////////////////////////////////////////////////////////////////////////////////////
// 설명  : lbutton down
// 인자  : 
// 날짜  : 2003-05-27 오후 3:51:40
////////////////////////////////////////////////////////////////////////////////////////////////////
void CHView::OnLButtonUp(UINT nFlags, CPoint point) 
{
	if(m_curfile==m_files.end()) return;

	CRect	rClient;
	CRect	rTop;
	GetClientRect(&rClient);

	if(g_Option.bRotate)
		rTop = CRect(0,0, rClient.Width()/2, rClient.Height());
	else
		rTop = CRect(0,0, rClient.Width(), rClient.Height()/2);

	if(rTop.PtInRect(point))
	{
		ShowNextFile(false);
	}
	else
	{
		ShowNextFile(true);		
	}
}
