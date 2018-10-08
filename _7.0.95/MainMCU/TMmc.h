/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     02.07.2013 16:08:00
    Purpose:  MultiMedia Card driver  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
#ifndef TMMC_H
#define TMMC_H
//------------------------------------------------------------------------------------
#include "ae_ext_cm.h"
#include "main.h"
//------------------------------------------------------------------------------------
enum	{CMD0=0, CMD1, CMD9, CMD10, CMD12, CMD13, CMD16,
		CMD17, CMD18, CMD24, CMD25, CMD27, CMD28, CMD29,
		CMD30, CMD32, CMD33, CMD34, CMD35, CMD36, CMD37,
		CMD38, CMD42, CMD58, CMD59, ACMD41, CMD_TOT};

enum	{GO_IDLE_STATE=0, SEND_OP_COND, SEND_CSD, SEND_CID,
		STOP_TRANSMISSION, SEND_STATUS, SET_BLOCKLEN,
		READ_SINGLE_BLOCK, READ_MULTIPLE_BLOCK, WRITE_BLOCK,
		WRITE_MULTIPLE_BLOCK, PROGRAM_CSD, SET_WRITE_PROT,
		CLR_WRITE_PROT, SEND_WRITE_PROT, TAG_SECTOR_START,
		TAG_SECTOR_END, UNTAG_SECTOR, TAG_ERASE_GROUP_START,
		TAG_ERASE_GROUP_END, UNTAG_ERASE_GROUP, ERASE,
		LOCK_UNLOCK, READ_OCR, CRC_ON_OFF};
		
enum	{RESP_1, RESP_1b, RESP_2, RESP_3};

enum	{NO_ARG, BLOCK_LEN, DATA_ADDR, STUFF_BITS};


//------------------------------------------------------------------------------------
enum {NO_ERR=0, INIT_ERR, File_ERR, WRITE_ERR, READ_ERR, NAME_ERR, READONLY_ERR,
		  SOF_ERR, EOF_ERR, ALLOC_ERR, POS_ERR, MODE_ERR, FAT_ERR, DISK_FULL, 
		  PATH_ERR, NO_ENTRY_AVAL, EXIST_ERR};
//------------------------------------------------------------------------------------
/*typedef struct
{
  BYTE InIdleState:1,
  BYTE EraseReset:1,
  BYTE IllegalCommand:1,
  BYTE ComCRCError:1,
  BYTE Erase_Seq_Error:1,
  BYTE AddressError:1,
  BYTE ParameterError:1,
  BYTE _always_zero:1;
} sR1ResponseByte;*/
//------------------------------------------------------------------------------------
//#define SD_CS_OFF()		OUT_MMCCS = 1 //PORTD |= 0x40
//#define SD_CS_ON()		OUT_MMCCS = 0 //PORTD &= 0xBF
//#define CS_DDR_SET()	CTRL_MMCCS = 1 //DDRD |= 0x40
//------------------------------------------------------------------------------------

class TMmc
{
  public:
    static bool MMCPresence();
    static void MMCInit();
    static BYTE MMCResponse;
    static BYTE GetResponse();
    static BYTE SendCommand(BYTE cmd, DWORD address, BYTE Crc);
    static BYTE initialize_media();
    static BYTE _FF_spi(BYTE mydata);
    static BYTE reset_sd(void);
    static WORD send_cmd(BYTE command, DWORD argument);
    static BYTE init_sd(void);
    static void clear_sd_buff(void);
    static BYTE _FF_read(DWORD sd_addr);
    static BYTE _FF_read_disp(DWORD sd_addr);
    static BYTE _FF_write(DWORD sd_addr);
    static BYTE BPB_SecPerClus;
    static WORD FirstDataSector;
  protected:
    static DWORD OCR_REG;
    static BYTE _FF_buff[512];
    static WORD PT_SecStart;
    static DWORD BS_jmpBoot;
    static WORD BPB_BytsPerSec;
    static WORD BPB_RsvdSecCnt;
    static BYTE BPB_NumFATs;
    static WORD BPB_RootEntCnt;
    static WORD BPB_FATSz16;
    static BYTE BPB_FATType;
    static DWORD BPB_TotSec;
    static DWORD BS_VolSerial;
    static char BS_VolLab[12];
    static DWORD _FF_PART_ADDR, _FF_ROOT_ADDR, _FF_DIR_ADDR;
    static DWORD _FF_FAT1_ADDR, _FF_FAT2_ADDR;
    static DWORD _FF_RootDirSectors;
    static DWORD FirstSectorofCluster;
    static BYTE _FF_error;
    static DWORD _FF_buff_addr;
    static DWORD clus_0_addr, _FF_n_temp;
    static WORD c_counter;
    static DWORD DataClusTot;

    static void SD_Select();
    static void SD_Unselect();
    static BYTE Change(BYTE dat);
    
    static void SD_CS_ON();
    static void SD_CS_OFF();


    //    static char  _FF_FULL_PATH[_FF_PATH_LENGTH];
  private:
};
//------------------------------------------------------------------------------------
#endif
