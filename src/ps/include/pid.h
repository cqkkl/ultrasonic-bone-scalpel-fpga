#ifndef __PID_H
#define __PID_H
#define buckswitch 0


// 位置PD
typedef struct PID
{
    double SetPoint;
    double err;
    double sum_err;
    double P;
    double I;
    double D;
    double err_last_1;
    double err_last_2;
    double output_last;
    double output;
} PID_def;
// 定义两个实体


///************************** Function Prototypes 函数原型******************************/
void IncPIDInit(void);
// 设置控制的目标相位
void set_freq_track_targe(int targe);
// 设置控制的目标电流
void set_current_targe(int targe);
// 相位控制
double freq_track_pid_cal(int phase_diff);
// 功率PID控制环路
int power_track_pid_cal(double power);
int current_pid_cal(int rms);
void set_ctr_resonance_phase(int phase);
void reset_out_freq(double freq);

#endif
