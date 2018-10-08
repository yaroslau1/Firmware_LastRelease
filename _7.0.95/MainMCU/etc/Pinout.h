/*
    Title:    
    Author:   VN Asliuk(Cardian)
    Date:     23.08.2010 15:30:15
    Purpose:  
    Software: 
    Hardware: 
    Note:    
*/
//------------------------------------------------------------------------------------
//------------------------------------------------------------------------------------
#ifndef PINOUT_H
#define PINOUT_H
//------------------------------------------------------------------------------------
#include "main.h"
//------------------------------------------------------------------------------------
#ifdef EV_BOARD
//------------------------------------------------------------------------------------

//#define LED0_GPIO_PORT              GPIOE
//#define LED0_GPIO_PIN               GPIO_Pin_2
///////////////
//#define LED1_GPIO_PORT              GPIOE
//#define LED1_GPIO_PIN               GPIO_Pin_3
///////////////
//#define LED2_GPIO_PORT              GPIOE
//#define LED2_GPIO_PIN               GPIO_Pin_4
///////////////
//#define LED3_GPIO_PORT              GPIOE
//#define LED3_GPIO_PIN               GPIO_Pin_5
///////////////
//#define _BUZZER_GPIO_PORT           GPIOB
//#define _BUZZER_GPIO_PIN            GPIO_Pin_0
///////////////
//#define _3V5V_GPIO_PORT             GPIOB
//#define _3V5V_GPIO_PIN              GPIO_Pin_8
/////////////
//#define WAKEUP_BUTTON_PORT          GPIOA
//#define WAKEUP_BUTTON_PIN           GPIO_Pin_0
/////////////
//#define TAMPER_BUTTON_PORT          GPIOC
//#define TAMPER_BUTTON_PIN           GPIO_Pin_13
/////////////
//#define UB0_BUTTON_PORT          		GPIOB
//#define UB0_BUTTON_PIN           		GPIO_Pin_5
///////////////
//#define UB1_BUTTON_PORT          		GPIOE
//#define UB1_BUTTON_PIN           		GPIO_Pin_14
///////////////
//#define UB2_BUTTON_PORT          		GPIOB
////#define UB2_BUTTON_PIN           		GPIO_Pin_7
///////////////
//#define IONISTOR_STATE_PORT         GPIOB
//#define IONISTOR_STATE_PIN          GPIO_Pin_9
///////////////
//#define LBO_STATE_PORT         			GPIOB
//#define LBO_STATE_PIN          			GPIO_Pin_1
///////////////
//#define EVTWAIN_USART1_GPIO         GPIOA
//#define EVTWAIN_USART1_RxPin        GPIO_Pin_10
//#define EVTWAIN_USART1_TxPin        GPIO_Pin_9
///////////
// #define _LMX_RST_GPIO_PORT           GPIOD
// #define _LMX_RST_GPIO_PIN            GPIO_Pin_3
///////////
// #define LMX_BR_SEL0_GPIO_PORT       GPIOE
// #define LMX_BR_SEL0_GPIO_PIN        GPIO_Pin_0
///////////
// #define LMX_BR_SEL1_GPIO_PORT       GPIOE
// #define LMX_BR_SEL1_GPIO_PIN        GPIO_Pin_1
///////////
// #define LMX_LSTAT_PORT         			GPIOE
// #define LMX_LSTAT_PIN          			GPIO_Pin_6
///////////
//#define LCD_RES_GPIO_PORT           GPIOB
//#define LCD_RES_GPIO_PIN            GPIO_Pin_14
///////////////
//#define LCD_CS_GPIO_PORT            GPIOD
//#define LCD_CS_GPIO_PIN             GPIO_Pin_13
///////////////
//#define LCD_ALDC_GPIO_PORT          GPIOA
//#define LCD_ALDC_GPIO_PIN           GPIO_Pin_8
///////////////
//#define LCD_BK_CONTROL_GPIO_PORT    GPIOB
//#define LCD_BK_CONTROL_GPIO_PIN     GPIO_Pin_6
///////////////
//#define LCD_SCL_GPIO_PORT           GPIOB
//#define LCD_SCL_GPIO_PIN            GPIO_Pin_13
///////////////
//#define LCD_SIN_GPIO_PORT           GPIOB
//#define LCD_SIN_GPIO_PIN            GPIO_Pin_15
///////////////
//#define EDAC_SCLK_GPIO_PORT         GPIOD
//#define EDAC_SCLK_GPIO_PIN          GPIO_Pin_10
///////////////
//#define EDAC_SYNC_GPIO_PORT         GPIOD
//#define EDAC_SYNC_GPIO_PIN          GPIO_Pin_9
///////////////
//#define EDAC_DIN_GPIO_PORT          GPIOD
//#define EDAC_DIN_GPIO_PIN           GPIO_Pin_8
///////////////
//#define AX_XOUT_GPIO_PORT         	GPIOC
//#define AX_XOUT_GPIO_PIN          	GPIO_Pin_0
///////////////
//#define AX_YOUT_GPIO_PORT         	GPIOC
//#define AX_YOUT_GPIO_PIN          	GPIO_Pin_1
///////////////
//#define AX_ZOUT_GPIO_PORT         	GPIOC
//#define AX_ZOUT_GPIO_PIN          	GPIO_Pin_2
///////////////
//#define EADC_CNV_GPIO_PORT         	GPIOE
//#define EADC_CNV_GPIO_PIN          	GPIO_Pin_15
///////////////
//#define EADC_SCK_GPIO_PORT         	GPIOB
//#define EADC_SCK_GPIO_PIN          	GPIO_Pin_12
///////////////
//#define EADC_SDI_GPIO_PORT         	GPIOB
//#define EADC_SDI_GPIO_PIN          	GPIO_Pin_10
///////////////
//#define EADC_SDO_PORT         		 	GPIOB
//#define EADC_SDO_PIN          		 	GPIO_Pin_11
///////////////
//#define ESW_GPIO_PORT         		 	GPIOC
//#define ESW_GPIO_PIN          		 	GPIO_Pin_4
///////////////
//#define _NIBP_PUMP_GPIO_PORT        GPIOC
//#define _NIBP_PUMP_GPIO_PIN         GPIO_Pin_7
///////////////
//#define _NIBP_VALVE_GPIO_PORT       GPIOC
//#define _NIBP_VALVE_GPIO_PIN        GPIO_Pin_6
///////////////
//#define NIBP_CS_GPIO_PORT         	GPIOA
//#define NIBP_CS_GPIO_PIN          	GPIO_Pin_1
///////////////
//#define NIBP_DIN_GPIO_PORT         	GPIOA
//#define NIBP_DIN_GPIO_PIN          	GPIO_Pin_2
///////////////
//#define NIBP_DOUT_RDY_GPIO_PORT     GPIOA
//#define NIBP_DOUT_RDY_GPIO_PIN      GPIO_Pin_3
///////////////
//#define NIBP_SCLK_GPIO_PORT         AX_ZOUT_GPIO_PORT
//#define NIBP_SCLK_GPIO_PIN          AX_ZOUT_GPIO_PIN
///////////////
//#define DAC1_GPIO_PORT         	 		GPIOA
//#define DAC1_GPIO_PIN          	 		GPIO_Pin_4
///////////////
//#define EPACE_PULSEG_GPIO_PORT      GPIOE
//#define EPACE_PULSEG_GPIO_PIN       GPIO_Pin_12
///////////////
//#define EPACE_OUT_BUTTON_PORT       GPIOC
//#define EPACE_OUT_BUTTON_PIN        GPIO_Pin_5
///////////////
//#define SD_SPI_SCLK_GPIO_PORT       GPIOA
//#define SD_SPI_SCLK_GPIO_PIN        GPIO_Pin_5
///////////////
//#define SD_SPI_DOUT_GPIO_PORT       GPIOA
//#define SD_SPI_DOUT_GPIO_PIN        GPIO_Pin_6
///////////////
//#define SD_SPI_DIN_GPIO_PORT        GPIOA
//#define SD_SPI_DIN_GPIO_PIN         GPIO_Pin_7
///////////////
//#define SD_CS_GPIO_PORT         	  GPIOC
//#define SD_CS_GPIO_PIN          	  GPIO_Pin_11
///////////////
//#define ATTINY_OUT_GPIO_PORT        GPIOE
//#define ATTINY_OUT_GPIO_PIN         GPIO_Pin_13
///////////////
//#define USB_DISCONNECT_GPIO_PORT    GPIOE
//#define USB_DISCONNECT_GPIO_PIN     GPIO_Pin_11
////------------------------------------------------------------------------------------
////Remapping
////PORT A
//#define WAKEUP_GPIO_PORT     				UB1_BUTTON_PORT
//#define WAKEUP_GPIO_PIN      				UB1_BUTTON_PIN
///////////////                				
//#define A85CNV_GPIO_PORT     				WAKEUP_BUTTON_PORT
//#define A85CNV_GPIO_PIN      				WAKEUP_BUTTON_PIN
///////////////                				
//#define DR_CS_GPIO_PORT      				SD_CS_GPIO_PORT
//#define DR_CS_GPIO_PIN       				SD_CS_GPIO_PIN
///////////////                				
//#define DR_CLK_GPIO_PORT     				GPIOA
//#define DR_CLK_GPIO_PIN      				GPIO_Pin_5
///////////////                				
//#define DR_DIN_GPIO_PORT     				GPIOA
//#define DR_DIN_GPIO_PIN      				GPIO_Pin_6
///////////////                				
//#define DR_DOUT_GPIO_PORT    				GPIOA
//#define DR_DOUT_GPIO_PIN     				GPIO_Pin_7
///////////////                				
//#define LMX_TXD_GPIO_PORT    				GPIOA
//#define LMX_TXD_GPIO_PIN     				GPIO_Pin_9
///////////////                				
//#define LMX_RXD_GPIO_PORT    				GPIOA
//#define LMX_RXD_GPIO_PIN     				GPIO_Pin_10
///////////////                				
//#define PS_DOUTRDY_GPIO_PORT 				NIBP_DOUT_RDY_GPIO_PORT
//#define PS_DOUTRDY_GPIO_PIN  				NIBP_DOUT_RDY_GPIO_PIN
///////////////                				
//#define PS_SCLK_GPIO_PORT    				NIBP_SCLK_GPIO_PORT
//#define PS_SCLK_GPIO_PIN     				NIBP_SCLK_GPIO_PIN
///////////////                				
////PORT B                     				
//#define ADXL_YOUT_GPIO_PORT  				AX_YOUT_GPIO_PORT
//#define ADXL_YOUT_GPIO_PIN   				AX_YOUT_GPIO_PIN
///////////////                				
//#define ADXL_ZOUT_GPIO_PORT  				GPIOC
//#define ADXL_ZOUT_GPIO_PIN   				GPIO_Pin_3
///////////////
//#define A85_SCK_SPICLK_GPIO_PORT  	GPIOB
//#define A85_SCK_SPICLK_GPIO_PIN   	GPIO_Pin_3
///////////////
//#define A85_SDO_GPIO_PORT  	        EADC_SDO_PORT
//#define A85_SDO_GPIO_PIN   	        EADC_SDO_PIN
/////////////////
////#define EE_SCL_GPIO_PORT  	        GPIOB
////#define EE_SCL_GPIO_PIN   	        GPIO_Pin_6
/////////////////
////#define EE_SDA_GPIO_PORT  	        GPIOB
////#define EE_SDA_GPIO_PIN   	        GPIO_Pin_7
///////////////
//#define PS_CS_GPIO_PORT  	          NIBP_CS_GPIO_PORT
//#define PS_CS_GPIO_PIN   	          NIBP_CS_GPIO_PIN
///////////////
//#define PS_DIN_GPIO_PORT  	        NIBP_DIN_GPIO_PORT
//#define PS_DIN_GPIO_PIN   	        NIBP_DIN_GPIO_PIN
///////////////
//#define A85_SCK_PWM_GPIO_PORT  	    GPIOB
//#define A85_SCK_PWM_GPIO_PIN   	    GPIO_Pin_10
///////////////
//#define LCD_A0_GPIO_PORT  	        LCD_ALDC_GPIO_PORT
//#define LCD_A0_GPIO_PIN   	        LCD_ALDC_GPIO_PIN
///////////////
//#define LCD_CLK_GPIO_PORT  	        LCD_SCL_GPIO_PORT
//#define LCD_CLK_GPIO_PIN   	        LCD_SCL_GPIO_PIN
///////////////
//#define LCD_RST_GPIO_PORT  	        LCD_RES_GPIO_PORT
//#define LCD_RST_GPIO_PIN   	        LCD_RES_GPIO_PIN
///////////////
//#define LCD_SI_GPIO_PORT  	        LCD_SIN_GPIO_PORT
//#define LCD_SI_GPIO_PIN   	        LCD_SIN_GPIO_PIN
///////////////
////PORT C                     				
//#define IPOL_0_GPIO_PORT  	        _3V5V_GPIO_PORT
//#define IPOL_0_GPIO_PIN   	        _3V5V_GPIO_PIN
/////////////////
////#define IPOL_1_GPIO_PORT  	        GPIOC
////#define IPOL_1_GPIO_PIN   	        GPIO_Pin_1
/////////////////
////#define IPOL_2_GPIO_PORT  	        GPIOC
////#define IPOL_2_GPIO_PIN   	        GPIO_Pin_2
///////////////
//#define ADXL_XOUT_GPIO_PORT  	      AX_XOUT_GPIO_PORT
//#define ADXL_XOUT_GPIO_PIN   	      AX_XOUT_GPIO_PIN
///////////////
//#define BAT_ZOND_GPIO_PORT  	      GPIOC
//#define BAT_ZOND_GPIO_PIN   	      GPIO_Pin_5
///////////////
//#define NIBP_VALVE_GPIO_PORT  	    _NIBP_VALVE_GPIO_PORT
//#define NIBP_VALVE_GPIO_PIN   	    _NIBP_VALVE_GPIO_PIN
///////////////
//#define NIBP_PUMP_GPIO_PORT  	      _NIBP_PUMP_GPIO_PORT
//#define NIBP_PUMP_GPIO_PIN   	      _NIBP_PUMP_GPIO_PIN
///////////////
//#define BKLED_CTRL_GPIO_PORT  	    LCD_BK_CONTROL_GPIO_PORT
//#define BKLED_CTRL_GPIO_PIN   	    LCD_BK_CONTROL_GPIO_PIN
///////////////
////#define HIPWR_EN_GPIO_PORT  	    	GPIOC
////#define HIPWR_EN_GPIO_PIN   	    	GPIO_Pin_9
///////////////
////#define ADUM_EN_GPIO_PORT  	    		GPIOC
////#define ADUM_EN_GPIO_PIN   	    		GPIO_Pin_11
///////////////
////PORT D                     				
////#define EE_CS_GPIO_PORT  	    			GPIOD
////#define EE_CS_GPIO_PIN   	    			GPIO_Pin_3
///////////////
//#define ION_ACT_GPIO_PORT  	    		IONISTOR_STATE_PORT
//#define ION_ACT_GPIO_PIN   	    		IONISTOR_STATE_PIN
///////////////
//#define LBO_GPIO_PORT  	    				LBO_STATE_PORT
//#define LBO_GPIO_PIN   	    				LBO_STATE_PIN
///////////////
////#define ADXL_PWR_GPIO_PORT  	    	GPIOD
////#define ADXL_PWR_GPIO_PIN   	    	GPIO_Pin_10
///////////////
//#define BUZZER_GPIO_PORT  	    		_BUZZER_GPIO_PORT
//#define BUZZER_GPIO_PIN   	    		_BUZZER_GPIO_PIN
///////////////
////PORT E                     				
//#define A85_ESW0_GPIO_PORT  	    	ESW_GPIO_PORT
//#define A85_ESW0_GPIO_PIN   	    	ESW_GPIO_PIN
///////////////
////#define A85_ESW1_GPIO_PORT  	    	GPIOE
////#define A85_ESW1_GPIO_PIN   	    	GPIO_Pin_1
///////////////
////#define ANAP_EN_GPIO_PORT  	    		GPIOE
////#define ANAP_EN_GPIO_PIN   	    		GPIO_Pin_2
///////////////
////#define A85_ESW2_GPIO_PORT  	    	GPIOE
////#define A85_ESW2_GPIO_PIN   	    	GPIO_Pin_3
///////////////
////#define LMX_EN_GPIO_PORT  	    		GPIOE
////#define LMX_EN_GPIO_PIN   	    		GPIO_Pin_12
///////////////
//#define LMX_RST_GPIO_PORT  	    		_LMX_RST_GPIO_PORT
//#define LMX_RST_GPIO_PIN   	    		_LMX_RST_GPIO_PIN
///////////////
//#define LMX_LSTAT_GPIO_PORT  	    	LMX_LSTAT_PORT
//#define LMX_LSTAT_GPIO_PIN   	    	LMX_LSTAT_PIN
///////////////
////#define LMX_READY_GPIO_PORT  	    	GPIOE
////#define LMX_READY_GPIO_PIN   	    	GPIO_Pin_15
///////////////
////------------------------------------------------------------------------------------
//#else
////------------------------------------------------------------------------------------
////PORT A
//#define WAKEUP_GPIO_PORT     				GPIOA
//#define WAKEUP_GPIO_PIN      				GPIO_Pin_0
///////////////                				
//#define A85CNV_GPIO_PORT     				GPIOA
//#define A85CNV_GPIO_PIN      				GPIO_Pin_1
///////////////                				
//#define DR_CS_GPIO_PORT      				GPIOA
//#define DR_CS_GPIO_PIN       				GPIO_Pin_4
///////////////                				
//#define DR_CLK_GPIO_PORT     				GPIOA
//#define DR_CLK_GPIO_PIN      				GPIO_Pin_5
///////////////                				
//#define DR_DIN_GPIO_PORT     				GPIOA
//#define DR_DIN_GPIO_PIN      				GPIO_Pin_6
///////////////                				
//#define DR_DOUT_GPIO_PORT    				GPIOA
//#define DR_DOUT_GPIO_PIN     				GPIO_Pin_7
///////////////                				
//#define LMX_RXD_GPIO_PORT    				GPIOA
//#define LMX_RXD_GPIO_PIN     				GPIO_Pin_10
///////////////                				
//#define PS_DIN_GPIO_PORT 						GPIOA
//#define PS_DIN_GPIO_PIN   				  GPIO_Pin_11
///////////////                				
//#define PS_SCLK_GPIO_PORT    				GPIOA
//#define PS_SCLK_GPIO_PIN     				GPIO_Pin_12
///////////////                				
////PORT B                     				
//#define ADXL_YOUT_GPIO_PORT  				GPIOB
//#define ADXL_YOUT_GPIO_PIN   				GPIO_Pin_0
///////////////                				
//#define ADXL_ZOUT_GPIO_PORT  				GPIOB
//#define ADXL_ZOUT_GPIO_PIN   				GPIO_Pin_1
///////////////
//#define A85_SCK_SPICLK_GPIO_PORT  	GPIOB
//#define A85_SCK_SPICLK_GPIO_PIN   	GPIO_Pin_3
///////////////
//#define A85_SDO_GPIO_PORT  	        GPIOB
//#define A85_SDO_GPIO_PIN   	        GPIO_Pin_5
///////////////
//#define EE_SCL_GPIO_PORT  	        GPIOB
//#define EE_SCL_GPIO_PIN   	        GPIO_Pin_6
///////////////
//#define EE_SDA_GPIO_PORT  	        GPIOB
//#define EE_SDA_GPIO_PIN   	        GPIO_Pin_7
///////////////
//#define PS_CS_GPIO_PORT  	          GPIOB
//#define PS_CS_GPIO_PIN   	          GPIO_Pin_8
///////////////
//#define PS_DOUTRDY_GPIO_PORT        GPIOB
//#define PS_DOUTRDY_GPIO_PIN   	    GPIO_Pin_9
///////////////
//#define A85_SCK_PWM_GPIO_PORT  	    GPIOB
//#define A85_SCK_PWM_GPIO_PIN   	    GPIO_Pin_10
///////////////
//#define LCD_A0_GPIO_PORT  	        GPIOB
//#define LCD_A0_GPIO_PIN   	        GPIO_Pin_12
///////////////
//#define LCD_CLK_GPIO_PORT  	        GPIOB
//#define LCD_CLK_GPIO_PIN   	        GPIO_Pin_13
///////////////
//#define LCD_RST_GPIO_PORT  	        GPIOB
//#define LCD_RST_GPIO_PIN   	        GPIO_Pin_14
///////////////
//#define LCD_SI_GPIO_PORT  	        GPIOB
//#define LCD_SI_GPIO_PIN   	        GPIO_Pin_15
///////////////
////PORT C                     				
//#define IPOL_0_GPIO_PORT  	        GPIOC
//#define IPOL_0_GPIO_PIN   	        GPIO_Pin_0
///////////////
//#define IPOL_1_GPIO_PORT  	        GPIOC
//#define IPOL_1_GPIO_PIN   	        GPIO_Pin_1
///////////////
//#define IPOL_2_GPIO_PORT  	        GPIOC
//#define IPOL_2_GPIO_PIN   	        GPIO_Pin_2
///////////////
//#define ADXL_XOUT_GPIO_PORT  	      GPIOC
//#define ADXL_XOUT_GPIO_PIN   	      GPIO_Pin_4
///////////////
//#define BAT_ZOND_GPIO_PORT  	      GPIOC
//#define BAT_ZOND_GPIO_PIN   	      GPIO_Pin_5
///////////////
//#define NIBP_VALVE_GPIO_PORT  	    GPIOC
//#define NIBP_VALVE_GPIO_PIN   	    GPIO_Pin_6
///////////////
//#define NIBP_PUMP_GPIO_PORT  	      GPIOC
//#define NIBP_PUMP_GPIO_PIN   	      GPIO_Pin_7
///////////////
//#define BKLED_CTRL_GPIO_PORT  	    GPIOC
//#define BKLED_CTRL_GPIO_PIN   	    GPIO_Pin_8
///////////////
//#define HIPWR_EN_GPIO_PORT  	    	GPIOC
//#define HIPWR_EN_GPIO_PIN   	    	GPIO_Pin_9
///////////////
//#define ADUM_EN_GPIO_PORT  	    		GPIOC
//#define ADUM_EN_GPIO_PIN   	    		GPIO_Pin_11
///////////////
////PORT D                     				
//#define EE_CS_GPIO_PORT  	    			GPIOD
//#define EE_CS_GPIO_PIN   	    			GPIO_Pin_3
///////////////
//#define ION_ACT_GPIO_PORT  	    		GPIOD
//#define ION_ACT_GPIO_PIN   	    		GPIO_Pin_8
///////////////
//#define LBO_GPIO_PORT  	    				GPIOD
//#define LBO_GPIO_PIN   	    				GPIO_Pin_9
///////////////
//#define ADXL_PWR_GPIO_PORT  	    	GPIOD
//#define ADXL_PWR_GPIO_PIN   	    	GPIO_Pin_10
///////////////
//#define BUZZER_GPIO_PORT  	    		GPIOD
//#define BUZZER_GPIO_PIN   	    		GPIO_Pin_13
///////////////
////PORT E                     				
//#define A85_ESW0_GPIO_PORT  	    	GPIOE
//#define A85_ESW0_GPIO_PIN   	    	GPIO_Pin_0
///////////////
//#define A85_ESW1_GPIO_PORT  	    	GPIOE
//#define A85_ESW1_GPIO_PIN   	    	GPIO_Pin_1
///////////////
//#define ANAP_EN_GPIO_PORT  	    		GPIOE
//#define ANAP_EN_GPIO_PIN   	    		GPIO_Pin_2
///////////////
//#define A85_ESW2_GPIO_PORT  	    	GPIOE
//#define A85_ESW2_GPIO_PIN   	    	GPIO_Pin_3
///////////////
//#define LMX_EN_GPIO_PORT  	    		GPIOE
//#define LMX_EN_GPIO_PIN   	    		GPIO_Pin_12
///////////////
//#define LMX_RST_GPIO_PORT  	    		GPIOE
//#define LMX_RST_GPIO_PIN   	    		GPIO_Pin_13
///////////////
//#define LMX_LSTAT_GPIO_PORT  	    	GPIOE
//#define LMX_LSTAT_GPIO_PIN   	    	GPIO_Pin_14
///////////////
//#define LMX_READY_GPIO_PORT  	    	GPIOE
//#define LMX_READY_GPIO_PIN   	    	GPIO_Pin_15
///////////////
#endif
////------------------------------------------------------------------------------------
#endif
