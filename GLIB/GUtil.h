/*//////////////////////////////////////////////////////////////////////////////////////////////////////

	HISTROY
		- 2000/03/17 : 제작 시작..

//////////////////////////////////////////////////////////////////////////////////////////////////////*/

#pragma once

//##################################################################################################################
//##	매크로들
//##################################################################################################################
#define	ZERO_MEMORY(x)			memset(&x,0,sizeof(x))
#define OPEN_URL(xxxx)			ShellExecute(NULL, "open", xxxx, NULL, NULL, SW_SHOW);
inline	void SET_EVENT(HANDLE hEvent) { if(hEvent) SetEvent(hEvent); }
#define SETICON() 				{HICON hIcon;hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);SetIcon(hIcon, FALSE);}
#define SETICONX(x) 			{HICON hIcon;hIcon = AfxGetApp()->LoadIcon(x);SetIcon(hIcon, FALSE);}
#define BEEP()					MessageBeep(MB_ICONASTERISK);

//##################################################################################################################
//##	CRC + ENC&DEC 관련
//##################################################################################################################
DWORD	crc32( DWORD crc, const BYTE *buf, UINT len);
bool	CheckFileCRC(const char* szFileName, DWORD &dwResult,int nStartPos=0);		// 파일의 CRC 를 체크하고 리턴한다.
#define MAX_PASSENCDEC_BUFSIZE	100													// 암호화된 패스워드의 결과 버퍼 사이즈
void	EncPass(char* sDest,const char* sPassWord);									// sPassWord 를 sDest[MAX_PASSENCDEC_BUFSIZE] 로 인크립션 시킨다.
char*	DecPass(char* sSrc);														// 인크립션된 binary 데이타를 복구시켜준다.

//##################################################################################################################
//##	파일 관련
//##################################################################################################################
bool	GetFileNameFromURL(const CString& sURL, CString& sResult);					// URL 에서 파일 이름을 뽑아낸다.
char* 	GetFileNameFromPath(const char* szPathName );								// PATH 명에서 파일 이름을 뽑는다.
char*	GetDriveNameFromPath(const char* szPathName);								// PATH 명에서 드라이브 (EX) "C:") 를 뽑아낸다.
char*	GetDirNameFromPath(const char* szPathName);									// PATH 명에서 디렉토리를 뽑아낸다.
CString GetLastDirFromPath(const CString& sPathName);								// 전체 폴더중 마지막 디렉토리 이름 구하기
CString GetParentPath(const CString& sPathName);									// PARENT 폴더 구하기 - 루트(EX) C:\)의 PARENT 일 경우는 ""를 리턴한다 
char* 	GetFileNameFromPathWithOutExt(const char* szPathName );						// PATH 명에서 확장자를 제외한 파일명을 뽑는다.
char*	GetExtFromPath(const char* szPathName);										// PATH 명에서 확장자 만을 뽑아내기.
int		GetFileSize(const CString& sPathName);										// 파일 사이즈 구하기
bool	IsFolder(const char* szPathName);											// 폴더 여부 판단
bool	IsFile(const char* szPathName);												// 파일 여부 판단 
CString GetDirectory(HWND hWndParent, const char* szRoot, const char* szTitle);		// 사용자로부터 폴더를 입력 받는다.
void	DeleteFolder(CString sFolder);												// 폴더를 지운다.
CString AddPath(CString sLeft, CString sRight);										// 두개의 폴더(혹은 폴더+파일)을 더한다.
bool	DigPath(CString sFolderPath);												// 폴더를 뚫는다.

//##################################################################################################################
//##	INI / REGISTRY
//##################################################################################################################
#define	DEFAULT_INI_FILE_NAME	"SETTING.INI"										// 기본 INI 파일 이름
/*		이 코드들은 적당한 기회를 봐서 지운다..
void	SetPrivateRegAppName(const CString& sAppName);
void 	SetPrivateRegInt(CString sKeyName, DWORD nValue);							// REGISTRY 에 INT 값 저장
DWORD 	GetPrivateRegInt(CString sKeyName, DWORD nDefaultValue=0);					// REGISTRY 에서 INT 값 읽기
void 	SetPrivateRegBin(CString sKeyName, void* pData, int nSize);					// REGISTRY 에 BINARY 값 저장
bool 	GetPrivateRegBin(CString sKeyName, void* pData, int nSize);					// REGISTRY 에서 BINARY 값 읽기
*/
CString	GetIniStr(const char* szSection, const char* szKey, 						// INI 파일에서 STRING 값 읽기
				CString sIniFileName=DEFAULT_INI_FILE_NAME,
				CString sDefaultValue="");										
