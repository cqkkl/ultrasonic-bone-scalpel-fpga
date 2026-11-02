/*
 * uart1.c
 *
 *  Created on: 2023年4月27日
 *      Author: Administrator
 */


#include "../include/uart1.h"
#include "xparameters.h"
#include "xuartps.h"
#include "xil_printf.h"
#include "xscugic.h"
#include "stdio.h"
#include "../include/gic.h"
#include <stdint.h>
#include <stdarg.h>
#include <stdlib.h>
#include "../include/pid.h"

// 串口外设配置ID
#define UART_DEVICE_ID XPAR_PS7_UART_1_DEVICE_ID
#define UART_INT_IRQ_ID XPAR_XUARTPS_1_INTR // 串口中断ID
static XUartPs Uart_Ps;                            // 串口驱动程序实例


#define RX_BUFFER_SIZE 64 // 定义接收缓冲区大小

// 接收缓冲区
volatile u8 RxBuffer[RX_BUFFER_SIZE];
volatile int RxBufferHead = 0; // 缓冲区头部索引
volatile int RxBufferTail = 0; // 缓冲区尾部索引


/*******************************************************************************
 函数名: sum_check
 描述:   校验和
 输入参数:   数据和数据长度
 输出参数:   校验和结果
 函数返回值: 无
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/
static unsigned char sum_check(unsigned char *buf, unsigned char  num)
{
    unsigned char i = 0, sum = 0;
    for (i = 0; i < num; i++)
    {
        sum += buf[i];
    }
    return sum;
}

//环形缓冲区，判满是空一个数据位
int IsRxBufferFull() {
    return ((RxBufferHead + 1) % RX_BUFFER_SIZE) == RxBufferTail;
}

// 缓冲区是否为空的检查函数
int IsRxBufferEmpty() {
    return RxBufferHead == RxBufferTail;
}

// 向缓冲区添加数据的函数
void AddToRxBuffer(u8 data) {
    if (!IsRxBufferFull()) {
        RxBuffer[RxBufferHead] = data;
        RxBufferHead = (RxBufferHead + 1) % RX_BUFFER_SIZE;		//取模一直环形存入
    }
}

// 从缓冲区读取数据的函数
u8 ReadFromRxBuffer() {
    if (!IsRxBufferEmpty()) {
        u8 data = RxBuffer[RxBufferTail];
        RxBufferTail = (RxBufferTail + 1) % RX_BUFFER_SIZE;
        return data;
    }
    return -1; // 如果缓冲区为空，返回-1
}

// 假设有全局变量来存储解析结果和临时数据
volatile int parsedInt = 0;
volatile float parsedFloat = 0.0;
volatile char tempStr[32]; // 临时字符串，用于存储解析过程中的字符
volatile int tempStrIndex = 0; // 临时字符串的索引

// 立即处理接收到的数据
//void ProcessReceivedData(u8 data) {
//    static const char *intFlag = "I"; // 假设接收到'I'后跟随整数数据
//    static const char *floatFlag = "F"; // 假设接收到'F'后跟随浮点数数据
//    static char parseState = 0; // 0 - 等待标识符, 1 - 解析整数, 2 - 解析浮点数
//
//    // 根据当前的解析状态处理接收到的数据
//    switch (parseState) {
//        case 0: // 等待标识符
//            if (data == intFlag[0]) {
//                parseState = 1; // 准备解析整数
//                tempStrIndex = 0; // 重置临时字符串索引
//            } else if (data == floatFlag[0]) {
//                parseState = 2; // 准备解析浮点数
//                tempStrIndex = 0;
//            }
//            break;
//        case 1: // 解析整数
//            if (data >= '0' && data <= '9') {
//                tempStr[tempStrIndex++] = data; // 存储数字字符
//                if (tempStrIndex >= sizeof(tempStr) - 1) {
//                    tempStr[tempStrIndex] = '\0'; // 添加字符串结束符
//                    parsedInt = atoi((char *)tempStr); // 转换为整数
//                    parseState = 0; // 重置状态为等待标识符
//                }
//            } else {
//            	tempStr[tempStrIndex] = '\0'; // 添加字符串结束符
//            	parsedInt = atoi((char *)tempStr); // 转换为整数
//            	if(data == 'P'){
//					Set_Power_Pid_Parameter(parsedInt,0,0);
//					//my_printf("P:%d\r\n",parsedInt);
//				}else if(data == 'I'){
//					Set_Power_Pid_Parameter(0,parsedInt,0);
//				}else if(data == 'D'){
//					Set_Power_Pid_Parameter(0,0,parsedInt);
//				}
//                // 接收到非数字字符，结束解析
//                parseState = 0;
//            }
//            break;
//        case 2: // 解析浮点数
//            // 这里可以添加浮点数的解析逻辑，包括数字和小数点
//            // 示例中简化处理，只接收数字字符，实际应用中需要更复杂的逻辑
//            if (data >= '0' && data <= '9') {
//                tempStr[tempStrIndex++] = data;
//                if (tempStrIndex >= sizeof(tempStr) - 1) {
//                    tempStr[tempStrIndex] = '\0';
//                    parsedFloat = atof(tempStr); // 转换为浮点数
//                    parseState = 0; // 重置状态为等待标识符
//                }
//            } else if (data == '.') {
//                // 假设浮点数只包含一个小数点
//                if (tempStrIndex == 0 || tempStr[tempStrIndex - 1] != '.') {
//                    tempStr[tempStrIndex++] = data;
//                }
//            } else {
//                tempStr[tempStrIndex] = '\0';
//                parsedFloat = atof(tempStr); // 转换为浮点数
//				if(data == 'P'){
//					Set_Power_Pid_Parameter(parsedFloat,0,0);
//				}else if(data == 'I'){
//					Set_Power_Pid_Parameter(0,parsedFloat,0);
//				}else if(data == 'D'){
//					Set_Power_Pid_Parameter(0,0,parsedFloat);
//				}
//                // 接收到非预期字符，结束解析
//                parseState = 0;
//            }
//            break;
//    }
//}



/*******************************************************************************
 * 函数名:		uart_intr_handler
 * 描述: 		串口中断
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void uart1_intr_handler(void *call_back_ref) {
	XUartPs *uart_instance_ptr = (XUartPs *) call_back_ref;
	u32 rec_data = 0;
	u32 isr_status;

	// 读取中断ID寄存器，判断触发的是哪种中断
	isr_status = XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,XUARTPS_IMR_OFFSET);
	isr_status &= XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,XUARTPS_ISR_OFFSET);

	// 判断中断标志位RxFIFO是否触发
	if (isr_status & (u32) XUARTPS_IXR_RXOVR)
	{
		rec_data = XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,XUARTPS_FIFO_OFFSET);
		//ProcessReceivedData((u8)rec_data);	//立即处理数据

		// 清除中断标志
		XUartPs_WriteReg(Uart_Ps.Config.BaseAddress, XUARTPS_ISR_OFFSET,XUARTPS_IXR_RXOVR);
	}

	//发送数据返回
	//XUartPs_SendByte(uart_instance_ptr->Config.BaseAddress, rec_data);
}



void SJANT_PC_USER_DAT1(int32_t *userbuf , uint8_t num)
{
    uint8_t i=0,sum=0;
    uint8_t buf[100],cnt=0;

    buf[cnt++]=0xaa;
    buf[cnt++]=0x55;
    buf[cnt++]=32;
    for(i=0;i<num;i++)
    {
        buf[cnt++]=( *( (char *)(&userbuf[i]) + 3) );
        buf[cnt++]=( *( (char *)(&userbuf[i]) + 2) );
        buf[cnt++]=( *( (char *)(&userbuf[i]) + 1) );
        buf[cnt++]=( *( (char *)(&userbuf[i]) + 0) );
    }

    for(i=0;i<cnt;i++)
    	sum+=buf[i];
    buf[cnt++]=sum;
/*
    for(i=0;i<cnt;i++)
    {
        while((USART3->SR&0X40)==0);
        USART3->DR = buf[i];
    }
*/
    for (i = 0; i < cnt; i++) {
    		XUartPs_SendByte(Uart_Ps.Config.BaseAddress, buf[i]);
    	}

}

