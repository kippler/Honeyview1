#pragma once

#pragma pack(1)

struct PCOFileInfo
{
	int		pos;
	int		size;
	char	dummy;
};

#define	LEN_TITLE	100
#define MAX_INFO	600

struct PCOHead
{
	int			dummy;
	char		title_len;
	char		title[LEN_TITLE];
	int			image_count;
	int			nHeaderSize;
	PCOFileInfo	info[MAX_INFO];
};

#pragma pack(4)


class CPco  
{
public:
	CPco();
	virtual ~CPco();
	bool			ReadHeader(CString sFileName);
	bool			GetFileInfo(int nFilePos, PCOFileInfo& info);


public :
	CString			m_sFileName;
	PCOHead			m_head;

};


/*
void PrintHead(Head& head)
{
	printf("n0    : %d\n", head.n0);
	printf("len   : %d\n", head.title_len);
	printf("title : [%s]\n", head.title);
	printf("count : %d\n", head.image_count);
	printf("head size : %d\n", head.nHeaderSize);
	int sum=0;
	for(int i=0;i<head.image_count;i++)
	{
		sum += head.info[i].size;
		printf("%3d  size:%8x   pos:%8x   dummy:%d\n", i, head.info[i].size, head.info[i].pos+head.nHeaderSize, head.info[i].dummy);
	}
}
*/

/*
void ReadHead(FILE* fp, Head& head)
{
	char* p = (char*)&head;

	memset(p, 0, sizeof(Head));

	fread(p,	1, sizeof(int), fp);							// c0 읽기
	p+= sizeof(int);
	fread(p,	1, sizeof(char),  fp);							// 제목 len 읽기
	p+= sizeof(char);
	fread(p,	1, head.title_len,  fp);						// 제목  읽기
	p+= LEN_TITLE;
	fread(p,	1, sizeof(int),  fp);							// image count;
	p+= sizeof(int);

	head.nHeaderSize = 4+1+head.title_len+4			// n0, title_len, title, image_count, 
		+ head.image_count * sizeof(Info);
	p+= sizeof(int);

	for(int i=0;i<head.image_count;i++)
	{
		fread(p,	1, sizeof(Info),  fp);						// info
		p+= sizeof(Info);
	}

}
*/
/*

void Parse(FILE* fpIn, Head &head)
{
	FILE* fpOut;
	char* buf;
	long pos;
	int  size;
	char szName[MAX_PATH];

	for(int i=0;i<head.image_count;i++)
	{
		size = head.info[i].size;
		buf = (char*)malloc(size);
		pos = head.info[i].pos+head.nHeaderSize;

		fseek(fpIn, pos, SEEK_SET);
		if(i==0)
			sprintf(szName, "%04d.jpg", i);
		else
			sprintf(szName, "%04d.gif", i);

		printf("writing file : %s, size : %d\n", szName, size);

		fpOut = fopen(szName, "wb");
		fread(buf, 1, size, fpIn);
		fwrite(buf, 1, size, fpOut);
		fclose(fpOut);
		free(buf);
	}

}
*/