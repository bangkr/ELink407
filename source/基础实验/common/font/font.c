
#include <stdio.h>
#include <string.h>
#include "font.h"
#include "delay.h"

//font 暂时未使用
//str 字符串
//buf 字体数据
//off 从字符串取字符数量

extern unsigned char FontHZ16_Start[];
extern unsigned char FontHZ16_End[];

extern unsigned char FontAsc16_Start[];
extern unsigned char FontAsc16_End[];

unsigned char * FONT_GetDataGB2312(FontData *data, const unsigned char *str)
{
	unsigned int offset;
	if (str && data)
	{
		if (*str >= (unsigned char)0x80)
		{
			//hz
			unsigned int qu, wei;
			qu = *str - 0xa1;
			wei = *(str+1) - 0xa1;
			offset = (qu * 94 + wei) * 32;

			if (FontHZ16_Start + offset < FontHZ16_End)
			{
				data->str_bytes = 2;
				data->w = 16;
				data->h = 16;
				data->data = &(FontHZ16_Start[offset]);
				return &(FontHZ16_Start[offset]);
			}
		}
		else
		{
			//ascii
			data->str_bytes = 1;
			data->w = 8;
			data->h = 16;
			data->data = &(FontAsc16_Start[*str * 16]);
			return &(FontAsc16_Start[*str * 16]);
		}
	}
	return 0;
}

#if 0


USDOS 16*16 点阵字库，地址＝ [(区码-1)×94 ＋ 位码 － 1]×32
区码 ＝ 内码第一字节 － A0H
位码 ＝ 内码第二字节 － A0H

输入: hz 一个汉字
输出：data 点阵数据

bool GetHZ(unsigned char hz[2], unsigned char data[32])
{
    int Offset, qu, wei;
    FILE *fp;

    if ((fp = fopen("c:\\ucdos\\hzk16", "rb"))==NULL)
    {
        ::MessageBox(NULL, "Open hzk16 error", "error", 0);
        return false;
    }

    if (strlen(hz) < 2)
    {
        return false;
    }

    qu = hz[0] - 0xa0;
    wei = hz[1] - 0xa0;
    if (qu < 0 || wei < 0)
    {
        return false;
    }
    Offset = ((qu - 1) * 94 + wei - 1) * 32;
    fseek(fp, Offset, SEEK_SET);
    fread(data, 1, 32, fp);
    fclose(fp);
    return true;
}
//---------------------------------------------------------------------------

void __fastcall TForm1::Button1Click(TObject *Sender)
{
    int n = 0;
    unsigned char  hz[2], data[32];
    char s[256];

    if (Edit1->Text.Length() < 2)
    {
        return;
    }
    memcpy(hz, Edit1->Text.c_str(),2);

    //取点阵数据
    if (GetHZ(hz, data))
    {
        //以16进制显示数据
        for (int i = 0; i < 4; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                sprintf(s + j * 5, "%02XH, ", data[i * 8 + j]);
            }
            Memo1->Lines->Add(s);
        }
    }

}
#endif

#if 0

void LCD_SetFont(sFONT *fonts,uint8_t Mode)
{
	LCD_Currentfonts = fonts;
	LCD_Currentfonts->Mode=Mode;
}

/************************************************************************************************
 * 描述   : 获取当前字体.
 * 参数   : 无
 * 返回   : 当前正在使用的字体.
 * 说明   :
 *************************************************************************************************/
sFONT *LCD_GetFont(void)
{
	return LCD_Currentfonts;
}

/************************************************************************************************
 * 描述   : 指定位置写一个字符
 * 参数   : X,Y： X坐标,Y坐标.
 *          char： 指向字符ascii代码的指针
 * 返回   : 无
 * 说明   :
 *************************************************************************************************/