void uart1_Send_data(unsigned char *rec_data, unsigned char length) {
	unsigned char i;
	for (i = 0; i < length; i++) {
		XUartPs_SendByte(Uart_Ps.Config.BaseAddress, rec_data[i]);
	}
}

void uart1_Send_char(unsigned char c)
{
	XUartPs_SendByte(Uart_Ps.Config.BaseAddress,c);
}



/*******************************************************************************
 * 函数名:		uart0_init
 * 描述: 串口初始化
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void uart1_init(long baud)
{
    XUartPs_Config *uart_cfg;

    XScuGic* intc;
    uint32_t addr=0;

    /************************串口配置********************************************/
    uart_cfg = XUartPs_LookupConfig(UART_DEVICE_ID);
    XUartPs_CfgInitialize(&Uart_Ps, uart_cfg, uart_cfg->BaseAddress);

    // UART设备自检
    XUartPs_SelfTest(&Uart_Ps);

    XUartPs_SetOperMode(&Uart_Ps, XUARTPS_OPER_MODE_NORMAL);
    // 设置波特率:115200
    XUartPs_SetBaudRate(&Uart_Ps, baud);
    // 设置RxFIFO的中断触发等级
    XUartPs_SetFifoThreshold(&Uart_Ps, 1);
    // 设置超时O的中断触发等级
    XUartPs_SetRecvTimeout(&Uart_Ps, 8);
    //设置UART的中断触发方式
     XUartPs_SetInterruptMask(&Uart_Ps, XUARTPS_IXR_RXOVR);

    /************************串口中断配置********************************************/
 	//获取中断控制器地址
 	gic_get_object(&addr,*intc);

 	intc = (XScuGic*) addr;
 	// 为中断设置中断处理函数
 	XScuGic_Connect(intc, UART_INT_IRQ_ID,
 			(Xil_ExceptionHandler) uart1_intr_handler, (void *) (&Uart_Ps));

 	// 使能GIC中的串口中断
 	XScuGic_Enable(intc, UART_INT_IRQ_ID);
}

