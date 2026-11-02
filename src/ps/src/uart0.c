#include "../include/uart0.h"
#include "../include/gic.h"
RunLevel Uart_Run_Level;



_UartHandle UartHandle;
#define FOOTBOARD_PROTOCOL_LEN 17
#define UART0_BUFSIZE 255
ringbuffer_t uart0_rx;
//static unsigned char Buffer_tx[UART0_BUFSIZE], Buffer_rx[UART0_BUFSIZE];
static unsigned char Buffer_rx[UART0_BUFSIZE];
uint8_t USART0_TXbuffbyte;
uint8_t USART0_TXbuffbytecount;

uint8_t usart0_rec_flag = 0;

#define USART0_BUFFER_LEN 32
uint8_t USART0_RXbuffbyte;
uint8_t USART0_RXbuff[USART0_BUFFER_LEN];
uint8_t USART0_TXbuff[USART0_BUFFER_LEN];
#define  AMPLITUDE_LEVEL_MAX            6
const unsigned int g_DesReg[AMPLITUDE_LEVEL_MAX] = { 800, 700, 600, 500, 400 }; //不同档位最大阻抗
const unsigned int g_DesPower[AMPLITUDE_LEVEL_MAX] = { 25, 29, 32, 35, 38 };    //目标功率

const unsigned int g_DesCurent[AMPLITUDE_LEVEL_MAX] =
		{ 200,258, 301, 344, 390, 430 };                                        //电流档位 //需要除以2 四档扫频
const unsigned int g_DesCurent_Trige[AMPLITUDE_LEVEL_MAX] = { 258, 301, 300,
		300, 330 }; //电流档位

extern PID_def PID_Current;
extern PID_def PID_Power_Track;
extern PID_def PID_Freq_Track;
extern UT_PARAM UtParam;
extern int freq_output;
//extern int v_UtParam->TrackParam.Volt;
extern int phase_input;
extern float power_input;
//extern int v_UtParam->TrackParam.Current;
extern int curre_output;
//extern int v_UtParam->TrackParam.Res;

