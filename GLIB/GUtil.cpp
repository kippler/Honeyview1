#include "stdafx.h"
#include "Gutil.h"
#include <windowsx.h>
#include <io.h>


//##################################################################################################################
//##	CRC 관련
//##################################################################################################################

const DWORD CRC32[256] = {
      0x00000000, 0x77073096, 0xee0e612c, 0x990951ba, 0x076dc419, 0x706af48f,      0xe963a535, 0x9e6495a3, 0x0edb8832, 0x79dcb8a4, 0xe0d5e91e, 0x97d2d988,      0x09b64c2b, 0x7eb17cbd, 0xe7b82d07, 0x90bf1d91, 0x1db71064, 0x6ab020f2,      0xf3b97148, 0x84be41de, 0x1adad47d, 0x6ddde4eb, 0xf4d4b551, 0x83d385c7,      0x136c9856, 0x646ba8c0, 0xfd62f97a, 0x8a65c9ec, 0x14015c4f, 0x63066cd9,      0xfa0f3d63, 0x8d080df5, 0x3b6e20c8, 0x4c69105e, 0xd56041e4, 0xa2677172,      0x3c03e4d1, 0x4b04d447, 0xd20d85fd, 0xa50ab56b, 0x35b5a8fa, 0x42b2986c,      0xdbbbc9d6, 0xacbcf940, 0x32d86ce3, 0x45df5c75, 0xdcd60dcf, 0xabd13d59,
      0x26d930ac, 0x51de003a, 0xc8d75180, 0xbfd06116, 0x21b4f4b5, 0x56b3c423,      0xcfba9599, 0xb8bda50f, 0x2802b89e, 0x5f058808, 0xc60cd9b2, 0xb10be924,      0x2f6f7c87, 0x58684c11, 0xc1611dab, 0xb6662d3d, 0x76dc4190, 0x01db7106,      0x98d220bc, 0xefd5102a, 0x71b18589, 0x06b6b51f, 0x9fbfe4a5, 0xe8b8d433,      0x7807c9a2, 0x0f00f934, 0x9609a88e, 0xe10e9818, 0x7f6a0dbb, 0x086d3d2d,      0x91646c97, 0xe6635c01, 0x6b6b51f4, 0x1c6c6162, 0x856530d8, 0xf262004e,      0x6c0695ed, 0x1b01a57b, 0x8208f4c1, 0xf50fc457, 0x65b0d9c6, 0x12b7e950,      0x8bbeb8ea, 0xfcb9887c, 0x62dd1ddf, 0x15da2d49, 0x8cd37cf3, 0xfbd44c65,
      0x4db26158, 0x3ab551ce, 0xa3bc0074, 0xd4bb30e2, 0x4adfa541, 0x3dd895d7,      0xa4d1c46d, 0xd3d6f4fb, 0x4369e96a, 0x346ed9fc, 0xad678846, 0xda60b8d0,      0x44042d73, 0x33031de5, 0xaa0a4c5f, 0xdd0d7cc9, 0x5005713c, 0x270241aa,      0xbe0b1010, 0xc90c2086, 0x5768b525, 0x206f85b3, 0xb966d409, 0xce61e49f,      0x5edef90e, 0x29d9c998, 0xb0d09822, 0xc7d7a8b4, 0x59b33d17, 0x2eb40d81,      0xb7bd5c3b, 0xc0ba6cad, 0xedb88320, 0x9abfb3b6, 0x03b6e20c, 0x74b1d29a,      0xead54739, 0x9dd277af, 0x04db2615, 0x73dc1683, 0xe3630b12, 0x94643b84,      0x0d6d6a3e, 0x7a6a5aa8, 0xe40ecf0b, 0x9309ff9d, 0x0a00ae27, 0x7d079eb1,
      0xf00f9344, 0x8708a3d2, 0x1e01f268, 0x6906c2fe, 0xf762575d, 0x806567cb,      0x196c3671, 0x6e6b06e7, 0xfed41b76, 0x89d32be0, 0x10da7a5a, 0x67dd4acc,      0xf9b9df6f, 0x8ebeeff9, 0x17b7be43, 0x60b08ed5, 0xd6d6a3e8, 0xa1d1937e,      0x38d8c2c4, 0x4fdff252, 0xd1bb67f1, 0xa6bc5767, 0x3fb506dd, 0x48b2364b,      0xd80d2bda, 0xaf0a1b4c, 0x36034af6, 0x41047a60, 0xdf60efc3, 0xa867df55,      0x316e8eef, 0x4669be79, 0xcb61b38c, 0xbc66831a, 0x256fd2a0, 0x5268e236,      0xcc0c7795, 0xbb0b4703, 0x220216b9, 0x5505262f, 0xc5ba3bbe, 0xb2bd0b28,      0x2bb45a92, 0x5cb36a04, 0xc2d7ffa7, 0xb5d0cf31, 0x2cd99e8b, 0x5bdeae1d,
      0x9b64c2b0, 0xec63f226, 0x756aa39c, 0x026d930a, 0x9c0906a9, 0xeb0e363f,      0x72076785, 0x05005713, 0x95bf4a82, 0xe2b87a14, 0x7bb12bae, 0x0cb61b38,      0x92d28e9b, 0xe5d5be0d, 0x7cdcefb7, 0x0bdbdf21, 0x86d3d2d4, 0xf1d4e242,      0x68ddb3f8, 0x1fda836e, 0x81be16cd, 0xf6b9265b, 0x6fb077e1, 0x18b74777,      0x88085ae6, 0xff0f6a70, 0x66063bca, 0x11010b5c, 0x8f659eff, 0xf862ae69,      0x616bffd3, 0x166ccf45, 0xa00ae278, 0xd70dd2ee, 0x4e048354, 0x3903b3c2,      0xa7672661, 0xd06016f7, 0x4969474d, 0x3e6e77db, 0xaed16a4a, 0xd9d65adc,      0x40df0b66, 0x37d83bf0, 0xa9bcae53, 0xdebb9ec5, 0x47b2cf7f, 0x30b5ffe9,
      0xbdbdf21c, 0xcabac28a, 0x53b39330, 0x24b4a3a6, 0xbad03605, 0xcdd70693,      0x54de5729, 0x23d967bf, 0xb3667a2e, 0xc4614ab8, 0x5d681b02, 0x2a6f2b94,      0xb40bbe37, 0xc30c8ea1, 0x5a05df1b, 0x2d02ef8d };

#define DO1(buf) crc = CRC32[((int)crc ^ (*buf++)) & 0xff] ^ (crc >> 8);
#define DO2(buf)  DO1(buf); DO1(buf);
#define DO4(buf)  DO2(buf); DO2(buf);
#define DO8(buf)  DO4(buf); DO4(buf);

/* ========================================================================= */
DWORD crc32( DWORD crc, const BYTE *buf, UINT len)
{
    if (buf == NULL) return 0L;
    crc = crc ^ 0xffffffffL;
    while (len >= 8)
    {
      DO8(buf);
      len -= 8;
    }
    if(len) do {
      DO1(buf);
    } while (--len);
    return crc ^ 0xffffffffL;
}

#define FILEBUFSIZE 4096
/* 파일의 CRC 를 체크하고 리턴한다. */
bool CheckFileCRC(const char* szFileName, DWORD &dwResult,int nStartPos)
{
	HANDLE hf;
	int		m_uhCRCBytes = 0;
	DWORD dwRead;
	BYTE dwBuffer[FILEBUFSIZE];
	DWORD dwCRC;

	memset(dwBuffer,0,sizeof(dwBuffer));
	hf = CreateFile(szFileName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
	FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hf==INVALID_HANDLE_VALUE) return false;

	SetFilePointer(hf, nStartPos, (PLONG) NULL, FILE_BEGIN);

	dwCRC = 0;
	while(ReadFile(hf, &dwBuffer, sizeof(dwBuffer), &dwRead, NULL))
	{
		if(dwRead==0) break;
		dwCRC = crc32(dwCRC, dwBuffer, dwRead);
	}
	CloseHandle(hf);

	dwResult = dwCRC;
	return true;
}


#define MAX_PASSENCKEY_SIZE		13
const char szPassEncKey[MAX_PASSENCKEY_SIZE] = "이런젠장할";

// sPassWord 를 sDest[MAX_PASSENCDEC_BUFSIZE] 로 인크립션 시킨다.
void EncPass(char* sDest,const char* sPassWord)
{
	int i,j;
	memset(sDest,0,MAX_PASSENCDEC_BUFSIZE);
	strcpy(sDest,sPassWord);

	for(i=0;i<MAX_PASSENCDEC_BUFSIZE;i++)
	{
		for(j=i;j<i+MAX_PASSENCDEC_BUFSIZE;j++)
		{
			sDest[(j+1)%MAX_PASSENCDEC_BUFSIZE] ^= sDest[j%MAX_PASSENCDEC_BUFSIZE];
			sDest[(j)%MAX_PASSENCDEC_BUFSIZE] ^= szPassEncKey[j%MAX_PASSENCKEY_SIZE];
		}
	}
}

// 인크립션된 binary 데이타를 복구시켜준다.
char* DecPass(char* sSrc)
{
	int i,j;
	static char sDest[MAX_PASSENCDEC_BUFSIZE];
	memset(sDest,0,MAX_PASSENCDEC_BUFSIZE);
	memcpy(sDest, sSrc, MAX_PASSENCDEC_BUFSIZE);

	for(i=MAX_PASSENCDEC_BUFSIZE-1;i>=0;i--)
	{
		for(j=i+MAX_PASSENCDEC_BUFSIZE-1;j>=i;j--)
		{
			sDest[(j)%MAX_PASSENCDEC_BUFSIZE] ^= szPassEncKey[j%MAX_PASSENCKEY_SIZE];
			sDest[(j+1)%MAX_PASSENCDEC_BUFSIZE] ^= sDest[j%MAX_PASSENCDEC_BUFSIZE];
		}
	}
	return sDest;
}

