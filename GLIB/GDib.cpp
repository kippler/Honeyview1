#include "StdAfx.h"
#include "GDib.h"
//#include "Gutil.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

const  int		MAXPALCOLORS =			256;
const  int		MAXPALCOLORS_4BIT =		16;
#define			PALVERSION				0x300						// magic number for LOGPALETTE

//IMPLEMENT_DYNAMIC(GDib, CObject)

BYTE GDib::AlphaTable[256][256];									// 알파 블렌딩 테이블
bool GDib::m_bInitAlphaTable=false;									// 알파 테이블이 초기화 되었나?
HINSTANCE GDib::m_hAppInst = NULL;

GDib::GDib()
{
//	m_hdd = NULL;
	m_pDIBits = NULL;
	m_bOK = false;
	m_pProgressFunc = NULL;
	m_pProgressParam = NULL;
	m_hGdiObj = NULL;
	if(m_bInitAlphaTable==false)
		InitAlphaTable();

	if(GDib::m_hAppInst==NULL)
		GDib::m_hAppInst = AfxGetInstanceHandle();
}

GDib::~GDib()
{
	DeleteObject();
}

/* 오브젝트 지우기 - 새로운 이미지를 생성할 경우 기존의 오브젝트를 지워야 한다 */
BOOL GDib::DeleteObject()
{
	m_dcMem.DeleteDC();

/*
	m_pal.DeleteObject();
	if (m_hdd) {
		DrawDibClose(m_hdd);
		m_hdd = NULL;
	}
*/
	if(m_hGdiObj)
		return ::DeleteObject(m_hGdiObj);

	return FALSE;
}

/* 현재 비트맵의 사이즈 리턴 */
CSize GetBitmapSize(CBitmap* pBitmap)
{
	BITMAP bm;
	return pBitmap->GetBitmap(&bm) ?
		CSize(bm.bmWidth, bm.bmHeight) : CSize(0,0);
}

//////////////////
// You can use this static function to draw ordinary
// CBitmaps as well as GDibs
//
/*
BOOL DrawBitmap(CDC& dc, CBitmap* pBitmap,
	const CRect* rcDst, const CRect* rcSrc)
{
	// Compute rectangles where NULL specified
	CRect rc;
	if (!rcSrc) {
		// if no source rect, use whole bitmap
		rc = CRect(CPoint(0,0), GetBitmapSize(pBitmap));
		rcSrc = &rc;
	}
	if (!rcDst) {
		// if no destination rect, use source
		rcDst=rcSrc;
	}

	// Create memory DC
	CDC memdc;
	memdc.CreateCompatibleDC(&dc);
	CBitmap* pOldBm = memdc.SelectObject(pBitmap);

	// Blast bits from memory DC to target DC.
	// Use StretchBlt if size is different.
	//
	BOOL bRet = FALSE;
	if (rcDst->Size()==rcSrc->Size()) {
		bRet = dc.BitBlt(rcDst->left, rcDst->top, 
			rcDst->Width(), rcDst->Height(),
			&memdc, rcSrc->left, rcSrc->top, SRCCOPY);
	} else {
		dc.SetStretchBltMode(COLORONCOLOR);
		bRet = dc.StretchBlt(rcDst->left, rcDst->top, rcDst->Width(),
			rcDst->Height(), &memdc, rcSrc->left, rcSrc->top, rcSrc->Width(),
			rcSrc->Height(), SRCCOPY);
	}
	memdc.SelectObject(pOldBm);

	return bRet;
}
*/
////////////////////////////////////////////////////////////////
// Draw DIB on caller's DC. Does stretching from source to destination
// rectangles. Generally, you can let the following default to zero/NULL:
//
//		bUseDrawDib = whether to use use DrawDib, default TRUE
//		pPal	      = palette, default=NULL, (use DIB's palette)
//		bForeground = realize in foreground (default FALSE)
//
// If you are handling palette messages, you should use bForeground=FALSE,
// since you will realize the foreground palette in WM_QUERYNEWPALETTE.
//
/*
BOOL GDib::Draw(CDC& dc, const CRect* rcDst, const CRect* rcSrc,
	BOOL bUseDrawDib, CPalette* pPal, BOOL bForeground)
{
	if (!m_hObject)
		return FALSE;

	// Select, realize palette
	if (pPal==NULL)				// no palette specified:
		pPal = GetPalette();		// use default
	CPalette* pOldPal = dc.SelectPalette(pPal, !bForeground);
	dc.RealizePalette();

	BOOL bRet = FALSE;
	if (bUseDrawDib) {
		// Compute rectangles where NULL specified
		//
		CRect rc(0,0,-1,-1);	// default for DrawDibDraw
		if (!rcSrc)
			rcSrc = &rc;
		if (!rcDst)
			rcDst=rcSrc;
		if (!m_hdd)
			VERIFY(m_hdd = DrawDibOpen());

		// Get BITMAPINFOHEADER/color table. I copy into stack object each time.
		// This doesn't seem to slow things down visibly.
		//
		DIBSECTION ds;
		VERIFY(GetObject(sizeof(ds), &ds)==sizeof(ds));
		char buf[sizeof(BITMAPINFOHEADER) + MAXPALCOLORS*sizeof(RGBQUAD)];
		BITMAPINFOHEADER& bmih = *(BITMAPINFOHEADER*)buf;
		RGBQUAD* colors = (RGBQUAD*)(&bmih+1);
		memcpy(&bmih, &ds.dsBmih, sizeof(bmih));
		GetColorTable(colors, MAXPALCOLORS);

		// Let DrawDib do the work!
		bRet = DrawDibDraw(m_hdd, dc,
			rcDst->left, rcDst->top, rcDst->Width(), rcDst->Height(),
			&bmih,			// ptr to BITMAPINFOHEADER + colors
			m_pDIBits,
//			m_bm.bmBits,	// bits in memory
			rcSrc->left, rcSrc->top, rcSrc->Width(), rcSrc->Height(),
			bForeground ? 0 : DDF_BACKGROUNDPAL);

	} else {
		// use normal draw function
		bRet = DrawBitmap(dc, this, rcDst, rcSrc);
	}
	if (pOldPal)
		dc.SelectPalette(pOldPal, TRUE);
	return bRet;
}
*/


