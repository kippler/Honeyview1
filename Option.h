#pragma once

class COption  
{
public:
	COption();
	virtual ~COption();

public :
	void			LoadOption();
	void			SaveOption();


public :
	BOOL			bFlipPage;
	BOOL			bInterpolation;
	BOOL			bSharpen;
	BOOL			bStretech;
	BOOL			bView2Page;
	BOOL			bCacheNextFile;
	BOOL			bAutoDetectWidth;
	BOOL			bIsMaximized;
	CString			sRecentFolder;
	BOOL			bRotate;

};
