#include "xil_printf.h"
#include <stdio.h>
#include "../include/main.h"
//#include "private_timer.h"
//#include "uart0.h"
#include <stddef.h>
#include "../include/gic.h"
#include "../include/gpio_intr.h"
//#include "fuzzypid.h"                  /* Model's header file */
//#include "rtwtypes.h"




#define Buck_Duty_Set_duty  0.06


unsigned char constcurnum = 0;
unsigned int UT_ATP_State = 0, UT_ATP_Run = 0;
unsigned char LedFlag = 0;

int current_cnt = 10;
int data, data1, data2;
int time_cnt;
extern ringbuffer_t uart0_rx;
extern _UT_Level UT_Level;
extern _UartHandle UartHandle;
extern uint32_t Rxnum;
extern PID_def PID_Current;
extern PID_def PID_Power_Track;
extern PID_def PID_Freq_Track;
extern resonance_paramter_t resonance_point;
extern unsigned int bcuksetvalue, BrigesetValue, phasesetValue, phaseflag;
extern int8_t TuneData[5];
unsigned char cnt = 0, ret, ret1 = 0;
unsigned int UtOutTimesForSig = 0, TrackParamState = 0;
unsigned int  ladderflag = 0, ladder_Current_set = 80;
float bucksetduty = 0;
int freq_output;
int phase_input;
float power_input;
int curre_pi_output;
unsigned int Irms = 0, freq, cnt2 = 0, Sendnum = 0;
UT_PARAM UtParam;
unsigned int SendDateFre = 0;





