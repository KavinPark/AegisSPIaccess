#pragma once

#include "libMPSSE_spi.h"
#include "ftd2xx.h"



class Utils
{
public:
	Utils(void);
	~Utils(void);

	static void Convert_Str2Hex(CString s, uint8 *pBuf); 
	static CString Convert_Hex2Str(uint32 nHex);
};

