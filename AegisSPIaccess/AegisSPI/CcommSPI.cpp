#include "StdAfx.h"
#include "CcommSPI.h"
#include "ftd2xx.h"
#include "libMPSSE_spi.h"


CcommSPI::CcommSPI(void)
{
	mobjSPIhandle = NULL;
}


CcommSPI::~CcommSPI(void)
{
}


FT_STATUS 
CcommSPI::Open(void)
{
	Init_libMPSSE();	

	FT_STATUS status;

	// Check how many MPSSE channels are available
/*
	uint32 channelCount = 0;
	status = SPI_GetNumChannels(&channelCount);
	if (status != FT_OK)
		SPIFailExit("Error while checking the number of available MPSSE channels.");
	else if (channelCount < 1)
		SPIFailExit("Error: no MPSSE channels are available.");
*/

	// Open the MPSSE channel (get the handle for it)
	status = SPI_OpenChannel(0/*numOfChannel*/, &mobjSPIhandle);
	if (status != FT_OK)
		SPIFailExit("Error while OpenChannel.");

//	status = FT_ResetDevice(mobjSPIhandle);

	// Init the channel (configure it)
	_ConstchannelConfig.ClockRate = 1000000;	// 1000KHz
	_ConstchannelConfig.configOptions = SPI_CONFIG_OPTION_MODE0 | SPI_CONFIG_OPTION_CS_DBUS3 | SPI_CONFIG_OPTION_CS_ACTIVELOW;
	_ConstchannelConfig.LatencyTimer = 2;
	_ConstchannelConfig.Pin = 0;	// All Input But SPI pins are Overwritten Automatically

	_ConstTransOptions = SPI_TRANSFER_OPTIONS_SIZE_IN_BYTES | SPI_TRANSFER_OPTIONS_CHIPSELECT_ENABLE | SPI_TRANSFER_OPTIONS_CHIPSELECT_DISABLE;

	status = SPI_InitChannel(mobjSPIhandle, &_ConstchannelConfig);
	if (status != FT_OK)
		SPIFailExit("Error while Initializing.");

	return FT_OK;
}



FT_STATUS 
CcommSPI::Close(void)
{
	FT_STATUS status;

	// Close
	status = SPI_CloseChannel(mobjSPIhandle); 
	Cleanup_libMPSSE();

	return FT_OK;
}

// return : 己傍茄 transferCount 
uint32 
CcommSPI::WriteData(uint8 *pBuffer, uint32 nSizeToTransfer)
{
	FT_STATUS status;
	uint32 nSizeTransfered;
	status = SPI_Write(mobjSPIhandle, pBuffer, nSizeToTransfer, &nSizeTransfered, _ConstTransOptions); 
//TODO: PBK Fail 贸府
	return nSizeTransfered;
}

// return : Read茄 Byte荐
uint32 
CcommSPI::ReadData(uint8 *pBuffer, uint32 nSizeToTransfer)
{
	FT_STATUS status;
	uint32 nSizeTransfered;

	status = SPI_Read(mobjSPIhandle, pBuffer, nSizeToTransfer, &nSizeTransfered, _ConstTransOptions); 
//TODO: PBK Fail 贸府
	return nSizeTransfered;
}

// return : Read茄 Byte 荐
uint32 
CcommSPI::WriteReadData(uint8 *pTxBuffer, uint8 *pRxBuffer, uint32 nSizeToTransfer)
{
	FT_STATUS status;
	uint32 nSizeTransfered;

	status = SPI_ReadWrite(mobjSPIhandle, pRxBuffer, pTxBuffer, nSizeToTransfer, &nSizeTransfered, _ConstTransOptions); 
//TODO: PBK Fail 贸府
	return nSizeTransfered;
}