//##################################################################################################################
//##	파일 관련
//##################################################################################################################
#include <AFXINET.H>
// URL 에서 파일 이름을 뽑아낸다.
bool		GetFileNameFromURL(const CString& sURL, CString& sResult)
{
	DWORD			dwServerType;
	CString			sServer, sObject;
	INTERNET_PORT	nPort;
	// 파일 이름을 얻어 온다. -- sObject 에는 "/dir/dir/objec.ext" 형식으로 들어가 있다..
	if(AfxParseURL(sURL,dwServerType, sServer, sObject, nPort)==FALSE)
	{
		sResult = "";
		return false;
	}
	// 진짜 로칼 파일 이름을 얻어 온다. 
	sResult = GetFileNameFromPath(sObject);
	return true;
}

// PATH 명에서 파일 이름을 뽑는다.
char* GetFileNameFromPath( const char* szPathName )
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   static char ret[_MAX_FNAME];
   _splitpath( szPathName, drive, dir, fname, ext );
   strcpy(ret,fname);
   strcat(ret,ext);
   return ret;
}

// PATH 명에서 확장자를 제외한 파일명을 뽑는다.
char* GetFileNameFromPathWithOutExt( const char* szPathName )
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   static char ret[_MAX_FNAME];
   _splitpath( szPathName, drive, dir, fname, ext );
   strcpy(ret,fname);
   return ret;
}

// PATH 명에서 확장자 만을 뽑아내기.
char*	GetExtFromPath(const char* szPathName)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   static char ret[_MAX_FNAME];
   _splitpath( szPathName, drive, dir, fname, ext );
   strcpy(ret,ext);
   return ret;
}

// PATH 명에서 드라이브 (EX) "C:") 를 뽑아낸다.
char*	GetDriveNameFromPath(const char* szPathName)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   static char ret[_MAX_DRIVE];
   _splitpath( szPathName, drive, dir, fname, ext );
   strcpy(ret,drive);
   return ret;
}

// PATH 명에서 디렉토리를 뽑아낸다.
char*	GetDirNameFromPath(const char* szPathName)
{
   char drive[_MAX_DRIVE];
   char dir[_MAX_DIR];
   char fname[_MAX_FNAME];
   char ext[_MAX_EXT];
   static char ret[_MAX_DIR];
   _splitpath( szPathName, drive, dir, fname, ext );
   strcpy(ret,dir);
   return ret;
}

// 전체 폴더중 마지막 디렉토리 이름 구하기
CString GetLastDirFromPath(const CString& sPathName)
{
	CString sRet;
	sRet = GetDirNameFromPath(sPathName);
	sRet = sRet.Left(sRet.GetLength()-1);			// 끝의 \\ 떼기 
	sRet = sRet.Mid(sRet.ReverseFind('\\')+1);
	return sRet;
}

// PARENT 폴더 구하기 - 루트(EX) C:\)의 PARENT 일 경우는 ""를 리턴한다 
CString GetParentPath(const CString& sPathName)
{
	CString sRet;
	sRet = sPathName;
	if(sRet.Right(1)=="\\")
		sRet = sRet.Left(sRet.GetLength()-1);			// 끝의 \\ 떼기 
	sRet = sRet.Left(sRet.ReverseFind('\\')+1);
	return sRet;
}

// 파일 사이즈 구하기
int GetFileSize(const CString& sPathName)
{
	HANDLE	hFile;
	int		nSize;
	hFile = ::CreateFile(sPathName, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, 
		FILE_FLAG_SEQUENTIAL_SCAN, NULL);
	if(hFile==INVALID_HANDLE_VALUE) return -1;
	nSize = ::GetFileSize(hFile, NULL);
	CloseHandle(hFile);
	return nSize;


	// 2000(혹은 NT) 에서 이 함수는 파일이 close 가 되지 않는 문제가 발생한다. 
	/*
	struct _finddata_t c_file;
	HFILE		hFile;
    if( hFile = _findfirst( sPathName, &c_file ) == -1L ) return -1;
	_findclose(hFile);
	return c_file.size;
	*/
}

// 폴더 여부 판단
bool IsFolder(const char* szPathName)
{
	DWORD dwRet;
	dwRet = GetFileAttributes(szPathName);
	if(dwRet==-1) return false;					// 오류 발생
	if(dwRet & FILE_ATTRIBUTE_DIRECTORY) return true;
	return false;
}
// 파일 여부 판단
bool IsFile(const char* szPathName)
{
	DWORD dwRet;
	dwRet = GetFileAttributes(szPathName);
	if(dwRet==-1) return false;					// 오류 발생
	if(dwRet & FILE_ATTRIBUTE_DIRECTORY) return false;
//	if(dwRet & FILE_ATTRIBUTE_NORMAL) return true;
	return true;
}

CString  g_sGetDirectoryDir;
int CALLBACK BrowseCallbackProc(HWND hwnd,UINT msg,LPARAM lp, LPARAM pData)
{
	TCHAR buf[MAX_PATH];
	switch(msg) 
	{
	// when dialog is first initialized, change directory to one chosen above
		case BFFM_INITIALIZED: 
			strcpy(buf,g_sGetDirectoryDir);
			::SendMessage(hwnd,BFFM_SETSELECTION,TRUE,(LPARAM)buf);
			break;
	// if you picked BIF_STATUSTEXT above, you can fill status here
		case BFFM_SELCHANGED:
			if (::SHGetPathFromIDList((LPITEMIDLIST) lp ,buf)) 
				SendMessage(hwnd,BFFM_SETSTATUSTEXT,0,(LPARAM)buf);
			break;
	}
	return 0;
}

// 사용자로부터 폴더를 입력 받는다.
CString GetDirectory(HWND hWndParent, const char* szRoot, const char* szTitle)
{
	CString str;
	BROWSEINFO bi;
    bi.hwndOwner=hWndParent;		//owner of created dialog box
    bi.pidlRoot=0;					//unused
    bi.pszDisplayName=0;			//buffer to receive name displayed by folder (not a valid path)
    bi.lpszTitle=szTitle;			//title is "Browse for Folder", this is an instruction
	bi.lpfn = BrowseCallbackProc;	//callback routine called when dialog has been initialized
    bi.lParam=0;					//passed to callback routine
    bi.ulFlags=
		BIF_RETURNONLYFSDIRS |		//only allow user to select a directory
		BIF_STATUSTEXT |			//create status text field we will be writing to in callback
//		BIF_BROWSEFORCOMPUTER|		//only allow user to select a computer
//		BIF_BROWSEFORPRINTER |		//only allow user to select a printer
//		BIF_BROWSEINCLUDEFILES|		//displays files too which user can pick
//		BIF_DONTGOBELOWDOMAIN|		//when user is exploring the "Entire Network" they
									// are not allowed into any domain
		0; 
	g_sGetDirectoryDir = szRoot;

	LPITEMIDLIST lpItemId=::SHBrowseForFolder(&bi); 
	if (lpItemId)
	{
		LPTSTR szBuf=str.GetBuffer(MAX_PATH);
		::SHGetPathFromIDList(lpItemId, szBuf);
		::GlobalFree(lpItemId);
		str.ReleaseBuffer();
	}
	return str;
}

// 폴더를 지운다.
void	DeleteFolder(CString sFolder)
{
	if(sFolder.Right(1)!="\\") sFolder+= "\\";

	CFileFind	FileFind;
	BOOL		bResult;

	bResult = FileFind.FindFile(sFolder+"*.*");

	while (bResult)
	{
		bResult = FileFind.FindNextFile();
		DeleteFile(FileFind.GetFilePath());
	}

	RemoveDirectory(sFolder);
}

// 두개의 폴더(혹은 폴더+파일)을 더한다.
CString AddPath(CString sLeft, CString sRight)
{
	CString sRet;
	sRight = 
		sRight.GetLength()==0 ? "" : 
		(sRight.GetLength()==1 && sRight.Left(1)=="\\") ? "" :  
		sRight.Left(1)=="\\" ? sRight.Mid(1) : 
		sRight;

	sRet = (sLeft.Right(1)=="\\") ?
				sLeft + sRight :
				sLeft + "\\" + sRight;
	return sRet;
}

// 폴더를 뚫는다.
bool	DigPath(CString sFolderPath)
{
	if(IsFolder(sFolderPath)) return true;						// 뚫을 필요 없다.


	CString	sSubPath;
	int		nFrom, nTo;

	nFrom = 0;
	nTo = 0;
	nFrom = sFolderPath.Find("\\", 0)+1;
	for(;;)
	{
		nTo = sFolderPath.Find("\\", nFrom);
		if(nTo==-1) break;
		sSubPath = sFolderPath.Left(nTo);
		::CreateDirectory(sSubPath, NULL);
		nFrom = nTo+1;
	}

	::CreateDirectory(sFolderPath, NULL);
	return IsFolder(sFolderPath);
}

//##################################################################################################################
//##	INI / REGISTRY
//##################################################################################################################