//////////////////
// Helper to get color table. Does all the mem DC voodoo.
//
/*
UINT GDib::GetColorTable(RGBQUAD* colorTab, UINT nColors)
{
	CWindowDC dcScreen(NULL);
	CDC memdc;
	memdc.CreateCompatibleDC(&dcScreen);
	CBitmap* pOldBm = memdc.SelectObject(this);
	nColors = GetDIBColorTable(memdc, 0, nColors, colorTab);
	memdc.SelectObject(pOldBm);
	return nColors;
}
*/
//////////////////
// Attach is just like the CGdiObject version,
// except it also creates the palette
//
BOOL GDib::Attach(HGDIOBJ hbm)
{
	m_hGdiObj = hbm;
/*
	BITMAP bm;
	if (CBitmap::Attach(hbm)) {
		if (!GetBitmap(&bm))			// load BITMAP for speed
		{
//			ASSERT(0);
//			return FALSE;
		}
		return TRUE;
	}
	ASSERT(0);
	return FALSE;	
*/
	return TRUE;
}

//////////////////
// Create the palette. Use halftone palette for hi-color bitmaps.
//
/*
BOOL GDib::CreatePalette(CPalette& pal)
{ 
	// should not already have palette
	ASSERT(pal.m_hObject==NULL);

	BOOL bRet = FALSE;
	RGBQUAD* colors = new RGBQUAD[MAXPALCOLORS];
	UINT nColors = GetColorTable(colors, MAXPALCOLORS);
	if (nColors > 0) {
		// Allocate memory for logical palette 
		int len = sizeof(LOGPALETTE) + sizeof(PALETTEENTRY) * nColors;
		LOGPALETTE* pLogPal = (LOGPALETTE*)new char[len];
		if (!pLogPal)
			return NULL;

		// set version and number of palette entries
		pLogPal->palVersion = PALVERSION;
		pLogPal->palNumEntries = nColors;

		// copy color entries 
		for (UINT i = 0; i < nColors; i++) {
			pLogPal->palPalEntry[i].peRed   = colors[i].rgbRed;
			pLogPal->palPalEntry[i].peGreen = colors[i].rgbGreen;
			pLogPal->palPalEntry[i].peBlue  = colors[i].rgbBlue;
			pLogPal->palPalEntry[i].peFlags = 0;
		}

		// create the palette and destroy LOGPAL
		bRet = pal.CreatePalette(pLogPal);
		delete [] (char*)pLogPal;
	} else {
		CWindowDC dcScreen(NULL);
		bRet = pal.CreateHalftonePalette(&dcScreen);
	}
	delete colors;
	return bRet;
}
*/

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// *****************************************   추가된 부분  *****************************************
//
///////////////////////////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 유틸 함수들 
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/* 각종 내부 변수들 초기화  - DIB 생성 직후 호출한다 */
BYTE* GDib::SetDIBits()
{
	BITMAP bm;
	if(::GetObject(m_hGdiObj, sizeof(BITMAP), &bm)==0)
//	if(GetBitmap(&bm)==0) 
	{
		m_pDIBits = NULL;
	}
	else
	{
		m_pDIBits = (BYTE*)bm.bmBits;
		// BITS PER PIXEL 지정하기.. //
		m_nPixelFormat = (bm.bmBitsPixel==4)?PF_4BIT:(bm.bmBitsPixel==8)?PF_8BIT:(bm.bmBitsPixel==24)?PF_24BIT:PF_UNKNOWN;
		if(m_nPixelFormat==PF_UNKNOWN) ASSERT(0);
		m_size.cx = bm.bmWidth;
		m_size.cy = bm.bmHeight;
	}

	return m_pDIBits;
}

/* 같은 비트맵을 생성한다 */
const GDib&	GDib::operator=(GDib& dibSrc) 
{
	if(dibSrc.m_bOK==false) { ASSERT(0); return *this; }
	CreateNullImage(dibSrc.m_size.cx, dibSrc.m_size.cy, dibSrc.m_nPixelFormat);

	if(dibSrc.m_nPixelFormat==PF_8BIT)					// 256 컬러일 경우 - 빨레트를 복사한다.
	{
		RGBQUAD	rgb[MAXPALCOLORS];
		::GetDIBColorTable(dibSrc.m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);
		::SetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);
	}
	dibSrc.DrawTo(this, 0,0);	

	return *this;
}

/* N 번째 라인의 첫번째 pixel 의 메모리 위치를 얻어 온다 */
BYTE* GDib::GetScanLine(int nLine)
{
	if(m_bOK==false || nLine<0 || m_size.cy<=nLine)
	{
		ASSERT(0);
		return NULL;
	}
	BYTE *pRet=NULL;
	if(m_nPixelFormat==PF_24BIT)
	{
		pRet = m_pDIBits +
			(WIDTHBYTES((DWORD)m_size.cx * nBitCount[m_nPixelFormat])*(m_size.cy-(nLine+1)));
	}
	else if(m_nPixelFormat==PF_8BIT)
	{
		pRet = m_pDIBits +
			(WIDTHBYTES((DWORD)m_size.cx * nBitCount[m_nPixelFormat])*(m_size.cy-(nLine+1)));
	}
	else if(m_nPixelFormat==PF_4BIT)
	{
		pRet = m_pDIBits +
			(WIDTHBYTES((DWORD)m_size.cx * nBitCount[m_nPixelFormat])*(m_size.cy-(nLine+1)));
	}
	else
	{
		ASSERT(0);								// 미구현..
	}
	return pRet;
}

