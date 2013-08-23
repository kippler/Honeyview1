#include "stdafx.h"
#include "Pco.h"
#include "GUTIl.h"

CPco::CPco()
{

}

CPco::~CPco()
{

}


bool CPco::ReadHeader(CString sFileName)
{
	FILE* fp;

	m_sFileName = sFileName;

	fp = fopen(m_sFileName, "rb");

	if(fp==NULL) 
		return false;


	char* p = (char*)&m_head;

	memset(p, 0, sizeof(PCOHead));

	fread(p,	1, sizeof(int), fp);							// dummy 읽기
	p+= sizeof(int);
	fread(p,	1, sizeof(char),  fp);							// 제목 len 읽기
	p+= sizeof(char);
	fread(p,	1, m_head.title_len,  fp);						// 제목  읽기
	p+= LEN_TITLE;
	fread(p,	1, sizeof(int),  fp);							// image count;
	p+= sizeof(int);

	m_head.nHeaderSize = 4+1+m_head.title_len+4			// n0, title_len, title, image_count, 
		+ m_head.image_count * sizeof(PCOFileInfo);
	p+= sizeof(int);

	if(m_head.image_count >=MAX_INFO)							// 그림이 너무 많거나 아마도 손상된 파일
	{
		fclose(fp);
		return false;
	}

	for(int i=0;i<m_head.image_count;i++)
	{
		fread(p,	1, sizeof(PCOFileInfo),  fp);						// info
		m_head.info[i].pos += m_head.nHeaderSize;						// 절대 위치 보정
		p+= sizeof(PCOFileInfo);
	}

	fclose(fp);

	if(GetFileSize(m_sFileName) <= min(m_head.nHeaderSize, 4+1+4))
		return false;											// 손상된 헤더..

	return true;

}


bool CPco::GetFileInfo(int nFilePos, PCOFileInfo& info)
{
	if(nFilePos<0 || nFilePos >= m_head.image_count) return false;

	info = m_head.info[nFilePos];
	return true;

}