unsigned char buf1[20] = { 0xab, 0x55, 0, 0, 0, 0, 0, 0, 0, 0, 0xed };








 /*******************************************************************************
UART 的启动顺序如下：
1、 复位UART控制器（在 PS 系统复位时）；
2、 配置 IO 引脚信号。RxD 和 TxD 可以连接至 MIO 或者 EMIO，只有 EMIO 可以使用串口的流控制；
3、 配置 UART 参考时钟；
4、 配置控制器功能（UART 控制器初始化）；
5、 配置中断，通过中断来管理 RxFIFO 和 TxFIFO；
6、 配置串口流控制（可选）；
7、 管理发送和接收的数据，可以采用轮询或中断驱动处理两种方式；
 ******************************************************************************/
  //UART 初始化函数
  int uart_init(XUartPs *uart_ps)
  {
     int status;
     XUartPs_Config *uart_cfg;

     uart_cfg = XUartPs_LookupConfig(UART_DEVICE_ID);
     if (NULL == uart_cfg)
     return XST_FAILURE;
     status = XUartPs_CfgInitialize(uart_ps, uart_cfg, uart_cfg->BaseAddress);
     if (status != XST_SUCCESS)
     return XST_FAILURE;

     //UART 设备自检，采用本地回环的模式看看uart是不是好的
     status = XUartPs_SelfTest(uart_ps);
     if (status != XST_SUCCESS)
     return XST_FAILURE;

     //设置工作模式:正常模式
     XUartPs_SetOperMode(uart_ps, XUARTPS_OPER_MODE_NORMAL);
     //设置波特率:115200
     XUartPs_SetBaudRate(uart_ps,115200);

     //设置 RxFIFO 的中断触发等级
     //XUartPs_SetFifoThreshold 函数用于设置RxFIFO 的中断触发等级，即触发 RxFIFO 中断的数据个数，
     //这里设置的值为 1，即每收到一个值就触发中断。注意，中断触发等级最大值不超过 63
     XUartPs_SetFifoThreshold(uart_ps, 1);

     return XST_SUCCESS;
  }


  //UART 中断处理函数
  // @param CallBackRef 是指向上层回调引用的指针
   void uart_intr_handler(void *call_back_ref)
   {
      XUartPs *uart_instance_ptr = (XUartPs *) call_back_ref;
      u32 rec_data = 0 ;
      u32 isr_status ; //中断状态标志

      //读取中断 ID 寄存器，判断触发的是哪种中断
      //ISR是中断服务寄存器，IRR是中断请求控制器，IMR是中断屏蔽寄存器。
      //判断中断种类，XUARTPS_IXR_RXOVR就是种类，那个被启动哪个为一
      isr_status =  XUartPs_ReadReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_IMR_OFFSET);//INT_MASK的非门
      //确定种类后，读取中断状态，哪个中断正在服务
      isr_status &= XUartPs_ReadReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_ISR_OFFSET);//INT_STAT

      //判断中断标志位 RxFIFO 是否触发
         if (isr_status & (u32)XUARTPS_IXR_RXOVR)//确定中断种类为：XUARTPS_IXR_RXOVR：RX FIFO触发中断
         {
         rec_data = XUartPs_RecvByte(XPAR_PS7_UART_0_BASEADDR);
         //清除中断标志
         XUartPs_WriteReg(uart_instance_ptr->Config.BaseAddress,XUARTPS_ISR_OFFSET, XUARTPS_IXR_RXOVR) ;
         }

      //ZYNQ 7020 串口工作为中断触发模式。使用XUartPs_Send()函数发送完成数据之后，
      //会产生一个 TX FIFO EMPTY中断（尽管串口初始化中断时并没有使能XUARTPS_IXR_TXEMPTY中断），
      //同时会调用串口中断处理函数，如果没有清除处中断标志位，
      //除非关闭中断XScuGic_Disable()否则会持续调用中断处理函数，影响程序正常运行。
      if((isr_status & (u32)XUARTPS_IXR_TXEMPTY))
      	{
      		XUartPs_WriteReg(uart_instance_ptr->Config.BaseAddress, XUARTPS_ISR_OFFSET, XUARTPS_IXR_TXEMPTY) ;//clear TX FIFO中断
      	}
      printf("捕获到,data\t\n");
      XUartPs_SendByte(XPAR_PS7_UART_0_BASEADDR,rec_data);
   }

    //串口中断初始化
    int uart_intr_init(XScuGic *intc, XUartPs *uart_ps)
    {
       int status;
       //初始化中断控制器
       XScuGic_Config *intc_cfg;
       intc_cfg = XScuGic_LookupConfig(INTC_DEVICE_ID);

       if (NULL == intc_cfg)
       return XST_FAILURE;
       status = XScuGic_CfgInitialize(intc, intc_cfg,intc_cfg->CpuBaseAddress);

       if (status != XST_SUCCESS)
       return XST_FAILURE;

      //设置并打开中断异常处理功能
       Xil_ExceptionInit();
       Xil_ExceptionRegisterHandler(XIL_EXCEPTION_ID_INT,(Xil_ExceptionHandler)XScuGic_InterruptHandler,(void *)intc);
       Xil_ExceptionEnable();

		//为中断设置中断处理函数
		XScuGic_Connect(intc, UART_INT_IRQ_ID,(Xil_ExceptionHandler) uart_intr_handler,(void *) uart_ps);
		//设置优先级，极性
		//XScuGic_SetPriorityTriggerType(&intc  ,UART_INT_IRQ_ID,  0x16,  0x3);//高电平触发，优先级逐次下降
		//设置 UART 的中断触发方式
		//函数输入的参数为 XUARTPS_IXR_RXOVR，表示达到 RxFIFO 的触发等级时，开始触发中断，
		//当然也可以设置成 RxFIFO 为满时触发中断或者为空时触发中断等。
		XUartPs_SetInterruptMask(uart_ps, XUARTPS_IXR_RXOVR);
		//使能 GIC 中的串口中断
		XScuGic_Enable(intc, UART_INT_IRQ_ID);
		return XST_SUCCESS;
    }