/* X, Y 좌표의 어드레스 구해오기 */
BYTE* GDib::GetAddrOfPos(int nX, int nY)
{
	if(m_bOK==false || nX<0 || nY< 0 || m_size.cy<=nY || m_size.cx<=nX ||
		(m_nPixelFormat!=PF_24BIT && m_nPixelFormat!=PF_8BIT && m_nPixelFormat!=PF_4BIT) )
	{
		ASSERT(0);
		return NULL;
	}
	BYTE *pRet=GetScanLine(nY);
	if(pRet==NULL) 
	{
		ASSERT(0);
		return NULL;
	}
	pRet += nByteCount[m_nPixelFormat]*nX;

	return pRet;
}

/* 8비트 일때 좌표의 값 읽기 */
int GDib::Get8BitPixel(int nX, int nY)
{
#ifdef _DEBUG
	if(m_bOK==false || nX<0 || nY< 0 || m_size.cy<=nY || m_size.cx<=nX 
		|| m_nPixelFormat!=PF_8BIT) { ASSERT(0); return 0;}
#endif
	return *(GetScanLine(nY)+nX);
}
/* 8비트 일때 좌표에 값 세팅하기 */
bool GDib::Set8BitPixel(int nX, int nY, int value)
{
#ifdef _DEBUG
	if(m_bOK==false || nX<0 || nY< 0 || m_size.cy<=nY || m_size.cx<=nX 
		|| m_nPixelFormat!=PF_8BIT) { ASSERT(0); return false;}
#endif
	*(GetScanLine(nY)+nX) = value;
	return true;
}


/* 빨레트에서 가장 가까운 컬러의 위치를 리턴한다  - 256 컬러일때만 유효 */
bool GDib::FindNearestPalColor(COLORREF cl, int& nRet)
{
	if(m_bOK==false || m_nPixelFormat!=PF_8BIT) { ASSERT(0); return false; };

	int nNearestPalPos=0;
	int nMinDistance=256*256*3;
	int nDistance;
	int r,g,b;
	RGBQUAD	rgb[MAXPALCOLORS];

	r = GetRValue(cl);
	g = GetGValue(cl);
	b = GetBValue(cl);
	::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

	for(int i=0;i<MAXPALCOLORS;i++)				
	{
		nDistance = (b- rgb[i].rgbBlue)*(b- rgb[i].rgbBlue) +
			(g- rgb[i].rgbGreen)*(g- rgb[i].rgbGreen) +
			(r- rgb[i].rgbRed)*(r- rgb[i].rgbRed);

		if(nDistance<nMinDistance)
		{
			nNearestPalPos = i;
			nMinDistance = nDistance;
		}
	}

	nRet = nNearestPalPos;
	return true;
}

/* 동일한 컬러를 가지는 빨레트의 컬러 위치를 가져온다(256컬러에서) */
bool GDib::FindExactPalColor(COLORREF cl, int& nRet)
{
	if(m_bOK==false || m_nPixelFormat!=PF_8BIT) {ASSERT(0);return false;}

	int r,g,b;
	RGBQUAD	rgb[MAXPALCOLORS];
	r = GetRValue(cl);
	g = GetGValue(cl);
	b = GetBValue(cl);

	::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);
	for(int i=0;i<MAXPALCOLORS;i++)				
	{
		if(r==rgb[i].rgbRed&& g==rgb[i].rgbGreen&&b==rgb[i].rgbBlue)
		{
			nRet = i;
			return true;
		}
	}
	return false;
}

/* DIB 에서 DIB 로 그릴때(Trans&Alpha 등 비트 조작함수들) 파라메터 체크하고 정확한 영역 계산하기 */
BOOL GDib::CalcAndCheckParamWhenDrawDibToDib(GDib *pDib, int &dx, int &dy, int &sx, int &sy, int &sw, int &sh)
{
	if(pDib->m_bOK==false||this->m_bOK==false) { ASSERT(0); return FALSE;}
	if(sw<=0||sh<=0) return FALSE;

	sx = sx - min(0,dx);					// 음수 클리핑 ( dx 나 dy 가 0 보다 작을 경우 )
	sy = sy - min(0,dy);
	sw = sw - min(0,dx);
	sh = sh - min(0,dy);
	dx = max(0,dx);
	dy = max(0,dy);

	dx = dx - min(0,sx);					// 음수 클리핑 ( sx 나 sy 가 0 보다 작을 경우 )
	dy = dy - min(0,sy);
	sw = sw - min(0,sx);
	sh = sh - min(0,sy);
	sx = max(0,sx);
	sy = max(0,sy);

	sw = min(sw, this->m_size.cx - sx);		// 넘어가는 값 클리핑( sw, sh 가 src 나 dest 의 범위를 벗어날 경우 )
	sh = min(sh, this->m_size.cy - sy);
	sw = min(sw, pDib->m_size.cx - dx);
	sh = min(sh, pDib->m_size.cy - dy);

	return TRUE;
}

/* 알파 블렌딩 테이블 초기화 하기 */
void GDib::InitAlphaTable()
{
	int i,j;
	for(i=0;i<256;i++)
	{
		for(j=0;j<256;j++)
		{
			if(i==0||j==0)
				AlphaTable[i][j]=0;
			else	
				AlphaTable[i][j]=i*j/255;
		}	
	}
	m_bInitAlphaTable = true;
}

/* x,y 좌표의 컬러 색 가져 오기.. - RGB 가 아니다. BGR 이다. */
UINT GDib::GetPixel(int x, int y)
{
	BYTE *p = GetAddrOfPos(x, y);
	if(p==NULL) return RGB(0,0,0);

	if(m_nPixelFormat==PF_24BIT)
	{
		return      *((COLORREF*)p) & 0x00ffffff;
	}
	else
	{
		ASSERT(0);					// 미구현..
	}
	return RGB(0,0,0);
}

UINT GDib::GetPixel24Fast(int x, int y)
{
	BYTE* p = m_pDIBits+ (WIDTHBYTES((DWORD)m_size.cx * BIT_PER_PIXEL_24BIT)*(m_size.cy-(y+1)))+BYTE_PER_PIXEL_24BIT*x;
	return *((COLORREF*)p) & 0x00ffffff;
}

