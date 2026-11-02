/*
 * Alarm.c
 *
 *  Created on: 2023年11月6日
 *      Author: yanghang
 */
#include "../include/alarm.h"
typedef unsigned int CPU_INT32U;

#define SCAN_FAULT_NONE                 0x000000
#define SCAN_FAULT_HIGH_PHASE          (0x000001<<0)
#define SCAN_FAULT_LOW_PHASE           (0x000001<<1)           /* 手柄超出使用次数 */
#define SCAN_FAULT_HIGH_RES            (0x000001<<2)           /* 手柄超出使用次数 */
#define SCAN_FAULT_LOW_RES	           (0x000001<<3)
#define SCAN_FAULT_HIGH_CURR           (0x000001<<4)           /* 频率跟踪错误     */
#define SCAN_FAULT_LOW_CURR 	       (0x000001<<5)
#define SCAN_FAULT_HIGH_VOLT   	       (0x000001<<6)
#define SCAN_FAULT_LOW_VOLT 	       (0x000001<<7)
#define SCAN_FAULT_HIGH_FRE	           (0x000001<<8)
#define SCAN_FAULT_LOW_FRE	           (0x000001<<9)
#define SCAN_FAULT_BLADE               (0x000001<<10)

#define SCAN_FAULT_PL_DISCON             (0x000001<<15)           /* PL端异常   */

#define MSG_SCAN_EVENT_FAULT             (0x14<<24)

extern _UT_Level UT_Level;
extern resonance_paramter_t resonance_point;
static int error_num_r = 0, error_num_v = 0, error_num_i = 0;
typedef struct controler {
//    HANDLE_TYPE     HandleType;
//    CPU_INT16U      Handleftimes;
//    SCALPEL_TYPE    ScalpelType;
//    STATE_MACHINE   StateMachine;
//    CTRL_TYPE       CtrlType;
//    FOOTBOARD_STATE FootboardState;
//    FPGABOARD_STATE FpgaboardState;
//    TUNE_STATE      TuneState;
//    UT_INTENSITY    OutputType;
//    CPU_INT08U      MinNum;
//    CPU_INT08U      ScalpeOk;
//    CPU_INT16U      StartupStep;
//    CPU_INT08U      FB_LData;
//    CPU_INT08U      FB_RData;
//    CPU_INT08U      Luminance;    //亮度

	CPU_INT32U FaultCode;
//    CPU_INT32U      OldFaultCode;
//
//    HANDLE_TYPE     OldHandleType;
//    SCALPEL_TYPE    OldScalpelType;
//
//    PAGE_ID_E       PageId;
//    CPU_INT08U      SetPageState;  //0:set页面，1系统信息页面
//    PAGE_ID_E       OldPageId;
//
//    ICONINFO_S      CurIconInfo;
//    ICONINFO_S      OldIconInfo;
//
//    CPU_INT32U      TotalUtMaxTime;
//    CPU_INT32U      TotalUtMinTime;
	CPU_INT16U SingleUtTime;

} CONTROLER;
CONTROLER Controler;

void test(void) {
	Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_HIGH_RES;
	SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
			sizeof(Controler.FaultCode));
}
void UTDrive_Scan_State(CPU_INT08U state) {
	switch (state) {

//	case SCAN_FAULT_READ:
//		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_READ;
//		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
//				sizeof(Controler.FaultCode));
//		break;
//	case SCAN_FAULT_RUNING:
//		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_RUNING;
//		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
//				sizeof(Controler.FaultCode));
//		break;
	}
}
//扫频开始清除所有的错误
void UTDrive_ScanFault_Clear(void) {

	if (Controler.FaultCode != SCAN_FAULT_NONE) {
		Controler.FaultCode &= ~(0xffff);
	}
}
CPU_BOOLEAN UTDrive_Scan_Alarm() {

	CPU_BOOLEAN Ret;
	if (resonance_point.resonance_z > 500) //阻抗过高
			{
		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_HIGH_RES;
		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,sizeof(Controler.FaultCode));
	        }
	if (resonance_point.resonance_z < 5) //阻抗过低
			{
		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_LOW_RES;
		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
				sizeof(Controler.FaultCode));
	}
	if (resonance_point.resonance_phase_diff > 13000) //相位过大
			{
		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_HIGH_PHASE;
		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
				sizeof(Controler.FaultCode));
	}
	if (resonance_point.resonance_phase_diff < 10)  //相位过低
			{
		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_LOW_PHASE;
		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
				sizeof(Controler.FaultCode));
	}
	//有错误状态下清除错误
	if (Controler.FaultCode == SCAN_FAULT_NONE) {
		Ret = DEF_TRUE;
		Controler.FaultCode = MSG_SCAN_EVENT_FAULT | SCAN_FAULT_NONE;
		SendDgusRegMulByte(0x08, (void*) &Controler.FaultCode,
				sizeof(Controler.FaultCode));
		//printf("FaultCode 123%x", Controler.FaultCode);
		Controler.FaultCode &= ~(MSG_SCAN_EVENT_FAULT | SCAN_FAULT_NONE);
	} else
		Ret = DEF_FALSE;

	return Ret;
}
CPU_BOOLEAN UTDriver_Alarm(UT_PARAM* v_UtParam) {
	//扫频

	if (v_UtParam->TrackParam.Res > 1000) {
		error_num_r++;
		if (error_num_r > 20) {
			UT_Level.Tune_Flag = 0x00;
			error_num_r = 0;
			SendMcuRegOneByte(0x08, 0x01);
		}
	}
	if (v_UtParam->TrackParam.Current < NO_CURRENT_DATA) //电流过低
	{
		error_num_v++;
		if (error_num_v > 20) {
			UT_Level.Tune_Flag = 0x00;
			error_num_v = 0;
			SendMcuRegOneByte(0x08, 0x05);
		}
	}
	if (v_UtParam->TrackParam.Volt > HIGH_TUNEVOLT_DATA) //电压过大
	{
		error_num_i++;
		if (error_num_i > 20) {
			UT_Level.Tune_Flag = 0x00;
			error_num_i = 0;
			SendMcuRegOneByte(0x08, 0x02);
		}
	}
	if (v_UtParam->TrackParam.Volt < LOW_VOLT_DATA) //电压过低
	{
		error_num_i++;
		if (error_num_i > 20) {
			error_num_i = 0;
			SendMcuRegOneByte(0x08, 0x03);
		}
	}
  return 0;
}

void initAlarmValue(void) {
	error_num_r = 0;
	error_num_v = 0;
	error_num_i = 0;
}