void	SetIniStr(const char* szSection, const char* szKey, const char* szValue,	// INI 파일에 STRING 값 저장
				CString sIniFileName=DEFAULT_INI_FILE_NAME);	
int		GetIniInt(const char* szSection, const char* szKey, int nDefault,			// INI 파일에서 INT 값 읽기
				CString sIniFileName=DEFAULT_INI_FILE_NAME);	
void	SetIniInt(const char* szSection, const char* szKey, int nValue,				// INI 파일에 INT 값 저장
				CString sIniFileName=DEFAULT_INI_FILE_NAME);	
void	GetIniKeyList(CStringList& slKey, CString sSection,							// INI 파일의 SECTION 의 모든 KEY 를 리스트로 얻기
				CString sIniFileName=DEFAULT_INI_FILE_NAME);

bool	GetRegStr(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue,		// 디폴트 값을 가지고 레지스트리에서 스트링을 읽어 들인다.
				  CString& sResult, CString sDefaultValue);
bool	GetRegStr(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에서 스트링을 읽어 들인다 //
				const CString& sValue, CString& sResult);	
bool	GetRegStr(const CString& sKeyRoot, const CString& sKeySub,					// 레지스트리에서 스트링을 읽어 들인다 //
				const CString& sValue, CString& sResult);
bool	GetRegStr(const CString& sKey, CString& sResult);							// 통짜로 레지스트리에서 스트링을 읽어 들인다. 예:"HKEY_LOCAL_MACHINE\\SOFTWARE\\Blizzard Entertainment\\Starcraft\\Program"
bool	SetRegStr(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에 스트링을 저장한다 //
				const CString& sValueName, const CString& sValue);
bool	SetRegStr(const CString& sKeyRoot, const CString& sKeySub,					// 레지스트리에 스트링을 저장한다 //
				const CString& sValueName, const CString& sValue);
bool	SetRegStr(const CString& sKey, const CString& sValue);						// 통짜로 레지스트리에 스트링을 저장한다 예:"HKEY_LOCAL_MACHINE\\SOFTWARE\\Blizzard Entertainment\\Starcraft\\Program"
bool	SetRegDWORD(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에 DWORD를 저장한다 //
				const CString& sValueName, DWORD dwValue);
bool	GetRegDWORD(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에서 DWORD를 읽어 들인다 //
				const CString& sValue, DWORD& dwResult, DWORD dwDefaultValue=0);
bool	SetRegBin(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에 BINARY를 저장한다 //
				const CString& sValueName, void* pData, int nDataSize);
bool	GetRegBin(HKEY hKeyRoot, const CString& sKeySub, const CString& sValue,		// 레지스트리에서 BINARY를 읽어 들인다 - 반드시 free() 로 해제해 주어야 한다.. //
				BYTE** pRetBuf, int& nBufSize);
bool	SetRegInt(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에 INT를 저장한다 //
				const CString& sValueName, int nValue);
bool	GetRegInt(HKEY hKeyRoot, const CString& sKeySub,							// 레지스트리에서 INT를 읽어 들인다 //
				const CString& sValue, int& nResult, int nDefaultValue=0);
bool	RegDeleteValue(HKEY hKeyRoot, const CString& sKeySub, 
				const CString& sValueName);											// 레지스트리 값(Value) 지우기.
bool	RegistAutoRun(const CString& sValueName, const CString &sFileName);			// 시스템 시작시 실행할 파일 이름을 지정한다. 
CString	GetAutoRun(const CString& sKeyName);										// 자동실행의 파일 이름을 구한다.