/* x,y 좌표의 컬러 색 가져 오기.. */
COLORREF GDib::GetRGBPixel(int x, int y)
{
	BYTE *p = GetAddrOfPos(x, y);
	if(p==NULL) return RGB(0,0,0);

	if(m_nPixelFormat==PF_24BIT)
	{
		return RGB(*(p+2), *(p+1), *(p+0));
	}
	else
	{
		ASSERT(0);					// 미구현..
	}
	return RGB(0,0,0);
}

/* X, Y 좌표에 점을 찍는다 - 24BIT 만, SAFE 하지 않다 */
void GDib::SetPixel(int x, int y, UINT clr)
{
#ifdef _DEBUG
	if(m_nPixelFormat!=PF_24BIT) {ASSERT(0); return; }
#endif
	BYTE *pRet=GetScanLine(y)+nByteCount[m_nPixelFormat]*x;
	*((COLORREF*)pRet) = (*((COLORREF*)pRet) & 0xff000000) | clr;
}

/* X, Y 좌표에 점을 찍는다 - 24BIT 만, SAFE 하지 않다 */
void GDib::SetRGBPixel(int x, int y, COLORREF clr)
{
#ifdef _DEBUG
	if(m_nPixelFormat!=PF_24BIT) {ASSERT(0); return; }
#endif
	BYTE *pRet=GetScanLine(y)+nByteCount[m_nPixelFormat]*x;
	clr = RGB(GetBValue(clr), GetGValue(clr), GetRValue(clr));
	*((COLORREF*)pRet) = (*((COLORREF*)pRet) & 0xff000000) | clr;
}

void GDib::Clear(COLORREF clr)
{
	ASSERT(m_bOK);
	m_dcMem.FillSolidRect(0, 0, m_size.cx, m_size.cy, clr);
}
///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DIB 생성 함수들 
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/* 파일에서 읽어 들이기 */
BOOL GDib::LoadFromFile(const char* szFileName)
{
	DeleteObject();	
	BOOL bRet;
	bRet = Attach(::LoadImage(NULL, szFileName, IMAGE_BITMAP, 0, 0,
		LR_LOADFROMFILE | LR_CREATEDIBSECTION | LR_DEFAULTSIZE));
	if(bRet==FALSE) return FALSE;

	m_dcMem.CreateCompatibleDC(NULL);
	m_dcMem.SelectObject(CBitmap::FromHandle((HBITMAP)m_hGdiObj));

	SetDIBits();
	m_bOK = true;
	return TRUE;
}

/* 리소스에서 읽어 들이기 */
BOOL GDib::LoadFromResource(const char* szResourceName)
{
	DeleteObject();	
	BOOL bRet;
	//HINSTANCE hInst = AfxGetInstanceHandle();
	bRet = Attach(::LoadImage(m_hAppInst, szResourceName, IMAGE_BITMAP, 0, 0,
		LR_CREATEDIBSECTION | LR_DEFAULTSIZE));
	if(bRet==FALSE) 
	{
//AfxMessageBox("attach 실패");
		return FALSE;
	}

	m_dcMem.CreateCompatibleDC(NULL);
	m_dcMem.SelectObject(CBitmap::FromHandle((HBITMAP)m_hGdiObj));

	SetDIBits();
	m_bOK = true;
	return TRUE;
}


/* 널 이미지 만들기 */		//[수정요] BBPP 에 대한 부분(빨레트등) 필요 //
BOOL GDib::CreateNullImage(int w, int h, PIXEL_FORMAT pf, COLORREF clrBkgnd)
{
	DeleteObject();	

	HBITMAP		hBitmap;
	BOOL bRet;
	BITMAPINFO	bmi;
/*
	memset(&bmi,0,sizeof(bmi));
	bmi.bmiHeader.biSize=sizeof(bmi);
	bmi.bmiHeader.biWidth=w;
	bmi.bmiHeader.biHeight=h;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=nBitCount[pf];
	bmi.bmiHeader.biCompression=BI_RGB;
	bmi.bmiHeader.biSizeImage=0;//WIDTHBYTES((DWORD)w * nBitCount[pf]) * h;
	bmi.bmiHeader.biXPelsPerMeter=0xece;
	bmi.bmiHeader.biYPelsPerMeter=0xec4;
	bmi.bmiHeader.biClrUsed=0;
	bmi.bmiHeader.biClrImportant=0;

	hBitmap = CreateDIBSection(pDC->GetSafeHdc(), &bmi,DIB_RGB_COLORS,(void **) &m_pDIBits, NULL, 0);
*/
	bmi.bmiHeader.biSize=0x28;
	bmi.bmiHeader.biWidth=w;
	bmi.bmiHeader.biHeight=h;
	bmi.bmiHeader.biPlanes=1;
	bmi.bmiHeader.biBitCount=nBitCount[pf];
	bmi.bmiHeader.biCompression=0;
	bmi.bmiHeader.biSizeImage=WIDTHBYTES((DWORD)w * nBitCount[pf]) * h;
	bmi.bmiHeader.biXPelsPerMeter=0xece;
	bmi.bmiHeader.biYPelsPerMeter=0xec4;
	bmi.bmiHeader.biClrUsed=0;
	bmi.bmiHeader.biClrImportant=0;

	CDC		dc;
	dc.CreateCompatibleDC(NULL);

	hBitmap = CreateDIBSection(dc.GetSafeHdc(), &bmi,DIB_RGB_COLORS,(void **) &m_pDIBits, NULL, 0);

	bRet = Attach(hBitmap);
	if(bRet==FALSE) return FALSE;

	m_dcMem.CreateCompatibleDC(NULL);
	m_dcMem.SelectObject(CBitmap::FromHandle((HBITMAP)m_hGdiObj));
	m_dcMem.FillSolidRect(0,0,w,h,clrBkgnd);								// 배경색으로 지우기.

//	SetDIBits();			// ※ 이상하게 Nt나 2000 에서는 CreateDibSection() 후 GetBitmap() 을 호출하면 실패한다, 그래서 이렇게 직접 넣어 준다.
	m_nPixelFormat = pf;
	m_size.cx = w;
	m_size.cy = h;


	m_bOK = true;

	return TRUE;
}


