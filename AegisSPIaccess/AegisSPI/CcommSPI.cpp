#include "StdAfx.h"
#include "CcommSPI.h"
#include "ftd2xx.h"
#include "Utils.h"


CcommSPI::CcommSPI(void)
{
	mobjSPIhandle = NULL;

	dwNumBytesToSend = 0;  //Index of output buffer 
	dwNumBytesSent = 0;
	dwNumBytesRead = 0;
	dwNumInputBuffer = 0; 

}


CcommSPI::~CcommSPI(void)
{
}

void
CcommSPI::SPI_CSEnable(void)
{
	for(int loop=0;loop<5;loop++) //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us  
	{    
		OutputBuffer[dwNumBytesToSend++] = FTcmd_PIN_SET;	//GPIO command for ADBUS    
		OutputBuffer[dwNumBytesToSend++] = '\x00';//set CS high, MOSI and SCL low    
		OutputBuffer[dwNumBytesToSend++] = '\x0b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK  
	} 	
}

void
CcommSPI::SPI_CSDisable(void)
{
	for(int loop=0;loop<5;loop++) //one 0x80 command can keep 0.2us, do 5 times to stay in this situation for 1us  
	{    
		OutputBuffer[dwNumBytesToSend++] = FTcmd_PIN_SET	;//GPIO command for ADBUS    
		OutputBuffer[dwNumBytesToSend++] = '\x08';//set CS, MOSI and SCL low    
		OutputBuffer[dwNumBytesToSend++] = '\x0b';//bit3:CS, bit2:MISO, bit1:MOSI, bit0:SCK  
	}
}

