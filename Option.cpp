#include "stdafx.h"
#include "honeyview.h"
#include "Option.h"
#include "Gutil.h"

#define			REG_MAIN_KEY	"SOFTWARE\\KIPPLER\\HONEYVIEW"


COption::COption()
{
	LoadOption();
}

COption::~COption()
{
	SaveOption();
}

void COption::LoadOption()
{
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bFlipPage",			bFlipPage,			FALSE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bInterpolation",	bInterpolation,		TRUE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bSharpen",			bSharpen,			FALSE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bStretech",			bStretech,			TRUE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bView2Page",		bView2Page,			FALSE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bCacheNextFile",	bCacheNextFile,		TRUE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bAutoDetectWidth",	bAutoDetectWidth,	FALSE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bIsMaximized",		bIsMaximized,		FALSE);
	GetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bRotate",			bRotate,			FALSE);
	
	
	
	GetRegStr(HKEY_CURRENT_USER, REG_MAIN_KEY, "sRecentFolder",		sRecentFolder);
}

void COption::SaveOption()
{
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bFlipPage",			bFlipPage);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bInterpolation",	bInterpolation);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bSharpen",			bSharpen);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bStretech",			bStretech);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bView2Page",		bView2Page);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bCacheNextFile",	bCacheNextFile);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bAutoDetectWidth",	bAutoDetectWidth);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bIsMaximized",		bIsMaximized);
	SetRegInt(HKEY_CURRENT_USER, REG_MAIN_KEY, "bRotate",			bRotate);
	
	SetRegStr(HKEY_CURRENT_USER, REG_MAIN_KEY, "sRecentFolder",		sRecentFolder);
}