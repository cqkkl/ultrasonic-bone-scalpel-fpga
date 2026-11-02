#include "../include/gic.h"






//定义中断地址
#define START_MASK                             0x00000001   //b01
#define INTRCLR_MASK                           0x00000002   //b10










                                       /*PL端的中断接收*/



//中断系统函数：初始化中断控制器、设置并使能异常处理、使能来自中断源的中断、设置中断触发类型、关联中断处理函数。
//XScuGic gic_ins 是一个指向 XScuGic 驱动实例
int IntrInitFuntion(XScuGic *gic_ins,u16 INTR_ID,int priority,IntrHandler_ptr IntrHandler_ptr)//接收PL端的intr中断
{
	XScuGic_Config *IntcConfig;//指针指向结构体，包含cpu接口寄存器基地址，中断分配器寄存器基地址，设备id

    int Status;

	//check device id，查找中断控制器配置信息
	IntcConfig = XScuGic_LookupConfig(XPAR_SCUGIC_SINGLE_DEVICE_ID);//根据器件 ID 查找配置信息，GIC
	if (NULL == IntcConfig) {
	 return XST_FAILURE;
	 }
	//Initialization，初始化中断控制器驱动
	Status = XScuGic_CfgInitialize(gic_ins  , IntcConfig, IntcConfig->CpuBaseAddress) ;//初始化GIC
    if (Status != XST_SUCCESS)
	     {
	     return XST_FAILURE;
	     }
    //为中断设置中断处理函数
	//因为中断是spi，共享外设的，那回调函数就设置为空，因为没有对应外设的实例指针
		Status   = XScuGic_Connect(gic_ins,   INTR_ID,  (Xil_ExceptionHandler)*IntrHandler_ptr,  (void *)NULL) ;
        if (Status != XST_SUCCESS)
         {
			return XST_FAILURE;
	     }


    //设置优先级，极性
XScuGic_SetPriorityTriggerType(gic_ins  ,INTR_ID,  priority,  0x3);//高电平触发，优先级逐次下降

    //使能来自于 pl的私有中断
XScuGic_Enable(gic_ins  , INTR_ID  ) ;

	//设置并使能中断异常
Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT, (Xil_ExceptionHandler)XScuGic_InterruptHandler, gic_ins);
Xil_ExceptionEnable();
return XST_SUCCESS ;

}



//中断处理函数：检测到中断时去完成想要实现的功能
// @param CallBackRef 是指向上层回调引用的指针，我这里没用到，要是spi（共享外设中断），要改
void IntrHandler(void *CallbackRef)//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,data\t\n");
    //就是470x(1/57000000)=8.24微秒采集一次数据
	//后经过验证，pl端7600个时钟周期大概133微秒算一次数据
    phase_sub   = get_phase_dec(470,100);
    modulus_div = get_modulus_div_dec(470,100);
    rms_u       = get_voltage_dec(470,100);
    rms_i       = get_curre_dec(470,100);
    rms_dynamic = get_current_dynamic_dec(470,100);
	Intr_flag_bram_rd = 1 ;

}


void IntrHandler_phase_sub(void *CallbackRef)//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,phase_sub\t\n");
    phase_sub = get_phase_dec(470,100);
	Intr_flag_phase_sub = 1 ;

}


void IntrHandler_modulus_div(void *CallbackRef)//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,modulus_div\t\n");
    modulus_div = get_modulus_div_dec(470,100);
	Intr_flag_modulus_div = 1 ;
}

void IntrHandler_rms_i(void *CallbackRef)//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,rms_i\t\n");
    rms_i = (int) get_curre_dec(470,100);
    printf("%d\t\n",rms_i);
	Intr_flag_rms_i = 1 ;
}

void IntrHandler_rms_u(void *CallbackRef)//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,rms_u\t\n");
    rms_u = (int) get_voltage_dec(470,100);
	Intr_flag_rms_u = 1 ;
}

void IntrHandler_rms_dynamic(void *CallbackRef )//中断服务函数
{

    //printf("捕获到PL写BRAM结束中断,rms_dynamic\t\n");
    rms_dynamic =(int) get_current_dynamic_dec(470,100);
	Intr_flag_rms_dynamic = 1 ;
}

/*******************************************************************************
 * 函数名:		gic_get_object
 * 描述: 获取中断控制器的对象
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void gic_get_object(uint32_t* addr,XScuGic gic_ins)
{
    *addr = &gic_ins;
}
