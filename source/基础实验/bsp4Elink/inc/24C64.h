
#ifndef __24C64_H
#define __24C64_H

/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx.h"
					  
#define ADDR_24L64		0xA0
#define I2C_PAGESIZE	4


/**
 * @brief Definition for connected to I2C1
 */
#define I2C                        	I2C1
#define I2C_CLK                    	RCC_APB1Periph_I2C1

#define I2C_SDA_PIN                 	GPIO_Pin_7
#define I2C_SDA_GPIO_PORT           	GPIOB
#define I2C_SDA_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define I2C_SDA_SOURCE              	GPIO_PinSource7
#define I2C_SDA_AF                  	GPIO_AF_I2C1

#define I2C_SCL_PIN                 	GPIO_Pin_6
#define I2C_SCL_GPIO_PORT           	GPIOB
#define I2C_SCL_GPIO_CLK            	RCC_AHB1Periph_GPIOB
#define I2C_SCL_SOURCE              	GPIO_PinSource6
#define I2C_SCL_AF                  	GPIO_AF_I2C1

#define I2C_SPEED               100000



/* Maximum Timeout values for flags and events waiting loops. These timeouts are
   not based on accurate values, they just guarantee that the application will 
   not remain stuck if the I2C communication is corrupted.
   You may modify these timeout values depending on CPU frequency and application
   conditions (interrupts routines ...). */   
#define I2C_Open_FLAG_TIMEOUT         ((uint32_t)0x1000)
#define I2C_Open_LONG_TIMEOUT         ((uint32_t)(100 * I2C_Open_FLAG_TIMEOUT))

/* Private function prototypes -----------------------------------------------*/
void I2C_Configuration(void);
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t *buf,uint16_t num);
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t value);
uint8_t I2C_Write(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t *buf,uint16_t num);
void I2C_EE_WaitEepromStandbyState(void) ;

void I2C_Erase(void);
#endif 

/*********************************************************************************************************
      END FILE
*********************************************************************************************************/

