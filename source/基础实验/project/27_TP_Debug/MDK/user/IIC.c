
#include "IIC.h"
#include "uart.h"
#include "stdio.h"
__IO uint32_t  I2CTimeout = I2C_Open_LONG_TIMEOUT;

static void I2C_delay(uint16_t cnt);
uint32_t I2C_TIMEOUT_UserCallback(void);
//static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr);

/*******************************************************************************
* Function Name  : I2C_Configuration
*******************************************************************************/
void I2C_Configuration(void)
{
	I2C_InitTypeDef  I2C_InitStructure;
	GPIO_InitTypeDef  GPIO_InitStructure; 
	
	RCC_AHB1PeriphClockCmd(Open_I2C_SDA_GPIO_CLK,ENABLE);
	RCC_AHB1PeriphClockCmd(Open_I2C_SCL_GPIO_CLK,ENABLE);
	
	RCC_APB1PeriphClockCmd(Open_I2C_CLK,ENABLE);
	
	GPIO_PinAFConfig(Open_I2C_SDA_GPIO_PORT, Open_I2C_SDA_SOURCE, Open_I2C_SDA_AF);
	GPIO_PinAFConfig(Open_I2C_SCL_GPIO_PORT, Open_I2C_SCL_SOURCE, Open_I2C_SCL_AF);
	
	GPIO_InitStructure.GPIO_Pin =  Open_I2C_SDA_PIN | Open_I2C_SCL_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_OD;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_NOPULL; //PB6/PB7
	GPIO_Init(GPIOB, &GPIO_InitStructure);
		
	I2C_DeInit(Open_I2C);
	I2C_InitStructure.I2C_Mode = I2C_Mode_I2C;
	I2C_InitStructure.I2C_DutyCycle = I2C_DutyCycle_2;
	I2C_InitStructure.I2C_OwnAddress1 = 0x0;
	I2C_InitStructure.I2C_Ack = I2C_Ack_Enable;
	I2C_InitStructure.I2C_AcknowledgedAddress = I2C_AcknowledgedAddress_7bit;
	I2C_InitStructure.I2C_ClockSpeed = I2C_SPEED;
	I2C_Init(Open_I2C, &I2C_InitStructure);
	
	I2C_Cmd(Open_I2C, ENABLE);
	I2C_AcknowledgeConfig(Open_I2C, ENABLE);
	
	 /* Send STOP condition */
  //I2C_GenerateSTOP(Open_I2C, ENABLE);	
	
}
 
/*******************************************************************************
* Function Name  : I2C_delay
*******************************************************************************/
static void I2C_delay(uint16_t cnt)
{
	while(cnt--);
}

/**
  * @brief  Basic management of the timeout situation.
  */
uint32_t I2C_TIMEOUT_UserCallback(void)
{
	/* Block communication and all processes */
	printf("\r\n The I2C communication is corrupted \r\n");
	//while (1);
	I2C_delay(10000);
	printf("try restart------------------------------------------------------------------------------\r\n");
	printf("try restart------------------------------------------------------------------------------\r\n");
	return 1;
}

#if 0
/*******************************************************************************
* Function Name  : I2C_AcknowledgePolling
* Description    : 
* Input          : I2C_TypeDef * , uint8_t
*******************************************************************************/
static void I2C_AcknowledgePolling(I2C_TypeDef *I2Cx,uint8_t I2C_Addr)
{
  vu16 SR1_Tmp;
  do
  {   
    I2C_GenerateSTART(I2Cx, ENABLE);

    SR1_Tmp = I2C_ReadRegister(I2Cx, I2C_Register_SR1);

	I2C_Send7bitAddress(I2Cx, I2C_Addr, I2C_Direction_Transmitter);

  }while(!(I2C_ReadRegister(I2Cx, I2C_Register_SR1) & 0x0002));
  
  I2C_ClearFlag(I2Cx, I2C_FLAG_AF);
    
  I2C_GenerateSTOP(I2Cx, ENABLE);
}
#endif


