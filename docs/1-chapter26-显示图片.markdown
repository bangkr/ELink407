# 显示图片实验 #
在LCD上显示图片

## 实验原理与基础知识 ##

LCD显示图片的过程是将图片上每一个像数信息写到显存中，所以首先我们需要将图片转换为图片数组，然后编写函数将数组中的像数信息写到显存。

## 电路设计 ##

## 库函数和数据结构 ##

## 软件设计、开发 ##

制作位图数组

利用tools目录中的image2lcd工具将elink_logo.bmp图片制作成数组文件logo.h，如图所示设置宽度和高度分别为126，400，设置输出灰度为24位，将图像数组到\基础实验\common\font\logo.h中。

![](img/chapter15/1.1.1.png) 

在lcd.c文件中添加LCD_Draw_Imag函数进行画图，同时在lcd.h文件中进行声明。

        void LCD_Draw_Imag(uint16_t x1,uint16_t y1,uint16_t weight,uint16_t hight,const u8 *img)    
        {                                                                                           
            uint32_t color,pix,i,j;                                      
            for(i=0; i<hight; i++)                                       
            {  	                  
                LCD_SetCursor(x1, y1+i);                 
                LCD_WriteRAM_Prepare();                  
                for (j = 0; j <weight; j++)              
                {                                        
                    color=0;                 
                    pix=3*(i*weight+j);      
                    color=img[pix]|(img[pix+1]<<8) |(img[pix+2]<<16);           
                    LCD_RAM_REG = color;            
                }                                        
            }                                            
        }                                                

编写main函数进行测试

        int main(void)
        {

            DrawMode Mode;

            Mode.FColor = 0xF800;
            Mode.BColor = 0x001F;
            Mode.Mode = 0;

            init_delay();
            init_lcd();	

            while(1)
            {
                LCD_Clear(Red);
                delay_ms(1000);

                LCD_Clear(White);
                delay_ms(1000);

                PutPixel(120, 200, Black);	
                delay_ms(1000);

                LCD_DrawString(0, 0, (uint8_t*)"你好 Elink407", &Mode);
                delay_ms(1000);

                LCD_DisplayNum(0, 80, 100, &Mode);
                delay_ms(1000);

                LCD_Draw_Imag(60,0,126,400, gImage_logo);
                delay_ms(1000);

            }
        }


## 软件下载、测试验证 ##


