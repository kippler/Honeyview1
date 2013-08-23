
#include "StdAfx.h"
#include "GDib.h"
#include "Gutil.h"


typedef int FDOUBLE;
#define	FD2INT(x) ((x)>>16)
#define INT2FD(x) ((x)<<16)
#define	FD2INT2(x) ((x)>>8)
#define INT2FD2(x) ((x)<<8)
void	SWAP(int& a, int& b){int temp;temp = a;a = b;b = temp;}

void GDib::SetProgressFunc(GDibProgressFunc* pFunc, void* param)
{
	m_pProgressFunc = pFunc;
	m_pProgressParam = param;
}

/* DIB 에서 DIB 로 확대 그리기 - INTERPOLATION 안함 */
BOOL GDib::DrawStretchTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2)
{
	int		dx1, dy1, dx2, dy2, dw, dh;
	int		sx1, sy1, sx2, sy2, sw, sh;
	CSize	sizeSrc, sizeDest;
	double	rx, ry;

	sizeSrc = this->GetSize();
	sizeDest = pDib->GetSize();

	rx = ((double)_sx2-_sx1+1)/(_dx2-_dx1+1);							// 배율..
	ry = ((double)_sy2-_sy1+1)/(_dy2-_dy1+1);

	// 클리핑 
	dx1 = min(max(0, _dx1), sizeDest.cx-1);							// destination
	dy1 = min(max(0, _dy1), sizeDest.cy-1);
	dx2 = min(max(0, _dx2), sizeDest.cx-1);
	dy2 = min(max(0, _dy2), sizeDest.cy-1);

	if((dx1>=sizeDest.cx-1 && dx2>=sizeDest.cx-1) ||
	   (dy1>=sizeDest.cy-1 && dy2>=sizeDest.cy-1) ||
	   (dx1<=0 && dx2<=0) ||
	   (dy1<=0 && dy2<=0)) return FALSE;		  // 화면을 벗어남..

	sx1 = min(max(0, _sx1), sizeSrc.cx-1);							// source
	sy1 = min(max(0, _sy1), sizeSrc.cy-1);
	sx2 = min(max(0, _sx2), sizeSrc.cx-1);
	sy2 = min(max(0, _sy2), sizeSrc.cy-1);

	sx1 = sx1 + (int)((dx1-_dx1)*rx);
	sy1 = sy1 + (int)((dy1-_dy1)*ry);
	sx2 = sx2 + (int)((dx2-_dx2)*rx);
	sy2 = sy2 + (int)((dy2-_dy2)*ry);

	dw = dx2-dx1+1;
	dh = dy2-dy1+1;
	sw = sx2-sx1+1;
	sh = sy2-sy1+1;

	int			x,y;
	FDOUBLE 	d_sx, d_sy;				// FDOUBLE 형 소스 x,y
	FDOUBLE		d_stepx, d_stepy;		// step;
	int			i_sx, i_sy;
	int			i_prev_sx;
	COLORREF	clrS;
	BYTE*		pDest;
	BYTE*		pSrc;
	const int	nByteCount = 3;

	d_sx = INT2FD(sx1);
	d_sy = INT2FD(sy1);
	d_stepx = (FDOUBLE)(INT2FD(1)*((double)sw/dw));
	d_stepy = (FDOUBLE)(INT2FD(1)*((double)sh/dh));

	for(y=dy1;y<=dy2;y++)
	for(y=dy1;y<dy2;y++)
	{
		pDest = pDib->GetScanLine(y)+nByteCount*dx1;
		pSrc  = this->GetScanLine(FD2INT(d_sy));
		d_sx = INT2FD(sx1);
		i_prev_sx = 0;
//		for(x=dx1;x<=dx2;x++)	// 		<<- 정상 코드
		for(x=dx1;x<dx2;x++)
		{
			i_sx = FD2INT(d_sx);
			pSrc = pSrc+nByteCount*(i_sx-i_prev_sx);
			i_prev_sx = i_sx;
			i_sy = FD2INT(d_sy);
			clrS = *((COLORREF*)(pSrc));
			*((COLORREF*)pDest) = clrS;
			d_sx += d_stepx;
			pDest += nByteCount;
		}
		d_sy+=d_stepy;
	}

	return TRUE;
}

