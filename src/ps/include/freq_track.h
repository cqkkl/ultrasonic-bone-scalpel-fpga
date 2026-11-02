#include "../include/gic.h"
#include "../include/buck_pwm.h"
#include "../include/data_dec.h"
#include "../include/pid.h"
#include "math.h"
#include "stdint.h"
#include "xuartps_hw.h"
#include "sleep.h"




#ifndef __FREQ_TRACK_H
#define __FREQ_TRACK_H

//扫描起始频率
#define FREQ_SCAN_F_STAR 24000
//扫描截至频率
#define FREQ_SCAN_F_END  25000
//扫描步进频率
#define FREQ_SCAN_F_STEP 1
//扫描点数
#define FREQ_SCAN_F_CNT ((FREQ_SCAN_F_END - FREQ_SCAN_F_STAR) / FREQ_SCAN_F_STEP) + 1
//扫描参数



/* ----------------- BOOLEAN DEFINES ------------------ */
#define  DEF_FALSE                                         0u
#define  DEF_TRUE                                          1u


#define HIGH_SCANRES_DATA   15

#define HIGH_RUNRES_DATA   350
#define HIGH_RUNVOLT_DATA   1000

typedef struct
{
    //当前扫描频率
    float freq_current_out;
    //当前的扫描下的电流值 单位mA
    float current_i;
    //当前的扫描下的电压 单位mV
    float current_v;
    //当前的阻抗
    float current_z;
    //当前扫描下的相位差
    float current_phase_diff;
} scan_paramter_resource_t;

typedef struct
{
    float z;     //阻抗
    float i_rms; //电流有效值
    float v_rms; //电压有效值
    float phase; //相位差
    int freq;  //频率
    int index; //下标
} paras_t;

//特征参数，从scan_paramter_resource_t序列中提取而来
typedef struct
{
    //最小电流
    paras_t min_i;
    //最大电流
    paras_t max_i;
    //最大电压
    paras_t max_v;
    //最小电流
    paras_t min_v;
    //最大阻抗
    paras_t max_z;
    //最小阻抗
    paras_t min_z;
    //最大相位
    paras_t max_phase;

} scan_features_info_t;

typedef struct
{
    //反谐振频率
    int reverse_resonance_freq;
    //反谐振时电压
    float reverse_resonance_v_rms;
    //反谐振时电流
    float reverse__resonance_i_rms;
    //反谐振时相差
    float reverse__resonance_phase_diff;
    //反谐振时阻抗
    float reverse_resonance_z;

    //谐振频率
    int resonance_freq;
    //谐振时电压
    float resonance_v_rms;
    //谐振时电流
    float resonance_i_rms;
    //谐振时相差
    float resonance_phase_diff;
    //谐振时阻抗
    float resonance_z;

    //正常工作起始频率
    int work_freq_start;
    //正常工作截至频率
    int work_freq_end;

} resonance_paramter_t;

void system_output_param_check();
float get_resonance_freq(void);
float get_resonance_phase();
float get_work_freq_start();
float get_work_freq_end();
void freq_track_func();
float get_current_phase_input();
float get_current_output_freq();
#endif