void LCD_ShowChar(uint16_t x,uint8_t y,uint8_t Char)
{   
	uint8_t temp,dy,dx;//位置偏量

	if(Char>31)		   
		Char=Char-32;//得到偏移后的值
	else  //其他字符忽略，设为空格
		Char=0;

	for(dy=0;dy<LCD_Currentfonts->Height;dy++)
	{
		temp=LCD_Currentfonts->table[Char][dy];//调用字体
		for(dx=0;dx<LCD_Currentfonts->Height/2;dx++)
		{                 
			if(temp&0x01)
			{ 
				LCD_SetCursor(x+dx,y+dy);//设置光标位置 
				LCD_WriteRAM_Prepare(); 
				LCD_WriteRAM(TextColor);
			}
			else
				if(LCD_Currentfonts->Mode)
				{
					LCD_SetCursor(x+dx,y+dy);//设置光标位置 
					LCD_WriteRAM_Prepare(); 
					LCD_WriteRAM(BackColor);
				}     
			temp>>=1; 
		}
	}

}
/************************************************************************************************
 * 描述   : 在LCD上显示一个字符(字符点阵是一维数组),这里只用于显示汉字
 * 参数   : _Xpos,_Ypos：X坐标,Y坐标.
 *          width：点阵的宽度，字符是8，汉字是16
 *          ptr:   指向字符点阵的指针
 * 返回   : 无
 * 说明   :
 *************************************************************************************************/
#if _USE_HZK16
static void TFT_DispChar(uint16_t _Xpos, uint16_t _Ypos, const uint8_t *c, uint8_t _width)
{
	uint32_t height = 0, i = 0;
	uint16_t x=0;
	uint8_t Yaddress;
	Yaddress = _Ypos;

	LCD_SetCursor(_Xpos, _Ypos);
	if (LCD_Currentfonts->Mode)//判断是否叠加
	{
		for (height = 0; height < 16; height++)  //字符高度
		{
			uint16_t x = _Xpos;
			for (i = 0; i < _width; i++)	  //字符宽度
			{
				LCD_SetCursor(x, Yaddress);
				LCD_WriteRAM_Prepare();

				if ((c[height * (_width / 8) + i / 8] & (0x80 >> (i % 8))) == 0x00)
				{
					LCD_WriteRAM(BackColor);
				}
				else
				{
					LCD_WriteRAM(TextColor);
				}
				x++;
			}
			Yaddress++;			
		}
	}
	else
	{
		for (height = 0; height < 16; height++)   //字符高度
		{
			x= _Xpos;

			for (i = 0; i < _width; i++)	   //字符宽度
			{
				if ((c[height * (_width / 8) + i / 8] & (0x80 >> (i % 8))) != 0x00)
				{
					LCD_SetCursor(x, Yaddress);
					LCD_WriteRAM_Prepare();
					LCD->LCD_RAM = TextColor;
				}
				x++;
			}
			Yaddress++;
		}
	}
}

#endif  

/************************************************************************************************
 * 描述   : 显示一行字符.
 * 参数   : Line,Column：行与列位置.
 *          *ptr: 指向要显示的字符串的指针.
 * 返回   : 无
 * 说明   :
 *************************************************************************************************/
uint16_t LCD_DisplayStringLine(uint8_t Line,uint16_t Column, uint8_t *ptr)
{
#if _USE_HZK16
	uint32_t address;
	uint16_t HZK16[16];
#endif
	uint16_t refcolumn =Column;
	uint8_t refLine =Line;
	uint8_t *P=ptr;
#if _USE_HZK16
	uint8_t code1,code2;
#endif

	while(*P++!='\0');P--;//得到字符串的结束指针

	if(refLine>=LCD_Pixel_Height)
	{  
		return (P-ptr);//返回未能显示的字符数
	}
	while (*ptr!='\0')
	{
#if _USE_HZK16	
		code1=*ptr;			     
		if(code1<0x80)	//判断是汉字代码，还是ascii代码 
		{
#endif
			if(refcolumn>=LCD_Pixel_Width)
			{	
				refcolumn=0;
				refLine+=LCD_Currentfonts->Height;//换行
				if(refLine>=LCD_Pixel_Height)
				{  
					return (P-ptr);//返回未能显示的字符数
				} 
			}
			if((*ptr!=13)&&(*ptr!=10))
			{  	
				LCD_ShowChar(refcolumn, refLine, *ptr);
				refcolumn += LCD_Currentfonts->Width;
			}
			else if(*ptr==13)//换行符号
			{
				refcolumn=0;
				refLine+=LCD_Currentfonts->Height;//换行
				if(refLine>=LCD_Pixel_Height)
				{  
					return (P-ptr);//返回未能显示的字符数
				} 
			}
			else if(*ptr==10) //汉字显示中的空两格
			{
				LCD_ShowChar(refcolumn, refLine, 32);
				refcolumn += LCD_Currentfonts->Width;
				LCD_ShowChar(refcolumn, refLine, 32);
				refcolumn += LCD_Currentfonts->Width;
			}
#if  _USE_HZK16
		}
		//汉字内码
		else
		{
			code2 = *++ptr;
			if(code2=='\0')
				return 1;//有半个汉字没有显示
			/*---------得到该汉字字模数据------------------------------*/
			//计算16点阵汉字点阵地址addreSS=[(code1-0xa1)*94 + (code2-0xa1)]*32	
			address =((code1-0xa1) * 94 + (code2-0xa1))<<5;
			if(address<0x41600)//防止地址越界
			{  	       
				SRAM_ReadBuffer(HZK16, address, 16);
			}
			else//越界填0
			{ 		
				for(code1=0;code1<16;code1++)
					HZK16[code1]=0x00;
			}
			/*--------------------------------------------------------*/
			if(refcolumn>=LCD_Pixel_Width)
			{	
				refcolumn=0;
				refLine+=LCD_Currentfonts->Height;//换行
				if(refLine>=LCD_Pixel_Height)
				{  
					return (P-ptr+1);//返回未能显示的字符数
				} 
			}
			TFT_DispChar(refcolumn, refLine, (const uint8_t *)HZK16, 16);
			refcolumn +=16;
		} 
#endif
		ptr++;
	}
	return 0;
}