/*******************************************************************************
     函数名: void Uart0DataSend(unsigned char *rec_data,unsigned char length)
     描述:  串口数据发送给
     输入参数:   length 数据个数,
     输出参数:   无
     函数返回值: 无
     历史修改记录:
     <作者><日期><描述>
******************************************************************************/
    void Uart0DataSend(unsigned char *rec_data, unsigned char length)
    {
    	unsigned char i;
    	for (i = 0; i < length; i++) {
    		XUartPs_SendByte(Uart_Ps.Config.BaseAddress, rec_data[i]);
    	}
    }

/*******************************************************************************
     函数名: uart0_recv
     描述:   读取数据
     输入参数:   length 数据个数,
     输出参数:   无
     函数返回值: 无
     历史修改记录:
     <作者><日期><描述>
******************************************************************************/
    short uart0_recv(unsigned char *buf, unsigned char length) {
    	// 参数无效
    	if (!buf || (length <= 0))
    		return 0;

    	// 读取接收缓存
    	return ringbuffer_pop(&uart0_rx, (void *) buf, length);
    }

/*******************************************************************************
     函数名: uart0_recv_delete
     描述:   删除数据
     输入参数:   length 数据个数,
     输出参数:   无
     函数返回值: 无
     历史修改记录:
     <作者><日期><描述>
******************************************************************************/
    short uart0_recv_delete(short length) {
    	// 参数无效
    	if (length < 0)
    		return 0;

    	// 读取接收缓存
    	return ringbuffer_delete(&uart0_rx, length);
    }

/*******************************************************************************
     函数名: sum_check
     描述:   校验和
     输入参数:   数据和数据长度
     输出参数:   校验和结果
     函数返回值: 无
     历史修改记录:
     <作者><日期><描述>
******************************************************************************/
    unsigned char sum_check(unsigned char *buf, unsigned char num) {
    	unsigned char i = 0, sum = 0;
    	for (i = 0; i < num; i++) {
    		sum += buf[i];
    	}
    	return sum;
    }

