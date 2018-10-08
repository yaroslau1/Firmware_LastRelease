#ifndef TADS1292_H
#define TADS1292_H
/*----------------------------------------------------------------------------------*/
#define ADS1292_OPCODE_WAKEUP		  0x02
#define ADS1292_OPCODE_STANDBY		0x04
#define ADS1292_OPCODE_RESET		  0x06
#define ADS1292_OPCODE_START		  0x08
#define ADS1292_OPCODE_STOP		    0x0A
#define ADS1292_OPCODE_OFFSETCAL	0x1A
#define ADS1292_OPCODE_RDATAC		  0x10
#define ADS1292_OPCODE_SDATAC		  0x11
#define ADS1292_OPCODE_RDATA		  0x12
#define ADS1292_OPCODE_RREG		    0x20
#define ADS1292_OPCODE_WREG		    0x40

#define ADS1292_REG_ID			      0x00
#define ADS1292_REG_CONFIG1		    0x01
#define ADS1292_REG_CONFIG2		    0x02
#define ADS1292_REG_LOFF		      0x03
#define ADS1292_REG_CH1SET		    0x04
#define ADS1292_REG_CH2SET		    0x05
#define ADS1292_REG_RLD_SENS		  0x06
#define ADS1292_REG_LOFF_SENS		  0x07
#define ADS1292_REG_LOFF_STAT		  0x08
#define ADS1292_REG_RESP1		      0x09
#define ADS1292_REG_RESP2		      0x0A
#define ADS1292_REG_GPIO		      0x0B



#endif