// 레지스트리 읽고 쓰는데 사용되는 APP 이름.
/*
CString			g_sRegAppName;
void SetPrivateRegAppName(const CString& sAppName)
{
	g_sRegAppName = sAppName;
}

// REGISTRY 에 INT 값 저장
void SetPrivateRegInt(CString sKeyName, DWORD nValue)
{
	if(g_sRegAppName=="") g_sRegAppName = AfxGetAppName();
	HKEY		hKey;
	HKEY		hkRoot, hSubKey;

	RegOpenKey(HKEY_LOCAL_MACHINE,NULL, &hkRoot);
	RegOpenKey(hkRoot,"SOFTWARE",&hKey);
    if(RegOpenKey(hKey,g_sRegAppName,&hSubKey)!=ERROR_SUCCESS)
    {
    	RegCreateKey(hKey,g_sRegAppName,&hSubKey);
    }
	::RegSetValueEx(hSubKey,sKeyName,NULL,REG_DWORD,(CONST BYTE*)&nValue,sizeof(DWORD));

	RegCloseKey(hSubKey);
	RegCloseKey(hKey);
	RegCloseKey(hkRoot);
	return ;
}

// REGISTRY 에서 INT 값 읽기
DWORD GetPrivateRegInt(CString sKeyName, DWORD nDefaultValue)
{
	if(g_sRegAppName=="") g_sRegAppName = AfxGetAppName();
	HKEY		hKey;
	DWORD		dwDispotion;
    DWORD		ret=0;
	RegCreateKeyEx(HKEY_LOCAL_MACHINE,										// 레지스트리 열기.
    	"SOFTWARE\\"+g_sRegAppName,
		NULL,
		"Class??",															// ????
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDispotion);
	// 기존 레지스트리가 존재하지 않으면, 모든 변수를 초기화 한다. 
	if(dwDispotion==REG_CREATED_NEW_KEY) 
	{
        ret = nDefaultValue;
	}
	else 
	{
		DWORD	size;
		size = sizeof(ret);
		int result = RegQueryValueEx(
			hKey,sKeyName,NULL,NULL,
			(unsigned char *)&ret,
			&size);
		if(ERROR_SUCCESS!=result) 
		{ // if 실패시.
        	ret = nDefaultValue;
        }
	}
    RegCloseKey(hKey);
    return ret;
}

// REGISTRY 에 BINARY 값 저장
void SetPrivateRegBin(CString sKeyName, void* pData, int nSize)
{
	if(g_sRegAppName=="") g_sRegAppName = AfxGetAppName();
	HKEY		hKey;
	HKEY		hkRoot, hSubKey;

	RegOpenKey(HKEY_LOCAL_MACHINE,NULL, &hkRoot);
	RegOpenKey(hkRoot,"SOFTWARE",&hKey);
    if(RegOpenKey(hKey,g_sRegAppName,&hSubKey)!=ERROR_SUCCESS)
    	RegCreateKey(hKey,g_sRegAppName,&hSubKey);
	::RegSetValueEx(hSubKey,sKeyName,NULL,REG_BINARY,(const unsigned char*)pData,nSize);
	RegCloseKey(hSubKey);
	RegCloseKey(hKey);
	RegCloseKey(hkRoot);
}

// REGISTRY 에서 BINARY 값 읽기
bool GetPrivateRegBin(CString sKeyName, void* pData, int nSize)
{
	if(g_sRegAppName=="") g_sRegAppName = AfxGetAppName();
	HKEY		hKey;
	DWORD		dwDispotion;

	RegCreateKeyEx(HKEY_LOCAL_MACHINE,				// 레지스트리 열기.
    	"SOFTWARE\\"+g_sRegAppName,
		NULL,
		"Class??",							
		REG_OPTION_NON_VOLATILE,
		KEY_ALL_ACCESS,
		NULL,
		&hKey,
		&dwDispotion);

	// 기존 레지스트리가 존재하지 않으면, 모든 변수를 초기화 한다. 
	if(dwDispotion==REG_CREATED_NEW_KEY) 
	{
		memset(pData,0,nSize);
	    RegCloseKey(hKey);
        return false;
	}
	else 
	{
		int result = RegQueryValueEx(
			hKey,sKeyName,NULL,NULL,
			(unsigned char *)pData,
			(unsigned long*)(&nSize));
		if(ERROR_SUCCESS!=result) 
		{ 
			memset(pData,0,nSize);
		    RegCloseKey(hKey);
            return false;
        }
	}
    RegCloseKey(hKey);
    return true;
}
*/
// INI 파일에서 STRING 값 읽기
CString GetIniStr(const char* szSection, const char* szKey, CString sIniFileName, CString sDefaultValue)
{
    char  szBuf[10000];
	if(sIniFileName.Find(':')==-1 && sIniFileName.Left(2)!="\\\\")
		sIniFileName = GetThisPath() + sIniFileName;
    GetPrivateProfileString(szSection,szKey,sDefaultValue,szBuf,9999,sIniFileName);
    return CString(szBuf);
}
// INI 파일에 STRING 값 저장
void SetIniStr(const char* szSection, const char* szKey, const char* szValue, CString sIniFileName)
{
	if(sIniFileName.Find(':')==-1)
	    sIniFileName = GetThisPath() + sIniFileName;
	WritePrivateProfileString(szSection, szKey, szValue, sIniFileName);
}
// INI 파일에서 INT 값 읽기
int	GetIniInt(const char* szSection, const char* szKey, int nDefault, CString sIniFileName)
{
	int		nRet;
	if(sIniFileName.Find(':')==-1)
	    sIniFileName = GetThisPath() + sIniFileName;
    nRet = GetPrivateProfileInt(szSection, szKey, nDefault, sIniFileName);
	return nRet;
}
// INI 파일에 INT 값 저장
void SetIniInt(const char* szSection, const char* szKey, int nValue, CString sIniFileName)
{
	CString sValue;
	sValue.Format("%d", nValue);
	SetIniStr(szSection, szKey, sValue, sIniFileName);
}
// INI 파일의 SECTION 의 모든 KEY 를 리스트로 얻기
void GetIniKeyList(CStringList& slKey, CString sSection, CString sIniFileName)
{
	char szBuf[32*1024];
	char* p;

	slKey.RemoveAll();
	if(sIniFileName.Find(':')==-1) sIniFileName = GetThisPath() + sIniFileName;
	memset(szBuf, 0, sizeof(szBuf));
	GetPrivateProfileString(sSection,NULL,"",szBuf,sizeof(szBuf),sIniFileName);
	p = szBuf;
	while(*p)
	{
		slKey.AddTail(p);
		p = p + strlen(p) + 1;
	}
}


// 디폴트 값을 가지고 레지스트리에서 스트링을 읽어 들인다.
bool  GetRegStr(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue, CString& sResult, CString sDefaultValue)
{
	if(GetRegStr(hKeyRoot, sKeySub, sValue, sResult)) return true;
	sResult = sDefaultValue;
	return false;
}

// 레지스트리에서 스트링을 읽어 들인다 //
bool  GetRegStr(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue, CString& sResult)
{
	LONG		lResult;
	BYTE*		pBuf=NULL;
	DWORD		dwBufSize = 0;
	DWORD		dwType = REG_SZ;
	HKEY		hKeySub=NULL;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS) 
		goto FAIL;

	// 스트링의 사이즈를 가져 온다.
	lResult = RegQueryValueEx(hKeySub, sValue, NULL, &dwType, pBuf, &dwBufSize);
	if(dwBufSize==0) goto FAIL;
	pBuf = (BYTE*)malloc(dwBufSize);
	if(pBuf==NULL) goto FAIL;

	// 다시 읽어 온다.
	lResult = RegQueryValueEx(hKeySub, sValue, NULL, &dwType, pBuf, &dwBufSize);
	if(lResult!=ERROR_SUCCESS) goto FAIL;

	sResult = (char*)pBuf;
	free(pBuf);

	if(hKeySub)
		RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)
		RegCloseKey (hKeySub);
	return false;

}

// 레지스트리에서 스트링을 읽어 들인다 //
bool  GetRegStr(const CString& sKeyRoot, const CString& sKeySub, const CString& sValue, CString& sResult)
{
	HKEY		hKeyRoot;

	// 스트링으로 root 키를 찾는다.
	if(sKeyRoot=="HKEY_CLASSES_ROOT")			hKeyRoot = HKEY_CLASSES_ROOT;
	else if(sKeyRoot=="HKEY_CURRENT_USER")		hKeyRoot = HKEY_CURRENT_USER;
	else if(sKeyRoot=="HKEY_LOCAL_MACHINE")		hKeyRoot = HKEY_LOCAL_MACHINE;
	else if(sKeyRoot=="HKEY_USERS")				hKeyRoot = HKEY_USERS;
	else if(sKeyRoot=="HKEY_PERFORMANCE_DATA")	hKeyRoot = HKEY_PERFORMANCE_DATA;
	else if(sKeyRoot=="HKEY_CURRENT_CONFIG")	hKeyRoot = HKEY_CURRENT_CONFIG;
	else if(sKeyRoot=="HKEY_DYN_DATA")			hKeyRoot = HKEY_DYN_DATA;
	else return false;

	return GetRegStr(hKeyRoot, sKeySub, sValue, sResult);
}


// 통짜로 레지스트리에서 스트링을 읽어 들인다. 예:"HKEY_LOCAL_MACHINE\\SOFTWARE\\Blizzard Entertainment\\Starcraft\\Program"
bool  GetRegStr(const CString& sKey, CString& sResult)
{
	CString		sKeyRoot;
	CString		sKeySub;
	CString		sValue;

	// 루트키, 서브키, 값 을 추출한다. 
	int nPosFrom = sKey.Find('\\', 0);
	int nPosTo   = sKey.ReverseFind('\\');
	if(nPosFrom==-1 || nPosTo==-1) return false;

	sKeyRoot = sKey.Left(nPosFrom);
	sKeySub  = sKey.Mid(nPosFrom+1, nPosTo-nPosFrom );
	sValue   = sKey.Mid(nPosTo+1);

	return GetRegStr(sKeyRoot, sKeySub, sValue, sResult);
}

// 레지스트리에 스트링을 저장한다 //
bool  SetRegStr(HKEY hKeyRoot, const CString& sKeySub, const CString& sValueName, const CString& sValue)
{
	LONG		lResult;
	HKEY		hKeySub=NULL;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS)												// 레지스트리가 존재하지 않으면 만든다.
	{
		if(	RegCreateKey(hKeyRoot,sKeySub,&hKeySub)!=ERROR_SUCCESS)
			goto FAIL;
	}
	if(::RegSetValueEx(hKeySub,sValueName,NULL,REG_SZ,(const unsigned char*)(const char*)sValue,sValue.GetLength())!=ERROR_SUCCESS)
		goto FAIL;


	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;

}

