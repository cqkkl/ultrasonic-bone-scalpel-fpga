#ifndef __GPIO_INTR_H
#define __GPIO_INTR_H

 #include "xparameters.h"
 #include "xgpiops.h"
 #include "xscugic.h"
 #include "xil_exception.h"
 #include "xplatform_info.h"
 #include <xil_printf.h>
 #include "sleep.h"
 /************************** Constant Definitions *****************************/

 //以下常量映射到 xparameters.h 文件
 #define GPIO_DEVICE_ID XPAR_XGPIOPS_0_DEVICE_ID //PS 端 GPIO 器件 ID
 #define INTC_DEVICE_ID XPAR_SCUGIC_SINGLE_DEVICE_ID //通用中断控制器 ID
 #define GPIO_INTERRUPT_ID XPAR_XGPIOPS_0_INTR //PS 端 GPIO 中断 ID

 //定义使用到的 MIO 引脚号
 #define KEY1 11 //KEY 连接到 MIO11
 #define KEY0 12 //KEY 连接到 MIO12
 #define MIOLED0 7 //连接到 MIO7
 #define MIOLED1 8 //连接到 MIO8
 #define MIOLED2 0 //连接到 MIO0，ps核心板的用不到
 /************************** Function Prototypes ******************************/
 int gpio_init();
 void intr_handler_gpio(void *callback_ref);
 int setup_interrupt_system(XScuGic gic_ins_ptr, XGpioPs gpio, u16 GpioIntrId);
 int gpio_polled_init();
 /**************************Global Variable Definitions ***********************/

  XGpioPs gpio; //PS 端 GPIO 驱动实例

  XScuGic intc; //通用中断控制器驱动实例
  u32 key1_press; //KEY 按键按下的标志
  u32 key0_press; //KEY 按键按下的标志

#endif
