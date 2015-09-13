
#include "stm32f4xx.h"

#include "stm32f4xx_it.h"
#include "IIC.h"

#include "tp.h"
#include "lcd.h"

#include "uart.h"

#include <stdio.h>
#include <string.h>


NVIC_InitTypeDef   NVIC_InitStructure12;

static void TpDelay(uint16_t cnt)
{
	while(cnt--);
}

#if 0
static void TpDisableInt(void)
{
	uint8_t WriteBuffer[4]={0};
	WriteBuffer[0] = 0xCC;
	I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );	
}
static void TpEnableInt(void)
{
	uint8_t WriteBuffer[4]={0};
	uint8_t ReadBuffer[4]={0};
		
	WriteBuffer[0] = 0xC8;
	I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );	
	
	I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, ReadBuffer, 2 );
}
#endif



void TpGpioIntDisable(void)
{
	//打开中断
	
	NVIC_InitStructure12.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure12.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占式优先级
    NVIC_InitStructure12.NVIC_IRQChannelSubPriority = 0x01;//指定从优先级
    NVIC_InitStructure12.NVIC_IRQChannelCmd = DISABLE;
	
    NVIC_Init(&NVIC_InitStructure12);
}

void TpGpioIntEnable(void)
{
	//打开中断
	NVIC_InitStructure12.NVIC_IRQChannel = EXTI15_10_IRQn;
    NVIC_InitStructure12.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占式优先级
    NVIC_InitStructure12.NVIC_IRQChannelSubPriority = 0x01;//指定从优先级
    NVIC_InitStructure12.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure12);	
}



#define TP_POINT_NUM (1)


void TpGetXY(tp_point_t *tpoint)
{
	//tp_point_t tp_point[TP_POINT_NUM];
    uint8_t WriteBuffer[4]={0};
    uint8_t ReadBuffer[4]={0};
    int i;     

	TpGpioIntDisable();

    I2C_Configuration();
		
//	I2C_Cmd(Open_I2C, ENABLE);
		

        WriteBuffer[0] = 0xCC;	
        I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );
        I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, ReadBuffer, 2 );	
      
        tpoint->x = (((ReadBuffer[0]<<8) | ReadBuffer[1]) >> 4);

        WriteBuffer[0] = 0xDC;
        I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );
        I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, ReadBuffer, 2 );

        tpoint->y = (((ReadBuffer[0]<<8) | ReadBuffer[1]) >> 4);	 

        WriteBuffer[0] = 0xE0;
        I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );
        I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, ReadBuffer, 2 );

        tpoint->z1 = (((ReadBuffer[0]<<8) | ReadBuffer[1]) >> 4);     

//      printf("\n\rz1=%d", tp_point[i].z1);

#if 0
        WriteBuffer[0] = 0xF0;
        I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, WriteBuffer, 1 );
        I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, ReadBuffer, 2 );

        tp_point[i].z2 = (((ReadBuffer[0]<<8) | ReadBuffer[1]) >> 4);     

       // printf("\n\rz2=%d", tp_point[i].z2);
#endif

  

	//I2C_Cmd(Open_I2C, DISABLE);
	TpGpioIntEnable();

 //printf("\r\nZT2083 X=%4d, Y=%4d", tpoint->x, tpoint->y);

	
}


void EXTILine_Config(uint16_t gpio_pin,uint32_t exti_line, int irqn)
{	
    GPIO_InitTypeDef   GPIO_InitStructure;
    EXTI_InitTypeDef   EXTI_InitStructure;

    /* Enable GPIOE clock */
    RCC_AHB1PeriphClockCmd(RCC_AHB1Periph_GPIOD, ENABLE);
    
    /* Configure PD12  pin as input floating */
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
    GPIO_InitStructure.GPIO_Pin = gpio_pin;
    GPIO_Init(GPIOD, &GPIO_InitStructure);
    
    /* Configure EXTI Line */
    EXTI_InitStructure.EXTI_Line = exti_line;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;  
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set EXTI Line Interrupt*/
    NVIC_InitStructure12.NVIC_IRQChannel = irqn;
    NVIC_InitStructure12.NVIC_IRQChannelPreemptionPriority = 0x01;//抢占式优先级
    NVIC_InitStructure12.NVIC_IRQChannelSubPriority = 0x01;//指定从优先级
    //NVIC_InitStructure12.NVIC_IRQChannelCmd = ENABLE;
    NVIC_InitStructure12.NVIC_IRQChannelCmd = DISABLE;
    NVIC_Init(&NVIC_InitStructure12);
}

volatile unsigned int pen_flag=0;
//static tp_point_t tp_xy;

void EXTI15_10_IRQHandler(void)
{
    uint8_t res;
    
    if(EXTI_GetITStatus(EXTI_Line12) != RESET)
    {
        //禁止中断
        NVIC_InitStructure12.NVIC_IRQChannelCmd = DISABLE;
        NVIC_Init(&NVIC_InitStructure12);
		
		TpDelay(50000);	
        
		res=GPIO_ReadInputDataBit(GPIOD, GPIO_Pin_12); 		 
        if(!res) 
        {
            pen_flag = 1;
            //printf("\r\n TP IRQ pen_flag = %d\r\n", pen_flag);
//            TpGetXY(&tp_xy);
        }	
        else 
        {			 
            	 
            pen_flag = 0;		 
        }

        /* Clear the EXTI line 12 pending bit */      
        EXTI_ClearFlag(EXTI_Line12);   
        EXTI_ClearITPendingBit(EXTI_Line12);

        //打开中断
        NVIC_InitStructure12.NVIC_IRQChannelCmd = ENABLE;
        NVIC_Init(&NVIC_InitStructure12);
        
    }
}



void TpInit(void)
{
	
	uint8_t I2CBuffer[4]={0};	

	I2CBuffer[0] = 0xC8;
	I2C_Write(Open_I2C, ADDR_ZT2083, 0x0, I2CBuffer, 1 );
	I2C_Read(Open_I2C, ADDR_ZT2083, 0x0, I2CBuffer, 2 );	

	TpDelay(10);

	EXTILine_Config(GPIO_Pin_12, EXTI_Line12, EXTI15_10_IRQn);
  
	/* Enable SYSCFG clock */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_SYSCFG, ENABLE);
		
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOD, EXTI_PinSource12); 
	 

	EXTI_ClearFlag(EXTI_Line12);        
	EXTI_ClearITPendingBit(EXTI_Line12);

	//打开中断
    NVIC_InitStructure12.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure12);

    //TpDelay(100);   

}


/*end*/