/*******************************************************************************
     * 函数名:		uart0_read_data
     * 描述: 串口缓冲区数据读取
     *
     * 输入参数: 无
     * 返回参数: 无
     * 历史修改记录:
     * <作者><日期><描述>
******************************************************************************/
    void uart0_read_data() {
    	unsigned char ret;
    	static unsigned char buf[30];
    	unsigned short CRC16;
    	switch (Uart_Run_Level) {
    	case Header:
    		ret = uart0_recv(buf, 1);
    		if (ret >= 1) {
    			if (buf[0] == 0xAA) {
    				UartHandle.DataTemp[0] = 0xAA;
    				Uart_Run_Level++;                        //帧头正确读取跳转数据读取长度
    				//printf("cmd Header \r\n");
    			} else {
    				Uart_Run_Level = Header;
    				//printf("cmd Header error = %x\r\n",buf[0]);
    			}
    		}
    		break;
    	case Header1:
    		ret = uart0_recv(buf, 1);
    		if (ret >= 1) {
    			if (buf[0] == 0x55) {
    				UartHandle.DataTemp[1] = 0x55;
    				Uart_Run_Level++;                        //帧头正确读取跳转数据读取长度
    				//printf("cmd Header1 \r\n");
    			} else {
    				Uart_Run_Level = Header;
    				//printf("cmd Header error = %x\r\n",buf[0]);
    			}
    		}
    		break;
    	case Length:
    		ret = uart0_recv(buf, 1);
    		if (ret >= 1) {
    			if (buf[0] > 30) { //数据长度超过250 错误
    				Uart_Run_Level = Header;
    			} else {
    				UartHandle.DataTemp[2] = buf[0];
    				Uart_Run_Level++;
    			}
    			printf("cmd Length= %d\r\n", buf[0]);
    		}
    		break;
    	case Cmd:
    		ret = uart0_recv(buf, 1);
    		if (ret >= 1) {

    			if (buf[0] > 250) { //命令错误
    				Uart_Run_Level = Header;
    			} else {
    				UartHandle.DataTemp[3] = buf[0];
    				Uart_Run_Level++;
    			}
    			printf("cmd recv=%x\r\n", buf[0]);
    		}

    		break;
    	case Data:
    		ret = uart0_recv(buf, UartHandle.DataTemp[2]); //读取需要数据长度
    		if (ret >= UartHandle.DataTemp[2])				//读取完成开始计算校验和
    				{
    			memcpy(&UartHandle.DataTemp[4], buf, (UartHandle.DataTemp[2]));
    			Uart_Run_Level++;
    				print("Data recv\r\n");
    		}
    		break;
    	case Sumcheck:
    		ret = uart0_recv(buf, 2);
    		if (ret >= 2) {
    			CRC16 = CalcArrayCrc16(UartHandle.DataTemp,
    					UartHandle.DataTemp[2] + 4);
    			if(buf[0] == (CRC16&0xff) && buf[1] == (CRC16 >> 8))
    			{
    				UartHandle.DataCmd = UartHandle.DataTemp[3];
    				UartHandle.DataLength = UartHandle.DataTemp[2];
    				memcpy(UartHandle.DataBuffer, &UartHandle.DataTemp[4],
    						UartHandle.DataLength);
    				UartHandle.DataCheckFlag = TRUE;
    				printf("DataCmd      =%x\r\n",UartHandle.DataCmd );
    				printf("DataLength   =%x\r\n",UartHandle.DataLength);
    				printf("DataBuffer[0]=%x\r\n",UartHandle.DataBuffer[0]);
    				printf("DataBuffer[1]=%x\r\n",UartHandle.DataBuffer[1]);
    			}
    			Uart_Run_Level = Header;
    		}
    		break;
    	default:
    		Uart_Run_Level = Header;
    		break;
    	}
    	//校验数据和
    }

/*******************************************************************************
     * 函数名:		uart0_intr_handler
     * 描述: 		串口中断

     * 输入参数: 无
     * 返回参数: 无
     * 历史修改记录:
  * <作者><日期><描述>
******************************************************************************/
    void uart0_intr_handler(void *call_back_ref) {
    	//XUartPs *uart_instance_ptr = (XUartPs *) call_back_ref;
    	unsigned int rec_data = 0;
    	unsigned int isr_status;

    	// 读取中断ID寄存器，判断触发的是哪种中断
    	isr_status = XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,
    			XUARTPS_IMR_OFFSET);
    	isr_status &= XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,
    			XUARTPS_ISR_OFFSET);

    	// 判断中断标志位RxFIFO是否触发
    	if (isr_status & (unsigned int) XUARTPS_IXR_RXOVR) {
    		rec_data = XUartPs_ReadReg(Uart_Ps.Config.BaseAddress,
    				XUARTPS_FIFO_OFFSET); //XUartPs_RecvByte(Uart_Ps.Config.BaseAddress);
    		ringbuffer_push(&uart0_rx, (void *) &rec_data);
    		// 清除中断标志
    		XUartPs_WriteReg(Uart_Ps.Config.BaseAddress, XUARTPS_ISR_OFFSET,
    				XUARTPS_IXR_RXOVR);
    	}

    	//发送数据返回
    	XUartPs_SendByte(Uart_Ps.Config.BaseAddress, rec_data);
    }
