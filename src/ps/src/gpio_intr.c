
#include "../include/gpio_intr.h"

 /************************** Function Definitions *****************************/

 int gpio_init()
 {
	 	 int status;
 	 	 XGpioPs_Config *ConfigPtr; //PS 端 GPIO 配置信息

 	 	 //xil_printf("Gpio interrupt test \r\n");

 	 	 	 //根据器件 ID 查找配置信息
 	 	 	 ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
 	 	 	 if (ConfigPtr == NULL)
 	 	 	 {
 	 	 	 return XST_FAILURE;
             }
        //初始化 Gpio driver
 	 	XGpioPs_CfgInitialize(&gpio, ConfigPtr, ConfigPtr->BaseAddr);

 	 	//设置 KEY1 所连接的 MIO 引脚的方向为输入
 	 	XGpioPs_SetDirectionPin(&gpio, KEY1, 0);



 	 	//建立中断,出现错误则打印信息并退出
 	 	status = setup_interrupt_system(intc, gpio, GPIO_INTERRUPT_ID);
 	 	if (status != XST_SUCCESS)
 	 	{
 	 		xil_printf("Setup interrupt system failed\r\n");
 	 		return XST_FAILURE;
 	 	}


 	 	return XST_SUCCESS;
  }

  //中断处理函数
  // @param CallBackRef 是指向上层回调引用的指针
  void intr_handler_gpio(void *callback_ref)
    {
	  //XGpioPs *gpio = (XGpioPs *) callback_ref;

	  //读取 KEY 按键引脚的中断状态，判断是否发生中断
	  if (XGpioPs_IntrGetStatusPin(&gpio, KEY1))
	  {
		  key1_press = TRUE;
		  XGpioPs_IntrDisablePin(&gpio, KEY1); //屏蔽按键 KEY1 中断
	  }
  }

  //建立中断系统，使能 KEY 按键的下降沿中断
  // @param GicInstancePtr 是一个指向 XScuGic 驱动实例的指针
  // @param gpio 是一个指向连接到中断的 GPIO 组件实例的指针
  // @param GpioIntrId 是 Gpio 中断 ID
  // @return 如果成功返回 XST_SUCCESS, 否则返回 XST_FAILURE
  int setup_interrupt_system(XScuGic gic_ins_ptr, XGpioPs gpio, u16 GpioIntrId)
  {
	  int status;
	  XScuGic_Config *IntcConfig; //中断控制器配置信息

	  //查找中断控制器配置信息并初始化中断控制器驱动
	  IntcConfig = XScuGic_LookupConfig(INTC_DEVICE_ID);
	  if (NULL == IntcConfig)
	  {
		  return XST_FAILURE;
	  }

	  status = XScuGic_CfgInitialize(&gic_ins_ptr, IntcConfig,
			  IntcConfig->CpuBaseAddress);
	  if (status != XST_SUCCESS)
	  {
		  return XST_FAILURE;
	  }

	  //设置并使能中断异常
	  Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,
			  (Xil_ExceptionHandler) XScuGic_InterruptHandler, &gic_ins_ptr);
	  Xil_ExceptionEnable();

	  //为中断设置中断处理函数
	  status = XScuGic_Connect(&gic_ins_ptr, GpioIntrId,
			  (Xil_ExceptionHandler) intr_handler_gpio, (void *) NULL);
	  if (status != XST_SUCCESS)
	  {
		  return status;
	  }

	  //使能来自于 Gpio 器件的中断
	  XScuGic_Enable(&gic_ins_ptr, GpioIntrId);

	  //设置 KEY 按键的中断类型为下降沿中断
	  XGpioPs_SetIntrTypePin(&gpio, KEY1, XGPIOPS_IRQ_TYPE_EDGE_FALLING);

	  //使能按键 KEY 中断
	  XGpioPs_IntrEnablePin(&gpio, KEY1);

	  return XST_SUCCESS;
  }

  int gpio_polled_init()
   {
  	// int Status;
  	// XGpioPs_Config *ConfigPtr;


  	 //ConfigPtr = XGpioPs_LookupConfig(GPIO_DEVICE_ID);
  	 //Status = XGpioPs_CfgInitialize(&gpio, ConfigPtr,ConfigPtr->BaseAddr);
  	 //if (Status != XST_SUCCESS)
   	 //{
  	 //	 return XST_FAILURE;
   	 //}
   //设置指定引脚的方向：0 输入，1 输出
  	 XGpioPs_SetDirectionPin(&gpio, KEY0, 0);
  	 XGpioPs_SetDirectionPin(&gpio, MIOLED1, 1);
  	 XGpioPs_SetDirectionPin(&gpio, MIOLED0, 1);
   //使能指定引脚输出：0 禁止输出使能，1 使能输出
  	 XGpioPs_SetOutputEnablePin(&gpio, KEY0, 1);
  	 XGpioPs_SetOutputEnablePin(&gpio, MIOLED1, 1);
  	 XGpioPs_SetOutputEnablePin(&gpio, MIOLED0, 1);
  	 return XST_SUCCESS;
   }

