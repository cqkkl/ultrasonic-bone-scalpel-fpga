/*
 * private_timer.c
 *
 *  Created on: 2023年11月7日
 *      Author: yanghang
 */
#include "../include/private_timer.h"
#include "xil_exception.h"
#include "xscutimer.h"
#include "xscugic.h"
#include "xparameters.h"


#define TIMER_DEVICE_ID XPAR_XSCUTIMER_0_DEVICE_ID //定时器 ID
#define INTC_DEVICE_ID  XPAR_SCUGIC_SINGLE_DEVICE_ID//中断 ID
#define TIMER_IRPT_INTR XPAR_SCUTIMER_INTR         //定时器中断 ID
//0.2s 闪烁一次,0.2*1000_000_000/(1000/333) - 1 = 3F83C3F
//#define TIMER_LOAD_VALUE （XPAR_CPU_CORTEXA9_0_CPU_CLK_FREQ_HZ/2-1） //定时器装载值，这个是1s
#define TIMER_LOAD_VALUE 0x8213 //定时器装载值，私有定时器时钟是cpu时钟的一半，0.1ms
XScuGic Intc; //中断控制器驱动程序实例
XScuTimer Timer; //定时器驱动程序实例

//定时器初始化程序
int timer_init(XScuTimer *timer_ptr) {
	int status;
	//私有定时器初始化
	XScuTimer_Config *timer_cfg_ptr;
	timer_cfg_ptr = XScuTimer_LookupConfig(TIMER_DEVICE_ID);
	if (NULL == timer_cfg_ptr)
		return XST_FAILURE;
	status = XScuTimer_CfgInitialize(timer_ptr, timer_cfg_ptr,
			timer_cfg_ptr->BaseAddr);
	if (status != XST_SUCCESS)
		return XST_FAILURE;

	XScuTimer_LoadTimer(timer_ptr, TIMER_LOAD_VALUE); // 加载计数周期，私有定时器是从装载值递减到0时发出中断
	XScuTimer_EnableAutoReload(timer_ptr); // 设置自动装载模式

	return XST_SUCCESS;
}

unsigned int Count = 0;


//定时器中断处理程序，0.3s定时中断结束
void timer_intr_handler(void *CallBackRef) {
	//LED 状态,用于控制 LED 灯状态翻转
	static int led_state = 0;
	XScuTimer *timer_ptr = (XScuTimer *) CallBackRef;
	if (led_state == 0)
		led_state = 1;
	else
		led_state = 0;
	//向指定引脚写入数据： 0 或 1
	Count++;
	if(Count > 3000)//100us*3000= 300ms
	{
	 Count = 0;
	 // xil_printf("intr_handler\n\r");
	}
	//清除定时器中断标志
	XScuTimer_ClearInterruptStatus(timer_ptr);
}



/********************************************************************
中断控制器的使用，主要分为几个步骤：
1、初始化中断控制器 GIC 
2、初始化中断异常
3、中断服务（处理）函数注册
4、在中断控制器中使能中断
5、使能外设中断
6、使能中断异常。
 ********************************************************************/
//定时器中断初始化
void timer_intr_init(XScuGic *intc_ptr, XScuTimer *timer_ptr) {
	//初始化中断控制器
	XScuGic_Config *intc_cfg_ptr;
	intc_cfg_ptr = XScuGic_LookupConfig(INTC_DEVICE_ID);
	XScuGic_CfgInitialize(intc_ptr, intc_cfg_ptr, intc_cfg_ptr->CpuBaseAddress);
	//设置并打开中断异常处理功能
	Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			(Xil_ExceptionHandler) XScuGic_InterruptHandler, intc_ptr);



	//中断服务函数注册，就是中断处理函数
	XScuGic_Connect(intc_ptr, TIMER_IRPT_INTR,
			(Xil_ExceptionHandler) timer_intr_handler, (void *) timer_ptr);

	XScuGic_Enable(intc_ptr, TIMER_IRPT_INTR); //使能 GIC 中的定时器中断
	XScuTimer_EnableInterrupt(timer_ptr); //使能定时器中断（外设）

	//使能中断异常
		Xil_ExceptionEnable();
}


void Time_Init_Function() {
	int status;
	status = timer_init(&Timer); //定时器初始化
	if (status != XST_SUCCESS) {
		xil_printf("Timer Initial Failed\r\n");
		return XST_FAILURE;
	}
	timer_intr_init(&Intc, &Timer); //定时器中断初始化
	XScuTimer_Start(&Timer); //启动定时器

}

