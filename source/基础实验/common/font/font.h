
/* 防止递归包含的定义-----------------------------------------------------------------------------*/
#ifndef __FONT_H
#define __FONT_H

#ifdef __cplusplus
 extern "C" {
#endif 

/* 包含 ------------------------------------------------------------------------------------------*/
#include "stm32f4xx.h"

typedef struct 
{
	void * font;
	unsigned int w;
	unsigned int h;
	unsigned int str_bytes;
	unsigned char *data;
} FontData, *pFontData;


unsigned char * FONT_GetDataGB2312(FontData *data, const unsigned char *str);


#ifdef __cplusplus
}
#endif


#endif /* __FONT_H */