void
CcommSPI::SPI_PurgeReceiveBuffer(void)
{
	FT_HANDLE ftHandle = mobjSPIhandle;
	FT_STATUS ftStatus;

	ftStatus = FT_GetQueueStatus(mobjSPIhandle, &dwNumInputBuffer);  // Get the number of bytes in the device receive buffer    
	if ((ftStatus == FT_OK) && (dwNumInputBuffer > 0))      
		FT_Read(mobjSPIhandle, InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out all the data from receive buffer 
 
	return;
}


uint32
CcommSPI::SPI_TransferBytesFullDuplex(uint32 nByteToTransfer, uint32 nByteToSend, uint8 *pBuffToSend, uint32 nByteToReceive, uint8 *pBuffToRead)
{
	FT_STATUS ftStatus;

// -----------------------------
// OutputBuffer Packing
	dwNumBytesToSend = 0;
	SPI_CSEnable();

	// Send CMD  
	OutputBuffer[dwNumBytesToSend++] = FTcmd_AEGIS_DATA_WRITE_READ;
	// Send Length : MPSSE는 (Length - 1)을 2Byte로 알려준다.
	OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>((nByteToTransfer-1) & 0xff);		// LSB
	OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>(((nByteToTransfer-1)>>8) & 0xff);	// MSB

	// Send Data fill
	for (uint32 i = 0; i < nByteToTransfer ; i++)
	{
		if (i < nByteToSend)
		{
			OutputBuffer[dwNumBytesToSend++] = pBuffToSend[i];
		} 
		else
		{
			OutputBuffer[dwNumBytesToSend++] = 0;	// Null
		}
	}

	SPI_CSDisable();
// -------------------------------

	//send out MPSSE command to MPSSE engine 
	ftStatus = FT_Write(mobjSPIhandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	

	// Read Data..
	if (nByteToReceive)
	{
		ftStatus = FT_Read(mobjSPIhandle, InputBuffer, nByteToTransfer, &dwNumBytesRead);

		for (uint32 i = 0; i < nByteToReceive; i++)
		{
			pBuffToRead[i] = InputBuffer[i + (nByteToTransfer - nByteToReceive)];
		}
	}
	
	return nByteToTransfer;
}


uint32
CcommSPI::SPI_TransferBytesHalfDuplex(uint32 nByteToSend, uint8 *pBuffToSend, uint32 nByteToReceive, uint8 *pBuffToRead)
{
	FT_STATUS ftStatus;
	
// -----------------------------
// OutputBuffer Packing
	dwNumBytesToSend = 0;
	SPI_CSEnable();

// Send chunk  : BYTE mode Write : Cmd - (2Byte, Length-1) - DataArray
	if (nByteToSend)
	{
		// Send CMD  
		OutputBuffer[dwNumBytesToSend++] = FTcmd_AEGIS_DATA_WRITE;
		// Send Length : MPSSE는 (Length - 1)을 2Byte로 알려준다.
		OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>((nByteToSend-1) & 0xff);		// LSB
		OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>(((nByteToSend-1)>>8) & 0xff);	// MSB
		// Send Data fill
		for (uint32 i = 0; i < nByteToSend ; i++)
		{
			OutputBuffer[dwNumBytesToSend++] = pBuffToSend[i];
		}
	}

	// Receive chunk
	if (nByteToReceive)
	{
		// Receive CMD
		OutputBuffer[dwNumBytesToSend++] = FTcmd_AEGIS_DATA_READ;
		// Receive Length : MPSSE는 (Length - 1)을 2Byte로 알려준다.
		OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>((nByteToReceive-1) & 0xff);		// LSB
		OutputBuffer[dwNumBytesToSend++] = static_cast<BYTE>(((nByteToReceive-1)>>8) & 0xff);	// MSB
	}

	SPI_CSDisable();
// -------------------------------

	//send out MPSSE command to MPSSE engine 
	ftStatus = FT_Write(mobjSPIhandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent);	

	// Read Data..
	if (nByteToReceive)
	{
		ftStatus = FT_Read(mobjSPIhandle, pBuffToRead, nByteToReceive, &dwNumBytesRead);
	}

	return dwNumBytesSent+dwNumBytesRead;
}

bool
CcommSPI::SPI_Initial(void)
{
	FT_HANDLE ftHandle = mobjSPIhandle;
	FT_STATUS ftStatus;

	DWORD dwCount;  ftStatus = FT_ResetDevice(ftHandle);  //Reset USB device  
		//Purge USB receive buffer first by reading out all old data from FT2232H receive buffer  
	ftStatus |= FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);  // Get the number of bytes in the FT2232H receive buffer  
	if ((ftStatus == FT_OK) && (dwNumInputBuffer > 0))   
		ftStatus |= FT_Read(ftHandle, InputBuffer, dwNumInputBuffer, &dwNumBytesRead);   //Read out the data from FT2232H receive buffer  
	
	ftStatus |= FT_SetUSBParameters(ftHandle, 65535, 65535); //Set USB request transfer size  
	ftStatus |= FT_SetChars(ftHandle, false, 0, false, 0);  //Disable event and error characters  
	ftStatus |= FT_SetTimeouts(ftHandle, 3000, 3000);  //Sets the read and write timeouts in 3 sec for the FT2232H  
	ftStatus |= FT_SetLatencyTimer(ftHandle, 1);  //Set the latency timer  
	ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x00);   //Reset controller  
	ftStatus |= FT_SetBitMode(ftHandle, 0x0, 0x02);   //Enable MPSSE mode 
 
	if (ftStatus != FT_OK)  
	{   
		FailExit("Fail on Initialize device! \n");  
		return false;
	}  
	
	Sleep(50); // Wait 50ms for all the USB stuff to complete and work  
	//////////////////////////////////////////////////////////////////  
	// Synchronize the MPSSE interface by sending bad command ＆xAA＊  
	////////////////////////////////////////////////////////////////// 

	dwNumBytesToSend = 0;  
	OutputBuffer[dwNumBytesToSend++] = '\xAA';  //Add BAD command ＆ xAA＊  
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); // Send off the BAD commands  
	dwNumBytesToSend = 0;   //Clear output buffer  
	do {   
		ftStatus = FT_GetQueueStatus(ftHandle, &dwNumInputBuffer);  // Get the number of bytes in the device input buffer  
	} while ((dwNumInputBuffer == 0) && (ftStatus == FT_OK));    //or Timeout    
	bool bCommandEchod = false;  
	ftStatus = FT_Read(ftHandle, InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out the data from input buffer  
	for (dwCount = 0; dwCount < (dwNumBytesRead - 1); dwCount++) //Check if Bad command and echo command received  
	{   
		if ((InputBuffer[dwCount] == BYTE('\xFA')) && (InputBuffer[dwCount+1] == BYTE('\xAA')))   
		{    
			bCommandEchod = true;    
			break;   
		}  
	}  
	if (bCommandEchod == false)   
	{    
		FailExit("fail to synchronize MPSSE with command '0xAA' \n");   
		return false; /*Error, can＊t receive echo command , fail to synchronize MPSSE interface;*/   
	}


	//////////////////////////////////////////////////////////////////  
	// Synchronize the MPSSE interface by sending bad command ＆xAB＊  
	//////////////////////////////////////////////////////////////////  
	dwNumBytesToSend = 0;   //Clear output buffer  
	OutputBuffer[dwNumBytesToSend++] = '\xAB'; //Send BAD command ＆xAB＊  
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); // Send off the BAD commands  
	dwNumBytesToSend = 0;   //Clear output buffer  
	do
	{   
		ftStatus = FT_GetQueueStatus(ftHandle, &dwNumInputBuffer); //Get the number of bytes in the device input buffer  
	} while ((dwNumInputBuffer == 0) && (ftStatus == FT_OK));   //or Timeout  
	bCommandEchod = false; 

	ftStatus = FT_Read(ftHandle, InputBuffer, dwNumInputBuffer, &dwNumBytesRead);  //Read out the data from input buffer  
	for (dwCount = 0;dwCount < (dwNumBytesRead - 1); dwCount++) //Check if Bad command and echo command received  
	{   
		if ((InputBuffer[dwCount] == BYTE('\xFA')) && (InputBuffer[dwCount+1] == BYTE( '\xAB')))   
		{    
			bCommandEchod = true;    
			break;   
		}  
	}  
	if (bCommandEchod == false)    
	{    
		FailExit("fail to synchronize MPSSE with command '0xAB' \n");   
		return false;    /*Error, can't receive echo command , fail to synchronize MPSSE interface;*/
	}

	////////////////////////////////////////////////////////////////////     
	//Configure the MPSSE for SPI communication with EEPROM     
	//////////////////////////////////////////////////////////////////     
	// <AN_108, p23 : Configuration>
	dwNumBytesToSend = 0;   //Clear output buffer    
	OutputBuffer[dwNumBytesToSend++] = '\x8A';  // Disable clock divide by 5 for 60Mhz master clock     
	OutputBuffer[dwNumBytesToSend++] = '\x97';  // turn off adaptive clocking     
	OutputBuffer[dwNumBytesToSend++] = '\x8D';  //disable 3 phase data clock     
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); // Send out the commands     
	
	dwNumBytesToSend = 0;   //Clear output buffer     