/* DIB 에서 DIB 로 확대 그리기 - INTERPOLATION 안함 + 알파 */
BOOL GDib::DrawStretchAlphaTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2, int nAlpha)
{
	int		dx1, dy1, dx2, dy2, dw, dh,sx1, sy1, sx2, sy2, sw, sh;
	CSize	sizeSrc, sizeDest;
	double	rx, ry;
	sizeSrc = this->GetSize();sizeDest = pDib->GetSize();
	rx = ((double)_sx2-_sx1+1)/(_dx2-_dx1+1);ry = ((double)_sy2-_sy1+1)/(_dy2-_dy1+1);
	dx1 = min(max(0, _dx1), sizeDest.cx-1);dy1 = min(max(0, _dy1), sizeDest.cy-1);
	dx2 = min(max(0, _dx2), sizeDest.cx-1);dy2 = min(max(0, _dy2), sizeDest.cy-1);
	if((dx1>=sizeDest.cx-1 && dx2>=sizeDest.cx-1) ||
	   (dy1>=sizeDest.cy-1 && dy2>=sizeDest.cy-1) ||
	   (dx1<=0 && dx2<=0) ||
	   (dy1<=0 && dy2<=0)) return FALSE;		  // 화면을 벗어남..
	sx1 = min(max(0, _sx1), sizeSrc.cx-1);sy1 = min(max(0, _sy1), sizeSrc.cy-1);
	sx2 = min(max(0, _sx2), sizeSrc.cx-1);sy2 = min(max(0, _sy2), sizeSrc.cy-1);
	sx1 = sx1 + (int)((dx1-_dx1)*rx);sy1 = sy1 + (int)((dy1-_dy1)*ry);
	sx2 = sx2 + (int)((dx2-_dx2)*rx);sy2 = sy2 + (int)((dy2-_dy2)*ry);
	dw = dx2-dx1+1;dh = dy2-dy1+1;sw = sx2-sx1+1;sh = sy2-sy1+1;
	int			x,y;
	FDOUBLE 	d_sx, d_sy;				// FDOUBLE 형 소스 x,y
	FDOUBLE		d_stepx, d_stepy;		// step;
	int			i_sx, i_sy;
	int			i_prev_sx;
	COLORREF	clrS;
	BYTE*		pDest;
	BYTE*		pSrc;
	const int	nByteCount = 3;
	d_sx = INT2FD(sx1);d_sy = INT2FD(sy1);
	d_stepx = (FDOUBLE)(INT2FD(1)*((double)sw/dw));d_stepy = (FDOUBLE)(INT2FD(1)*((double)sh/dh));
	for(y=dy1;y<=dy2;y++)
	{
		pDest = pDib->GetScanLine(y)+nByteCount*dx1;pSrc  = this->GetScanLine(FD2INT(d_sy));
		d_sx = INT2FD(sx1);
		i_prev_sx = 0;
		for(x=dx1;x<=dx2;x++)
		{
			i_sx = FD2INT(d_sx);
			pSrc = pSrc+nByteCount*(i_sx-i_prev_sx);
			i_prev_sx = i_sx;
			i_sy = FD2INT(d_sy);
			clrS = *((COLORREF*)(pSrc));
			// ★DrawStretchTo() 랑 다른 부분..
			//*((COLORREF*)pDest) = clrS;
			//pDest += nByteCount;
			*pDest = AlphaTable[GetRValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;		
			*pDest = AlphaTable[GetGValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
			*pDest = AlphaTable[GetBValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
			d_sx += d_stepx;
		}
		d_sy+=d_stepy;
	}
	return TRUE;
}

// 두점사이의 distance - sqrt 안씀. FDOUBLE 형에 대해서 유효.
#define   DIST(x1, y1, x2, y2) ( (((x1>x2)?(x1-x2):(x2-x1)) + ((y1>y2)?(y1-y2):(y2-y1))) /2)

// 늘려서 그리기 - 인터폴레이션..
BOOL GDib::DrawStretchInterpolationTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2)
{
	int		dx1, dy1, dx2, dy2, dw, dh;
	int		sx1, sy1, sx2, sy2, sw, sh;
	CSize	sizeSrc, sizeDest;
	double	rx, ry;

	sizeSrc = this->GetSize();
	sizeDest = pDib->GetSize();

	rx = ((double)_sx2-_sx1+1)/(_dx2-_dx1+1);						// 배율..
	ry = ((double)_sy2-_sy1+1)/(_dy2-_dy1+1);

	// 클리핑 
	dx1 = min(max(0, _dx1), sizeDest.cx-1);							// destination
	dy1 = min(max(0, _dy1), sizeDest.cy-1);
	dx2 = min(max(0, _dx2), sizeDest.cx-1);
	dy2 = min(max(0, _dy2), sizeDest.cy-1);

	if((dx1>=sizeDest.cx-1 && dx2>=sizeDest.cx-1) ||
	   (dy1>=sizeDest.cy-1 && dy2>=sizeDest.cy-1) ||
	   (dx1<=0 && dx2<=0) ||
	   (dy1<=0 && dy2<=0)) return FALSE;							// 화면을 벗어남..

	sx1 = min(max(0, _sx1), sizeSrc.cx-1);							// source
	sy1 = min(max(0, _sy1), sizeSrc.cy-1);
	sx2 = min(max(0, _sx2), sizeSrc.cx-2);							// INTERPOLATION 이므로 2 를 빼야 한다.
	sy2 = min(max(0, _sy2), sizeSrc.cy-2);

	sx1 = sx1 + (int)((dx1-_dx1)*rx);
	sy1 = sy1 + (int)((dy1-_dy1)*ry);
	sx2 = sx2 + (int)((dx2-_dx2)*rx);
	sy2 = sy2 + (int)((dy2-_dy2)*ry);

	dw = dx2-dx1+1;
	dh = dy2-dy1+1;
	sw = sx2-sx1+1;
	sh = sy2-sy1+1;

	int			x,y;
	FDOUBLE	 	d_sx, d_sy;				// FDOUBLE 형 소스 x,y
	FDOUBLE		d_stepx, d_stepy;		// step;
	int			i_sx;
	int			i_prev_sx;
	COLORREF	clrS1,clrS2,clrS3,clrS4, clrS;
	BYTE*		pDest;
	BYTE*		pSrc;
	BYTE*		pSrcNext;
	const int	nByteCount = 3;
	const int	nWidthBytes = WIDTHBYTES((DWORD)this->m_size.cx * nBitCount[m_nPixelFormat]);
	FDOUBLE		d1, d2, d3, d4, dsum;
/*
	(SX,SY) (SX+1,SY)                          
	+-----+-----+                              
	+  c1 +  c2 +                              
	+     +     +                              
	+-----+-----+          <=========   DX,DY  
	+  c3 +  c4 +                              
	+     +     +                              
	+-----+-----+                              
	(SX,SY+1) (SX+1,SY+1)                      
*/
	d_sx = INT2FD(sx1);
	d_sy = INT2FD(sy1);
	d_stepx = (FDOUBLE)(INT2FD(1)*((double)sw/dw));
	d_stepy = (FDOUBLE)(INT2FD(1)*((double)sh/dh));

	for(y=dy1;y<=dy2;y++)
	{
		if(m_pProgressFunc)
			(*m_pProgressFunc)(y-dy1, dy2-dy1, m_pProgressParam);

		pDest    = pDib->GetScanLine(y)+nByteCount*dx1;
		pSrc     = this->GetScanLine(FD2INT(d_sy));
		pSrcNext = this->GetScanLine(FD2INT(d_sy)+1);													// 인터폴레이션을 위한 y+1 라인.
		d_sx = INT2FD(sx1);
		i_prev_sx = 0;
		for(x=dx1;x<dx2;x++)		
//		for(x=dx1;x<=dx2;x++)	//	<<- 정상 코드..
		{
			i_sx		= FD2INT(d_sx);
			pSrc		= pSrc+nByteCount*(i_sx-i_prev_sx);
			pSrcNext	= pSrcNext+nByteCount*(i_sx-i_prev_sx);
			i_prev_sx	= i_sx;

			clrS1	= *((COLORREF*)(pSrc));																// 평균을 내기 위한 4점의 색.
			clrS2	= *((COLORREF*)(pSrc+nByteCount));
			clrS3	= *((COLORREF*)(pSrcNext));
			clrS4	= *((COLORREF*)(pSrcNext+nByteCount));

			d1 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx             )&0xffff0000)), (FDOUBLE)(((d_sy             )&0xffff0000)));	// 각 4점과의 거리
			d2 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx + 0x00010000)&0xffff0000)), (FDOUBLE)(((d_sy             )&0xffff0000)));
			d3 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx             )&0xffff0000)), (FDOUBLE)(((d_sy + 0x00010000)&0xffff0000)));
			d4 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx + 0x00010000)&0xffff0000)), (FDOUBLE)(((d_sy + 0x00010000)&0xffff0000)));

			if(d1==0) clrS = clrS1;
			else if(d2==0) clrS = clrS2;
			else if(d3==0) clrS = clrS3;
			else if(d4==0) clrS = clrS4;
			else
			{
				d1 = 0x1000000/d1;																		// 거리에 반비례하는 가중치
				d2 = 0x1000000/d2;
				d3 = 0x1000000/d3;
				d4 = 0x1000000/d4;
				dsum = d1 + d2 + d3 + d4;
				clrS = RGB( 
					int((GetRValue(clrS1)*d1 + GetRValue(clrS2)*d2 + GetRValue(clrS3)*d3 + GetRValue(clrS4)*d4)/(dsum)),
					int((GetGValue(clrS1)*d1 + GetGValue(clrS2)*d2 + GetGValue(clrS3)*d3 + GetGValue(clrS4)*d4)/(dsum)),
					int((GetBValue(clrS1)*d1 + GetBValue(clrS2)*d2 + GetBValue(clrS3)*d3 + GetBValue(clrS4)*d4)/(dsum))
					);
			}
