#include "../include/freq_track.h"
/*****************************************************************************************************
DDS的占空比和频率在这个c文件设置，同时定义了扫频函数system_output_param_check；
 *****************************************************************************************************/




static double freq_output;
static int    phase_input;

/************************************数据观测存储空间***************************************/
// 扫描数据存放地
scan_paramter_resource_t  freq_scan_data_buf[FREQ_SCAN_F_CNT];
resonance_paramter_t      resonance_point;
scan_features_info_t      scan_features_info;

typedef enum {
	CURRENT_MIN = 0,
	CURRENT_MAX,
	VOLTAGE_MIN,
	VOLTAGE_MAX,
	Z_MAX,
	Z_MIN,
	PHASE_MAX,
} find_t;


/*******************************************************************************
 * freq_track_func
  *  频率追踪任务，根据相位差进行跟踪
  * 输入参数: 无
  * 返回参数: 无
  * 历史修改记录:
 * <作者><日期><描述>
******************************************************************************/
int  man_ctr = 0; //55584
//实际频率转DDS的频率控制字

uint32_t fre_to_fword(unsigned int  freq)
{
	uint32_t fword;//频率控制字
	uint64_t fword_width = 4294967296;//2^32
	uint32_t fc = 57000000;//采样率50mhz
    fword = (freq * fword_width) / fc;
    return fword;
};

uint32_t phase_to_pword(unsigned int  phase)
{
	uint32_t pword;//相位控制字
	uint16_t pword_width = 1U << 12;//2^12
	uint32_t cycle = 360;//周期360°
    pword = (phase * pword_width) / cycle;
    return pword;
};

void freq_track_func(void) {
	unsigned int  i;
	unsigned int  freq;

	uint64_t fword;//频率控制字
	unsigned char buf[7] = { 0xfb, 0x06, 0x00, 0x00, 0x00, 0x00, 0xed };
	// 获取相位差
	phase_input = phase_sub; //获取相位

	buf[2] = (unsigned char) (phase_input >> 8);
	buf[3] = (unsigned char) (phase_input);

	// 1.根据相位差计算输出频率，ps：间隔250ms,重置频率，防止进入相位曲线峰值右侧
	freq_output = freq_track_pid_cal(phase_input);
	freq = (unsigned int) freq_output;
	fword = fre_to_fword(freq);

	buf[4] = (unsigned char) (freq >> 8);
	buf[5] = (unsigned char) (freq);

	for (i = 0; i < 7; i++)
		XUartPs_SendByte(XPAR_XUARTPS_0_BASEADDR, buf[i]);
	 //频率输出，DDS模块输出频率控制字
	ps_wr_bram_pword(5,0);   //DDS两路方波无相位差
	ps_wr_bram_fword(5,fword);

}