// <AN_108, p18>
	// Set Data Bit LowByte
	OutputBuffer[dwNumBytesToSend++] = '\x80';  // Command to set directions of lower 8 pins and force value on bits set as output      
	OutputBuffer[dwNumBytesToSend++] = 0x08;  //Set SDA, SCL high, WP disabled by SK, DO at bit ＆＊, GPIOL0 at bit ＆＊     
	OutputBuffer[dwNumBytesToSend++] = 0x0B; //Set SK,DO,GPIOL0 pins as output with bit ＊＊, other pins as input with bit ＆＊     
				// The SK clock frequency can be worked out by below algorithm with divide by 5 set as off     
				// SK frequency  = 60MHz /((1 +  [(1 +0xValueH*256) OR 0xValueL])*2)     
// <AN_108, p20>
	OutputBuffer[dwNumBytesToSend++] = '\x86';    //Command to set clock divisor     
	OutputBuffer[dwNumBytesToSend++] = BYTE(FTconfig_ClockDivisor & '\xFF'); //Set 0xValueL of clock divisor     
	OutputBuffer[dwNumBytesToSend++] = BYTE(FTconfig_ClockDivisor >> 8); //Set 0xValueH of clock divisor
    ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); // Send out the commands     
	
	dwNumBytesToSend = 0;   //Clear output buffer     
	Sleep(20);  //Delay for a while          
	
	//Turn off loop back in case     
	OutputBuffer[dwNumBytesToSend++] = '\x85';  //Command to turn off loop back of TDI/TDO connection     
	ftStatus = FT_Write(ftHandle, OutputBuffer, dwNumBytesToSend, &dwNumBytesSent); // Send out the commands     
	dwNumBytesToSend = 0;   //Clear output buffer     
	Sleep(30);  //Delay for a while  
	
	//printf("SPI initial successful\n");  
	return true; 
}