//			*((COLORREF*)pDest) = clrS;			// 빠른코드 - but, 해상도가 8 의 배수일때 마지막 바이트에서 쫑난다.
//			pDest += nByteCount;
			*pDest = GetRValue(clrS);
			pDest++;
			*pDest = GetGValue(clrS);
			pDest++;
			*pDest = GetBValue(clrS);
			pDest++;
			d_sx += d_stepx;
		}
		d_sy+=d_stepy;
	}
	return TRUE;
}

// 늘려서 그리기 - 인터폴레이션.. + 알파. DrawStretchToInterpolation() 와 99% 동일하다.
BOOL GDib::DrawStretchInterpolationAlphaTo(GDib* pDib, int _dx1, int _dy1, int _dx2, int _dy2, int _sx1, int _sy1, int _sx2, int _sy2, int nAlpha)
{
	int		dx1, dy1, dx2, dy2, dw, dh;	int		sx1, sy1, sx2, sy2, sw, sh;	CSize	sizeSrc, sizeDest;	double	rx, ry;
	sizeSrc = this->GetSize();	sizeDest = pDib->GetSize();
	rx = ((double)_sx2-_sx1+1)/(_dx2-_dx1+1);ry = ((double)_sy2-_sy1+1)/(_dy2-_dy1+1);
	dx1 = min(max(0, _dx1), sizeDest.cx-1);dy1 = min(max(0, _dy1), sizeDest.cy-1);
	dx2 = min(max(0, _dx2), sizeDest.cx-1);dy2 = min(max(0, _dy2), sizeDest.cy-1);
	if((dx1>=sizeDest.cx-1 && dx2>=sizeDest.cx-1) ||
	   (dy1>=sizeDest.cy-1 && dy2>=sizeDest.cy-1) ||
	   (dx1<=0 && dx2<=0) ||
	   (dy1<=0 && dy2<=0)) return FALSE;							// 화면을 벗어남..
	sx1 = min(max(0, _sx1), sizeSrc.cx-1);sy1 = min(max(0, _sy1), sizeSrc.cy-1);
	sx2 = min(max(0, _sx2), sizeSrc.cx-2);sy2 = min(max(0, _sy2), sizeSrc.cy-2);
	sx1 = sx1 + (int)((dx1-_dx1)*rx);sy1 = sy1 + (int)((dy1-_dy1)*ry);
	sx2 = sx2 + (int)((dx2-_dx2)*rx);sy2 = sy2 + (int)((dy2-_dy2)*ry);
	dw = dx2-dx1+1;dh = dy2-dy1+1;sw = sx2-sx1+1;sh = sy2-sy1+1;
	int			x,y,i_sx,i_prev_sx;
	FDOUBLE	 	d_sx, d_sy,d_stepx, d_stepy,d1, d2, d3, d4, dsum;
	COLORREF	clrS1,clrS2,clrS3,clrS4,clrS;
	BYTE*		pDest,*pSrc,*pSrcNext;
	const int	nByteCount = 3;
	const int	nWidthBytes = WIDTHBYTES((DWORD)this->m_size.cx * nBitCount[m_nPixelFormat]);
	d_sx = INT2FD(sx1);d_sy = INT2FD(sy1);
	d_stepx = (FDOUBLE)(INT2FD(1)*((double)sw/dw));d_stepy = (FDOUBLE)(INT2FD(1)*((double)sh/dh));
	for(y=dy1;y<=dy2;y++)
	{
		pDest    = pDib->GetScanLine(y)+nByteCount*dx1;
		pSrc     = this->GetScanLine(FD2INT(d_sy));
		pSrcNext = this->GetScanLine(FD2INT(d_sy)+1);		
		d_sx = INT2FD(sx1);
		i_prev_sx = 0;
		for(x=dx1;x<=dx2;x++)
		{
			i_sx		= FD2INT(d_sx);
			pSrc		= pSrc+nByteCount*(i_sx-i_prev_sx);
			pSrcNext	= pSrcNext+nByteCount*(i_sx-i_prev_sx);
			i_prev_sx	= i_sx;
			clrS1	= *((COLORREF*)(pSrc));					
			clrS2	= *((COLORREF*)(pSrc+nByteCount));
			clrS3	= *((COLORREF*)(pSrcNext));
			clrS4	= *((COLORREF*)(pSrcNext+nByteCount));
			d1 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx             )&0xffff0000)), (FDOUBLE)(((d_sy             )&0xffff0000)));	// 각 4점과의 거리
			d2 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx + 0x00010000)&0xffff0000)), (FDOUBLE)(((d_sy             )&0xffff0000)));
			d3 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx             )&0xffff0000)), (FDOUBLE)(((d_sy + 0x00010000)&0xffff0000)));
			d4 = DIST(d_sx, d_sy , (FDOUBLE)(((d_sx + 0x00010000)&0xffff0000)), (FDOUBLE)(((d_sy + 0x00010000)&0xffff0000)));
			if(d1==0) clrS = clrS1;
			else if(d2==0) clrS = clrS2;
			else if(d3==0) clrS = clrS3;
			else if(d4==0) clrS = clrS4;
			else
			{
				d1 = 0x1000000/d1;d2 = 0x1000000/d2;
				d3 = 0x1000000/d3;d4 = 0x1000000/d4;
				dsum = d1 + d2 + d3 + d4;
				clrS = RGB( int((GetRValue(clrS1)*d1 + GetRValue(clrS2)*d2 + GetRValue(clrS3)*d3 + GetRValue(clrS4)*d4)/(dsum)),
							int((GetGValue(clrS1)*d1 + GetGValue(clrS2)*d2 + GetGValue(clrS3)*d3 + GetGValue(clrS4)*d4)/(dsum)),
							int((GetBValue(clrS1)*d1 + GetBValue(clrS2)*d2 + GetBValue(clrS3)*d3 + GetBValue(clrS4)*d4)/(dsum)));
			}
			// ★DrawStretchInterpolationTo() 랑 다른 부분..
			*pDest = AlphaTable[GetRValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;		
			*pDest = AlphaTable[GetGValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
			*pDest = AlphaTable[GetBValue(clrS)][nAlpha] + AlphaTable[*pDest][255-nAlpha];pDest++;
			d_sx += d_stepx;
		}
		d_sy+=d_stepy;
	}
	return TRUE;
}


