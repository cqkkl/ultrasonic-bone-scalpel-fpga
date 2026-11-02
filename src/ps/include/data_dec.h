#ifndef __DATA_DEC_H
#define __DATA_DEC_H
#include "buck_pwm.h"
#include "../include/bram_wr_rd.h"
// 频率跟踪结构体
typedef struct track_param
{
//    TRACK_STATE         State;
	CPU_INT16U          UT_ATP_State;
    CPU_INT32U          Frequence;
    CPU_INT32U          Current;
    CPU_INT32U          Volt;
    CPU_INT32S          Phasecha;
    CPU_INT32S          Power;
    CPU_INT32U          Res;
    CPU_INT32S          DesPhasecha;
//    CPU_INT32U          Counter;
//    DIRECTION           Direction;
//    CPU_INT32U          CurrentThreshold;
    CPU_INT16U          DesCurrent;
//    FPGA_DATA_S         FpgaData;
//    CPU_INT32U          MaxRes;
//    CPU_INT32U          MaxFreq;
//    CPU_INT32U          MinFreq;
//    CPU_INT08U          FChFlag;   //0:当前运行于谐振点附近 1:当前运行于反谐振点附近
} TRACK_PARAM;


// 超声结构体
typedef struct ut_param
{
//    UT_STATE        State;
//    CPU_INT08U      UtMin;
//    CPU_INT08U      UtMax;
//    CPU_INT08U      UtRun;
//    CPU_INT08U      CurUtRun;
//    CPU_INT16U      BuckPwmDuty;
//    CPU_INT32U      UtOutTimesForSig;
//    CPU_INT16U      UtTimesWaitSig;
//    SCAN_PARAM      TuneParam;
//    SCAN_PARAM      FastScan;
    TRACK_PARAM     TrackParam;

}UT_PARAM;
//#define RMS0_Offset    AXI_IP_CON_S00_AXI_SLV_REG0_OFFSET
//#define RMS1_Offset    AXI_IP_CON_S00_AXI_SLV_REG1_OFFSET
//#define Angle          AXI_IP_CON_S00_AXI_SLV_REG2_OFFSET
float get_phase_dec(int num,int index);
float get_curre_dec(int num,int index);
float get_voltage_dec(int num,int index);
float get_current_dynamic_dec(int num,int index);
float get_modulus_div_dec(int num,int index);
float fixed_0_32_8_point_to_decimal(unsigned int fixed_point);
float fixed_1_32_20_point_to_decimal( int fixed_point) ;
float averageWithoutMinMax(float arr[], int size);



#endif