// 레지스트리에 스트링을 저장한다 //
bool  SetRegStr(const CString& sKeyRoot, const CString& sKeySub, const CString& sValueName, const CString& sValue)
{
	HKEY		hKeyRoot;

	// 스트링으로 root 키를 찾는다.
	if(sKeyRoot=="HKEY_CLASSES_ROOT")			hKeyRoot = HKEY_CLASSES_ROOT;
	else if(sKeyRoot=="HKEY_CURRENT_USER")		hKeyRoot = HKEY_CURRENT_USER;
	else if(sKeyRoot=="HKEY_LOCAL_MACHINE")		hKeyRoot = HKEY_LOCAL_MACHINE;
	else if(sKeyRoot=="HKEY_USERS")				hKeyRoot = HKEY_USERS;
	else if(sKeyRoot=="HKEY_PERFORMANCE_DATA")	hKeyRoot = HKEY_PERFORMANCE_DATA;
	else if(sKeyRoot=="HKEY_CURRENT_CONFIG")	hKeyRoot = HKEY_CURRENT_CONFIG;
	else if(sKeyRoot=="HKEY_DYN_DATA")			hKeyRoot = HKEY_DYN_DATA;
	else return false;

	return SetRegStr(hKeyRoot, sKeySub, sValueName, sValue);
}

// 통짜로 레지스트리에 스트링을 저장한다 예:"HKEY_LOCAL_MACHINE\\SOFTWARE\\Blizzard Entertainment\\Starcraft\\Program"
bool  SetRegStr(const CString& sKey, const CString& sValue)
{
	CString		sKeyRoot;
	CString		sKeySub;
	CString		sValueName;

	// 루트키, 서브키, 값 을 추출한다. 
	int nPosFrom = sKey.Find('\\', 0);
	int nPosTo   = sKey.ReverseFind('\\');
	if(nPosFrom==-1 || nPosTo==-1) return false;

	sKeyRoot	= sKey.Left(nPosFrom);
	sKeySub		= sKey.Mid(nPosFrom+1, nPosTo-nPosFrom );
	sValueName  = sKey.Mid(nPosTo+1);

	return SetRegStr(sKeyRoot, sKeySub, sValueName, sValue);
}

// 레지스트리에 DWORD를 저장한다 //
bool  SetRegDWORD(HKEY hKeyRoot, const CString& sKeySub, const CString& sValueName, DWORD dwValue)
{
	LONG		lResult;
	HKEY		hKeySub=NULL;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS)												// 레지스트리가 존재하지 않으면 만든다.
	{
		if(	RegCreateKey(hKeyRoot,sKeySub,&hKeySub)!=ERROR_SUCCESS) 
			goto FAIL;
	}
	if(::RegSetValueEx(hKeySub,sValueName,NULL,REG_DWORD,(const unsigned char*)&dwValue,sizeof(DWORD)))
		goto FAIL;


	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;
}

// 레지스트리에서 DWORD를 읽어 들인다 //
bool  GetRegDWORD(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue, DWORD& dwResult, DWORD dwDefaultValue)
{
	LONG		lResult;
	BYTE		pBuf[4];
	DWORD		dwBufSize = sizeof(DWORD);
	DWORD		dwType = REG_DWORD;
	HKEY		hKeySub=NULL;

	dwResult = dwDefaultValue;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS) goto FAIL;

	// 읽어 온다.
	lResult = RegQueryValueEx(hKeySub, sValue, NULL, &dwType, pBuf, &dwBufSize);
	if(lResult!=ERROR_SUCCESS) goto FAIL;

	dwResult = *((DWORD*)pBuf);

	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;
}

// 레지스트리에 BINARY를 저장한다 //
bool  SetRegBin(HKEY hKeyRoot, const CString& sKeySub, const CString& sValueName, void* pData, int nDataSize)
{
	LONG		lResult;
	HKEY		hKeySub=NULL;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS)												// 레지스트리가 존재하지 않으면 만든다.
	{
		if(	RegCreateKey(hKeyRoot,sKeySub,&hKeySub)!=ERROR_SUCCESS)
			goto FAIL;
	}
	if(::RegSetValueEx(hKeySub,sValueName,NULL,REG_BINARY,(const unsigned char*)pData,nDataSize))
		goto FAIL;

	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;
}

// 레지스트리에서 BINARY를 읽어 들인다 - 반드시 free() 로 해제해 주어야 한다.. //
bool  GetRegBin(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue, BYTE** pRetBuf, int& nBufSize)
{
	LONG		lResult;
	DWORD		dwBufSize = nBufSize;
	DWORD		dwType = REG_BINARY;
	HKEY		hKeySub=NULL;

	// 레지스트리를 연다.
	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);
	if(lResult!=ERROR_SUCCESS) goto FAIL;

	// 바이너리의 사이즈를 가져 온다.
	lResult = RegQueryValueEx(hKeySub, sValue, NULL, &dwType, (unsigned char*)*pRetBuf, &dwBufSize);
	if(dwBufSize==0) goto FAIL;
	*pRetBuf = (BYTE*)malloc(dwBufSize);
	if(*pRetBuf==NULL) goto FAIL;

	// 읽어 온다.
	lResult = RegQueryValueEx(hKeySub, sValue, NULL, &dwType,(unsigned char*) *pRetBuf, &dwBufSize);
	if(lResult!=ERROR_SUCCESS) goto FAIL;
	nBufSize = dwBufSize;

	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;


}
// 레지스트리에 INT를 저장한다 //
bool SetRegInt(HKEY hKeyRoot, const CString& sKeySub, const CString& sValueName, int nValue)
{
	return SetRegDWORD(hKeyRoot, sKeySub, sValueName, (DWORD)nValue);
}
// 레지스트리에서 INT를 읽어 들인다 //				
bool GetRegInt(HKEY hKeyRoot, const CString& sKeySub,const CString& sValue, int& nResult, int nDefaultValue)
{
	return GetRegDWORD(hKeyRoot, sKeySub, sValue, (DWORD&) nResult, (DWORD)nDefaultValue);
}

// 레지스트리 지우기.
bool  RegDeleteValue(HKEY hKeyRoot, const CString& sKeySub, const CString& sValueName)
{
	LONG		lResult;
	HKEY		hKeySub=NULL;

	lResult = RegOpenKeyEx(hKeyRoot, sKeySub, 0, KEY_ALL_ACCESS, &hKeySub);			// 레지스트리를 연다.
	if(lResult!=ERROR_SUCCESS)	goto FAIL;											// 레지스트리가 존재하지 않으면..
	lResult = RegDeleteValue(hKeySub, sValueName);									// 레지스트리를 지운다.
	if(lResult!=ERROR_SUCCESS) goto FAIL;

	if(hKeySub)	RegCloseKey (hKeySub);
	return true;

FAIL :
	if(hKeySub)	RegCloseKey (hKeySub);
	return false;
}

#define	REG_AUTORUN "Software\\Microsoft\\Windows\\CurrentVersion\\Run"
// 시스템 시작시 실행할 파일 이름을 지정한다. 
bool RegistAutoRun(const CString& sValueName, const CString &sFileName)
{
	if(sFileName=="")
	{
		LONG ret;
		ret = RegDeleteValue(HKEY_CURRENT_USER, CString(REG_AUTORUN), sValueName);
		if(ret==ERROR_SUCCESS) return true;
		return false;
	}
	return SetRegStr(HKEY_CURRENT_USER, REG_AUTORUN, sValueName, sFileName);
}
// 자동실행의 파일 이름을 구한다.
CString	GetAutoRun(const CString& sKeyName)
{
	CString sResult;
	GetRegStr(HKEY_CURRENT_USER, REG_AUTORUN, sKeyName, sResult);
	return sResult;
}

//##################################################################################################################
//##	시스템 정보 얻기
//##################################################################################################################

// 프로그램이 시작된 디렉토리..명
CString  GetThisPath()
{
	char szFileName[MAX_PATH];
	char szPathName[MAX_PATH];
	char szDrive[_MAX_DRIVE];
	::GetModuleFileName(NULL,szFileName,MAX_PATH);
	_splitpath(szFileName,szDrive,szPathName,NULL,NULL);
	return CString(szDrive)+szPathName;
}
// 나 자신의 파일 이름
CString	GetThisFileName()
{
	char szFileName[MAX_PATH];
	::GetModuleFileName(NULL,szFileName,MAX_PATH);
	return szFileName;
}

// 명령행 파라메터 구하기.
int GetCommandLineList(CStringArray &sa)
{
	CString				sCmd = GetCommandLine();
	CString				sToken;
	int					nFrom=0, nTo=0;

	sa.RemoveAll();


	if(sCmd[0]=='\"')
	{
		nTo=sCmd.Find("\"", 1);
		if(nTo!=-1)
		{
			sToken = sCmd.Mid(nFrom+1, nTo-nFrom-1);
			sa.Add(sToken);
			nFrom = nTo+1;
		}
	}

	for(;;)
	{
		nTo = sCmd.Find(" ", nFrom);
		if(nTo==-1)
			sToken = sCmd.Mid(nFrom);
		else
			sToken = sCmd.Mid(nFrom, nTo-nFrom);
		if(sToken!="")
			sa.Add(sToken);
		nFrom = nTo+1;
		if(nTo==-1) break;
	}

	return sa.GetSize();
}


// 스크린 폭 얻기
int		GetScreenWidth()  { return GetSystemMetrics(SM_CXFULLSCREEN); }

