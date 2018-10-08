/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     06.04.2004 12:13
    Purpose:  
    Software:
    Hardware:
    Note:    
*/
//------------------------------------------------------------------------------------
#include "TMmc.h"
/*-----------------------------------------------------------------------*/
#include "sd_spi.h"
//------------------------------------------------------------------------------------
//#define MMC_SEND_CARDINFO
//------------------------------------------------------------------------------------
struct CMD
{
	WORD index;
	WORD tx_data;
	WORD arg;
	WORD resp;
};


const struct CMD sd_cmd[CMD_TOT] =
{
	{CMD0,	0x40,	NO_ARG,		RESP_1},		// GO_IDLE_STATE
	{CMD1,	0x41,	NO_ARG,		RESP_1},		// SEND_OP_COND (ACMD41 = 0x69)
	{CMD9,	0x49,	NO_ARG,		RESP_1},		// SEND_CSD
	{CMD10,	0x4A,	NO_ARG,		RESP_1},		// SEND_CID
	{CMD12,	0x4C,	NO_ARG,		RESP_1},		// STOP_TRANSMISSION
	{CMD13,	0x4D,	NO_ARG,		RESP_2},		// SEND_STATUS
	{CMD16,	0x50,	BLOCK_LEN,	RESP_1},		// SET_BLOCKLEN
	{CMD17, 0x51,	DATA_ADDR,	RESP_1},		// READ_SINGLE_BLOCK
	{CMD18, 0x52,	DATA_ADDR,	RESP_1},		// READ_MULTIPLE_BLOCK
	{CMD24, 0x58,	DATA_ADDR,	RESP_1},		// WRITE_BLOCK
	{CMD25, 0x59,	DATA_ADDR,	RESP_1},		// WRITE_MULTIPLE_BLOCK
	{CMD27,	0x5B,	NO_ARG,		RESP_1},		// PROGRAM_CSD
	{CMD28, 0x5C,	DATA_ADDR,	RESP_1b},		// SET_WRITE_PROT
	{CMD29, 0x5D,	DATA_ADDR,	RESP_1b},		// CLR_WRITE_PROT
	{CMD30, 0x5E,	DATA_ADDR,	RESP_1},		// SEND_WRITE_PROT
	{CMD32,	0x60,	DATA_ADDR,	RESP_1},		// TAG_SECTOR_START
	{CMD33,	0x61,	DATA_ADDR,	RESP_1},		// TAG_SECTOR_END
	{CMD34,	0x62,	DATA_ADDR,	RESP_1},		// UNTAG_SECTOR
	{CMD35,	0x63,	DATA_ADDR,	RESP_1},		// TAG_ERASE_GROUP_START
	{CMD36,	0x64,	DATA_ADDR,	RESP_1},		// TAG_ERASE_GROUP_END
	{CMD37,	0x65,	DATA_ADDR,	RESP_1},		// TAG_ERASE_GROUP
	{CMD38,	0x66,	STUFF_BITS,	RESP_1b},		// ERASE
	{CMD42,	0x6A,	STUFF_BITS,	RESP_1b},		// LOCK_UNLOCK
	{CMD58,	0x7A,	NO_ARG,		RESP_3},		// READ_OCR
	{CMD59,	0x7B,	STUFF_BITS,	RESP_1},		// CRC_ON_OFF
	{ACMD41, 0x69,	NO_ARG,		RESP_1}
};
//------------------------------------------------------------------------------------
BYTE  TMmc::MMCResponse = 0x55;
DWORD TMmc::OCR_REG;
BYTE  TMmc::_FF_buff[512];
WORD  TMmc::PT_SecStart;
DWORD TMmc::BS_jmpBoot;
WORD  TMmc::BPB_BytsPerSec;
BYTE  TMmc::BPB_SecPerClus;
WORD  TMmc::BPB_RsvdSecCnt;
BYTE  TMmc::BPB_NumFATs;
WORD  TMmc::BPB_RootEntCnt;
WORD  TMmc::BPB_FATSz16;
BYTE  TMmc::BPB_FATType;
DWORD TMmc::BPB_TotSec;
DWORD TMmc::BS_VolSerial;
char  TMmc::BS_VolLab[12];
DWORD TMmc::_FF_PART_ADDR, TMmc::_FF_ROOT_ADDR, TMmc::_FF_DIR_ADDR;
DWORD TMmc::_FF_FAT1_ADDR, TMmc::_FF_FAT2_ADDR;
DWORD TMmc::_FF_RootDirSectors;
WORD  TMmc::FirstDataSector;
DWORD TMmc::FirstSectorofCluster;
BYTE  TMmc::_FF_error;
DWORD TMmc::_FF_buff_addr;
DWORD TMmc::clus_0_addr, TMmc::_FF_n_temp;
WORD  TMmc::c_counter;
DWORD TMmc::DataClusTot;
//char  TMmc::_FF_FULL_PATH[_FF_PATH_LENGTH];
//------------------------------------------------------------------------------------

