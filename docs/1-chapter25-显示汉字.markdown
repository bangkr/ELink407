# LCD 显示汉字实验 #

LCD驱动成功后，再考虑一个问题，如果让LCD上能显示文本信息？我们可以通过代码实现在屏幕的坐标位置显示我们想要的点阵信息，通过多个像数点让屏幕上的点阵显示成像一个字母，然后再组成文本信息。这样想的思路是对的，实际上不需要我们自己建立字符的点阵，而是通过调用现成的点阵字库，实现文本的显示，本案例将实现在LCD上显示中文文本信息。

## 实验原理与基础知识 ##
计算机中的图像类型分为两种：位图和矢量图。我们用点阵表示图形叫位图，用数学公式来表达直线或圆的方式就是矢量方法。

字库也分为矢量字库和点阵字库，矢量字体中每一个字形是通过数学曲线来描述的，它包含了字形边界上的关键点，连线的导数信息等，字体的渲染引擎通过读取这些数学矢量，然后进行一定的数学运算来进行渲染。这类字体的好处是字体可以无限放大而不产生变形。点阵从本质上讲就是单色位图，它使用一个比特来表示一个点，如果这个比特为0，表示某个位置没有点，如果为1表示某个位置有点。毋庸置疑，矢量字库的显示效果是最好的，但是点阵字库运算量少，简单并且是免费的。所以在很多场合，特别是嵌入式产品，点阵字库仍然是产品开发的首选。本案例中我们介绍大家点阵字库的使用。

点阵字符是如何表示，其数据又是怎样存储。比如说 16*16 的点阵，也就是说每一行有16个点，由于一个点使用一个比特来表示，如果这个比特的值为1，则表示这个位置有点，如果这个比特的值为0，则表示这个位置没有点，那么一行也就需要16个比特，而8个比特就是一个字节，也就是说，这个点阵中，一行的数据需要两个字节来存放。

第一行的前八个点的数据存放在点阵数据的第一个字节里面，第一行的后面八个点的数据存放在点阵数据的第二个字节里面，第二行的前八个点的数据存放在点阵数据的第三个字节里面...，然后后面的就以此类推了。这样我们可以计算出存放一个点阵总共需要32个字节（2*16）。看看下面这个图形化的例子：
           
![](img/chapter25/1.1.1.png) 

像这样的文字点阵数据就是被保存在点阵字库文件中，那么显示该字符的过程就是获取该字符点阵数据的过程了，下面我们来说明点阵字符的获取。

汉字在汉字库中的具体位置计算公式为：94*(区号-1)+位号-1。减1是因为数组是以0为开始而区号位号是以1为开始的。这仅为以汉字为单位该汉字在汉字库中的位置，那么，如何得到以字节为单位得到该汉字在汉字库中的位置呢？只需乘上一个汉字字模占用的字节数即可，即：(94*(区号-1)+位号-1)*一个汉字字模占用字节数，而按每种汉字库的汉字大小不同又会得到不同的结果。以 16*16点阵字库为例，计算公式则为：(94*(区号-1)+(位号-1))*32。汉字库文该从该位置起的32字节信息即该字的点阵信息。


## 电路设计 ##

## 库函数和数据结构 ##
\基础实验\common\font\将目录下的 font.c，font_data.s 和font.h头文件加入工程中

        typedef struct 
        {
          int16_t FColor;       //字符颜色
          int16_t BColor;       //字符背景色
          int32_t Mode;         //背景色模式使能
        } DrawMode, *pDrawMode;
        
字符显示模式结构体

        typedef struct 
        {
            void * font;            // 暂时未使用
            unsigned int w;         //字符像数宽度
            unsigned int h;         //字符像数高度
            unsigned int str_bytes; //字符大小 汉字2个字节  ascii 为1个字节
            unsigned char *data;    //字符在字库中的偏移
        } FontData, *pFontData;
字符信息结构体

        unsigned char * FONT_GetDataGB2312(FontData *data, const unsigned char *str);
        
根据字符编码，获取字符属性，设置字符属性结构体的值

## 软件设计、开发 ##

显示一个字符 x, y 字符显示坐标; Font  字体信息;ShowMode  显示模式

        void LCD_DrawFont(uint16_t x, uint16_t y, FontData *Font, DrawMode *ShowMode)
        {   
            int row,col;
            int i;
            uint8_t temp;

            if (Font && Font->data && ShowMode)
            {
                for(row=0; row < Font->h; row++)
                {
                    for(col=0; col < Font->w / 8; col++)
                    {                 
                        temp=Font->data[row * Font->w / 8 + col];
                        for (i=0; i < 8; i++)
                        {
                            if(temp & 0x80)
                            {
                                //设置光标位置 
                                LCD_SetCursor(x+col*8+i, y+row);
                                LCD_WriteRAM_Prepare(); 
                                LCD_WriteRAM(ShowMode->FColor);
                            }
                            else
                            {
                                if(ShowMode->Mode & DRAW_BACK_COLOR)
                                {
                                    //设置光标位置 
                                    LCD_SetCursor(x+col*8+i, y+row);
                                    LCD_WriteRAM_Prepare(); 
                                    LCD_WriteRAM(ShowMode->BColor);
                                }
                            }
                            temp<<=1; 
                        }
                    }
                }
            }
        }

显示字符串 x, y 字符显示坐标; Str 字符串;Mode  显示模式

        void LCD_DrawString(uint32_t x, uint32_t y, const uint8_t *Str, DrawMode *Mode)
        {
            FontData Font;
            uint32_t x0;

            x0 = x;

            while(*Str)
            {
                if (FONT_GetDataGB2312(&Font, Str))
                {
                    LCD_DrawFont(x0, y, &Font, Mode);
                    x0 += Font.w;
                    if (x0 >= LCD_W)
                    {
                        x0 = x;
                        y += Font.h;
                        if (y >= LCD_H)
                        {
                            break;
                        }
                    }
                    Str += Font.str_bytes;
                }
                else
                {
                    Str++;
                }
            }
            return ;
        }

显示一个整数，x, y 显示起始坐标 ,Num整数值 ,Mode  显示模式

        void LCD_DisplayNum(uint32_t x, uint32_t y, uint32_t Num, DrawMode *Mode)
        { 
            char  buf[16];
            sprintf(buf, "%d", Num);
            LCD_DrawString(x, y, (uint8_t *)buf, Mode);
        }
        
编写main 函数进行测试

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
            }
        }

## 软件下载、测试验证 ##


