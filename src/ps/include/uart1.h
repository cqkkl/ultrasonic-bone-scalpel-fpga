#ifndef __UART1_H
#define __UART1_H
#include "stdint.h"





void uart1_init(long baud);
void uart1_Send_data(unsigned char *rec_data, unsigned char length);
void SJANT_PC_USER_DAT1(int32_t *userbuf , uint8_t num);
void my_printf(const char *format, ...) ;
void uart1_Send_char(unsigned char c);
#endif