///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DIB 그리기 함수들
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/* 윈도우에 그리기 */
BOOL GDib::DrawTo(CWnd *pWnd, int dx, int dy)
{
	CDC *pDC;
	BOOL bRet;
	pDC = pWnd->GetDC();
	bRet = DrawTo(pDC, dx, dy);
	pWnd->ReleaseDC(pDC);
	return bRet;
}
/* 윈도우에 그리기 */
BOOL GDib::DrawTo(CWnd *pWnd, int dx, int dy, int sx, int sy, int sw, int sh)
{
	CDC *pDC;
	BOOL bRet;
	pDC = pWnd->GetDC();
	bRet = DrawTo(pDC, dx, dy, sx, sy, sw, sh);
	pWnd->ReleaseDC(pDC);
	return bRet;
}

/* 디씨에 뿌리기 */
BOOL GDib::DrawTo(CDC *pDC, int dx, int dy)
{
	return DrawTo(pDC, dx, dy, 0, 0, m_size.cx, m_size.cy);
}

/* sx, sy - this 의 시작 위치. */
/* dx,dy,dw,dy - pDC 의 대상 위치 */
BOOL GDib::DrawTo(CDC *pDC, int dx, int dy, int sx, int sy, int sw, int sh)
{
	ASSERT(m_bOK);
	if(pDC->m_hDC==NULL) return FALSE;
	BOOL bRet;
	bRet = pDC->BitBlt(dx, dy, sw, sh, &m_dcMem, sx, sy, SRCCOPY);
	return bRet;
}

/* 가로로 쪼개서 그리기 함수 */
BOOL GDib::DrawSplitTo(CWnd *pWnd, int dx, int dy, int nPos, int nSplit, bool bHsplit)
{
	CDC *pDC;
	BOOL bRet;
	pDC = pWnd->GetDC();
	bRet = DrawSplitTo(pDC,dx,dy,nPos,nSplit,bHsplit);
	pWnd->ReleaseDC(pDC);
	return bRet;
}

/* 가로로 쪼개서 그리기 - nPos 는 zero based.. */
BOOL GDib::DrawSplitTo(CDC  *pDC,  int dx, int dy, int nPos, int nSplit, bool bHsplit)
{
	ASSERT(!(nPos<0||nPos>=nSplit));

	int sw, sh, sx, sy;
	if(bHsplit)
	{
		sw = m_size.cx / nSplit;
		sh = m_size.cy;
		sx = sw * nPos;
		sy = 0;
	}
	else
	{
		sw = m_size.cx ;
		sh = m_size.cy / nSplit;
		sx = 0;
		sy = sh * nPos;
	}
	BOOL bRet;
	bRet = DrawTo(pDC, dx, dy, sx, sy, sw, sh);
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// DIB 끼리 그리는 함수들
//
///////////////////////////////////////////////////////////////////////////////////////////////////

BOOL GDib::DrawTo(GDib *pDib, int dx, int dy)
{
	return DrawTo(pDib, dx, dy, 0,0, m_size.cx, m_size.cy);
}


BOOL GDib::DrawTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh)
{
	if(pDib->m_bOK==false||this->m_bOK==false) return FALSE;
	return DrawTo(pDib->GetMemDC(), dx, dy, sx, sy, sw, sh);
}

BOOL GDib::DrawTransTo(GDib *pDib, int dx, int dy, COLORREF clrTrans)
{
	int sx, sy, sw, sh;
	sx = 0;
	sy = 0;
	sw = m_size.cx;
	sh = m_size.cy;
	return DrawTransTo(pDib, dx, dy, sx, sy, sw, sh, clrTrans);
}

BOOL GDib::DrawTransTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, COLORREF clrTrans)
{
	if(CalcAndCheckParamWhenDrawDibToDib(pDib, dx, dy, sx, sy, sw, sh)==FALSE) return FALSE;
	if( !(	(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_24BIT) ||
			(pDib->m_nPixelFormat==PF_8BIT  &&this->m_nPixelFormat==PF_8BIT ) ||
			(pDib->m_nPixelFormat==PF_24BIT  &&this->m_nPixelFormat==PF_8BIT )  ))
		{ ASSERT(0); return FALSE;}

	int x,y;				
	BYTE *pSrc, *pDest;

	if(pDib->m_nPixelFormat==PF_24BIT && this->m_nPixelFormat==PF_24BIT)	// 24 bit & 24bit 컬러시
	{
		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				if( ((*((UINT*)pSrc))&0x00ffffff) != clrTrans) 
				{
					*pDest = *pSrc;											//[수정요] 4BYTE (INT) 연산으로 속도를 높일 수 있다.
					*(pDest+1) = *(pSrc+1);
					*(pDest+2) = *(pSrc+2);
				}
				pSrc+= 3;
				pDest+= 3;
			}
		}
	}
	else if(pDib->m_nPixelFormat==PF_8BIT &&this->m_nPixelFormat==PF_8BIT)	// 8비트&8비트 일경우
	{
		int nClrPos;
		bool bRet;
		bRet = this->FindNearestPalColor(clrTrans,nClrPos);					// RGB 컬러에서 빨레트를 통하여 8BIT 컬러의 위치를 뽑아낸다.
		if(bRet==false) return FALSE;

		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				if( *pSrc!=nClrPos)
				{
					*pDest = *pSrc;
				}
				pSrc++;
				pDest++;
			}
		}
	}
	else if(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_8BIT)	// 24BIT 에 8비트
	{

		RGBQUAD	rgb[MAXPALCOLORS];
		COLORREF		clr;

		// 빨레트를 구한다.
		::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				clr = *((COLORREF*)(&rgb[*pSrc]));									// 8비트 컬러는 빨레트의 위치 이다.
				if(clr != clrTrans) 
				{
					*pDest		= GetRValue(clr);									//[수정요] 4BYTE (INT) 연산으로 속도를 높일 수 있다.
					pDest++;
					*pDest		= GetGValue(clr);
					pDest++;
					*pDest		= GetBValue(clr);
					pDest++;
				}
				else
					pDest+=3;
				pSrc++;
			}
		}
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;	
}