bool TMmc::MMCPresence()
{
	if(reset_sd() == 0) return false;
  return true;
}
//------------------------------------------------------------------------------------

void TMmc::SD_Select()
{
  CS_LOW();
}

void TMmc::SD_Unselect()
{
  CS_HIGH();
}

BYTE TMmc::Change(BYTE dat)
{
  return xmit_spi(dat);
}
//------------------------------------------------------------------------------------

void TMmc::SD_CS_ON()
{
  CS_LOW();
}
//------------------------------------------------------------------------------------

void TMmc::SD_CS_OFF()
{
  CS_HIGH();
}
//------------------------------------------------------------------------------------

void TMmc::MMCInit()
{
  SD_Unselect();
  vTaskDelay( 2 / portTICK_RATE_MS );
//  __delay_cycles(USEC(100));
  for(BYTE i = 0;i < 10;i++) Change(0xFF);
//  MMCResponse = GetResponse();
//  SendCommand(CMD1,0,0xFF);
}
//------------------------------------------------------------------------------------

BYTE TMmc::_FF_spi(BYTE mydata)
{
  return Change(mydata);
}
//------------------------------------------------------------------------------------

WORD TMmc::send_cmd(BYTE command, DWORD argument)
{
	BYTE spi_data_out;
	BYTE response_1;
	DWORD response_2;
	WORD c, i;
	
	SD_CS_ON();			// select chip
	
	spi_data_out = sd_cmd[command].tx_data;
	_FF_spi(spi_data_out);
	
	c = sd_cmd[command].arg;
	if (c == NO_ARG)
		for (i=0; i<4; i++)
			_FF_spi(0);
	else
	{
		spi_data_out = (argument & 0xFF000000) >> 24;
		_FF_spi(spi_data_out);
		spi_data_out = (argument & 0x00FF0000) >> 16;
		_FF_spi(spi_data_out);
		spi_data_out = (argument & 0x0000FF00) >> 8;
		_FF_spi(spi_data_out);
		spi_data_out = (argument & 0x000000FF);
		_FF_spi(spi_data_out);
	}
	if (command == CMD0)
		spi_data_out = 0x95;		// CRC byte, don't care except for first signal=0x95
	else
		spi_data_out = 0xFF;
	_FF_spi(spi_data_out);
	_FF_spi(0xff);	

	c = sd_cmd[command].resp;
	switch(c)
	{
		case RESP_1:
			return (_FF_spi(0xFF));
		case RESP_1b:
			response_1 = _FF_spi(0xFF);
			response_2 = 0;
			while (response_2 == 0)
				response_2 = _FF_spi(0xFF);
			return (response_1);
		case RESP_2:
			response_2 = _FF_spi(0xFF);
			response_2 = (response_2 << 8) | _FF_spi(0xFF);
			return (response_2);
		case RESP_3:
			response_1 = _FF_spi(0xFF);
			OCR_REG = 0;
			response_2 = _FF_spi(0xFF);
			OCR_REG = response_2 << 24;
			response_2 = _FF_spi(0xFF);
			OCR_REG |= (response_2 << 16);
			response_2 = _FF_spi(0xFF);
			OCR_REG |= (response_2 << 8);
			response_2 = _FF_spi(0xFF);
			OCR_REG |= (response_2);
			return (response_1);
	}
	return (0);
}
//------------------------------------------------------------------------------------