/************************************************************************************************
 * 描述   : 显示{数字数值(0~4294967295)}.
 * 参数   : Line,Column：行与列位置.
 *          num:数值(0~4294967295).
 * 返回   : 无
 * 说明   :
 *************************************************************************************************/	 
void LCD_DisplayNum(uint8_t Line,uint16_t Column, uint32_t num)
{ 
	uint32_t temp=num;       	
	uint16_t refcolumn =Column;
	uint8_t  refLine =Line,len=0; 
	//计算数值长度
	do
	{
		temp = temp/10;
		len++;
	}
	while(temp); 
	len--;
	/********************************/
	refcolumn +=len*LCD_Currentfonts->Width;//从后往前显示
	//显示数字
	do
	{ 
		temp=num%10;
		LCD_ShowChar(refcolumn,refLine,temp+'0'); 
		refcolumn-=LCD_Currentfonts->Width;
		num=num/10;
	}
	while(len--);					   
}

/************************************************************************************************
 * 描述   : 显示浮点数.
 * 参数   : Line,Column：行与列位置.
 *          num:数值.
 * 返回   : 无
 * 说明   :整数部分小于2147483647大于-2147483648，
 *         但整数部分大的一定值后小数部分不准确了，小数位保留5位
 *************************************************************************************************/
#if _USE_Float

void LCD_Display_FloatNum(uint8_t Line,uint16_t Column, double num)
{
	uint64_t Temp;
	uint32_t Temp1;
	uint8_t len=0;
	if(num<0)//负数
	{
		num=-num;
		LCD_DisplayStringLine(Line,Column,"-");
		len++;
	}
	Temp=(uint64_t)(num*100000);//化为整数
	//整数部分显示
	Temp1=(uint32_t)(Temp/100000);
	//显示整数部分
	LCD_DisplayNum(Line,Column+Column(len),Temp1);
	//计算整数部分长度
	do
	{
		Temp1 = Temp1/10;
		len++;
	}
	while(Temp1);
	//小数部分
	Temp1=(uint32_t)(Temp%100000);
	//去掉末尾多余0
	while(Temp1%10==0)
	{
		Temp1=Temp1/10;
	}
	//显示小数点
	if(Temp1!=0)//判断是否是小数，是则显示
	{
		LCD_DisplayStringLine(Line,Column+Column(len),".");
		len++;
		LCD_DisplayNum(Line,Column+Column(len),Temp1);
	}
}

#endif

#if _USE_GRAPH

/************************************************************************************************
 * 描述   : 在LCD上显示一个bin格式的图片.
 * 参数   : ptr ： 图片点阵指针.
 *          ptr ： 图片数据大小.
 * 返回   : 无
 * 说明   :
 *************************************************************************************************/
void LCD_WeBMP_SIZE(uint16_t *ptr,uint32_t SIZE)
{
	uint32_t index;

	for (index = 0; index < SIZE; index++)
	{
		LCD_WriteRAM(*ptr++)
	}
}

#endif

#endif