/*******************************************************************************
* Function Name  : I2C_Read
*******************************************************************************/
uint8_t I2C_Read(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t *buf,uint16_t num)
{
	if(num==0){
		return 1;
	}
	
	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY));
		
	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	I2C_GenerateSTART(I2Cx, ENABLE);
	
	while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_MODE_SELECT));

	I2C_Send7bitAddress(I2Cx,  I2C_Addr, I2C_Direction_Receiver);
  
	while (!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_RECEIVER_MODE_SELECTED)){}		
 
	while (num)
	{
		if(num==1){
			I2C_AcknowledgeConfig(I2Cx, DISABLE);
    		I2C_GenerateSTOP(I2Cx, ENABLE);
		}
		while(!I2C_CheckEvent(I2Cx, I2C_EVENT_MASTER_BYTE_RECEIVED)){}
		
		*buf = I2C_ReceiveData(I2Cx);
		buf++;
		/* Decrement the read bytes counter */
		num--;
	}

	I2C_AcknowledgeConfig(I2Cx, ENABLE);

	return 0;
}	

/*******************************************************************************
* Function Name  : I2C_WriteOneByte
*******************************************************************************/
uint8_t I2C_WriteOneByte(I2C_TypeDef *I2Cx,uint8_t I2C_Addr,uint16_t addr,uint8_t value)
{

	while(I2C_GetFlagStatus(I2Cx, I2C_FLAG_BUSY)){
    	if((I2CTimeout--) == 0) 
				return I2C_TIMEOUT_UserCallback();
	}
	
	/* Send STRAT condition */
	I2C_GenerateSTART(I2C1, ENABLE);

	/* Test on EV5 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_MODE_SELECT));  

	/* Send EEPROM address for write */
	I2C_Send7bitAddress(I2C1, I2C_Addr, I2C_Direction_Transmitter);
  
	/* Test on EV6 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_TRANSMITTER_MODE_SELECTED));
  
	I2C_AcknowledgeConfig(I2Cx, DISABLE);
	
	/* Send the byte to be written */
	I2C_SendData(I2C1, value); 
	/* Test on EV8 and clear it */
	while(!I2C_CheckEvent(I2C1, I2C_EVENT_MASTER_BYTE_TRANSMITTED));
  
	/* Send STOP condition */
	I2C_GenerateSTOP(I2C1, ENABLE);  
  
	I2C_delay(1000);

	return 0;
}


/*******************************************************************************
* Function Name  : I2C_Write
*******************************************************************************/
uint8_t I2C_Write(I2C_TypeDef *I2Cx, uint8_t I2C_Addr, uint16_t addr, uint8_t *buf, uint16_t num)
{
	uint8_t err=0;
	while(num--)
	{
		if(I2C_WriteOneByte(I2Cx, I2C_Addr,addr++,*buf))
		{
			err++;
		}
		buf++;
	}
	if(err)
		return 1;
	else 
		return 0;	
}


void I2C_Erase(void)
{
	uint16_t i;
	for (i = 0; i <8*1024; i++)
	{
		I2C_WriteOneByte(Open_I2C,ADDR_ZT2083,i,0xFF);
	}
}

void I2C_EE_WaitEepromStandbyState(void)      
{
	vu16 SR1_Tmp = 0;
	do
	{
		/* Send START condition */
		I2C_GenerateSTART(I2C1, ENABLE);
		/* Read I2C1 SR1 register */
		SR1_Tmp = I2C_ReadRegister(I2C1, I2C_Register_SR1);
		/* Send EEPROM address for write */
		I2C_Send7bitAddress(I2C1, ADDR_ZT2083, I2C_Direction_Transmitter);
	}while(!(I2C_ReadRegister(I2C1, I2C_Register_SR1) & 0x0002));
  
	/* Clear AF flag */
	I2C_ClearFlag(I2C1, I2C_FLAG_AF);
    /* STOP condition */    
    I2C_GenerateSTOP(I2C1, ENABLE); 
}
/*end*/