int main() {


	IncPIDInit();




	//freq_output = 24489;
	//led_init();
	//led_turn_state(LED0, 0);
	//UT_Level.now_level = 3;
	//resonance_point.resonance_phase_diff = 6; //度
	buck_pwm_set_duty(0.15);    // 6% 左右占空
    buck_pwm_set_fre (40000);    //40khz的pwm
	Time_Init_Function();


	//phasesetValue = 6;
	//PID_Freq_Track.SetPoint = 6;
	gpio_init();
	gpio_polled_init();



   // uart0_init(115200);

    uart_init(&Uart_Ps); //串口初始化
    uart_intr_init(&Intc, &Uart_Ps); //串口中断初始化
    //执行该初始化函数后，可以通过中断获取pl上传的数据,pl设计的时候同步了时序，所以一个标志把所有数据取了，一一对应
    IntrInitFuntion(&Intc,INTR_ID_PHASE_SUB_ID,0x0,IntrHandler);
//    IntrInitFuntion(&Intc,INTR_ID_MODULUS_DIV_ID ,0x0,IntrHandler_modulus_div);
//    IntrInitFuntion(&Intc,INTR_ID_RMS_I_ID       ,0x8,IntrHandler_rms_i);
//    IntrInitFuntion(&Intc,INTR_ID_RMS_U_ID       ,0x8,IntrHandler_rms_u);
//    IntrInitFuntion(&Intc,INTR_ID_RMS_DYNAMIC_ID ,0x8,IntrHandler_rms_dynamic);
    //定时器中断

	getpstempInit();
	//初始状态需要dds发一个频率的方波25khz
	while (1)
	{
		usleep(99); //99us
		//Usart_Handle();
		//LEDRun();




//			if (UT_Level.Debug_mode == Fast_Mode) {
//				SendDateFre = 300;
//			} else if (UT_Level.Debug_mode == Biology_Mode) {
//				SendDateFre = 50;
//			}
//
//
//			if (cnt2 > SendDateFre) { //打印显示
//				cnt2 = 0;
//
//				if (UT_Level.Debug_mode == 0x01 || UT_Level.Debug_mode == 0x03)
//					SendToSTM32BoardData(&UtParam); //调试信息
//
//				UtParam_Send(&UtParam); //输出波形
//			} else
//               cnt2 += 1;
//            } else if (UT_Level.Tune_Flag == 1) { //扫频测试


		 if (key1_press)
		 	{
			 XGpioPs_WritePin(&gpio, MIOLED1, 0x0);
		 	 //usleep(20000);//中断触发时，key_press 为 TURE，延时一段时间后判断按键是否按下，
		 	 //if (XGpioPs_ReadPin(&gpio, KEY1) == 1)//如果为 0 代表按下

		    system_output_param_check();   //扫频
		 	SendDgusRegMulByte(0x09, TuneData, 0x0A); //发送扫频数据

		    key1_press = FALSE;
		 	XGpioPs_IntrClearPin(&gpio, KEY1); //清除按键 KEY 中断
		 	XGpioPs_IntrEnablePin(&gpio, KEY1); //使能按键 KEY 中断
            //输出结束
			//initAlarmValue(); //清除报警标记
			freq_output = resonance_point.resonance_freq ; //扫频频率作为当前的输出频率
			XGpioPs_WritePin(&gpio, MIOLED1, 0x1);

			//ladderflag = 0;
			//bucksetduty = 30;
			//ladder_Current_set = 100;
			//UtOutTimesForSig = 0;
			//TrackParamState = 0;
			//buck_pwm_set_fre(0);
			//buck_pwm_set_duty(0); //关闭bridge输出
            }

		 else if (XGpioPs_ReadPin(&gpio, KEY0) == 0)
		 {

			 XGpioPs_WritePin(&gpio, MIOLED0, 0x1);
/*******************************相位控制***********************************/
			 //		if (UT_Level.Tune_Flag == 0x03) {

			 buck_pwm_set_duty(Buck_Duty_Set_duty);
			 buck_pwm_set_fre(40000);//40khz
			 UtParam.TrackParam.Phasecha = resonance_point.resonance_phase_diff;		//获取相位
			 UtParam.TrackParam.Volt = resonance_point.resonance_v_rms;		            //放大十倍电压
			 	                        //频率输出
			 UtParam.TrackParam.Frequence = freq_track_pid_cal(UtParam.TrackParam.Phasecha);	//频率PID控制
			 freq = (unsigned int) UtParam.TrackParam.Frequence;
			 UtParam.TrackParam.Res =(unsigned int) ((UtParam.TrackParam.Volt * 10)/ (UtParam.TrackParam.Current / 10.0));	//计算阻抗
			 UtParam.TrackParam.Power = (UtParam.TrackParam.Volt / 100.0* UtParam.TrackParam.Current / 1000.0);	//计算功率
/*******************************end***********************************/
			 cnt++;
			 if (cnt >= current_cnt)
			 {		//恒流
			 	cnt = 0;
			 	UtParam.TrackParam.Current = resonance_point.resonance_i_rms; //获取电流
			 	//UTDriver_Alarm(&UtParam); //报警
			 	curre_pi_output = current_pid_cal(UtParam.TrackParam.Current); //电流PID控制  pid显示输出的幅值
			 	buck_pwm_set_duty(curre_pi_output); //桥占空比输出
			 }

			 			/***********************end******************************************************/
			 print("MIO Test! \n\r");
         }
		 XGpioPs_WritePin(&gpio, MIOLED0, 0x0);
		 //		if (UT_Level.Tune_Flag == 1) { //扫频测试，调节模式
		 //			//usleep(20000);//中断触发时，key_press 为 TURE，延时一段时间后判断按键是否按下，是则反转 LED
		 //			//UTDrive_Scan_State(0x02);
		 //			//SendMcuRegOneByte(0x02, 0x02); //结束扫频
		 //            system_output_param_check();   //扫频
		 //            //UTDrive_Scan_Alarm();      //报警
		 //			//SendMcuRegOneByte(0x02, 0x03); //结束扫频
		 //			SendDgusRegMulByte(0x09, TuneData, 0x0A); //发送扫频数据
		 //			UT_Level.Tune_Flag = 0;
		 //		}

		}

	return 0;
 }