BYTE TMmc::reset_sd(void)
{
	BYTE resp, n, c;

	for (c=0; c<4; c++)		// try reset command 3 times if needed
	{
		SD_CS_OFF();
		for (n=0; n<10; n++)	// initialize clk signal to sync card
			_FF_spi(0xFF);
		resp = send_cmd(CMD0,0);
		for (n=0; n<200; n++)
		{
			if (resp == 0x1)
  			{
  				SD_CS_OFF();
  				return(1);
  			}
	    resp = _FF_spi(0xFF);
		}
	}
	return 0;
}
//------------------------------------------------------------------------------------

void TMmc::clear_sd_buff(void)
{
	SD_CS_OFF();
	_FF_spi(0xFF);
	_FF_spi(0xFF);
}	
//------------------------------------------------------------------------------------

BYTE TMmc::init_sd(void)
{
	BYTE resp;
	WORD c;
	
	clear_sd_buff();

    for (c=0; c<1000; c++)
    {
    	resp = send_cmd(CMD1, 0);
    	if (resp == 0)
    		break;
   		resp = _FF_spi(0xFF);
   		if (resp == 0)
   			break;
   		resp = _FF_spi(0xFF);
   		if (resp == 0)
   			break;
	}
   	if (resp == 0)
	{
		return (1);
	}
	else
	{
		return (0);
 	}        		
}
//------------------------------------------------------------------------------------

BYTE TMmc::_FF_read_disp(DWORD sd_addr)
{
	BYTE resp;
	DWORD n, remainder;
	
	if (sd_addr % 0x200)
	{	// Not a valid read address, return 0
		_FF_error = READ_ERR;
		return (0);
	}

	clear_sd_buff();
	resp = send_cmd(CMD17, sd_addr);		// Send read request
	
	while(resp!=0xFE)
		resp = _FF_spi(0xFF);
	for (n=0; n<512; n++)
	{
		remainder = n % 0x10;
		if (remainder == 0)
			printf("\n\r");
		_FF_buff[n] = _FF_spi(0xFF);
		if (_FF_buff[n]<0x10)
			putchar(0x30);
		printf("%X ", _FF_buff[n]);
	}
	_FF_spi(0xFF);
	_FF_spi(0xFF);
	_FF_spi(0xFF);
	SD_CS_OFF();
	return (1);
}
//------------------------------------------------------------------------------------

// Read data from a SD card @ address
BYTE TMmc::_FF_read(DWORD sd_addr)
{
	BYTE resp;
	DWORD n;
	
	if (sd_addr % BPB_BytsPerSec)
	{	// Not a valid read address, return 0
		_FF_error = READ_ERR;
		return (0);
	}
		
//  __watchdog_reset();
	for (;;)
	{
		clear_sd_buff();
		
		resp = send_cmd(CMD17, sd_addr);	// read block command
		
		for(n =0;n < 1000; n++)
  		{
  			if (resp == 0xFE)
    			{	// waiting for start byte
    				break;
    			}
  			resp = _FF_spi(0xFF);
  		}
		if(resp == 0xFE)
  		{	// if it is a valid start byte => start reading SD Card
  			for (n = 0;n < BPB_BytsPerSec; n++)
  				_FF_buff[n] = _FF_spi(0xFF);
  			_FF_spi(0xFF);
  			_FF_spi(0xFF);
  			_FF_spi(0xFF);
  			SD_CS_OFF();
  			_FF_error = NO_ERR;
  			_FF_buff_addr = sd_addr;
  			return (1);
  		}

		SD_CS_OFF();
	}	
}
//------------------------------------------------------------------------------------