// 스크린 높이 얻기 
int		GetScreenHeight() { return GetSystemMetrics(SM_CYFULLSCREEN); }

// 작업 영역 얻기
void GetWorkArea(CRect& r)
{
	SystemParametersInfo(SPI_GETWORKAREA,NULL,&r,0);
}

// 텍스트 문자열의 폭 얻기
int TextWidth (HDC hDC, const char* szText)
{
	SIZE	size ;
	if(szText==NULL) return 0;
    GetTextExtentPoint32 (hDC, szText, lstrlen (szText), &size) ;
    return  size.cx ;
}

// 텍스트 문자의 높이 얻기
int FontHeight (HDC hDC,	BOOL bIncludeLeading)
{
	TEXTMETRIC     tm ;
    GetTextMetrics (hDC, &tm) ;
    if (bIncludeLeading)
    	return (tm.tmHeight + tm.tmExternalLeading) ;
    else
    	return (tm.tmHeight) ;
}

// 문단의 높이를 리턴 ( 주의 : \n 만을 LF 로 본다 )
int ParagraphHeight(HDC hDC, BOOL bIncludeLeading, const char* pText)
{
	int nCount=1;
	int i;
	for(i=0;i<(int)strlen(pText);i++)
	{
		if(pText[i]=='\n') nCount++;
	}

	return FontHeight(hDC, bIncludeLeading)*nCount;
}

// 문단의 폭을 리턴한다 ( 주의 : \n 만을 LF 로 본다 )
int ParagraphWidth(HDC hDC, const char* szText)
{
	char* strTmp;
	char* token;
	char seps[]   = "\n";
	int		nMaxWidth=0;
	int		nWidth;

	// 소스 스트링을 복사한다..
	strTmp = new char[strlen(szText)+1];
	memcpy(strTmp, szText, strlen(szText)+1);

	// 라인별로 높이를 계산한다.
	token = strtok( strTmp, seps );
	while( token != NULL )
	{
		nWidth = TextWidth(hDC, token);
		nMaxWidth = max(nWidth, nMaxWidth);		// 제일 폭이 넓은 라인을 찾는다.
		token = strtok( NULL, seps );
	}

	// 메모리 해제..
	delete strTmp;
	return nMaxWidth;
}

#pragma comment(lib,"version.lib")
// 파일의 버전 정보를 얻어옴.
bool GetFileVersion(char *szFileName, DWORD* verMS, DWORD* verLS)
{
   DWORD   dwVerInfoSize;
   LPSTR   lpVersion;
   DWORD   dwVerHnd = 0;
   BOOL    bRetCode;
   bool    bResult;

   bResult = false;
   dwVerInfoSize = GetFileVersionInfoSize(szFileName, &dwVerHnd);
   if (dwVerInfoSize)
   {
      LPSTR   lpstrVffInfo;
      UINT    uVersionLen;

      lpstrVffInfo  = (char *)GlobalAllocPtr(GMEM_MOVEABLE, dwVerInfoSize);
      GetFileVersionInfo(szFileName, dwVerHnd, dwVerInfoSize, lpstrVffInfo);

      bRetCode = VerQueryValue((LPVOID)lpstrVffInfo, TEXT("\\"), (LPVOID *)&lpVersion, &uVersionLen);
      if (bRetCode)
      {
         VS_FIXEDFILEINFO *pfi;
         pfi = (VS_FIXEDFILEINFO*)lpVersion;
         *verMS = pfi->dwFileVersionMS;
         *verLS = pfi->dwFileVersionLS;
         bResult = true;
      }
      GlobalFreePtr(lpstrVffInfo);
   }
   return bResult;
}

// 파일의 버전 정보를 얻어옴.
bool GetFileVersion(CString sFileName, CString& sFileVersion)
{
	//__int64 ver;
	DWORD	verMS, verLS;
	if(GetFileVersion((char*)(const char*)sFileName, &verMS, &verLS)==false) return false;
	sFileVersion.Format("%d, %d, %d, %d", 
		//ver>>48, (ver>>32)&0xff, (ver>>16)&0xff, (ver)&0xff);
		verMS >> 16, verMS&0xffff,
		verLS >> 16, verLS&0xffff);
	return true;
}

// 하드 여유공간 정보구하기(KB)
BOOL GetDiskFreeSpace(char cDrive, int& nResult)
{
	CString s;
	s.Format("%c:\\", cDrive);
	return GetDiskFreeSpace(s, nResult);
}
// 하드 여유공간 정보구하기(KB)
BOOL GetDiskFreeSpace(const char* s, int& nResult)
{
    DWORD spc,bps,fc,tc;
    if (GetDiskFreeSpace(s,&spc,&bps,&fc,&tc))
    {
      DWORD r;
      DWORD v=0x7fffffff;
      r=bps*spc*(fc>>10);
      if (!r) r=(bps*spc*fc)>>10;
      if (r > v) r=v;
      nResult=(int)r;
	  return TRUE;
    }
	return FALSE;
}

// OS VERSION 얻어오기
OS_VERSION CheckOSVerion(CString* psServiceRelease)
{
	OSVERSIONINFO osv;
	osv.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
	if (GetVersionEx(&osv))
	{
		if(psServiceRelease)
			*psServiceRelease = osv.szCSDVersion;	// note: szCSDVersion =  service pack  release  
		switch(osv.dwPlatformId)
		{
		case VER_PLATFORM_WIN32s:					//Win32s on Windows 3.1.
			return OSVER_31;
			
		case VER_PLATFORM_WIN32_WINDOWS:			//WIN32 on 95 or 98
			if (osv.dwMinorVersion == 0)			return OSVER_95;
			else if(osv.dwMinorVersion == 10)		return OSVER_98;
			else if(osv.dwMinorVersion == 90)		return OSVER_ME;
			break;

		case VER_PLATFORM_WIN32_NT:					//Win32 on Windows NT.
			if(osv.dwMajorVersion == 3) 			return OSVER_NT351;
			else if(osv.dwMajorVersion == 4) 		return OSVER_NT4;
			else if(osv.dwMajorVersion == 5)
			{
				if(osv.dwMinorVersion == 0)			return OSVER_2000;
				else if(osv.dwMinorVersion == 1)	return OSVER_XP;
			}
		}
		return OSVER_UNKNOWN;
	}
	return OSVER_ERROR;
}

// OS VERSION 을 스트링으로 바꾸기
CString OSVer2Str(OS_VERSION nOS)
{
	switch(nOS)
	{
	case OSVER_UNKNOWN	: return "Unknown OS";
	case OSVER_31		: return "Windows 3.1";
	case OSVER_95		: return "Windows 95";
	case OSVER_98		: return "Windows 98";
	case OSVER_ME		: return "Windows ME";
	case OSVER_NT351	: return "Windows NT 3.51";
	case OSVER_NT4		: return "Windows NT 4";
	case OSVER_2000		: return "Windows 2000";
	case OSVER_XP		: return "Windows XP";
	};
	ASSERT(0);
	return "";
}

//##################################################################################################################
//##	문자열 처리 함수
//##################################################################################################################

// 안전한 스트링 카피 함수.
void	StringCopy(char* szDst, const char* szSrc, int nCount)
{
	if(szDst==NULL) {ASSERT(0);return;}					// 오류 발생.
	if(szSrc==NULL) {memset(szDst,0,nCount);return;}
//	memcpy(szDst, szSrc, nCount-1);
	strncpy(szDst, szSrc, nCount-1);
	szDst[nCount-1] = NULL;
}
void	StringCopy(BYTE* szDst, const char* szSrc, int nCount)
{
	StringCopy((char*)szDst, (const char*)szSrc, nCount);
}
void	StringCopy(char* szDst, const BYTE* szSrc, int nCount)
{
	StringCopy((char*)szDst, (const char*)szSrc, nCount);
}
void	StringCopy(BYTE* szDst, const BYTE* szSrc, int nCount)
{
	StringCopy((char*)szDst, (const char*)szSrc, nCount);
}
void	StringCopy(CString& sDst,BYTE* szSrc, int nCount)
{
	*(szSrc + nCount-1) = NULL;
	sDst = (const char*)szSrc;
}
// 파일의 확장자를 바꾼다.
CString	ChangeFileExtString(const CString& sFileName, const CString& sNewExt)
{
	return CString(GetFileNameFromPathWithOutExt(sFileName))+"."+sNewExt;
}

// URL의 확장자를 바꾼다.
CString	ChangeURLExtString(const CString& sPathName, const CString& sNewExt)
{
	int				nPos;
	CString			sResult;
	nPos= sPathName.ReverseFind('.');
	if(nPos==-1) return sResult;						// .을 찾을 수 없다..
	sResult = sPathName.Left(nPos)+"."+sNewExt;
	return sResult;
}

// 이메일 주소를 id와 서버 주소로 분리한다 
bool SplitEmailAddress(const char* szEmail, CString &sID, CString &sAddress)
{
	CString sEmail = szEmail;
	int			nPos;
	nPos = sEmail.Find('@',0);
	if(nPos<0) return false;							// @ 없는 문자열일 경우
	if(nPos==0) return false;							// @ 로 시작되는 문자열일 경우
	if(nPos==sEmail.GetLength()-1) return false;		// @ 로 끝나는 문자열일 경우

	sID = sEmail.Left(nPos);
	sAddress = sEmail.Right(sEmail.GetLength()-nPos);
	return true;
}


//##################################################################################################################
//##	시간 처리 함수
//##################################################################################################################

// 현재 시간을 포맷을 통해서 가져오기
CString	GetTimeString(const CString& sFormat )
{
	CTime			t = CTime::GetCurrentTime();
	CString	sTime;
	sTime = t.Format(sFormat);
	return sTime;
}