/*******************************************************************************
 * 函数名:uart0_init
 * 描述: 串口初始化

 * 输入参数: baud，波特率
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void uart0_init(long baud) {
	XUartPs_Config *uart_cfg;

	//XScuGic* intc;
	//uint32_t addr = 0;

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
	XUartPs_SetRecvTimeout(&Uart_Ps, 8); //time = 4x8bit period
	//设置UART的中断触发方式
	XUartPs_SetInterruptMask(&Uart_Ps, XUARTPS_IXR_RXOVR);

	/************************串口中断配置********************************************/

	//获取中断控制器地址
	//gic_get_object(&addr,Intc);

	//intc = (XScuGic*) addr;
	// 为中断设置中断处理函数
	XScuGic_Connect(&Intc, UART_INT_IRQ_ID,
			(Xil_ExceptionHandler) uart0_intr_handler, (void *) (&Uart_Ps));
	//设置优先级，极性
	//XScuGic_SetPriorityTriggerType(&Intc  ,UART_INT_IRQ_ID,  0x8,  0x3);//高电平触发，优先级逐次下降
	// 使能GIC中的串口中断
	XScuGic_Enable(&Intc, UART_INT_IRQ_ID);
	ringbuffer_init(&uart0_rx, Buffer_rx, sizeof(unsigned char), UART0_BUFSIZE);
	//Uart_Run_Level = Header;
	//PID_Current.SetPoint = g_DesCurent[UT_Level.now_level]; //默认三档电流
	//UT_Level.Debug_mode = Normal_Mode;//正常模式
}
void BiologyMode_SendRealData()
{

}
void Usart_Handle() {
	uart0_read_data(); //串口数据处理
	if (UartHandle.DataCheckFlag == TRUE) {
		UartHandle.DataCheckFlag = FALSE;
		//校验完成一次数据 开始处理数据
		switch (UartHandle.DataTemp[3]) {
		case 0x01: //握手回复
			SendMcuRegOneByte(0x00, 0x01);
			break;
		case 0x80: //扫频开关
			UT_Level.Tune_Flag = UartHandle.DataTemp[4];
			SendMcuRegOneByte(0x02, UT_Level.Tune_Flag);//回复扫频状态
			break;
		case 0x81: //输出开关

			if((UartHandle.DataBuffer[0]&0x80) == 0x80)
			{
				//UT_Level.now_level = (UartHandle.DataBuffer[0] &0x7f)+ 1;
				PID_Current.SetPoint = g_DesCurent[UT_Level.now_level];
				UT_Level.Tune_Flag = 0x03;//启动输出

				//printf("now_level %d \r\n",UT_Level.now_level);
			}
			else
			{
				//UT_Level.now_level = (UartHandle.DataBuffer[0] &0x7f) +1;
				UT_Level.Tune_Flag = 0x00;//停止输出
				//printf("now_level %d \r\n",UT_Level.now_level);
			}
			SendMcuRegOneByte(0x07, UT_Level.now_level);//回复扫频状态

			break;
		case 0x82: //调试模式下开关
			UT_Level.Debug_mode = UartHandle.DataTemp[4];//01 正常模式 02 调试模式  03 快速模式
			printf("Debug_mode %d \r\n",UT_Level.Debug_mode);
			break;
		case 0x85: //档位 +
			if (UartHandle.DataBuffer[0] == 0x01) {
				UT_Level.now_level += 1;
				if (UT_Level.now_level > 5)
					UT_Level.now_level = 5;
				SendMcuRegOneByte(0x03, UT_Level.now_level);
				PID_Current.SetPoint = g_DesCurent[UT_Level.now_level]; //控制电流档位
			//	printf("now_level %d \r\n",UT_Level.now_level);
			}
			else if (UartHandle.DataBuffer[0] == 0x02)
			{
				if (UT_Level.now_level > 1)
					UT_Level.now_level -= 1;
				SendMcuRegOneByte(0x03, UT_Level.now_level);
				PID_Current.SetPoint = g_DesCurent[UT_Level.now_level];
			//	printf("now_level %d \r\n",UT_Level.now_level);
			}
			break;
		case 0x04: //档位 -

			break;
		case 0x06: //直接设置档位
//			UT_Level.now_level = UartHandle.DataTemp[4] + 1;
//			PID_Current.SetPoint = g_DesCurent[UT_Level.now_level];
//			SendMcuRegOneByte(0x07, UT_Level.now_level);
			break;
		case 0x08:  //扫频电压设置
			bcuksetvalue = UartHandle.DataTemp[4];
			bcuksetvalue = bcuksetvalue << 8;
			bcuksetvalue += UartHandle.DataTemp[5];
			SendDgusRegTwoByte(0x0c, bcuksetvalue);
			break;
		case 0x09:  //扫频buck设置
			BrigesetValue = UartHandle.DataTemp[4];
			BrigesetValue = BrigesetValue << 8;
			BrigesetValue += UartHandle.DataTemp[5];
			SendDgusRegTwoByte(0x0D, BrigesetValue);
			buck_pwm_set_duty(BrigesetValue);
			break;
		case 0x86:  //相位设置
			phasesetValue = UartHandle.DataTemp[5];
			phasesetValue = phasesetValue << 8;
			phasesetValue += UartHandle.DataTemp[4];
			//SendDgusRegTwoByte(0x0A, phasesetValue);
			PID_Freq_Track.SetPoint = phasesetValue;  //相位目标
			//printf("phasesetValue %d \r\n",phasesetValue);
			break;
		}
	}
}


