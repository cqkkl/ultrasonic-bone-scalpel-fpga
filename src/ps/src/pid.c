#include "../include/pid.h"
#include <math.h>
#include "../include/freq_track.h"


#define  MAXOUT           1000                     // 输出最大值

#define  StartFreq        54800
#define  EndFreq          56100

#define  Start1Freq       54800                    //长刀
#define  End1Freq         56100
#define  Phase_Amp_Posi   1450                     //误差限幅
#define  Phase_Amp_Neg    -1450

PID_def PID_Current;
resonance_paramter_t resonance_point;
PID_def PID_Freq_Track;
PID_def PID_Power_Track;
void IncPIDInit(void) {
	// 设定电流值
	PID_Current.SetPoint = 200;          //电流初始值，也是目标值
	PID_Current.err_last_1 = 0;
	PID_Current.err_last_2 = 0;
	PID_Current.output_last = 0;         //电流实际输出值
	PID_Current.sum_err = 0.0;

	PID_Current.P = 0.5;                 // -0.01
	PID_Current.I = 0.0001;              // -0.002
	PID_Current.D = 0;                   //0       时间5.22

	// 设定频率追踪
	PID_Freq_Track.SetPoint = 6000.0;     //相位
	PID_Freq_Track.err_last_1 = 0;
	PID_Freq_Track.err_last_2 = 0;
	PID_Freq_Track.output_last = 0;
	PID_Freq_Track.P = 0.003;//-0.03;             //0.05
	PID_Freq_Track.I = 0.0006;//-0.0001;           //0
	PID_Freq_Track.D = 0;                 //0.1  时间5.22

	PID_Power_Track.SetPoint = 2;
	PID_Power_Track.err_last_1 = 0;
	PID_Power_Track.err_last_2 = 0;
	PID_Power_Track.output_last = 0;
	PID_Power_Track.P = 0.5;
	PID_Power_Track.I = 0.002;
	PID_Power_Track.D = 0;
}

// 设置控制的目标相位
void set_freq_track_targe(int targe) {
	PID_Freq_Track.SetPoint = targe;
}

// 设置控制的目标电流
void set_current_targe(int targe) {
	PID_Current.SetPoint = targe;
}

// 设置复位输出频率
void reset_out_freq(double freq) {
	PID_Freq_Track.output_last = freq;
}

//功率控制
/*一开始采用恒流的方法恒功率，但是负载使阻抗上升太大，
 电流变得太小即便电压拉到上线也提升不到目标电流，
 选择恒功率p=ui*cos（phase_diff）的方案*/
int power_track_pid_cal(double power) {
	int err = 0;
	// 计算当前误差
	PID_Power_Track.err = PID_Power_Track.SetPoint - (double) power;

	//误差限幅
//    if (PID_Power_Track.err > 50)
//        PID_Power_Track.err = 50;
//    else if (PID_Power_Track.err < -50)
//        PID_Power_Track.err = -50;

	// 计算输出增量
	PID_Power_Track.output = PID_Power_Track.output_last
			+ (PID_Power_Track.P
					* (PID_Power_Track.err - PID_Power_Track.err_last_1)
					+ PID_Power_Track.I * PID_Power_Track.err
					+ PID_Power_Track.D
							* (PID_Power_Track.err
									- 2 * PID_Power_Track.err_last_1
									+ PID_Power_Track.err_last_2));

	// 迭代

	PID_Power_Track.err_last_2 = PID_Power_Track.err_last_1;
	PID_Power_Track.err_last_1 = err;

	//限制恒功率输出范围，0-600
	PID_Power_Track.output =
			PID_Power_Track.output > 600.0 ? 600.0 : PID_Power_Track.output;
	PID_Power_Track.output =
			PID_Power_Track.output < 0.0 ? 0.0 : PID_Power_Track.output;

	PID_Power_Track.output_last = PID_Power_Track.output;

	return (int) PID_Power_Track.output;
}


// 相位控制
double freq_track_pid_cal(int phase_diff) {

	// 计算当前误差 相位
	PID_Freq_Track.err = PID_Freq_Track.SetPoint - (double) phase_diff;

	// 误差限幅
//       if (PID_Freq_Track.err > Phase_Amp_Posi)
//    	PID_Freq_Track.err = Phase_Amp_Posi;
//    else if (PID_Freq_Track.err < Phase_Amp_Neg)
//    	PID_Freq_Track.err = Phase_Amp_Neg;

	PID_Freq_Track.output = PID_Freq_Track.output_last
			+ (PID_Freq_Track.P
					* (PID_Freq_Track.err - PID_Freq_Track.err_last_1)
					+ PID_Freq_Track.I * PID_Freq_Track.err
					+ PID_Freq_Track.D
							* (PID_Freq_Track.err
									- 2 * PID_Freq_Track.err_last_1
									+ PID_Freq_Track.err_last_2));

	if (resonance_point.resonance_freq < 55500) {
		if (PID_Freq_Track.output > EndFreq)
			PID_Freq_Track.output = StartFreq;
		else if (PID_Freq_Track.output < StartFreq)
			PID_Freq_Track.output = EndFreq;   //resonance_point.resonance_freq;
	} else {
		if (PID_Freq_Track.output > End1Freq)
			PID_Freq_Track.output = Start1Freq;
		else if (PID_Freq_Track.output < Start1Freq)
			PID_Freq_Track.output = End1Freq;  //resonance_point.resonance_freq;
	}
	// 迭代
	PID_Freq_Track.output_last = PID_Freq_Track.output;
	PID_Freq_Track.err_last_2 = PID_Freq_Track.err_last_1;
	PID_Freq_Track.err_last_1 = (double) PID_Freq_Track.err;
	return PID_Freq_Track.output;
}

// 复位pwm输出频率

// 电流PID控制环路
int current_pid_cal(int rms) {

	int err = 0;
	// 计算当前误差
	PID_Current.err = PID_Current.SetPoint - (double) rms;

	// 误差限幅
//
//    if (PID_Current.err > 200)
//        PID_Current.err = 200;
//    else if (PID_Current.err < -200)
//        PID_Current.err = -200;

	// 计算输出增量
	PID_Current.output = PID_Current.output_last
			+ (PID_Current.P * (PID_Current.err - PID_Current.err_last_1)
					+ PID_Current.I * PID_Current.err
					+ PID_Current.D
							* (PID_Current.err - 2 * PID_Current.err_last_1
									+ PID_Current.err_last_2));

	// 迭代

	PID_Current.err_last_2 = PID_Current.err_last_1;
	PID_Current.err_last_1 = err;

	PID_Current.output =
			PID_Current.output > 1000.0 ? 1000.0 : PID_Current.output;
	PID_Current.output = PID_Current.output < 0.0 ? 0.0 : PID_Current.output;

	PID_Current.output_last = PID_Current.output;

	return (int) PID_Current.output;
}


//设置谐振下的相位

void set_ctr_resonance_phase(int phase) {
	PID_Freq_Track.SetPoint = (double) phase;
}