// 현재 시간을 초까지 문자열로 리턴
CString GetCurrentTimeString()
{
	CTime			t = CTime::GetCurrentTime();
	CString	sTime;
	sTime = t.Format("%Y년%m월%d일 %H시%M분%S초");
	return sTime;
}

// 날짜&시간을 문자열로 리턴
CString GetCurrentHourString()
{
	CTime			t = CTime::GetCurrentTime();
	CString	sTime;
	sTime = t.Format("%Y년%m월%d일 %H시");
	return sTime;
}

// 현재 시간을 분까지 문자열로 리턴
CString GetCurrentMinString()
{
	CTime			t = CTime::GetCurrentTime();
	CString	sTime;
	sTime = t.Format("%Y년%m월%d일 %H시%M분");
	return sTime;
}

// 오늘 날짜를 문자열로 리턴
CString GetCurrentDayString()
{
	CTime			t = CTime::GetCurrentTime();
	CString	sTime;
	sTime = t.Format("%Y년%m월%d일");
	return sTime;
}

// 지금 현재의 년도 
int		GetThisYear()
{
	CTime			t = CTime::GetCurrentTime();
	return t.GetYear();
}


//##################################################################################################################
//##	디버깅
//##################################################################################################################

#define MAX_MSGBUFFER_SIZE		(4096*16)
// 윈도우에 스트링 메시지 보내는 함수 - 내부함수 
void SendStringMessageToDebugWindow(const char* szWndNameToRecv, const char* szMessage)
{
#ifndef _FINAL_RELEASE
    HWND    hWndRecv;
    hWndRecv = FindWindow(NULL,szWndNameToRecv);
    if(hWndRecv==NULL) return;

    COPYDATASTRUCT cd;
    cd.dwData = 0;
    cd.cbData = strlen(szMessage)+1;
    cd.lpData = (void*)szMessage;

    SendMessage(hWndRecv,WM_COPYDATA, NULL, (LPARAM) (PCOPYDATASTRUCT)&cd);
#endif
}

// 디버깅 정보를 RECV 에게 보낸다.
void _cdecl gebug(const char* str, ...)
{
#ifndef _FINAL_RELEASE
	CString sBuffer;
	va_list args;

	va_start(args, str);
    sBuffer.FormatV(str, args);
	va_end(args);
	SendStringMessageToDebugWindow("XXXXXXXXXX", sBuffer);
/*	static TCHAR szBuffer[MAX_MSGBUFFER_SIZE];
	va_list args;

	va_start(args, str);
	memset(szBuffer,0,sizeof(szBuffer));
    vsprintf(szBuffer, str, args);
	va_end(args);
	SendStringMessageToDebugWindow("RECV", szBuffer);*/
#endif
}
// 디버깅 정보를 보낼 상대를 지정해서 보낸다.
void _cdecl gebugto(const char* szWndNameToRecv, const char* str, ...)
{
#ifndef _FINAL_RELEASE
	static TCHAR szBuffer[MAX_MSGBUFFER_SIZE];
	va_list args;

	va_start(args, str);
	memset(szBuffer,0,sizeof(szBuffer));
    vsprintf(szBuffer, str, args);
	va_end(args);
	SendStringMessageToDebugWindow(szWndNameToRecv, szBuffer);
#endif
}

// 기본 메시지 파일 이름.
CString g_sGEBUGFileName = DEFAULT_GEBUG_FILENAME;

// 메시지 파일 이름 지정하기
void SetDefaultGEBUGFileName(const char* sFIleName)						
{
	g_sGEBUGFileName = sFIleName;
}

// 디버그 메시지를 파일로 저장
void _cdecl fgebug(const char* str, ...)
{
#ifndef _FINAL_RELEASE
	static TCHAR szBuffer[MAX_MSGBUFFER_SIZE];
	va_list args;
	va_start(args, str);
	memset(szBuffer,0,sizeof(szBuffer));
    vsprintf(szBuffer, str, args);
	va_end(args);
	fgebugto(g_sGEBUGFileName,szBuffer);
#endif
}

// 특정 파일에 메시지 남기기
void _cdecl fgebugto(const char* szFileName, const char* str, ...)
{
#ifndef _FINAL_RELEASE
	FILE*	fp;
	fp = fopen(GetThisPath()+szFileName,"a+");
	if(fp==NULL)
	{
		gebug("파일 열기 실패:%s", szFileName);
		return;
	}

	static TCHAR szBuffer[MAX_MSGBUFFER_SIZE];
	va_list args;

	va_start(args, str);
	memset(szBuffer,0,sizeof(szBuffer));
    vsprintf(szBuffer, str, args);
	va_end(args);

	fprintf(fp, GetCurrentTimeString()+" : ");
	strcat(szBuffer,"\n");
	fprintf(fp,szBuffer);
	fclose(fp);
#endif
}

// 로그파일에 기록하기
void _cdecl flog(const char* str, ...)
{

#ifndef _FINAL_RELEASE
	CString sLogFileName;
	CString	sStr;
	sLogFileName = GetCurrentDayString()+".log";

	static TCHAR szBuffer[MAX_MSGBUFFER_SIZE];
	va_list args;
	va_start(args, str);
	memset(szBuffer,0,sizeof(szBuffer));
    vsprintf(szBuffer, str, args);
	va_end(args);
	sStr = GetCurrentTimeString()+" : "+szBuffer;

	FILE* fp;
	fp = fopen(GetThisPath()+sLogFileName, "a+");
	if(fp==NULL) return;
	fprintf(fp,"%s\n", sStr);
	fclose(fp);
#endif
}

BOOL QueryShellFolders(char *name, char *out)
{
	HKEY hKey;
	if ( RegOpenKeyEx(HKEY_CURRENT_USER,"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Shell Folders",0,KEY_READ,&hKey) == ERROR_SUCCESS)
	{
		ULONG l = 1024;
		ULONG t=REG_SZ;
		RegQueryValueEx(hKey,name,NULL,&t,(BYTE*)out,&l );
		RegCloseKey(hKey);
		return TRUE;
	}
	return FALSE;
}



//##################################################################################################################
//##	메뉴 관련
//##################################################################################################################
//----코드구루에서 가져옴-------------------------------------------------------
// Function: MergeMenu
// @func		Merges two menus
// @rdesc	bool: false = error occured
// @parm	CMenu * 			|	pMenuDestination		|	[in, retval] destination menu handle    
// @parm	const CMenu *	|	pMenuAdd					|	[in]	menu to merge
// @parmopt bool			|	bTopLevel	|	false	|	[in]	indicator for special top level behavior
//	@comm	This function calles itself recursivley. If bTopLevel is set to true,
//			we append popups at top level or we insert before "Window" or "Help".
//------------------------------------------------------------------------------
bool MergeMenu(CMenu * pMenuDestination, const CMenu * pMenuAdd, bool bTopLevel /* =false */)
{
	int iMenuAddItemCount	= pMenuAdd->GetMenuItemCount();					// get the number menu items in the menus
	int iMenuDestItemCount	= pMenuDestination->GetMenuItemCount();
	
	if (iMenuAddItemCount == 0)	return true;								// if there are no items return
	
	// if we are not at top level and the destination menu is not empty
	// -> we append a seperator
	if (!bTopLevel && iMenuDestItemCount > 0) pMenuDestination->AppendMenu(MF_SEPARATOR);
	
	for(int iLoop = 0; iLoop < iMenuAddItemCount; iLoop++)					// iterate through the top level of <pMenuAdd>
	{
		CString sMenuAddString;												// get the menu string from the add menu
		pMenuAdd->GetMenuString(iLoop, sMenuAddString, MF_BYPOSITION);

		CMenu* pSubMenu = pMenuAdd->GetSubMenu(iLoop);						// try to get the submenu of the current menu item
		
		if (!pSubMenu)														// check if we have a sub menu
		{
			// normal menu item
			// read the source and append at the destination
			UINT nState	 = pMenuAdd->GetMenuState(iLoop, MF_BYPOSITION);
			UINT nItemID = pMenuAdd->GetMenuItemID(iLoop);
			if (pMenuDestination->AppendMenu(nState, nItemID, sMenuAddString))
			{
				iMenuDestItemCount++;										// menu item added, don't forget to correct the item count
			}
			else
			{
				TRACE("MergeMenu: AppendMenu failed!\n");
				return false;
			}
		}
		else																// create or insert a new popup menu item
		{	
			int iInsertPosDefault = -1;										// default insert pos is like ap
			if(bTopLevel)													// if we are at top level merge into existing popups rather than creating new ones
			{
				ASSERT(sMenuAddString != "&?" && sMenuAddString != "?");
				CString csAdd(sMenuAddString);
				csAdd.Remove('&');	// for comparison of menu items supress '&'
				bool bAdded = false;

				for( int iLoop1 = 0; iLoop1 < iMenuDestItemCount; iLoop1++ )	// try to find existing popup
				{
					CString sDest;												// get the menu string from the destination menu
					pMenuDestination->GetMenuString(iLoop1, sDest, MF_BYPOSITION);
					sDest.Remove('&'); // for a better compare (s.a.)
					
					if (csAdd == sDest)
					{
						// we got a hit -> merge the two popups
						// try to get the submenu of the desired destination menu item
						CMenu* pSubMenuDest = pMenuDestination->GetSubMenu(iLoop1);
						
						if (pSubMenuDest)
						{
							// merge the popup recursivly and continue with outer for loop
							if (!MergeMenu(pSubMenuDest, pSubMenu))
								return false;
							bAdded = true;
							break;
						}
					}

					// alternativ insert before <Window> or <Help>
					if (iInsertPosDefault == -1 && (sDest == "Window" || sDest == "?" || sDest == "Help"))
					{
						iInsertPosDefault = iLoop1;
					}
				} // for (iLoop1)
				if (bAdded)
				{
					continue;											// menu added, so go on with loop over pMenuAdd's top level
				}
			} // if (bTopLevel)

			if( iInsertPosDefault == -1 )								// if the top level search did not find a position append the menu
			{
				iInsertPosDefault = pMenuDestination->GetMenuItemCount();
			}
			
			CMenu NewPopupMenu;											// create a new popup and insert before <Window> or <Help>
			if (!NewPopupMenu.CreatePopupMenu())
			{
				TRACE("MergeMenu: CreatePopupMenu failed!\n");
				return false;
			}
			
			if (!MergeMenu(&NewPopupMenu, pSubMenu)) return false;		// merge the new popup recursivly
			
			HMENU hNewMenu = NewPopupMenu.GetSafeHmenu();				// insert the new popup menu into the destination menu
			if (pMenuDestination->InsertMenu(iInsertPosDefault,
				MF_BYPOSITION | MF_POPUP | MF_ENABLED, 
				(UINT)hNewMenu, sMenuAddString ))
			{
				iMenuDestItemCount++;									// don't forget to correct the item count
			}
			else
			{
				TRACE("MergeMenu: InsertMenu failed!\n");
				return false;
			}

			// don't destroy the new menu		
			NewPopupMenu.Detach();
		} // if (pSubMenu)
	} // for (iLoop)
	return true;
}