//##################################################################################################################
//##	시스템 정보 얻기
//##################################################################################################################
CString	GetThisPath();															// 프로그램이 시작된 디렉토리..명
CString	GetThisFileName();														// 나 자신..
int		GetCommandLineList(CStringArray &sa);									// 명령행 파라메터 구하기.
int		GetScreenWidth() ;														// 스크린 폭 얻기
int		GetScreenHeight() ;														// 스크린 높이 얻기 
void	GetWorkArea(CRect& r);													// 작업 영역 얻기
int		TextWidth (HDC hDC, const char* szText);								// 텍스트 문자열의 폭 얻기
int 	FontHeight (HDC hDC,      BOOL bIncludeLeading=TRUE);					// 텍스트 문자의 높이 얻기
int		ParagraphHeight(HDC hDC, BOOL bIncludeLeading, const char* pText);		// 문단의 높이를 리턴 ( 주의 : \n 만을 LF 로 본다 )
int		ParagraphWidth(HDC hdC, const char* szText);							// 문단의 폭을 리턴한다 ( 주의 : \n 만을 LF 로 본다 )
bool	GetFileVersion(char *szFileName, DWORD* verMS, DWORD* verLS);			// 파일의 버전 정보를 얻어옴.
bool	GetFileVersion(CString sFileName, CString& sFileVersion);				// 파일의 버전 정보를 얻어옴.
BOOL	GetDiskFreeSpace(char cDrive, int& nResult);							// 하드 여유공간 정보구하기(KB)
BOOL	GetDiskFreeSpace(const char* szDrive, int& nResult);					// 하드 여유공간 정보구하기(KB)
enum OS_VERSION
{
	OSVER_ERROR,	OSVER_UNKNOWN,
	OSVER_31,		OSVER_95,	OSVER_98,	OSVER_ME,
	OSVER_NT351,	OSVER_NT4,	OSVER_2000,	OSVER_XP,
};
OS_VERSION	CheckOSVerion(CString* psServiceRelease=NULL);						// OS VERSION 얻어오기
CString		OSVer2Str(OS_VERSION nOS);											// OS VERSION 을 스트링으로 바꾸기


//##################################################################################################################
//##	문자열 처리 함수
//##################################################################################################################
void	StringCopy(char* szDst, const char* szSrc, int nCount);
void	StringCopy(BYTE* szDst, const char* szSrc, int nCount);
void	StringCopy(char* szDst, const BYTE* szSrc, int nCount);
void	StringCopy(BYTE* szDst, const BYTE* szSrc, int nCount);
void	StringCopy(CString& sDst,BYTE* szSrc, int nCount);
CString	ChangeFileExtString(const CString& sFileName, const CString& sNewExt);		// 파일의 확장자를 바꾼다.
CString	ChangeURLExtString(const CString& sPathName, const CString& sNewExt);		// URL의 확장자를 바꾼다.
bool	SplitEmailAddress(const char* szEmail, CString &sID, CString &sAddress);	// 이메일 주소를 id와 서버 주소로 분리한다 

//##################################################################################################################
//##	시간 처리 함수
//##################################################################################################################
CString	GetTimeString(const CString& sFormat );										// 현재 시간을 포맷을 통해서 가져오기
CString	GetCurrentTimeString();														// 현재 시간을 초까지 문자열로 리턴
CString	GetCurrentHourString();														// 날짜&시간을 문자열로 리턴
CString	GetCurrentMinString();														// 현재 시간을 분까지 문자열로 리턴
CString	GetCurrentDayString();														// 오늘 날짜를 문자열로 리턴
int		GetThisYear();																// 지금 현재의 년도 

//##################################################################################################################
//##	디버깅
//##################################################################################################################
#define		DEFAULT_GEBUG_FILENAME	"GEBUG.txt"										// 기본 로그 파일 이름
void _cdecl gebug(const char* str, ...);											// 디버깅 정보를 RECV 에게 보낸다.
void _cdecl gebugto(const char* szWndNameToRecv, const char* str, ...);				// 디버깅 정보를 보낼 상대를 지정해서 보낸다.
void		SetDefaultGEBUGFileName(const char* sFIleName);							// 디버그 메시지 파일 이름 지정하기
void _cdecl fgebug(const char* str, ...);											// 디버그 메시지를 파일로 저장
void _cdecl fgebugto(const char* szFileName, const char* str, ...);					// 특정 파일에 메시지 남기기
void _cdecl flog(const char* str, ...);												// 로그파일에 기록하기
#define		err(x)	gebug("$$ERROR$$ : [%s,%d] - %s",__FILE__,__LINE__,x);			// 에러 매크로
#define		ferr(x)	flog ("$$ERROR$$ : [%s,%d] - %s",__FILE__,__LINE__,x);			// 에러 매크로