FT_STATUS 
CcommSPI::Open(void)
{
	FT_STATUS ftStatus;

	ftStatus = FT_Open(0, &mobjSPIhandle);
	if (ftStatus != FT_OK)
		FailExit("Can't Open device! \n");
	else
	{
		// TODO
	}

	if (SPI_Initial() != true)
	{
		FailExit("SPI Initialize Fail \n");
	}

	SPI_PurgeReceiveBuffer();

	return FT_OK;
}



FT_STATUS 
CcommSPI::Close(void)
{
	FT_STATUS ftStatus;

	// Close
	ftStatus = FT_Close(mobjSPIhandle); 

	return FT_OK;
}


//*************************************************************************//

// Byte 단위, MOSI로 출력
// return : 성공한 transferCount 
uint32 
CcommSPI::SendData(uint8 *pSendBuffer, uint32 nByteToSend)
{
	uint32 nByteSent;

	nByteSent = SPI_TransferBytesHalfDuplex(nByteToSend, pSendBuffer, 0, NULL);
//	nByteSent = SPI_TransferBytesFullDuplex(nByteToSend, nByteToSend, pSendBuffer, 0, NULL);
		
	return nByteSent;
}

// return : Read한 Byte수
uint32 
CcommSPI::ReceiveData(uint8 *pReceiveBuffer, uint32 nByteToReceive)
{
	uint32 nByteSent;

	nByteSent = SPI_TransferBytesHalfDuplex(0, NULL, nByteToReceive, pReceiveBuffer);
//	nByteSent = SPI_TransferBytesFullDuplex(nByteToReceive, 0, NULL, nByteToReceive, pReceiveBuffer);
		
	return nByteSent;
}

// Byte 단위, MOSI로 출력하면서 동시에 MISO로 수신할 수 있다.
// return :
uint32 
CcommSPI::SendReceiveData(uint32 nByteTotalToTransfer, uint8 *pSendBuffer, uint32 nByteToSend, uint8 *pReceiveBuffer, uint32 nByteToReceive)
{
	uint32 nByteSent;
	uint32 nByteTotal = nByteToSend + nByteToReceive;

	if (nByteTotalToTransfer == nByteTotal)
	{
		nByteSent = SPI_TransferBytesHalfDuplex(nByteToSend, pSendBuffer, nByteToReceive, pReceiveBuffer);
	//	nByteSent = SPI_TransferBytesFullDuplex(nByteToalToTransfer, nByteToSend, pSendBuffer, nByteToReceive, pReceiveBuffer);
	}
	else if (nByteTotalToTransfer < nByteTotal)
	{
		nByteSent = SPI_TransferBytesFullDuplex(nByteTotalToTransfer, nByteToSend, pSendBuffer, nByteToReceive, pReceiveBuffer);
	} 
	else
	{
		// Error
	}
		
	return nByteSent;
}

//*************************************************************************//

// Kaist dependent
uint32
CcommSPI::WriteRegisterData(uint8 *pAddress, uint8 *pData)
{
	uint32 nByteSent;
	uint8 SendData[8];

	// Address 4B
	for (int i = 0; i < 4; i++)
	{
		SendData[0+i] = pAddress[i];
	}
	// Data 4B
	for (int i = 0; i < 4; i++)
	{
		SendData[4+i] = pData[i];
	}
	// Register Write Command Bit : '1'
	SendData[0] |= 0x80;

	nByteSent = SPI_TransferBytesHalfDuplex(8, SendData, 0, NULL);
//	nByteSent = SPI_TransferBytesFullDuplex(8, 8, SendData, 0, NULL);

	return nByteSent;
}

uint32
CcommSPI::ReadRegisterData(uint8 *pAddress, uint8 *pData)
{
	uint32 nByteSent;
	uint8 SendData[8];

	// Address 4B
	for (int i = 0; i < 4; i++)
	{
		SendData[0+i] = pAddress[i];
	}
	// Data 4B
		// None

	// Register Read Command Bit : '0'
	SendData[0] &= ~(0x80);

	nByteSent = SPI_TransferBytesHalfDuplex(4, SendData, 4, pData);
//	nByteSent = SPI_TransferBytesFullDuplex(8, 4, SendData, 4, pData);

	return nByteSent;
}