/***********************手搓printf()***********************/

void send_int(int num) {
    char buffer[12]; // 足够存储32位整数及其符号
    char isNegative = 0;
    if(num < 0)
    	isNegative = 1;
    else
    	isNegative = 0;

    int i = 0;

    if (isNegative) {
        num = -num; // 转换为正数
    }

    do {
        buffer[i++] = (num % 10) + '0'; // 计算当前最低位的数字并转换为字符
        num /= 10; // 移除当前最低位
    } while (num != 0);

    if (isNegative) {
        uart1_Send_char('-'); // 发送负号
    }

    while (i > 0) {
        uart1_Send_char(buffer[--i]); // 从最高位发送到最低位
    }
}

// 自定义的printf函数
void v_printf(const char *format, va_list args) {
    const char *ptr = format;
    while (*ptr) { // 遍历格式字符串
        if (*ptr == '%') {
            // 处理格式化指令
            switch (*++ptr) {
                case 'd': // 整型
                case 'i':
                	send_int(va_arg(args, int));	//发送整型
                	break;
                case 'c': // 字符
                    uart1_Send_char(va_arg(args, int)); // 发送字符参数
                    break;
                case '%':
                    uart1_Send_char('%');
                    break;
                default:
                    uart1_Send_char(*ptr); // 未知的格式化指令，直接发送
                    break;
            }
            ptr++; // 跳过已处理的格式化指令
        } else {
            uart1_Send_char(*ptr++); // 发送普通字符
        }
    }
}

// 自定义的 printf 函数
void my_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    v_printf(format, args);
    va_end(args);
}