void SendToSTM32BoardData(UT_PARAM* v_UtParam) {

	unsigned char buf[10];
	buf[0] = v_UtParam->TrackParam.Current;
	buf[1] = v_UtParam->TrackParam.Current>>8;

	buf[2] = v_UtParam->TrackParam.Frequence;
	buf[3] = v_UtParam->TrackParam.Frequence>>8;

	buf[4] = v_UtParam->TrackParam.Volt;
	buf[5] = v_UtParam->TrackParam.Volt>>8;

	buf[6] = v_UtParam->TrackParam.Res;
	buf[7] = v_UtParam->TrackParam.Res>>8;

	buf[8] = v_UtParam->TrackParam.Phasecha;
	buf[9] = v_UtParam->TrackParam.Phasecha>>8;
	SendDgusRegMulByte(0x05,buf,0x0A);//发送扫频数据
}
void UtParam_Send(UT_PARAM* v_UtParam) {
	unsigned char buf[11] = { 0xfB, 0x0a, 0, 0, 0, 0, 0, 0, 0, 0, 0xed };
	unsigned char i;
	buf[2] = (unsigned char) (v_UtParam->TrackParam.Phasecha >> 8);
	buf[3] = (unsigned char) (v_UtParam->TrackParam.Phasecha);
	buf[4] = (unsigned char) (v_UtParam->TrackParam.Frequence >> 8);
	buf[5] = (unsigned char) (v_UtParam->TrackParam.Frequence);
	buf[6] = (unsigned char) (v_UtParam->TrackParam.Current >> 8); //电流
	buf[7] = (unsigned char) (v_UtParam->TrackParam.Current);
	buf[8] = (unsigned char) (v_UtParam->TrackParam.Res >> 8); //buck占空比输出
	buf[9] = (unsigned char) (v_UtParam->TrackParam.Res);
	for (i = 0; i < 11; i++) //主串口打印
		XUartPs_SendByte(XPAR_PS7_UART_0_BASEADDR, buf[i]);
}

/*
 ****************************************************************************************************
 * 函 数 名: SendDgusRegOneByte
 * 功能描述: 通过串口向串口屏寄存器发送1个字节数据
 * 输入参数: id      寄存器ID
 *           val     寄存器值
 * 输出参数: 无
 * 返 回 值: 无
 ****************************************************************************************************
 */