/* 알파 블렌딩 그리기 - nAlpha 는 0~255 값을 가진다.. */
BOOL GDib::DrawAlphaTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, int nAlpha)
{
	if(CalcAndCheckParamWhenDrawDibToDib(pDib, dx, dy, sx, sy, sw, sh)==FALSE) return FALSE;
	if( !(	(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_24BIT) ||
			(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_8BIT ) ) )
		{ ASSERT(0); return FALSE;}
	if(nAlpha<0||nAlpha>255) { ASSERT(0); return FALSE;}

	int x,y;				
	BYTE *pSrc, *pDest;

	if(pDib->m_nPixelFormat==PF_24BIT&&this->m_nPixelFormat==PF_24BIT)			// 24 bit 컬러시
	{
		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
				*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
				*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
			}
		}
	}
	else if(pDib->m_nPixelFormat==PF_24BIT&&this->m_nPixelFormat==PF_8BIT)		// 8비트 컬러 처리
	{
		RGBQUAD		rgb[MAXPALCOLORS];
		RGBQUAD*	pRGB;
		::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				pRGB = &(rgb[*pSrc]);
				*pDest = AlphaTable[pRGB->rgbBlue  ][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
				*pDest = AlphaTable[pRGB->rgbGreen][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
				*pDest = AlphaTable[pRGB->rgbRed ][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
				pSrc++;
			}
		}
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;	
}

/* 알파와 투명처리를 동시헤 수행한다 */
BOOL GDib::DrawAlphaTransTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, int nAlpha, COLORREF clrTrans)
{
	if(CalcAndCheckParamWhenDrawDibToDib(pDib, dx, dy, sx, sy, sw, sh)==FALSE) return FALSE;
	if( !(	(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_24BIT) ||
			(pDib->m_nPixelFormat==PF_24BIT &&this->m_nPixelFormat==PF_8BIT ) ) )
		{ ASSERT(0); return FALSE;}
	if(nAlpha<0||nAlpha>255) { ASSERT(0); return FALSE;}

	int x,y;				
	BYTE *pSrc, *pDest;

	if(pDib->m_nPixelFormat==PF_24BIT&&this->m_nPixelFormat==PF_24BIT)			// 24 bit 컬러시
	{
		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;
			for(x=0;x<sw;x++)
			{
				if((((*((UINT*)pSrc))&0x00ffffff) != clrTrans) )
				{
					*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
					*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
					*pDest = AlphaTable[*pSrc][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;pSrc++;
				}
				else
				{
					pDest+=3;
					pSrc+=3;
				}

			}
		}
	}
	else if(pDib->m_nPixelFormat==PF_24BIT&&this->m_nPixelFormat==PF_8BIT)		// 8비트 컬러 처리
	{
		RGBQUAD	rgb[MAXPALCOLORS];
		RGBQUAD*	pRGB;
		int nCount;
		nCount = ::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);		// 빨레트 가져 오기.
		if(nCount!=MAXPALCOLORS) {ASSERT(0);return FALSE;}

		for(y=0;y<sh;y++)
		{
			pSrc  = this->GetScanLine(y+sy) + nByteCount[this->m_nPixelFormat]*sx;
			pDest = pDib->GetScanLine(y+dy) + nByteCount[pDib->m_nPixelFormat]*dx;

			for(x=0;x<sw;x++)
			{
				pRGB = &(rgb[*pSrc]);
				if((((*((UINT*)pRGB))&0x00ffffff) != clrTrans) )
				{
					*pDest = AlphaTable[pRGB->rgbBlue ][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
					*pDest = AlphaTable[pRGB->rgbGreen][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
					*pDest = AlphaTable[pRGB->rgbRed  ][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
					pSrc++;
				}
				else
				{
					pDest+=3;
					pSrc++;
				}

			}
		}
	}
	else
	{
		ASSERT(0);
		return FALSE;
	}

	return TRUE;	
}

/* 가로로 쪼개서 그리기 + 알파 처리 */
BOOL GDib::DrawSplitAlphaTo(GDib *pDib, int dx, int dy, int nPos, int nSplit, int nAlpha, bool bHsplit)
{
	ASSERT(!(nPos<0||nPos>=nSplit));

	int sw, sh, sx, sy;
	if(bHsplit)
	{
		sw = m_size.cx / nSplit;
		sh = m_size.cy;
		sx = sw * nPos;
		sy = 0;
	}
	else
	{
		sw = m_size.cx ;
		sh = m_size.cy / nSplit;
		sx = 0;
		sy = sh * nPos;
	}
	BOOL bRet;
	bRet = DrawAlphaTo(pDib, dx, dy, sx, sy, sw, sh,nAlpha);
	return bRet;
}

/* 가로로 쪼개서 그리기 + 투명 처리 */
BOOL GDib::DrawSplitTransTo(GDib *pDib, int dx, int dy, int nPos, int nSplit, COLORREF clrTrans, bool bHsplit)
{
	ASSERT(!(nPos<0||nPos>=nSplit));

	int sw, sh, sx, sy;
	if(bHsplit)
	{
		sw = m_size.cx / nSplit;
		sh = m_size.cy;
		sx = sw * nPos;
		sy = 0;
	}
	else
	{
		sw = m_size.cx ;
		sh = m_size.cy / nSplit;
		sx = 0;
		sy = sh * nPos;
	}
	BOOL bRet;
	bRet = DrawTransTo(pDib, dx, dy, sx, sy, sw, sh,clrTrans);
	return bRet;
}

/* 가로로 쪼개서 그리기 + 알파 + 투명 처리 */
BOOL GDib::DrawSplitAlphaTransTo(GDib *pDib , int dx, int dy, int nPos, int nSplit, int nAlpha,  COLORREF clrTrans, bool bHsplit)
{
	ASSERT(!(nPos<0||nPos>=nSplit));

	int sw, sh, sx, sy;
	if(bHsplit)
	{
		sw = m_size.cx / nSplit;
		sh = m_size.cy;
		sx = sw * nPos;
		sy = 0;
	}
	else
	{
		sw = m_size.cx ;
		sh = m_size.cy / nSplit;
		sx = 0;
		sy = sh * nPos;
	}
	BOOL bRet;
	bRet = DrawAlphaTransTo(pDib, dx, dy, sx, sy, sw, sh, nAlpha, clrTrans);
	return bRet;
}

///////////////////////////////////////////////////////////////////////////////////////////////////
//
// 이미지 처리 함수들
//
///////////////////////////////////////////////////////////////////////////////////////////////////

/* 흑백 이미지 만들기 */
bool GDib::MakeGray()
{
	if(m_bOK==false) {	ASSERT(0);	return false; }

	int x,y;
	if(m_nPixelFormat==PF_24BIT)									// 24bit 처리
	{
		BYTE*	p;
		int		nAverage;
		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);
			for(x=0;x<m_size.cx;x++)
			{
				nAverage = (*p+*(p+1)+*(p+2))/3;					// 평균
				*(p+0) = nAverage;							
				*(p+1) = nAverage;
				*(p+2) = nAverage;
				p+= nByteCount[m_nPixelFormat];						// 3byte 씩 증가.
			}
		}
		return true;
	}
	else if(m_nPixelFormat==PF_8BIT)								// 256 컬러 처리..
	{
		RGBQUAD	rgb[MAXPALCOLORS];
		int nAverage;

		::GetDIBColorTable(m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

		for(int i=0;i<MAXPALCOLORS;i++)								// 빨레트를 평균낸다.
		{
			nAverage = (rgb[i].rgbBlue+rgb[i].rgbGreen+rgb[i].rgbRed)/3;
			rgb[i].rgbBlue  = nAverage;
			rgb[i].rgbGreen = nAverage;
			rgb[i].rgbRed   = nAverage;
		}
		::SetDIBColorTable(m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

		return true;
	}

	return false;													// 미구현..
}

/* 하프톤 이미지 - 구멍 뚫린 이미지 만들기 */
bool GDib::MakeHalftone(COLORREF cl)
{
	int x,y;
	BYTE*	p;
	if(m_nPixelFormat==PF_8BIT)										// 256 컬러처리
	{
		int nPos;
		if(FindNearestPalColor(cl,nPos)==false) return false;
		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);
			p += ( (y%2) ? nByteCount[m_nPixelFormat] : 0);

			for(x=0;x<m_size.cx;x+=2)
			{
				*p = nPos;
				p+= (nByteCount[m_nPixelFormat]*2);
			}
		}
	}
	else if(m_nPixelFormat==PF_24BIT)								// 24비트 컬러 처리
	{
		BYTE r,g,b;
		r = GetRValue(cl);								//[개선요]
		g = GetGValue(cl);
		b = GetBValue(cl);
		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);
			p += ( (y%2) ? nByteCount[m_nPixelFormat] : 0);

			for(x=0;x<m_size.cx;x+=2)
			{
				*(p+0) = b;
				*(p+1) = g;
				*(p+2) = r;
				p+= (nByteCount[m_nPixelFormat]*2);
			}
		}
	}
	else
	{
		ASSERT(0);
		return false;
	}
	return true;
}

