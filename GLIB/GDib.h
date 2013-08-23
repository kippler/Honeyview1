///////////////////////////////////////////////////////////////////////////////////////////////////////
//
// GDib 클래스
//	- 비트맵 처리에 범용적으로 쓰이기 위해 만들어진 클래스
//	- MSJ 1997/07(??) 호 소스에 기반을 둠.
//	- 최초 작성일 : 1999/12/29 BY HONG
//	- 수정 노트 
//		. 1999/12/29 ; 파라메터 약간 수정
//		. 2000/01/03 : GetScanLine, MakeGray, MakeHalftone, FindNearestPalColor, FindExactPalColor 추가.
//		. 2000/01/07 : DrawTransTo 추가, DrawTo 파라메터 순서 바꿈..
//		. 2000/01/10 : GetAddrOfPos(), ChangeBkColor() 추가.
//		. 2000/01/14 : 이미지 Create 시 CDC 파라메터 전부 없앰. - 필요 없어서.. 나중에 다시 필요할찌도...
//		. 2000/01/23 : DrawAlphaTo(), DrawAlphaTransTo() 추가 
//		. 2000/01/26 : GetAddrOfPos()의 버그 수정
//		. 2000/03/18 : DrawHSplitAlphaTransTo() 추가.
//		. 2000/06/16 : DrawStretchTo(), DrawStretchToInterpolation() 완성
//		. 2000/06/26 : GDibExt.cpp 분리, DrawWarpingTo() 추가.
//
//	- 추가 해야할 사항들
//		. 256 컬러 비트맵에서 팔레트 처리 
//		. CreateNullImage 에서 256 컬러로 생성할 경우 팔레트 처리 부분 필요
//		. 투명/축소/확대/알파 그리기
//		. 기타..
//		. 
//
//
///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifndef _DIB_H_
#define _DIB_H_

enum			PIXEL_FORMAT { PF_UNKNOWN, PF_4BIT, PF_8BIT, PF_24BIT };
const	int		nBitCount[4]  = { 0, 4, 8, 24 };					// 픽셀당 비트수
const	int		nByteCount[4] = { 0, 0, 1, 3  };					// 픽셀당 바이트수

#define			GDIB_TRANS_COLOR	RGB(255,0,255)					// 투명 컬러(분홍색)
#define			BYTE_PER_PIXEL_24BIT	3
#define			BIT_PER_PIXEL_24BIT		24

// CALL BACK FUNCTIONS
typedef void GDibProgressFunc(int nCur, int nRange, void* param);


/////////////////////////////////////////////////////////////////////////////////////////////////////////
// PRIVATE 
/////////////////////////////////////////////////////////////////////////////////////////////////////////
#define			WIDTHBYTES(bits) ((((bits) + 31) / 32) * 4)			// 한 라인이 차지하는 바이트 수..(4바이트 정렬)

class GDib  {

protected:
/*
	DECLARE_DYNAMIC(GDib)
	CPalette			m_pal;										// palette
	HDRAWDIB			m_hdd;										// for DrawDib
*/
	CDC					m_dcMem;
	HGDIOBJ				m_hGdiObj;

	BYTE*				m_pDIBits;									// 픽셀에 직접 접근하기 위한 포인터 
	bool				m_bOK;										// 이미지가 생성 되었나 여부
	PIXEL_FORMAT		m_nPixelFormat;								// 현재 이미지의 pixel format;
	CSize				m_size;										// 이미지의 크기..
	static BYTE			AlphaTable[256][256];						// 알파 블렌딩 테이블
	static bool			m_bInitAlphaTable;							// 알파 테이블이 초기화 되었나?

public:					/* 생성 & 소멸자 */
	GDib();
	~GDib();

public :				/* 오리지날 함수들 */
	
	BOOL				Draw(CDC& dc, const CRect* rcDst=NULL, const CRect* rcSrc=NULL,		
							BOOL bUseDrawDib=TRUE, CPalette* pPal=NULL, BOOL bForeground=FALSE);	// 원래 있던 DRAW 펑션.. 이놈은 256 컬러에서도 팔레트 처리를 제대로 한다 //
	//CPalette*			GetPalette()  { return &m_pal; }

private :				/* 내부 함수들 */
	BOOL				Attach(HGDIOBJ hbm);
	BOOL				CreatePalette(CPalette& pal);
	UINT				GetColorTable(RGBQUAD* colorTab, UINT nColors);
	BOOL				DeleteObject();

public :				/* 유틸 함수들 */
	CSize				GetSize() { return m_size; }
	CDC*				GetMemDC() { return &m_dcMem; }
	BYTE*				SetDIBits();
	BYTE*				GetDIBits() { return m_pDIBits;}
	const GDib&			operator=(GDib& dibSrc) ;
	operator			CDC*(){return GetMemDC();};
	BYTE*				GetScanLine(int nLine);
	BYTE*				GetAddrOfPos(int nX, int nY);
	int					Get8BitPixel(int nX, int nY);
	bool				Set8BitPixel(int nX, int nY, int value);
	bool				FindNearestPalColor(COLORREF cl, int& nRet);
	bool				FindExactPalColor(COLORREF cl, int& nRet);
	BOOL				CalcAndCheckParamWhenDrawDibToDib(GDib *pDib, int &dx, int &dy, int &sx, int &sy, int &sw, int &sh);
	void				InitAlphaTable();
	PIXEL_FORMAT		GetPixelFormat() { return m_nPixelFormat; }
	UINT				GetPixel(int x, int y);
	UINT				GetPixel24Fast(int x, int y);
	COLORREF			GetRGBPixel(int x, int y);
	void				SetPixel(int x, int y, UINT clr);
	void				SetRGBPixel(int x, int y, COLORREF clr);
	void				Clear(COLORREF clr);