/* 와핑 하기 */
BOOL GDib::DrawWarpingTo(GDib* pDib,  int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int nAlpha)
{
//	if(	sx1<0 || sx2<0 || sx3<0 || sx1>=m_size.cx|| sx2>=m_size.cx|| sx3>=m_size.cx ||
//		sy1<0 || sy2<0 || sy3<0 || sy1>=m_size.cy|| sy2>=m_size.cy|| sy3>=m_size.cy ||
	if(	sx1<0 || sx2<0 || sx3<0 || sx1>m_size.cx|| sx2>m_size.cx|| sx3>m_size.cx ||
		sy1<0 || sy2<0 || sy3<0 || sy1>m_size.cy|| sy2>m_size.cy|| sy3>m_size.cy ||
		pDib->m_nPixelFormat!=PF_24BIT || this->m_nPixelFormat !=PF_24BIT ) {ASSERT(0); return FALSE; }

	double		a,b;
	int			*line123, *line13;						// 테이블..
	int			x, y;
	int			sy, ey;
	int			sx, ex;
	int			ox, oy;
	COLORREF	clr;

	if(dy1>dy2) { SWAP(dy1, dy2); SWAP(dx1, dx2); SWAP(sy1, sy2); SWAP(sx1, sx2); };		// 점 버블 소팅
	if(dy1>dy3) { SWAP(dy1, dy3); SWAP(dx1, dx3); SWAP(sy1, sy3); SWAP(sx1, sx3); }; 
	if(dy2>dy3) { SWAP(dy2, dy3); SWAP(dx2, dx3); SWAP(sy2, sy3); SWAP(sx2, sx3); }; 

	sy = max(0, min(dy1, min(dy2,dy3)));													// 최상단 & 최 하단
	ey = min(pDib->m_size.cy-1, max(dy1, max(dy2,dy3)));

	line123 = (int*)malloc(pDib->m_size.cy*sizeof(int)+12);									// 이미지의 높이만큼 배열을 잡는다.
	line13 = (int*)malloc(pDib->m_size.cy*sizeof(int)+12);
	
	a = (double(dy1-dy2))/(dx1-dx2);														// 1번에서 2번 점으로의 선분.
	b = dy1-dx1*a;
	for(y=dy1;y<=dy2;y++) 
		if( y>=0 && y < pDib->m_size.cy ) 
			line123[y] = (a==0 || dx1==dx2 )? dx1 : int((double(y-b))/a);
	
	a = (double(dy2-dy3))/(dx2-dx3);														// 2번에서 3번으로 선분.
	b = dy2-dx2*a;
	for(y=dy2;y<=dy3;y++) 
		if( y>=0 && y < pDib->m_size.cy ) 
			line123[y] = (a==0 || dx2==dx3)? dx2 : int((double(y-b))/a);

	a = (double(dy1-dy3))/(dx1-dx3);														// 1번에서 3번으로 선분.
	b = dy1-dx1*a;																			
	for(y=dy1;y<=dy3;y++) 
		if( y>=0 && y < pDib->m_size.cy ) 
			line13[y] = (a==0||dx3==dx1)? dx3 : int((double(y-b))/a);

	int			factor1, factor2, factor3;
	FDOUBLE		a1, a2, a3;
	BYTE*		pDest;

	factor1 = (((dx1-dx2)*(dy1-dy3)-(dx1-dx3)*(dy1-dy2)));
	factor2 = (((dx2-dx3)*(dy2-dy1)-(dx2-dx1)*(dy2-dy3)));
	factor3 = (((dx3-dx1)*(dy3-dy2)-(dx3-dx2)*(dy3-dy1)));


	DWORD widthbyte = WIDTHBYTES((DWORD)m_size.cx * BIT_PER_PIXEL_24BIT);
	if(nAlpha==-1)																			// ALPHA 안하기 //
	{
		for(y=sy;y<=ey;y++)
		{
			sx = max(0,min(line123[y], line13[y]));
			ex = min(pDib->m_size.cx-1, max(line123[y], line13[y]));

			pDest = pDib->GetScanLine(y) + BYTE_PER_PIXEL_24BIT*sx;

			for(x=sx;x<=ex;x++)
			{
				a1 = ( INT2FD2(((x-dx2)*(y-dy3)-(x-dx3)*(y-dy2))) / factor1 );			// 3점 factor.
				a2 = ( INT2FD2(((x-dx3)*(y-dy1)-(x-dx1)*(y-dy3))) / factor2 );
				a3 = ( INT2FD2(((x-dx1)*(y-dy2)-(x-dx2)*(y-dy1))) / factor3 );

				ox = FD2INT2((a1*sx1 + a2*sx2 + a3*sx3));								// ORIGINAL 이미지의 X,Y
				oy = FD2INT2((a1*sy1 + a2*sy2 + a3*sy3));

				ox = max(0,ox);
				oy = max(0,oy);

//				clr = GetPixel24Fast(ox,oy);
//				*((COLORREF*)pDest) = clr;
				*((COLORREF*)pDest) = (*((COLORREF*)(m_pDIBits+(widthbyte*(m_size.cy-(oy+1)))+BYTE_PER_PIXEL_24BIT*ox)))&0x00ffffff;
				pDest += BYTE_PER_PIXEL_24BIT;
			}
		}
	}
	else																					// ALPHA 하기 //
	{
		int y_dy1, y_dy2, y_dy3;	// (y-dy1, y-dy2, y-dy3)  를 다시 연산하지 않기 위한 변수.
		int x_dx1, x_dx2, x_dx3;	// (x-dx1, x-dx2, x-dx3)  를 다시 연산하지 않기 위한 변수.
		int	r_Alpha = 255-nAlpha;

		for(y=sy;y<=ey;y++)
		{
			sx = max(0,min(line123[y], line13[y]));
			ex = min(pDib->m_size.cx-1, max(line123[y], line13[y]));

			pDest = pDib->GetScanLine(y) + BYTE_PER_PIXEL_24BIT*sx;

			y_dy1 = y-dy1;
			y_dy2 = y-dy2;
			y_dy3 = y-dy3;
			x_dx1 = sx-dx1;
			x_dx2 = sx-dx2;
			x_dx3 = sx-dx3;
			for(x=sx;x<=ex;x++)
			{
				a1 = ( INT2FD2(((x_dx2)*(y_dy3)-(x_dx3)*(y_dy2))) / factor1 );			// 3점 factor.
				a2 = ( INT2FD2(((x_dx3)*(y_dy1)-(x_dx1)*(y_dy3))) / factor2 );
				a3 = ( INT2FD2(((x_dx1)*(y_dy2)-(x_dx2)*(y_dy1))) / factor3 );

				x_dx1++;
				x_dx2++;
				x_dx3++;

				ox = FD2INT2((a1*sx1 + a2*sx2 + a3*sx3));								// ORIGINAL 이미지의 X,Y
				oy = FD2INT2((a1*sy1 + a2*sy2 + a3*sy3));

//				ox = max(0,ox);					// ? 필요한가
//				oy = max(0,oy);

//				clr = GetPixel24Fast(ox,oy);
				clr = (*((COLORREF*)(m_pDIBits+(widthbyte*(m_size.cy-(oy+1)))+BYTE_PER_PIXEL_24BIT*ox)))&0x00ffffff;

				*pDest = AlphaTable[GetRValue(clr)][nAlpha] + AlphaTable[*pDest][r_Alpha];pDest++;		
				*pDest = AlphaTable[GetGValue(clr)][nAlpha] + AlphaTable[*pDest][r_Alpha];pDest++;
				*pDest = AlphaTable[GetBValue(clr)][nAlpha] + AlphaTable[*pDest][r_Alpha];pDest++;
			}
		}
	}

	free(line123);
	free(line13);

	return TRUE;
}