/*******************************************************************************
 * 函数名:		updat_feature_para_from_scan
 * 描述: 将扫描的原始数据特征点提取到scan_features_info中
 *
 * 输入参数: dirction目标地址，resource：原始数据，index,原始数据中的下标
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
static void updat_feature_para_from_scan(paras_t *dirction, scan_paramter_resource_t resource, int index)
{
	dirction->index = index;
	dirction->freq = resource.freq_current_out;
	dirction->i_rms = resource.current_i;
	dirction->phase = resource.current_phase_diff;
	dirction->v_rms = resource.current_v;
	dirction->z = resource.current_z;
}

/*******************************************************************************
 * 函数名:		find_max_min_index
 * 描述: 取出扫频的最小最大值
 * scanning：扫描
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
static void find_max_min_index(scan_paramter_resource_t *resource, find_t type,
		int *max, int *min) {

	int buf[FREQ_SCAN_F_CNT] = { 0 };

	//在找最大最小值这里设限，但我觉得没必要，没用
	int i, max_tem = 0, min_tem = 0, max_tem_index = 0, min_tem_index = 0;

	// 提出数据
	for (i = 0; i < FREQ_SCAN_F_CNT; i++) {
		// p = *(resource+i);
		switch (type) {
		case CURRENT_MAX:
		case CURRENT_MIN:
			buf[i] = (resource + i)->current_i;
			break;
		case VOLTAGE_MAX:
		case VOLTAGE_MIN:
			buf[i] = (resource + i)->current_v;
			break;
		case Z_MAX:
		case Z_MIN:
			buf[i] = (resource + i)->current_z;
			break;
		case PHASE_MAX:
			buf[i] = (resource + i)->current_phase_diff;
		default:
			break;
		}
	}

	// 查找最大最小值
	for (i = 0; i < FREQ_SCAN_F_CNT - 1; i++) {
		if (buf[i] > buf[max_tem_index]) {
			max_tem = buf[i];
			max_tem_index = i;
		}
		if (buf[i] <  buf[min_tem_index]) {
			min_tem = buf[i];
			min_tem_index = i;
		}
	}

	*max = max_tem_index;
	*min = min_tem_index;
}

/*******************************************************************************
 * 函数名:		freq_scan
 * 描述: 频率扫描，扫描结果存放在freq_scan_data_buf中
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
unsigned int sleep_time = 1000, fre = 0;
static void freq_scan(void) {
	int i, fword = 0;
	static float phase;
	for (i = 0; i < FREQ_SCAN_F_CNT; i++)
	{
		freq_scan_data_buf[i].freq_current_out =
				(double) (FREQ_SCAN_F_STAR + FREQ_SCAN_F_STEP * i);

		//更新dds频率
		fword = fre_to_fword(fre);
		ps_wr_bram_fword(1,fword);
		// 输出稳定和电流采集
		usleep(sleep_time); //1ms=1000um
		if (Intr_flag_bram_rd==1)
		{
			freq_scan_data_buf[i].current_i = rms_i        ;//获取的是有效值
			freq_scan_data_buf[i].current_v = rms_u        ;//获取的是有效值;
			freq_scan_data_buf[i].current_z = 1/modulus_div  ;//阻抗扩大了了63倍，还原真实值
			phase = phase_sub;
			freq_scan_data_buf[i].current_phase_diff = phase;
			Intr_flag_bram_rd = 0;
		}
		fre = FREQ_SCAN_F_STAR + FREQ_SCAN_F_STEP * i;
//        buf[4] =fre >>8;// (unsigned char) (((unsigned int)freq_scan_data_buf[i].freq_current_out) >> 8);
//		buf[5] =fre; //(unsigned char) ((unsigned int)freq_scan_data_buf[i].freq_current_out);
//		buf[6] = (unsigned char) (freq_scan_data_buf[i].current_v >> 8);//扫频
//		buf[7] = (unsigned char) (freq_scan_data_buf[i].current_v);
//		buf[2] = (unsigned char) ( freq_scan_data_buf[i].current_i >> 8);//扫频电流
//		buf[3] = (unsigned char) ( freq_scan_data_buf[i].current_i);
//		buf[8] = (unsigned char) (freq_scan_data_buf[i].current_z >> 8);//扫频阻抗
//		buf[9] = (unsigned char) (freq_scan_data_buf[i].current_z);
//        printf("%d,%3d,%4.2f, %3d ,%d\r\n",fre,freq_scan_data_buf[i].current_v,
//        freq_scan_data_buf[i].current_i,freq_scan_data_buf[i].current_z,phase);
//		for (k = 0; k < 11; k++) //打印扫频数据
//			XUartPs_SendByte(XPAR_PS7_UART_1_BASEADDR, buf[k]);
		//    led_turn_state(LED0, (led_sta_t)(i & 0x01));
	}

	//  led_turn_state(LED0, OFF);
}

/*******************************************************************************
 * 函数名:		update_featrue_param
 * 描述: 根据扫描参数，提取特征参数
 *
 * 输入参数: 无
 * 返回参数: 电流最大频率点
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
static double update_featrue_param() {
	int min_point_index = 0, max_point_index = 0;


	// 查找阻抗
	find_max_min_index(freq_scan_data_buf, Z_MAX, &max_point_index,
			&min_point_index);
	updat_feature_para_from_scan(&scan_features_info.min_z,
			freq_scan_data_buf[min_point_index], min_point_index);
	updat_feature_para_from_scan(&scan_features_info.max_z,
			freq_scan_data_buf[max_point_index], max_point_index);

	find_max_min_index(freq_scan_data_buf, CURRENT_MAX, &max_point_index,
			&min_point_index);
	updat_feature_para_from_scan(&scan_features_info.min_i,
			freq_scan_data_buf[min_point_index], min_point_index);
	updat_feature_para_from_scan(&scan_features_info.max_i,
			freq_scan_data_buf[max_point_index], max_point_index);

	find_max_min_index(freq_scan_data_buf, VOLTAGE_MAX, &max_point_index,
			&min_point_index);
	updat_feature_para_from_scan(&scan_features_info.min_v,
			freq_scan_data_buf[min_point_index], min_point_index);
	updat_feature_para_from_scan(&scan_features_info.max_v,
			freq_scan_data_buf[max_point_index], max_point_index);

	find_max_min_index(freq_scan_data_buf, PHASE_MAX, &max_point_index,
			&min_point_index);
	updat_feature_para_from_scan(&scan_features_info.max_phase,
			freq_scan_data_buf[max_point_index], max_point_index);

	return 0;
}
 char TuneData[10] = { 0 };  //这里为了数据传输到pc端，对数据进行截取，串口只能8位传输
/*******************************************************************************
 * 函数名:		find_resonce_para
 * 描述: 查找谐振参数
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void find_resonce_para(find_t type) {
	paras_t *scan_features_info_tmp;
	switch (type) {
	case CURRENT_MAX:
		scan_features_info_tmp = &scan_features_info.max_i;
		break;
	case CURRENT_MIN:
		scan_features_info_tmp = &scan_features_info.min_i;
		break;
	case VOLTAGE_MAX:
		scan_features_info_tmp = &scan_features_info.max_v;
		break;
	case VOLTAGE_MIN:
		scan_features_info_tmp = &scan_features_info.min_v;
		break;
	case PHASE_MAX:
		scan_features_info_tmp = &scan_features_info.max_phase;
		break;
	case Z_MAX:
		scan_features_info_tmp = &scan_features_info.max_z;
		break;
	case Z_MIN:
		scan_features_info_tmp = &scan_features_info.min_z;
		break;
	default:
		return;
		break;
	}
	resonance_point.resonance_freq        = scan_features_info_tmp->freq;      //24489hz
    resonance_point.resonance_i_rms       = scan_features_info_tmp->i_rms;
	resonance_point.resonance_phase_diff  = scan_features_info_tmp->phase;
	resonance_point.resonance_v_rms       = scan_features_info_tmp->v_rms;
	resonance_point.resonance_z           = scan_features_info_tmp->z;
	resonance_point.work_freq_start       = resonance_point.resonance_freq - 100.0;
	resonance_point.work_freq_end         = resonance_point.resonance_freq + 1000.0;
//	printf("freq   = %d\r\n",     resonance_point.resonance_freq);
//	printf("diff   = %f\r\n",     resonance_point.resonance_phase_diff);
//	printf("z      = %f\r\n",     resonance_point.resonance_z);
//	printf("v_rms  = %f\r\n",     resonance_point.resonance_v_rms);
//	printf("i_rms  = %f\r\n",     resonance_point.resonance_i_rms);

	TuneData[1] = resonance_point.resonance_freq >> 8;
    TuneData[0] = resonance_point.resonance_freq;
	//resonance_point.resonance_phase_diff = resonance_point.resonance_phase_diff /100;
    TuneData[3] = (int)((resonance_point.resonance_phase_diff)*100) >> 8;//相位差扩大100倍转int发给上位机
	TuneData[2] = (int)((resonance_point.resonance_phase_diff)*100);

	TuneData[5] = (unsigned int)((resonance_point.resonance_z)*1) >> 8;
	TuneData[4] = (unsigned int)((resonance_point.resonance_z)*1);

	TuneData[7] = (int)resonance_point.resonance_v_rms >> 8;//本身就扩了1000倍了
	TuneData[6] = (int)resonance_point.resonance_v_rms;

	TuneData[9] = (int)resonance_point.resonance_i_rms >> 8;
	TuneData[8] = (int)resonance_point.resonance_i_rms;

//发送数据给上位机
    //SJANT_PC_USER_DAT(TuneData , 10);

}
float bucksetvalue_duty =  0.06;
unsigned int   bucksetvalue_fre = 40000, BrigesetValue = 400, phasesetValue = 6000; //375 300

/*******************************************************************************
 * 函数名:		system_output_param_check
 * 描述: 输出参数确定，扫频
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
void system_output_param_check() {

	// buck_set_duty(PWM, 300);
	// 1.频率扫描
	//buck_pwm_set_duty(bucksetvalue_duty);    // 6% 左右占空
	//buck_pwm_set_fre(bucksetvalue_fre);    //40khz的pwm
	freq_scan();


	// 2.送显示
	//wave_updat(freq_scan_data_buf, FREQ_SCAN_F_CNT);

	// 3.更新特征参数
	update_featrue_param();

	// 4.查找谐振点
	find_resonce_para(Z_MIN);

	// 设置控制相位
	// set_ctr_resonance_phase(resonance_point.resonance_phase_diff);
}

/*******************************************************************************
 * 函数名返回刀具的谐振频率
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_resonance_freq(void) {
	return resonance_point.resonance_freq;
}

/*******************************************************************************
 * 函数名返回刀具的谐振相位
 *
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_resonance_phase() {
	return resonance_point.resonance_phase_diff;
}
/*******************************************************************************
 * 返回频率控制范围
 *get_work_freq_start
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_work_freq_start() {
	return resonance_point.work_freq_start;
}
/*******************************************************************************
 * 返回频率控制范围
 *get_work_freq_end
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_work_freq_end() {
	return resonance_point.work_freq_end;
}

/*******************************************************************************
 * get_current_output_freq
 *  返回当前正在输出的频率
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_current_output_freq() {
	return freq_output;
}

/*******************************************************************************
 * get_current_phase_input
 *  返回当前的相位差
 * 输入参数: 无
 * 返回参数: 无
 * 历史修改记录:
 * <作者><日期><描述>
 ******************************************************************************/
float get_current_phase_input() {
	return phase_input;
}

