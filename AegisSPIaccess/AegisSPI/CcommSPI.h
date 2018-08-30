
#pragma once
#include "libMPSSE_spi.h"
#include "ftd2xx.h"


class CcommSPI
{
private:
	FT_HANDLE mobjSPIhandle;
//TODO:  [PBK : Check] 
	ChannelConfig _ConstchannelConfig;
		//TODO: [PBK : CHECK]
	uint32 _ConstTransOptions;


	void SPIFailExit(char cstring[])
	{
		AfxMessageBox(_T("No USB to Serial devices(FTXX seriese) found\n"));
		exit(1);
	}

public:
	CcommSPI(void);
	~CcommSPI(void);

	FT_STATUS Open(void);
	FT_STATUS Close(void);

	uint32 WriteData(uint8 *pBuffer, uint32 nSizeToTransfer);
	uint32 ReadData(uint8 *pBuffer, uint32 nSizeToTransfer);
	
	uint32 WriteReadData(uint8 *pTxBuffer, uint8 *pRxBuffer, uint32 nSizeToTransfer);


};
