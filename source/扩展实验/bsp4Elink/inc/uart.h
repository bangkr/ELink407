#ifndef __UART_H
#define __UART_H 

void init_uart(void);

void USART_DMA_TxConfig(unsigned int  *BufferSRC, unsigned int  BufferSize);
void USART_DMA_RxConfig(unsigned int  *BufferDST, unsigned int  BufferSize);
#endif
