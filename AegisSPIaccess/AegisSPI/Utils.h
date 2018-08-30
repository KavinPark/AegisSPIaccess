#pragma once


#include "ftd2xx.h"

/******************************************************************************/
/*								Type defines								  */
/******************************************************************************/

typedef unsigned char   uint8;
typedef unsigned short  uint16;
typedef unsigned long long uint64;

typedef signed char   int8;
typedef signed short  int16;
typedef signed long long int64;

#ifndef __cplusplus
typedef unsigned char	bool;
#endif

/*20111025: 64bit linux doesn't work is uint32 is unsigned long*/
	typedef unsigned int   uint32;
	typedef signed int   int32;


class Utils
{
public:
	Utils(void);
	~Utils(void);

	static void Convert_Str2Hex(CString s, uint8 *pBuf); 
	static CString Convert_Hex2Str(uint32 nHex);
};

