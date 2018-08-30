
#include "StdAfx.h"
#include "Utils.h"
#include "ftd2xx.h"

static Utils s_objUtils;

Utils::Utils(void)
{
}


Utils::~Utils(void)
{
}


void 
Utils::Convert_Str2Hex(CString s, uint8 *pBuf)
{
	int nNum;

	char hex[20];
	int nLength = s.GetLength();
	WideCharToMultiByte(CP_ACP, 0, (LPCWSTR)s, nLength, hex, nLength, NULL, NULL);

	// strtol()은 0x7fffffff까지만 된다.  그래서 만든다.

//	hex[2..9] 변환
	nNum = strtoul(hex, NULL, 16);

	pBuf[0] = (char)(nNum >> 24);
	pBuf[1] = (char)(nNum >> 16);
	pBuf[2] = (char)(nNum >> 8);
	pBuf[3] = (char)(nNum >> 0);
}

CString 
Utils::Convert_Hex2Str(uint32 nHex)
{
	char display_str[100];
	sprintf_s(display_str, sizeof(display_str), "0x%08x", nHex);

	CString  out(display_str);

	return out;
}