void SendMcuRegOneByte(unsigned char id, unsigned char val)
{
	unsigned char cmd[7];
	cmd[0] = 0xAB;
	cmd[1] = 0x55;
	cmd[2] = 0x05;
	cmd[3] = id;
	cmd[4] = val & 0xFF;
	cmd[5] = (unsigned char) CalcArrayCrc16(cmd, 5) & 0xff;
	cmd[6] = (unsigned char) (CalcArrayCrc16(cmd, 5) >> 8) & 0xff;
	Uart0DataSend(cmd, 7);
}
/*
 ****************************************************************************************************
 * 函 数 名: SendDgusRegTwoByte
 * 功能描述: 通过串口向串口屏寄存器发送1个字节数据
 * 输入参数: id      寄存器ID
 *           val     寄存器值
 * 输出参数: 无
 * 返 回 值: 无
 ****************************************************************************************************
 */
void SendDgusRegTwoByte(unsigned char id, unsigned int val)
{
	unsigned char cmd[7];
	cmd[0] = 0xAB;
	cmd[1] = 0x55;
	cmd[2] = 0x06;
	cmd[3] = id;
	cmd[4] = val >> 8;
	cmd[5] = val;
	cmd[6] = (unsigned char) CalcArrayCrc16(cmd, 6) & 0xff;
	cmd[7] = (unsigned char) (CalcArrayCrc16(cmd, 6) >> 8) & 0xff;
	Uart0DataSend(cmd, 8);
}

/*****************************************************************************************************
 * 函 数 名: SendDgusRegTwoByte
 * 功能描述: 通过串口向串口屏寄存器发送1个字节数据
 * 输入参数: id      寄存器ID
 *        val     寄存器值
 * 输出参数: 无
 * 返 回 值: 无
 *****************************************************************************************************/
void SendDgusRegMulByte(unsigned char id, unsigned char *val, unsigned char len) {
	unsigned char cmd[255], i1 = 0;
	cmd[i1++] = 0xAB;
	cmd[i1++] = 0x55;
	cmd[i1++] = len + 4;
	cmd[i1++] = id;
	memcpy(&cmd[4], val, len);//从存储区 str2 复制 n 个字节到存储区 str1, void *memcpy(void *str1, const void *str2, size_t n)
	i1 += len;
	//cmd[i1++] = 0xAA;
    //cmd[i1++] = 0x66;
	cmd[i1++] = (unsigned char) CalcArrayCrc16(cmd, len + 4) & 0xff;//crc低位先发
	cmd[i1++] = (unsigned char) (CalcArrayCrc16(cmd, len + 4) >> 8) & 0xff;//再发高位
	Uart0DataSend(cmd, len + 6);
}


/*******************************************************************************
 * 函数名:		SJANT_PC_USER_DAT
 * 描述: 		按照数据帧格式输出数据
 *
 * 输入参数: num：数据部分个数
 *          *userbuf 发送数据内容
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void SJANT_PC_USER_DAT(char *userbuf , uint8_t num)
{
    uint8_t i=0,sum=0;
    char buf[100],cnt=0;

    buf[cnt++]=0xaa;
    buf[cnt++]=0x55;
    buf[cnt++]=num*8;
    for(i=0;i<num;i++)
    {
        buf[cnt++]= ( *( (char *)(&userbuf[i]) ) );
    }

    for(i=0;i<cnt;i++)
    	sum+=buf[i];
    buf[cnt++]=0x66;
/*
    for(i=0;i<cnt;i++)
    {
        while((USART3->SR&0X40)==0);
        USART3->DR = buf[i];
    }
*/
    for (i = 0; i < cnt; i++) {
    		XUartPs_SendByte(XPAR_PS7_UART_0_BASEADDR, buf[i]);
    	}

}