	bool				IsCreated() { return m_bOK; };

public :				/* 인스턴스 관련 */
	static HINSTANCE	m_hAppInst;									// APP INSTANCE;
	static void			SetInstance(HINSTANCE hInst) { m_hAppInst = hInst; };

public :				/* DIB 생성 함수들 */
	BOOL				LoadFromFile(const char* szFileName);
	BOOL				LoadFromResource(const char* szResourceName);
	BOOL				LoadFromResource(UINT uID) { return LoadFromResource(MAKEINTRESOURCE(uID)); }
	BOOL				CreateNullImage(int w, int h, PIXEL_FORMAT pf=PF_24BIT, COLORREF clrBkgnd=RGB(255,255,255));

public :				/* DIB 조작 함수들 */
	bool				MakeGray();
	bool				MakeHalftone(COLORREF cl=RGB(0,0,0));
	bool				ChangeBkColor(COLORREF cl);
	bool				MakeBright(int nDegree);
	bool				MakeMono(BYTE nDegree=127, BYTE nResult=0);
	bool				Gradation(COLORREF clrFrom, COLORREF clrTo, bool bHorizontal);
	bool				Gradation(COLORREF clrFrom, COLORREF clrTo, bool bHorizontal, CRect r);
	bool				Gradation3(COLORREF clrFrom, COLORREF clrOver, COLORREF clrTo, bool bHorizontal, CRect r);


public :				/* 화면에 그리는 함수들 */
	BOOL				DrawTo(CDC *pDC, int dx=0, int dy=0);
	BOOL				DrawTo(CDC *pDC, int dx, int dy, int sx, int sy, int sw, int sh);
	BOOL				DrawTo(CWnd *pWnd, int dx=0, int dy=0);
	BOOL				DrawTo(CWnd *pWnd, int dx, int dy, int sx, int sy, int sw, int sh);
	BOOL				DrawSplitTo(CWnd *pWnd, int dx, int dy, int nPos, int nSplit, bool bHsplit=true);
	BOOL				DrawSplitTo(CDC  *pDC,  int dx, int dy, int nPos, int nSplit, bool bHsplit=true);

public :				/* DIB 끼리 그리는 함수들 */
	BOOL				DrawTo(GDib *pDib, int dx=0, int dy=0);
	BOOL				DrawTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh);
	BOOL				DrawTransTo(GDib *pDib, int dx=0, int dy=0, COLORREF clrTrans=GDIB_TRANS_COLOR);
	BOOL				DrawTransTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, COLORREF clrTrans=GDIB_TRANS_COLOR);
	BOOL				DrawAlphaTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, int nAlpha);
	BOOL				DrawAlphaTransTo(GDib *pDib, int dx, int dy, int sx, int sy, int sw, int sh, int nAlpha, COLORREF clrTrans=GDIB_TRANS_COLOR);
	BOOL				DrawSplitAlphaTo(GDib *pDib, int dx, int dy, int nPos, int nSplit, int nAlpha, bool bHsplit=true);
	BOOL				DrawSplitTransTo(GDib *pDib, int dx, int dy, int nPos, int nSplit, COLORREF clrTrans=GDIB_TRANS_COLOR, bool bHsplit=true);
	BOOL				DrawSplitAlphaTransTo(GDib *pDib, int dx, int dy, int nPos, int nSplit, int nAlpha,  COLORREF clrTrans=GDIB_TRANS_COLOR, bool bHsplit=true);

	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
	// GDibExt
	//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
public :				/* 확대 그리기 */
	BOOL				DrawStretchTo(GDib* pDib, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh);
	BOOL				DrawStretchAlphaTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2, int nAlpha);
	BOOL				DrawStretchInterpolationTo(GDib* pDib, int dx, int dy, int dw, int dh, int sx, int sy, int sw, int sh);
	BOOL				DrawStretchInterpolationAlphaTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2, int nAlpha);

public :				/* 와핑 */
	BOOL				DrawWarpingTo(GDib* pDib,  int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int nAlpha=-1);
	BOOL				DrawWarpingRectTo(GDib* pDib,  int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int dx4, int dy4, int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int sx4, int sy4, int nAlpha=-1);

public :				/* OLE PICTURE 사용 */
	bool				OLELoadFromFile(CString sFileName);
	bool				OLELoadFromBuffer(BYTE* pBuff, int nSize);
	bool				OLELoadFromResource(CString sResourceType, CString sResource);
private :
	bool				OLEGetResource(LPSTR lpName, LPSTR lpType, void* pResource, int& nBufSize);


public :				/* PNG 지원 */
	bool				PNGLoadFromFile(CString sFileName);
private :
	void				PNGCopyRow(BYTE* pSrc, BYTE* pDest, int nWidth, int nBitperPixel);

public :				/* DIB 조작 함수들 */
	bool				Sharpen();

public :				/* CALL BACK 지원 */
	void				SetProgressFunc(GDibProgressFunc* pFunc, void* param=NULL);
private :
	GDibProgressFunc*	m_pProgressFunc;
	void*				m_pProgressParam;


};

#endif