/* 와핑 하기 */
BOOL GDib::DrawWarpingRectTo(GDib* pDib,  int dx1, int dy1, int dx2, int dy2, int dx3, int dy3, int dx4, int dy4, int sx1, int sy1, int sx2, int sy2, int sx3, int sy3, int sx4, int sy4, int nAlpha)
{
	/*
		(1)	
		+---+---+(2)
		|   |   |
		+---+---+
		|   |   |
	 (4)+---+---+
			  (3)
  */
	BOOL bRet;
	bRet = DrawWarpingTo(pDib, dx1, dy1, dx2, dy2, dx3, dy3, 
		sx1, sy1, sx2, sy2, sx3, sy3, nAlpha);
	if(bRet==FALSE) return FALSE;
	bRet = DrawWarpingTo(pDib, dx3, dy3, dx4, dy4, dx1, dy1, 
		sx3, sy3, sx4, sy4, sx1, sy1, nAlpha);
	return bRet;
}


//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
// OLE 사용
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
bool GDib::OLELoadFromFile(CString sFileName)
{
	bool bResult = false;

	CFile			cFile;
	CFileException	e;

	if (cFile.Open(sFileName, CFile::modeRead | CFile::typeBinary, &e))
	{
		BYTE* pBuff = new BYTE[cFile.GetLength()];

		if (cFile.Read(pBuff, cFile.GetLength()) > 0)
		{
			if (OLELoadFromBuffer(pBuff, cFile.GetLength()))
				bResult = true;
		}

		delete [] pBuff;
	}
	return bResult;
}

