#ifndef __UART0_H
#define __UART0_H

 #include "xparameters.h"   //器件参数信息
 #include "xuartps.h"       //包含 PS UART 的函数声明
 #include "xil_printf.h"    //包含 print()函数
 #include "xscugic.h"       //包含中断的函数声明
 #include "stdio.h"         //包含 printf 函数的声明
 
#include "../include/crc.h"
#include "../include/data_dec.h"
#include "../include/pid.h"
#include "../include/ringbuffer.h"

/*******************************************************************************
 * 利用中断完成uart0控制器的数据传输，方便调试，比cpu轮询快很多，节省资源。
* <作者><日期><描述>
******************************************************************************/
// 串口外设配置ID
XUartPs Uart_Ps;                             // 串口驱动程序实例
XScuGic Intc;                                // 中断控制器驱动程序实例
#define UART_DEVICE_ID  XPAR_PS7_UART_0_DEVICE_ID       //串口设备 ID
//#define INTC_DEVICE_ID  XPAR_SCUGIC_0_DEVICE_ID         //中断控制器 ID
//串口中断是共享外设中断，他的终端号为59
#define UART_INT_IRQ_ID XPAR_XUARTPS_0_INTR            //串口中断 ID，需要根据fifo的空终端来实现数据传输


unsigned int bcuksetvalue, BrigesetValue, phasesetValue, phaseflag;
//采用中断
int uart_init(XUartPs *uart_ps);
void uart_intr_handler(void *call_back_ref);
int uart_intr_init(XScuGic *intc, XUartPs *uart_ps);
//采用轮询
int uart0_intr_init(XScuGic *intc, XUartPs *uart_ps);
void uart0_init(long baud);

void uart0_read_data();
void UtParam_Send(UT_PARAM* v_UtParam);
void Uart0DataSend(unsigned char *rec_data,unsigned char length);
unsigned char sum_check(unsigned char *buf, unsigned char num);
void SendMcuRegOneByte( unsigned char id, unsigned char val);
void SendDgusRegTwoByte( unsigned char id, unsigned int val);
void SendDgusRegMulByte(unsigned char id, unsigned char *val,unsigned char len);
void SendToSTM32BoardData(UT_PARAM* v_UtParam);
void SJANT_PC_USER_DAT(char *userbuf , uint8_t num);
typedef enum
{
  Header,
  Header1,
  Length,
  Cmd,
  Data,
  Sumcheck,
}RunLevel;

typedef enum
{
  Biology_Mode = 0x01,
  Normal_Mode = 0x02,
  Fast_Mode = 0x03,
}DebugmodeSet;

// 串口数据接收处理
typedef struct
{
	unsigned char DataTemp[40];
	unsigned char DataLength;        // 读取的位置索引
	unsigned char DataCmd;           // 写入的位置索引， 当前的无效的
	unsigned char DataBuffer[20];    // 数据结构的大小
	unsigned char DataCheckFlag;
}_UartHandle;

//串口处理数据状态机
typedef struct
{
	unsigned char open_Flag; //输出标记
	unsigned char now_level; //当前档位
	unsigned char Tune_Flag; //扫频标记
	unsigned char Ult_Output; //扫频标记
	unsigned char Debug_mode; //扫频标记
}_UT_Level;

_UT_Level UT_Level;


#endif
