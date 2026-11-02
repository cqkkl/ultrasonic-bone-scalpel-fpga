
#include "xparameters.h"
#include "xil_io.h"
#include "../include/rms.h"
#include "../include/gic.h"

#include "../include/data_dec.h"


double kaerman_Q1 = 0.01;
double kaerman_R1 = 1.0;
double x11 = 0.0;
double p1 = 0.0; //
double kaerman_filter1(double dat)
{
    double x1 = 0.0;
    double kg1 = 0.0;             //
    x1 = x11;                     //
    p1 = p1 + kaerman_Q1;         //
    kg1 = p1 / (p1 + kaerman_R1); // kalman gain
    x1 = x1 + kg1 * (dat - x1);
    p1 = (1 - kg1) * p1;
    x11 = x1;
    return x1;
}

int math_func_average(int dat)
{
    static int buf[32] = {0}, sum = 0;
    int i = 0;

    sum -= buf[0];
    sum = sum < 0 ? 0 : sum;

    for (i = 0; i < 31; i++)
    {
        buf[i] = buf[i + 1];
    }

    buf[31] = dat;
    sum += buf[31];

    return (sum >> 5);
}

// 数据更新频率为1ms
// 电流环采样电阻、线缆、采样板的输入阻抗不匹配，数据采集有偏差，需要后期矫正
// 获取通道电流有效值
int get_i_rms_from_ch(int *res)
{
    int buf = 0;

    buf = rms_i; //获取电流
    // *res = buf;
    *res = (int)kaerman_filter1((double)buf);

    //*res = math_func_average(buf);
    return (buf);
}

// 获取通道电压有效值
int get_v_rms_from_ch()
{
    return  rms_u;//获取电压值
}