bool GDib::OLELoadFromResource(CString sResourceType, CString sResource)
{
	bool bResult = false;

	BYTE*	pBuff = NULL;
	int		nSize = 0;
	if (OLEGetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
	{
		if (nSize > 0)
		{
			pBuff = new BYTE[nSize];

			if (OLEGetResource(sResource.GetBuffer(0), sResourceType.GetBuffer(0), pBuff, nSize))
			{
				if (OLELoadFromBuffer(pBuff, nSize))
					bResult = true;
			}

			delete [] pBuff;
		}
	}
	return bResult;
}

#define	HIMETRIC_INCH 2540
bool GDib::OLELoadFromBuffer(BYTE* pBuff, int nSize)
{
	bool bResult = false;

	HGLOBAL hGlobal = GlobalAlloc(GMEM_MOVEABLE, nSize);
	void* pData = GlobalLock(hGlobal);
	memcpy(pData, pBuff, nSize);
	GlobalUnlock(hGlobal);

	IStream* pStream = NULL;

	IPicture* pPicture;
	if (CreateStreamOnHGlobal(hGlobal, TRUE, &pStream) == S_OK)
	{
		HRESULT hr;
		// 파일에서 읽기
		if ((hr = OleLoadPicture(pStream, nSize, FALSE, IID_IPicture, (LPVOID *)&pPicture)) == S_OK)
		{
			bResult = true;

			// 파일에서 읽기 성공시 MEMDC 로 복사..
			long hmWidth;
			long hmHeight;
			int	 nWidth, nHeight;
			CDC* pDC;
			pPicture->get_Width(&hmWidth);
			pPicture->get_Height(&hmHeight);

			if(!this->m_bOK)								// 비트맵이 생성 안되어 있었으면 임시 비트맵 생성
				this->CreateNullImage(8, 1);
			pDC = this->GetMemDC();

			nWidth	= MulDiv(hmWidth,	pDC->GetDeviceCaps(LOGPIXELSX), HIMETRIC_INCH);
			nHeight	= MulDiv(hmHeight,	pDC->GetDeviceCaps(LOGPIXELSY), HIMETRIC_INCH);
			if(this->GetSize()!=CSize(nWidth, nHeight))		// 크기가 다를 경우만 새로 비트맵 생성
				this->CreateNullImage(nWidth, nHeight);

			if(pPicture->Render(this->GetMemDC()->m_hDC, 0, 0, nWidth, nHeight, 0, hmHeight, hmWidth, -hmHeight, NULL)!=S_OK)
				bResult = false;
		}
	
		pStream->Release();
		if(pPicture) pPicture->Release();
	}

	return bResult;
}

bool GDib::OLEGetResource(LPSTR lpName, LPSTR lpType, void* pResource, int& nBufSize)
{ 
	HRSRC		hResInfo;
	HANDLE		hRes;
	HMODULE		hInst	= NULL; 
	LPSTR		lpRes	= NULL; 
	int			nLen	= 0;
	bool		bResult	= FALSE;

	// Find the resource
	hResInfo = FindResource(hInst, lpName, lpType);

	if (hResInfo == NULL) 
		return false;

	// Load the resource
	hRes = LoadResource(hInst, hResInfo);

	if (hRes == NULL) 
		return false;

	// Lock the resource
	lpRes = (char*)LockResource(hRes);

	if (lpRes != NULL)
	{ 
		if (pResource == NULL)
		{
			nBufSize = SizeofResource(hInst, hResInfo);
			bResult = true;
		}
		else
		{
			if (nBufSize >= (int)SizeofResource(hInst, hResInfo))
			{
				memcpy(pResource, lpRes, nBufSize);
				bResult = true;
			}
		} 

		UnlockResource(hRes);  
	}

	// Free the resource
	FreeResource(hRes);

	return bResult;
}


/////////////////////
/* DIB 조작 함수들 */
/////////////////////
bool GDib::Sharpen()
{
	BYTE*	p1;
	BYTE*	p2;
	BYTE*	p3;
	BYTE*	p;
	int		x,y;
	int		nResult;

	if(m_size.cx <3 || m_size.cy <3 ) {ASSERT(0); return false; }			// 최소 3x3 이상 되어야 한다.
	if(m_nPixelFormat!=PF_24BIT) {ASSERT(0); return false; }


	// 임시 dib
	GDib	dib;

	// 복사
	dib = *this;

	BYTE	*p11, *p12, *p13;
	BYTE	*p21, *p22, *p23;
	BYTE	*p31, *p32, *p33;
	for(y=1;y<m_size.cy-1;y++)
	{
		if(m_pProgressFunc)
			(*m_pProgressFunc)(y, m_size.cy, m_pProgressParam);

		p1 = dib.GetScanLine(y-1)+3;
		p2 = dib.GetScanLine(y)+3;
		p3 = dib.GetScanLine(y+1)+3;
		p  = this->GetScanLine(y)+3;			// 0 이 아니라 1 에서 시작 (3x3 격자 이므로 )

		for(x=0;x<(m_size.cx-2)*3;x++)
		{
			//       b		             g                     r
			// p1 -  p1-3 , p1 , p1+3    p1-2 , p1+1 , p1+4    p1-1 , p1+2 , p1+5
			// p2 -  p2-3 , p2 , p2+3    p2-2 , p2+1 , p2+4    p2-1 , p2+2 , p2+5
			// p3 -  p3-3 , p3 , p3+3    p3-2 , p3+1 , p3+4    p3-1 , p3+2 , p3+5
/*
			nResult =  *(p1)*8;
			nResult -=	( (*(p1-3)+*(p1)+*(p1+3)) + (*(p2-3)+*(p2+3)) + (*(p3-3)+*(p3)+*(p3+3)) );
			nResult = max(0,nResultB);
			nResult /= 9;
			nResult += *(p1);
*/
			p11 = p1-3;		p12 = p1;	p13 = p1+3;
			p21 = p2-3;		p22 = p2;	p23 = p2+3;
			p31 = p3-3;		p32 = p3;	p33 = p3+3;

//			nResult =  -0.3 * *p11 + -0.3 * *p12 + -0.3 * *p13 +
//				       -0.3 * *p21 +  3.4 * *p22 + -0.3 * *p23 +
//				 	   -0.3 * *p31 + -0.3 * *p32 + -0.3 * *p33;

			nResult =  (int)(
						-0.2 * *p11 + -0.2 * *p12 + -0.2 * *p13 +
						-0.2 * *p21 +  2.6 * *p22 + -0.2 * *p23 +
				 		-0.2 * *p31 + -0.2 * *p32 + -0.2 * *p33);


			nResult = max(min(nResult,255),0);
			*p = nResult;

			p++;
			p1++;
			p2++;
			p3++;
		}
	}
	return true;

}