/* 배경색 바꾸기-- (0,0) 에 있는 컬러를 다른 색으로 바꾼다 */
bool GDib::ChangeBkColor(COLORREF cl)
{
	if(m_nPixelFormat ==PF_24BIT)				// 테스트 못해봄..
	{
		BYTE		r,g,b; 
		COLORREF	clOld;
		int			x,y;
		BYTE		*p =  GetAddrOfPos(0,0);

		if(p==NULL) return false;
		r = GetRValue(cl);
		g = GetGValue(cl);
		b = GetBValue(cl);
		clOld =  (COLORREF)((*(UINT*)p)&0x00ffffff);

		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);

			for(x=0;x<m_size.cx;x++)
			{
				if( ((*((UINT*)p))&0x00ffffff) == clOld)
				{
					*(p+0) = b;					//[수정요] int 연산을 하면 빨라질찌도.
					*(p+1) = g;
					*(p+2) = r;
				}
				p+= nByteCount[m_nPixelFormat];
			}
		}
		return true;
	}
	else if(m_nPixelFormat ==PF_8BIT||m_nPixelFormat ==PF_4BIT)		// 8비트 일경우 - 팔레트 자체를 바꾸어 버린다.
	{
		RGBQUAD		rgb[MAXPALCOLORS];
		BYTE*		p;
		int			nPalPos;

		p = GetAddrOfPos(0,0);				// 빨레트의 위치를 찾는다.

		if(m_nPixelFormat ==PF_8BIT) nPalPos = *p;
		else if(m_nPixelFormat ==PF_4BIT) nPalPos = (*p)&0xf;
		else { ASSERT(0); return false;}

		::GetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);
		rgb[nPalPos].rgbRed   = GetRValue(cl);		
		rgb[nPalPos].rgbGreen = GetGValue(cl);
		rgb[nPalPos].rgbBlue  = GetBValue(cl);

		//*((COLORREF*)(&rgb[nPalPos])) = cl;
		::SetDIBColorTable(this->m_dcMem.GetSafeHdc(), 0,MAXPALCOLORS,rgb);

		return true;
	}
	ASSERT(0);								// 미처리..
	return false;
}

