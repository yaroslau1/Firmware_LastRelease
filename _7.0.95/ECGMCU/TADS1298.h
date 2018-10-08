#ifndef TADS1298_H
#define TADS1298_H
/*----------------------------------------------------------------------------------*/

#define ADS1298_OPCODE_WAKEUP     0x02
#define ADS1298_OPCODE_STANDBY		0x04
#define ADS1298_OPCODE_RESET		  0x06
#define ADS1298_OPCODE_START		  0x08
#define ADS1298_OPCODE_STOP		    0x0A
#define ADS1298_OPCODE_RDATAC		  0x10
#define ADS1298_OPCODE_SDATAC		  0x11
#define ADS1298_OPCODE_RDATA		  0x12
#define ADS1298_OPCODE_RREG		    0x20
#define ADS1298_OPCODE_WREG		    0x40

#define ADS1298_REG_ID			      0x00
#define ADS1298_REG_CONFIG1		    0x01
#define ADS1298_REG_CONFIG2		    0x02
#define ADS1298_REG_CONFIG3		    0x03
#define ADS1298_REG_LOFF		      0x04
#define ADS1298_REG_CH1SET		    0x05
#define ADS1298_REG_CH2SET		    0x06
#define ADS1298_REG_CH3SET		    0x07
#define ADS1298_REG_CH4SET		    0x08
#define ADS1298_REG_CH5SET		    0x09
#define ADS1298_REG_CH6SET		    0x0A
#define ADS1298_REG_CH7SET		    0x0B
#define ADS1298_REG_CH8SET		    0x0C
#define ADS1298_REG_RLD_SENSP		  0x0D
#define ADS1298_REG_RLD_SENSN		  0x0E
#define ADS1298_REG_LOFF_SENSP		0x0F
#define ADS1298_REG_LOFF_SENSN		0x10
#define ADS1298_REG_LOFF_FLIP		  0x11
#define ADS1298_REG_LOFF_STATP		0x12
#define ADS1298_REG_LOFF_STATN		0x13
#define ADS1298_REG_GPIO		      0x14
#define ADS1298_REG_PACE		      0x15
#define ADS1298_REG_RESP		      0x16
#define ADS1298_REG_CONFIG4		    0x17
#define ADS1298_REG_WCT1		      0x18
#define ADS1298_REG_WCT2		      0x19

//---- Установка скорости обработки АЦП: 250, 500 ------------------------------
#define ADS1298_SAMPLING_FREQUENCY		250

//---- Делитель для нормализации по ГОСТ, на 10Гц, должно быть 1 мВ ------------
#if ADS1298_SAMPLING_FREQUENCY == 250
//	#define ADS1298_NORMALIZE_DIVIDER		175		/* PGA = 2 */
//	#define ADS1298_NORMALIZE_DIVIDER		250		/* PGA = 3 */
	#define ADS1298_NORMALIZE_DIVIDER		557//540		/* для PGA = 6 */
#elif ADS1298_SAMPLING_FREQUENCY == 500
	#define ADS1298_NORMALIZE_DIVIDER		180
#endif



//------------------------------------------------------------------------------


#endif