//##################################################################################################################
//##	쉘 관련
//##################################################################################################################
HRESULT		CreateShortcut(LPCSTR pszShortcutFile, LPCTSTR pszLink, LPSTR pszDesc);	// 단축 아이콘을 만든다.
HRESULT		SHCreateSystemShortcut(LPCTSTR szLnkFile, int nFolder, LPCTSTR szFile);	// 단축 아이콘 만들기..
BOOL		QueryShellFolders(char *name, char *out);

//##################################################################################################################
//##	메뉴 관련
//##################################################################################################################
bool		MergeMenu(CMenu * pMenuDestination, 
				const CMenu * pMenuAdd, bool bTopLevel = false);					// 두개의 메뉴를 합친다.

//##################################################################################################################
//##	트레이 관련
//##################################################################################################################
extern UINT WM_TRAY_NOTIFY;
void		Tray_NotifyDelete(HWND hWnd, UINT uID);
void		Tray_NotifyAdd(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip);
void		Tray_NotifyModify(HWND hWnd, UINT uID, HICON hIcon, LPSTR lpszTip);

//##################################################################################################################
//##	네트웍  관련
//##################################################################################################################
#ifdef _GUTIL_NETWORK
CString		GetIP(bool bHexType=false, bool bUseDot=true);							// 현재 컴퓨터의 아이피 구하기 
int			GetIPList(CStringArray& ipList, bool bHexType=false, bool bUseDot=true);
#endif

//##################################################################################################################
//##	프로토콜 파싱 관련
//##################################################################################################################
#define		DELIM_PACKET					'\n'
#define		DELIM_RECORD					','
int			ParsingMessage(CString sMsg, CStringArray& msgArray, bool bUsingWhiteSpace=false);	// DELIM으로 나누어진 STRING ARRAY 구하기
CString		Escaping(CString sStr);													// DELIM을 ESCAPING
CString		DeEscaping(CString sStr);												// DELIM을 DEESCAPING

//##################################################################################################################
//##	기타
//##################################################################################################################
void PumpUntilEmpty(HWND hWnd=NULL);												// 메시지 펌핑
bool PumpUntilMessage(UINT nMessage);												// 특정 메시지가 올때까지 펌핑
UINT GetWindowShowState(HWND hWnd);													// 윈도우의 보여주기 상태 
bool StrRetToBuf(LPSTRRET pstr, LPITEMIDLIST pidl, LPTSTR* ppszBuf);				// STRRET를 스트링으로 변환

//##################################################################################################################
//##	파싱 유틸 클래스 모음
//##################################################################################################################
class GrowBuf
{
public:
    GrowBuf();
    ~GrowBuf();
    int Add(void *data, int len);
    void Resize(int newlen);
    int Getlen();
    void *Get();
private:
    void *m_s;
    int m_alloc;
    int m_used;
};

class StringList
{
public:
	StringList();
	~StringList();
	int Add(char *str, int case_sensitive);
	// use 2 for case sensitive end-of-string matches too
	int Find(char *str, int case_sensitive); // returns -1 if not found
	char *Get();
	int Getlen();
private:
	GrowBuf gr;
};

class LineParser {
public:
    LineParser();
    ~LineParser();
    int Parse(char *line);
    int Parse2(char *line);
    int GetNumTokens();
    void EatToken();
    char* GetTokenStr(int token);
private:
    void FreeTokens();
    int Doline(char *line);
    int Doline2(char *line);
	bool IsWhiteSpace(char c);
    int m_eat;
    int m_nTokenNum;
    char **m_tokens;
};