BYTE TMmc::_FF_write(DWORD sd_addr)
{
	BYTE resp, calc, valid_flag;
	WORD n;
	
//	LedOn();
	
	if ((sd_addr % BPB_BytsPerSec) || (sd_addr <= _FF_PART_ADDR))
  	{	// Not a valid write address, return 0
  		_FF_error = WRITE_ERR;
  		return (0);
  	}

	for (;;)
  	{
  		clear_sd_buff();
  		resp = send_cmd(CMD24, sd_addr);
  		valid_flag = 0;
  		for (n=0; n<1000; n++)
    		{
    			if (resp == 0x00)
      			{
      				valid_flag = 1;
      				break;
      			}
    			resp = _FF_spi(0xFF);
    		}
  		if (valid_flag)
    		{
	        BYTE *ArrPtr;
	        ArrPtr = &_FF_buff[0];
    			_FF_spi(0xFF);
          Change(0xFE);	
//    			ChangeWithoutWaiting(0xFE);					// Start Block Token
    			for (n=0; n<512; n++)		// Write Data in buffer to card
    			  {
    			    BYTE ChByte;
    			    
    			    ChByte = *ArrPtr;
    			    ArrPtr++;
              Change(ChByte);	
//    				  WaitThenChange(ChByte);
    				}
//    			WaitForChange();	
   				SD_CS_ON();
    			_FF_spi(0xFF);					// Send 2 blank CRC bytes
    			_FF_spi(0xFF);
    			resp = _FF_spi(0xFF);			// Response should be 0bXXX00101
    			calc = resp | 0xE0;
    			if (calc==0xE5)
      			{
      				while(_FF_spi(0xFF)==0)
      					;	// Clear Buffer before returning 'OK'
      				SD_CS_OFF();
      				_FF_error = NO_ERR;
      				return(1);
    			  }
    		}
		SD_CS_OFF(); 
	}
}
//------------------------------------------------------------------------------------
//const char sIn1[] = {"Part Address:"};
//const char sIn2[] = {"BPB_BytsPerSec:"};
//const char sIn3[] = {"BPB_SecPerClus:"};
//const char sIn4[] = {"BPB_RsvdSecCnt:"};
//const char sIn5[] = {"BPB_NumFATs:"};
//const char sIn6[] = {"BPB_FATType:"};
//const char sIn7[] = {"ClusterCnt:"};
//const char sInOD[] = {"\n\r"};
//------------------------------------------------------------------------------------