//##################################################################################################################
//##	트레이 관련
//##################################################################################################################
UINT	WM_TRAY_NOTIFY	= ::RegisterWindowMessage("WM_TRAY_NOTIFY");
BOOL TrayMessage(HWND hWnd, DWORD dwMessage, UINT uID, HICON hIcon, LPSTR lpszTip)
{
   BOOL res;
   NOTIFYICONDATA tnd;
   
   tnd.cbSize      = sizeof(NOTIFYICONDATA);
   tnd.hWnd        = hWnd;
   tnd.uID         = uID;
   tnd.uFlags      = NIF_MESSAGE|NIF_ICON|NIF_TIP;
   tnd.uCallbackMessage   = WM_TRAY_NOTIFY;
   tnd.hIcon       = hIcon;

   if (lpszTip)
      lstrcpyn(tnd.szTip, lpszTip, sizeof(tnd.szTip));
   else
      tnd.szTip[0] = '\0';
   
   res = Shell_NotifyIcon(dwMessage, &tnd);
   
   if (hIcon)
      DestroyIcon(hIcon);
   
   return res;
}

void Tray_NotifyDelete(HWND hWnd, UINT uID)
{
   TrayMessage(hWnd, NIM_DELETE, uID, NULL, NULL);
}

void Tray_NotifyAdd(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip)
{
   TrayMessage(hWnd, NIM_ADD, uID, hIcon, lpszTip);
}

void Tray_NotifyModify(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip)
{
   TrayMessage(hWnd, NIM_MODIFY, uID, hIcon, lpszTip);
}


//##################################################################################################################
//##	네트웍  관련
//##################################################################################################################

