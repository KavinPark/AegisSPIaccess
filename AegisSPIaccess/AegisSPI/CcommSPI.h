
#pragma once
#include "ftd2xx.h"
#include "Utils.h"

// [PBK CAUTION] 바꾸면 동작 안됨. FT DEVICE가 오동작한다.
#define PBK_OP_RISING_OUT	0	// Falling Out => Device latch on Rising Edge
								// Rising In => Device output on Falling Edge


//declare for BAD command 
const BYTE AA_ECHO_CMD_1 = '\xAA'; 
const BYTE AB_ECHO_CMD_2 = '\xAB'; 
const BYTE BAD_COMMAND_RESPONSE = '\xFA'; 

//declare for MPSSE command  : <AN_108> 8 page
const BYTE FTcmd_MSB_BYTE_RISING_OUT = 0x10;
const BYTE FTcmd_MSB_BYTE_FALLING_OUT = 0x11;
const BYTE FTcmd_MSB_BYTE_RISING_IN = 0x20;
const BYTE FTcmd_MSB_BYTE_FALLING_IN = 0x24;
const BYTE FTcmd_MSB_BYTE_FALLING_OUT_RISING_IN = 0x31;
const BYTE FTcmd_MSB_BYTE_RISING_OUT_FALLING_IN = 0x34;

const BYTE FTcmd_MSB_BIT_RISING_OUT = 0x12;
const BYTE FTcmd_MSB_BIT_FALLING_OUT = 0x13;
const BYTE FTcmd_MSB_BIT_RISING_IN = 0x22;
const BYTE FTcmd_MSB_BIT_FALLING_IN = 0x26;
const BYTE FTcmd_MSB_BIT_FALLING_OUT_RISING_IN = 0x33;
const BYTE FTcmd_MSB_BIT_RISING_OUT_FALLING_IN = 0x36;

// Declare for MPSSE command : Pin : <AN-108> 17 page
const BYTE FTcmd_PIN_SET = 0x80;
 // bit Position : bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK
 // bit value : "1":high, "0":low
 // bit Direction : "1" out, "0":in


// PBK
#if (PBK_OP_RISING_OUT == 0)	// 
// Half duplex
const BYTE FTcmd_AEGIS_DATA_WRITE = FTcmd_MSB_BYTE_FALLING_OUT;
const BYTE FTcmd_AEGIS_DATA_READ = FTcmd_MSB_BYTE_RISING_IN;
	// Full duplex
const BYTE FTcmd_AEGIS_DATA_WRITE_READ = FTcmd_MSB_BYTE_FALLING_OUT_RISING_IN;
#else
	// Half duplex
const BYTE FTcmd_AEGIS_DATA_WRITE = FTcmd_MSB_BYTE_RISING_OUT;
const BYTE FTcmd_AEGIS_DATA_READ = FTcmd_MSB_BYTE_FALLING_IN;
	// Full duplex
const BYTE FTcmd_AEGIS_DATA_WRITE_READ = FTcmd_MSB_BYTE_RISING_OUT_FALLING_IN;
#endif

// <AN_108, p20>
const DWORD FTconfig_ClockDivisor = 10;//29;   //Value of clock divisor, SCL Frequency = 60/((1+29)*2) (MHz) = 1Mhz 


class CcommSPI
{
private:
	FT_HANDLE mobjSPIhandle;

	// hold, shadow : cmd, data 모두 사용
	BYTE OutputBuffer[512];  	//Buffer to hold MPSSE commands and data to be sent to FT2232H 
	BYTE InputBuffer[512];		//Buffer to hold Data bytes to be read from FT2232H 

	DWORD dwNumBytesToSend;  //Index of output buffer 
	DWORD dwNumBytesSent;
	DWORD dwNumBytesRead;
	DWORD dwNumInputBuffer; 
	BYTE ByteDataRead; 


	bool SPI_Initial(void);
	void SPI_PurgeReceiveBuffer(void);
	void SPI_CSEnable(void);
	void SPI_CSDisable(void);

	uint32 SPI_TransferBytesHalfDuplex(uint32 nByteToSend, uint8 *pBuffToSend, uint32 nByteToReceive, uint8 *pBuffToRead);
	uint32 SPI_TransferBytesFullDuplex(uint32 nByteToTransfer, uint32 nByteToSend, uint8 *pBuffToSend, uint32 nByteToReceive, uint8 *pBuffToRead);

public:
	CcommSPI(void);
	~CcommSPI(void);

	void FailExit(char cstring[])
	{
		AfxMessageBox(_T("No USB to Serial devices(FTXX seriese) found\n"));
		exit(1);
	}


	FT_STATUS Open(void);
	FT_STATUS Close(void);

	uint32 SendData(uint8 *pSendBuffer, uint32 nByteToSend);
	uint32 ReceiveData(uint8 *pReceiveBuffer, uint32 nByteToReceive);
	uint32 SendReceiveData(uint32 nByteTotalToTransfer, uint8 *pSendBuffer, uint32 nByteToSend, uint8 *pReceiveBuffer, uint32 nByteToReceive);


// KAIST =============================================
	uint32 WriteRegisterData(uint8 *pAddress, uint8 *pData);
	uint32 ReadRegisterData(uint8 *pAddress, uint8 *pData);


};