BYTE TMmc::initialize_media()
{
	BYTE data_temp = 1;
	DWORD n;
	
	// SPI BUS SETUP
	// SPI initialization
	// SPI Type: Master
	// SPI Clock Rate: 921.600 kHz
	// SPI Clock Phase: Cycle Half
	// SPI Clock Polarity: Low
	// SPI Data Order: MSB First
		
	BPB_BytsPerSec = 512;	// Initialize sector size to 512 (all SD cards have a 512 sector size)
        _FF_n_temp = 0;
	if (reset_sd()==0)
		return 0;
	// delay_ms(50);
	for (n=0; ((n<100)||(data_temp==0)) ; n++)
	{
		SD_CS_ON();
		data_temp = _FF_spi(0xFF);
		SD_CS_OFF();
	}
	// delay_ms(50);
	for (n=0; n<100; n++)
	{
		if (init_sd())		// Initialization Succeeded
			break;
		if (n==99)
			return 0;
	}

	if (_FF_read(0x0)==0)
	{
		_FF_error = INIT_ERR;
		return 0;
	}
	
	PT_SecStart = ((WORD) _FF_buff[0x1c7] << 8) | (WORD) _FF_buff[0x1c6];
	
	if ((((_FF_buff[0]==0xEB)&&(_FF_buff[2]==0x90))||(_FF_buff[0]==0xE9))&&((_FF_buff[510]==0x55)&&(_FF_buff[511]==0xAA)))
    	PT_SecStart = 0;
 
	_FF_PART_ADDR = (DWORD) PT_SecStart * (DWORD) BPB_BytsPerSec;

	if (PT_SecStart)
	{
		if (_FF_read(_FF_PART_ADDR)==0)
		{
			_FF_error = INIT_ERR;
			return (0);
		}
	}

  BS_jmpBoot = (((DWORD) _FF_buff[0] << 16) | ((WORD) _FF_buff[1] << 8) | (WORD) _FF_buff[2]);    		
	BPB_BytsPerSec = ((WORD) _FF_buff[0xC] << 8) | (WORD) _FF_buff[0xB];
  BPB_SecPerClus = _FF_buff[0xD];
	BPB_RsvdSecCnt = ((WORD) _FF_buff[0xF] << 8) | (WORD) _FF_buff[0xE];	
	BPB_NumFATs = _FF_buff[0x10];
	BPB_RootEntCnt = ((WORD) _FF_buff[0x12] << 8) | (WORD) _FF_buff[0x11];	
	BPB_FATSz16 = ((WORD) _FF_buff[0x17] << 8) | (WORD) _FF_buff[0x16];
	BPB_TotSec = ((WORD) _FF_buff[0x14] << 8) | (WORD) _FF_buff[0x13];

	if (BPB_TotSec==0)
		BPB_TotSec = ((DWORD) _FF_buff[0x23] << 24) | ((DWORD) _FF_buff[0x22] << 16)
					| ((DWORD) _FF_buff[0x21] << 8) | ((DWORD) _FF_buff[0x20]);
	BS_VolSerial = ((DWORD) _FF_buff[0x2A] << 24) | ((DWORD) _FF_buff[0x29] << 16)
				| ((DWORD) _FF_buff[0x28] << 8) | ((DWORD) _FF_buff[0x27]);

	for (n=0; n<11; n++)
		BS_VolLab[n] = _FF_buff[0x2B+n];

	BS_VolLab[11] = 0;		// Terminate the string
	_FF_FAT1_ADDR = _FF_PART_ADDR + ((DWORD) BPB_RsvdSecCnt * (DWORD) BPB_BytsPerSec); 
	_FF_FAT2_ADDR = _FF_FAT1_ADDR + ((DWORD) BPB_FATSz16 * (DWORD) BPB_BytsPerSec);
	_FF_ROOT_ADDR = ((DWORD) BPB_NumFATs * (DWORD) BPB_FATSz16) + (DWORD) BPB_RsvdSecCnt;
	_FF_ROOT_ADDR *= BPB_BytsPerSec;
	_FF_ROOT_ADDR += _FF_PART_ADDR;
	
	_FF_RootDirSectors = ((BPB_RootEntCnt * 32) + BPB_BytsPerSec - 1) / BPB_BytsPerSec;
	FirstDataSector = (BPB_NumFATs * BPB_FATSz16) + BPB_RsvdSecCnt + _FF_RootDirSectors; 
	
	DataClusTot = BPB_TotSec - FirstDataSector;
	DataClusTot /= BPB_SecPerClus;
	clus_0_addr = 0;		// Reset Empty Cluster table location
	c_counter = 1;
	
	if (DataClusTot < 4085)				// FAT12
		BPB_FATType = 0x32;
	else 
	if (DataClusTot < 65525)		// FAT16
		BPB_FATType = 0x36;
	else
	{
		BPB_FATType = 0;
		_FF_error = FAT_ERR;
		return (0);
	}
    
	_FF_DIR_ADDR = _FF_ROOT_ADDR;		// Set current directory to root address

//	_FF_FULL_PATH[0] = 0x5C;	// a '\'
//	_FF_FULL_PATH[1] = 0;


	
	#ifdef MMC_SEND_CARDINFO
	
		TAppProcessor::SendString(sIn1);
		CharLong Tmp;
		Tmp.Long = _FF_PART_ADDR;
		TAppProcessor::SendHex(Tmp.Char[3]);
		TAppProcessor::SendHex(Tmp.Char[2]);
		TAppProcessor::SendHex(Tmp.Char[1]);
		TAppProcessor::SendHex(Tmp.Char[0]);
		TAppProcessor::SendString(sInOD);
		
		TAppProcessor::SendString(sIn2);
		Tmp.Long = BPB_BytsPerSec;
		TAppProcessor::SendHex(Tmp.Char[1]);
		TAppProcessor::SendHex(Tmp.Char[0]);
		TAppProcessor::SendString(sInOD);
	
		TAppProcessor::SendString(sIn3);
		TAppProcessor::SendHex(BPB_SecPerClus);
		TAppProcessor::SendString(sInOD);
		TAppProcessor::SendString(sIn4);
		Tmp.Long = BPB_RsvdSecCnt;
		TAppProcessor::SendHex(Tmp.Char[1]);
		TAppProcessor::SendHex(Tmp.Char[0]);
		TAppProcessor::SendString(sInOD);
	
		TAppProcessor::SendString(sIn5);
		TAppProcessor::SendHex(BPB_NumFATs);
		TAppProcessor::SendString(sInOD);
		TAppProcessor::SendString(sIn6);
		TAppProcessor::SendHex(BPB_FATType);
		TAppProcessor::SendString(sInOD);
	
		TAppProcessor::SendString(sIn7);
		Tmp.Long = DataClusTot;
		TAppProcessor::SendHex(Tmp.Char[3]);
		TAppProcessor::SendHex(Tmp.Char[2]);
		TAppProcessor::SendHex(Tmp.Char[1]);
		TAppProcessor::SendHex(Tmp.Char[0]);
		TAppProcessor::SendString(sInOD);
		TAppProcessor::SendString(BS_VolLab);
		TAppProcessor::SendString(sInOD);
	
	#endif

/*	
	#ifdef _MYDEBUG_ON_
		printf("\n\rPart Address:  %lX", _FF_PART_ADDR);
		printf("\n\rBS_jmpBoot:  %lX", BS_jmpBoot);
		printf("\n\rBPB_BytsPerSec:  %X", BPB_BytsPerSec);
		printf("\n\rBPB_SecPerClus:  %X", BPB_SecPerClus);
		printf("\n\rBPB_RsvdSecCnt:  %X", BPB_RsvdSecCnt);
		printf("\n\rBPB_NumFATs:  %X", BPB_NumFATs);
		printf("\n\rBPB_RootEntCnt:  %X", BPB_RootEntCnt);
		printf("\n\rBPB_FATSz16:  %X", BPB_FATSz16);
		printf("\n\rBPB_TotSec16:  %lX", BPB_TotSec);
		if (BPB_FATType == 0x32)
			printf("\n\rBPB_FATType:  FAT12");
		else if (BPB_FATType == 0x36)
			printf("\n\rBPB_FATType:  FAT16");
		else
			printf("\n\rBPB_FATType:  FAT ERROR!!");
		printf("\n\rClusterCnt:  %lX", DataClusTot);
		printf("\n\rROOT_ADDR:  %lX", _FF_ROOT_ADDR);
		printf("\n\rFAT2_ADDR:  %lX", _FF_FAT2_ADDR);
		printf("\n\rRootDirSectors:  %X", _FF_RootDirSectors);
		printf("\n\rFirstDataSector:  %X", FirstDataSector);
	#endif*/
	
	return 1;	
}
//------------------------------------------------------------------------------------