// 밝게(혹은 어둡게) 만든다.
bool GDib::MakeBright(int nDegree)
{
	int x,y;
	if(m_nPixelFormat==PF_24BIT)									// 24bit 처리
	{
		BYTE*	p;
		int		r,g,b;
		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);
			for(x=0;x<m_size.cx;x++)
			{
				b = *(p+0);
				g = *(p+1);
				r = *(p+2);
				b = max(0,min(255,(b+nDegree)));
				g = max(0,min(255,(g+nDegree)));
				r = max(0,min(255,(r+nDegree)));
				*(p+0) = b;							
				*(p+1) = g;
				*(p+2) = r;
				p+= nByteCount[m_nPixelFormat];						// 3byte 씩 증가.
			}
		}
		return true;
	}
	else
	{
		ASSERT(0);
		return false;
	}
}

// 모노 이미지로 만든다 //
bool GDib::MakeMono(BYTE nDegree, BYTE nResult)
{
	int x,y;
	if(m_nPixelFormat==PF_24BIT)									// 24bit 처리
	{
		BYTE*	p;
		int		r,g,b;
		int		avr;
		for(y=0;y<m_size.cy;y++)
		{
			p = GetScanLine(y);
			for(x=0;x<m_size.cx;x++)
			{
				b = *(p+0);
				g = *(p+1);
				r = *(p+2);
				avr = (r+g+b)/3;									// 평균
				if(avr>nDegree)										// over -> white
				{
					*(p+0) = 255;							
					*(p+1) = 255;
					*(p+2) = 255;
				}
				else
				{
					*(p+0) = nResult;							
					*(p+1) = nResult;
					*(p+2) = nResult;
				}
				p+= nByteCount[m_nPixelFormat];						// 3byte 씩 증가.
			}
		}
		return true;
	}
	else
	{
		ASSERT(0);
		return false;
	}
}



// 그라데이션을 친다.
bool GDib::Gradation(COLORREF clrFrom, COLORREF clrTo, bool bHorizontal)
{
	return Gradation(clrFrom, clrTo, bHorizontal, CRect(CPoint(0,0),m_size));
}
bool GDib::Gradation(COLORREF clrFrom, COLORREF clrTo, bool bHorizontal, CRect rect)
{
	if(m_bOK==false) {	ASSERT(0);	return false; }

	int w = rect.Width();
	int h = rect.Height();
	int x,y;
	int r, g, b, rs, gs, bs, rd, gd, bd;

	rs = GetRValue(clrFrom);gs = GetGValue(clrFrom);bs = GetBValue(clrFrom);
	rd = GetRValue(clrTo);	gd = GetGValue(clrTo);	bd = GetBValue(clrTo);

	if(bHorizontal)
	{
		for(x=rect.left; x<rect.right; x++)
		{
			r = rs - (rs-rd)*(x-rect.left)/w;
			g = gs - (gs-gd)*(x-rect.left)/w;
			b = bs - (bs-bd)*(x-rect.left)/w;
			m_dcMem.FillSolidRect(x,rect.top, 1, h, RGB(r,g,b));
		}
	}
	else
	{
		for(y=rect.top; y<rect.bottom; y++)
		{
			r = rs - (rs-rd)*(y-rect.top)/h;
			g = gs - (gs-gd)*(y-rect.top)/h;
			b = bs - (bs-bd)*(y-rect.top)/h;
			m_dcMem.FillSolidRect(rect.left,y, w, 1, RGB(r,g,b));
		}
	}
	return true;
}
bool GDib::Gradation3(COLORREF clrFrom, COLORREF clrOver, COLORREF clrTo, bool bHorizontal, CRect rect)
{
	if(m_bOK==false) {	ASSERT(0);	return false; }

	int w = rect.Width();
	int h = rect.Height();
	int x,y;
	int r, g, b, rs, gs, bs, rd, gd, bd, rm, gm, bm;

	rs = GetRValue(clrFrom);gs = GetGValue(clrFrom);bs = GetBValue(clrFrom);
	rd = GetRValue(clrTo);	gd = GetGValue(clrTo);	bd = GetBValue(clrTo);
	rm = GetRValue(clrOver);gm = GetGValue(clrOver);bm = GetBValue(clrOver);

	if(bHorizontal)
	{
		w = w/2;
		if(w==0) return false;
		for(x=rect.left; x<rect.right-w; x++)
		{
			r = rs - (rs-rm)*(x-rect.left)/w;
			g = gs - (gs-gm)*(x-rect.left)/w;
			b = bs - (bs-bm)*(x-rect.left)/w;
			m_dcMem.FillSolidRect(x,rect.top, 1, h, RGB(r,g,b));
		}
		for(x=rect.left+w; x<rect.right; x++)
		{
			r = rm - (rs-rd)*(x-(rect.left+w))/w;
			g = gm - (gs-gd)*(x-(rect.left+w))/w;
			b = bm - (bs-bd)*(x-(rect.left+w))/w;
			m_dcMem.FillSolidRect(x,rect.top, 1, h, RGB(r,g,b));
		}
	}
	else
	{
		h = h/2;
		if(h==0) return false;
		for(y=rect.top; y<rect.bottom-h; y++)
		{
			r = rs - (rs-rm)*(y-rect.top)/h;
			g = gs - (gs-gm)*(y-rect.top)/h;
			b = bs - (bs-bm)*(y-rect.top)/h;
			m_dcMem.FillSolidRect(rect.left,y, w, 1, RGB(r,g,b));
		}
		for(y=rect.top+h; y<rect.bottom; y++)
		{
			r = rm - (rm-rd)*(y-(rect.top+h))/h;
			g = gm - (gm-gd)*(y-(rect.top+h))/h;
			b = bm - (bm-bd)*(y-(rect.top+h))/h;
			m_dcMem.FillSolidRect(rect.left,y, w, 1, RGB(r,g,b));
		}
	}
	return true;
}