// 현재 내 컴퓨터의 아이피를 얻는다 
#ifdef _GUTIL_NETWORK
#include "winsock.h"
CString GetIP(bool bHexType, bool bUseDot)
{
	char	szHostName[1024];
	CString	strHostName;
	CString	strIPAddr = "";

	if (! ::gethostname(szHostName, 1024))
	{
		strHostName = szHostName;
		struct hostent*	pHost;

		pHost = gethostbyname(szHostName);
		if (pHost != NULL)
		{
			if (pHost->h_addr_list[0])
			{
				BYTE				bAddr[4];
				CString				strIP;
				bAddr[0] = (BYTE) pHost->h_addr_list[0][0];
				bAddr[1] = (BYTE) pHost->h_addr_list[0][1];
				bAddr[2] = (BYTE) pHost->h_addr_list[0][2];
				bAddr[3] = (BYTE) pHost->h_addr_list[0][3];

				if(bHexType)
				{
					if(bUseDot)
					{
						strIPAddr.Format("%02x.%02x.%02x.%02x", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
					}
					else
					{
						strIPAddr.Format("%02x%02x%02x%02x", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
					}
				}
				else
				{
					strIPAddr.Format("%d.%d.%d.%d", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
				}
			}
		}
	}
	return strIPAddr;
}
int	GetIPList(CStringArray& ipList, bool bHexType, bool bUseDot)
{
	char	szHostName[1024];
	CString	strHostName;
	CString	strIPAddr = "";
	int		i;

	if (! ::gethostname(szHostName, 1024))
	{
		strHostName = szHostName;
		struct hostent*	pHost;

		pHost = gethostbyname(szHostName);
		if (pHost != NULL)
		{
			for(i=0;i<255;i++)
			{
				if (pHost->h_addr_list[i])
				{
					BYTE				bAddr[4];
					CString				strIP;
					bAddr[0] = (BYTE) pHost->h_addr_list[i][0];
					bAddr[1] = (BYTE) pHost->h_addr_list[i][1];
					bAddr[2] = (BYTE) pHost->h_addr_list[i][2];
					bAddr[3] = (BYTE) pHost->h_addr_list[i][3];

					if(bHexType)
					{
						if(bUseDot)
							strIPAddr.Format("%02x.%02x.%02x.%02x", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
						else
							strIPAddr.Format("%02x%02x%02x%02x", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
					}
					else
						strIPAddr.Format("%d.%d.%d.%d", bAddr[0], bAddr[1], bAddr[2], bAddr[3]);
					ipList.Add(strIPAddr);
				}
				else
					break;
			}
		}
	}
	return ipList.GetSize();
}
#endif

//##################################################################################################################
//##	프로토콜 파싱 관련
//##################################################################################################################
// ESCAPING 하기 - 보내기전 
CString Escaping(CString sStr)
{
	/*
	CString sRet = sStr;
	sRet.Replace("`","``");	sRet.Replace(",","`.");	sRet.Replace("\n","`n");	sRet.Replace("\r","`r");
	return sRet;
	*/
	if(sStr.Find(DELIM_RECORD)>=0||sStr.Find(DELIM_PACKET)>=0||sStr.Find('\r')>=0||sStr.Find('\"')==0)				// 내부에 DELIMITER 가 있을 경우.
	{
		sStr.Replace("\\","\\\\");			/* \       -> \\   */
		sStr.Replace("\"","\\\"");			/* "       -> \"   */
		sStr.Replace("\n","\\n");			/* cr      -> \n   */
		sStr.Replace("\r","\\r");			/* lf      -> \r   */

		sStr = "\"" + sStr + "\"";			// 따옴표로 묶기 
	}
	return sStr;
}

// DEESCAPING 하기 - 받은후
CString DeEscaping(CString sStr)
{
	CString		sMsg;
	LineParser	parser;
	int			ret;

	ret = parser.Parse(sStr.GetBuffer(0));
	if(ret<0) { ASSERT(0); return "";}
	if(ret>1) { ASSERT(0); return "";}

	return parser.GetTokenStr(0);
}

// 메시지 파싱하기 - 실패시 <0 리턴 
int ParsingMessage(CString sMsg, CStringArray& msgArray, bool bUsingWhiteSpace)
{
	LineParser	parser;
	int			ret,i;
	if(bUsingWhiteSpace)
		ret = parser.Parse2(sMsg.GetBuffer(0));	// white space 를 이용한 parsing..
	else
		ret = parser.Parse(sMsg.GetBuffer(0));
	if(ret<0) { ASSERT(0); msgArray.RemoveAll(); return ret;}
	msgArray.SetSize(parser.GetNumTokens());
	for(i=0;i<parser.GetNumTokens();i++)
	{
		msgArray[i] = parser.GetTokenStr(i);
	}
	return parser.GetNumTokens();
}

//##################################################################################################################
//##	파싱 유틸 클래스 모음
//##################################################################################################################
GrowBuf::GrowBuf()	{ m_alloc=m_used=0; m_s=NULL; }
GrowBuf::~GrowBuf() { free(m_s); }
int GrowBuf::Add(void *data, int len) 
{ 
	if (len<=0) return 0;Resize(m_used+len);memcpy((char*)m_s+m_used-len,data,len);return m_used-len;
}
void GrowBuf::Resize(int newlen)
{
	m_used=newlen;
	if (newlen > m_alloc)
	{
		m_alloc = newlen + 32768;
		m_s = realloc(m_s, m_alloc);
		if (!m_s)
		{
			flog("Internal compiler error #12345: GrowBuf realloc(%d) failed.\n",m_alloc);
			ASSERT(0); return;
		}
	}
	if (!m_used && m_alloc > 65535) // only free if you resize to 0 and we're > 64k
	{
		m_alloc=0;free(m_s);m_s=NULL;
	}
}
int GrowBuf::Getlen() { return m_used; }
void* GrowBuf::Get() { return m_s; }
////////////////////////////////////////////////////////////////////////////////
StringList::StringList() { }
StringList::~StringList() { }

int StringList::Add(char *str, int case_sensitive)
{
	int a=Find(str,case_sensitive);
	if (a >= 0 && case_sensitive!=-1) return a;
	return gr.Add(str,strlen(str)+1);
}
// use 2 for case sensitive end-of-string matches too
int StringList::Find(char *str, int case_sensitive) // returns -1 if not found
{
	char *s=(char*)gr.Get();
	int ml=gr.Getlen();
	int offs=0;
	while (offs < ml)
	{
		if ((case_sensitive && !strcmp(s+offs,str)) ||(!case_sensitive && !stricmp(s+offs,str)))
		{
			return offs;
		}
		if (case_sensitive==2 && 
			strlen(str) < strlen(s+offs) &&  // check for end of string
			!strcmp(s+offs+strlen(s+offs)-strlen(str),str))
		{
			return offs+strlen(s+offs)-strlen(str);
		}
		offs+=strlen(s+offs)+1;
	}
	return -1;
}
char* StringList::Get() { return (char*)gr.Get(); }
int StringList::Getlen() { return gr.Getlen(); }
////////////////////////////////////////////////////////////////////////////////
LineParser::LineParser()   {m_nTokenNum=m_eat=0;m_tokens=0;}
LineParser::~LineParser()  {FreeTokens();}
int LineParser::Parse(char *line) // returns -1 on error, ',' 로 분리하여 파싱..
{
	m_eat = 0;
	FreeTokens();
	int n;
	n = Doline(line);
	if (n) return n;
	if (m_nTokenNum) 
	{
		m_tokens=(char**)malloc(sizeof(char*)*m_nTokenNum);
		n = Doline(line);
		if (n) {FreeTokens();return -1;}
	}
	return 0;
}
int LineParser::Parse2(char *line) // returns -1 on error   white space 로 분리하여 파싱.
{
	m_eat = 0;
	FreeTokens();
	int n;
	n = Doline2(line);
	if (n) return n;
	if (m_nTokenNum) 
	{
		m_tokens=(char**)malloc(sizeof(char*)*m_nTokenNum);
		n = Doline2(line);
		if (n) {FreeTokens();return -1;}
	}
	return 0;
}
int LineParser::GetNumTokens() 
{ 
	return m_nTokenNum-m_eat; 
}
void LineParser::EatToken() 
{ 
	m_eat++; 
}
char* LineParser::GetTokenStr(int token) 
{ 
	token+=m_eat;
	if (token < 0 || token >= m_nTokenNum) return "";
	return m_tokens[token]; 
}
void LineParser::FreeTokens()
{
	if (m_tokens)
	{
		int x;
		for (x = 0; x < m_nTokenNum; x ++)
			free(m_tokens[x]);
		free(m_tokens);
	}
	m_tokens=0;
	m_nTokenNum=0;
}
int LineParser::Doline(char *line)
{
	int		state=0;
	int		count=0;
	int		size=0;
	bool	escaping=false;
	m_nTokenNum=0;
	while(*line)
	{
		if(m_nTokenNum && *line == ',')line++;			// 맨처음은 아니고, 쉼표로만 문장이 있을때.. 예) hello,,,

		count = 0;
		state = 0;
		size = 0;
		escaping=false;
		if(*line=='\"') state=1;
		if(state) line++;
		while(*line)
		{
			if(state==1)								// 따옴표가 있을 경우만 escaping 처리를 한다.
			{
				if(*line=='\\'&&*(line+1)=='\"') { line+=2; count+=2; size++; escaping=true; continue; }	// \" 처리
				if(*line=='\\'&&*(line+1)=='\\') { line+=2; count+=2; size++; escaping=true; continue; }	// \\ 처리 
				if(*line=='\\'&&*(line+1)=='r')  { line+=2; count+=2; size++; escaping=true; continue; }	// \r 처리 
				if(*line=='\\'&&*(line+1)=='n')  { line+=2; count+=2; size++; escaping=true; continue; }	// \n 처리 
			}
			if(state==1&&*line=='\"') 
			{
				if(*(line+1)!=NULL &&*(line+1)!=',') 
					return -1;			// 따옴표 다음이 문장의 마지막도 아니고, 쉼표도 아닐경우..
				break;
			}
			if(state==0&&*line==',') break;
			line++;
			count++;
			size++;
		}
		if(m_tokens)
		{
			if(escaping)
			{
				char* s;
				char* d;
				int	i;
				m_tokens[m_nTokenNum] = (char*)malloc(size+1);
				d = m_tokens[m_nTokenNum];
				s = line-count;
				for(i=0;i<size;i++)
				{
					if(*s=='\\' && *(s+1)=='\\') { *d = '\\'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='r')  { *d = '\r'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='n')  { *d = '\n'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='\"') { *d = '\"'; s+=2; d++; continue; }
					*d = *s;
					s++; d++;
				}
				*d = 0;
			}
			else
			{
				m_tokens[m_nTokenNum] = (char*)malloc(count+1);
				strncpy(m_tokens[m_nTokenNum], line-count,count);
				m_tokens[m_nTokenNum][count]=0;
			}
		}
		m_nTokenNum++;
		if(state)							// 괄호가 열린채로 문장이 끝났을 경우..
		{
			if(*line) line++;				// 이런 케이스는 없다..
			else return -1;					// 에러..
		}
	}
	return 0;
}
bool LineParser::IsWhiteSpace(char c)
{
	if(c==' '||c=='\t'||c=='\n'||c=='\r') return true;
	return false;
}
int LineParser::Doline2(char *line)
{
	int		state=0;
	int		count=0;
	int		size=0;
	bool	escaping=false;
	m_nTokenNum=0;
	while(*line)
	{
		while (IsWhiteSpace(*line)) line++;	// white space 제거.

		count = 0;
		state = 0;
		size = 0;
		escaping=false;
		if(*line=='\"') state=1;
		if(state) line++;
		while(*line)
		{
			if(state==1)								// 따옴표가 있을 경우만 escaping 처리를 한다.
			{
				if(*line=='\\'&&*(line+1)=='\"') { line+=2; count+=2; size++; escaping=true; continue; }	// \" 처리
				if(*line=='\\'&&*(line+1)=='\\') { line+=2; count+=2; size++; escaping=true; continue; }	// \\ 처리 
				if(*line=='\\'&&*(line+1)=='r')  { line+=2; count+=2; size++; escaping=true; continue; }	// \r 처리 
				if(*line=='\\'&&*(line+1)=='n')  { line+=2; count+=2; size++; escaping=true; continue; }	// \n 처리 
			}
			if(state==1&&*line=='\"') 
			{
				//if(*(line+1)!=NULL &&*(line+1)!=',') 
				//	return -1;			// 따옴표 다음이 문장의 마지막도 아니고, 쉼표도 아닐경우..
				break;
			}
			if(state==0&& (IsWhiteSpace(*line))) break;
			line++;
			count++;
			size++;
		}
		if(m_tokens)
		{
			if(escaping)
			{
				char* s;
				char* d;
				int	i;
				m_tokens[m_nTokenNum] = (char*)malloc(size+1);
				d = m_tokens[m_nTokenNum];
				s = line-count;
				for(i=0;i<size;i++)
				{
					if(*s=='\\' && *(s+1)=='\\') { *d = '\\'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='r')  { *d = '\r'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='n')  { *d = '\n'; s+=2; d++; continue; }
					if(*s=='\\' && *(s+1)=='\"') { *d = '\"'; s+=2; d++; continue; }
					*d = *s;
					s++; d++;
				}
				*d = 0;
			}
			else
			{
				m_tokens[m_nTokenNum] = (char*)malloc(count+1);
				strncpy(m_tokens[m_nTokenNum], line-count,count);
				m_tokens[m_nTokenNum][count]=0;
			}
		}
		m_nTokenNum++;
		if(state)							// 괄호가 열린채로 문장이 끝났을 경우..
		{
			if(*line) line++;				// 이런 케이스는 없다..
			else return -1;					// 에러..
		}
		while (IsWhiteSpace(*line)) line++;	// white space 제거.
	}
	return 0;
}

//##################################################################################################################
//##	기타
//##################################################################################################################
// 메시지 펌핑
void PumpUntilEmpty(HWND hWnd)
{
	MSG msg;
	while(::PeekMessage(&msg, hWnd, 0, 0, PM_REMOVE))
	{
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
}

// 특정 메시지가 올때까지 펌핑
bool PumpUntilMessage(UINT nMessage)
{
	MSG msg;
	while(::PeekMessage(&msg, NULL, 0, 0, PM_NOREMOVE))
	{
		if(msg.message==nMessage) return true;
		::PeekMessage(&msg, NULL, 0, 0, PM_REMOVE);
		::TranslateMessage(&msg);
		::DispatchMessage(&msg);
	}
	return false;
}

// 윈도우의 보여주기 상태 
UINT GetWindowShowState(HWND hWnd)
{
	WINDOWPLACEMENT wndpl;
	wndpl.length = sizeof(WINDOWPLACEMENT);
	if(GetWindowPlacement(hWnd, &wndpl)==FALSE) {ASSERT(0); return SW_HIDE;}
	return wndpl.showCmd;
}

bool StrRetToBuf(STRRET* pstr, LPITEMIDLIST pidl, LPTSTR* ppszBuf)
{
    LPMALLOC pMalloc = NULL;
	HRESULT hr = S_OK;
	int cch;
	LPSTR strOffset;
	bool bRet = true;

    SHGetMalloc(&pMalloc);

	*ppszBuf = NULL;  // Assume failure

	switch (pstr->uType)
	{
		case STRRET_WSTR: 
			cch = WideCharToMultiByte(CP_OEMCP, 0, pstr->pOleStr, -1, NULL, 0, NULL, NULL); 
			*ppszBuf = (LPTSTR)pMalloc->Alloc(cch * sizeof(TCHAR));

			if (*ppszBuf != NULL)
				WideCharToMultiByte(CP_OEMCP, 0, pstr->pOleStr, -1, *ppszBuf, cch, NULL, NULL); 
			else
				bRet = false;
			break;

		case STRRET_OFFSET: 
			strOffset = (((char *) pidl) + pstr->uOffset);

			cch = strlen(strOffset) + 1; // NULL terminator
			*ppszBuf = (LPTSTR)pMalloc->Alloc(cch * sizeof(TCHAR));

			if (*ppszBuf != NULL)
				strcpy(*ppszBuf, strOffset);
			else
				bRet = false;
			break;

		case STRRET_CSTR: 
			cch = strlen(pstr->cStr) + 1; // NULL terminator
			*ppszBuf = (LPTSTR)pMalloc->Alloc(cch * sizeof(TCHAR));

			if (*ppszBuf != NULL)
				strcpy(*ppszBuf, pstr->cStr);
			else
				bRet = false;

			break;
	}

	pMalloc->Release();

	return bRet;
}
