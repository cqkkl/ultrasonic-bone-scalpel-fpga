/*******************************************************************************
 *  将PL端获取的二进制数据进行换算,变为int有符号十进制整数DEC，与实际值对应，具有一定的比例关系


 * <作者><日期><描述>
 ******************************************************************************/
#include "../include/data_dec.h"
#include <math.h>

#define PI acos(-1)
double kaerman_Q = 0.0001;
double kaerman_R = 100.0;
double x1 = 0.0;
double p = 0.0; //协方差
#define Calc_voltage_r(x) (5*x)//放大5倍，mv
#define Current_r 5 //电流比例
double kaerman_filter(double dat)
{
    double x = 0.0;
    double kg = 0.0;          //权重
    x = x1;                   //预估计k时刻状态变量的值
    p = p + kaerman_Q;        //对应于预估值的协方差
    kg = p / (p + kaerman_R); // kalman gain
    x = x + kg * (dat - x);
    p = (1 - kg) * p;
    x1 = x;
    return x;
}

void Bubble_sort(int *arr, int size)
{
    int j, i, tem;
    for (i = 0; i < size - 1; i++) // size-1是因为不用与自己比较，所以比的数就少一个
    {
        int count = 0;
        for (j = 0; j < size - 1 - i; j++) // size-1-i是因为每一趟就会少一个数比较
        {
            if (arr[j] > arr[j + 1]) //这是升序排法，前一个数和后一个数比较，如果前数大则与后一个数换位置
            {
                tem = arr[j];
                arr[j] = arr[j + 1];
                arr[j + 1] = tem;
                count = 1;
            }
        }
        if (count == 0) //如果某一趟没有交换位置，则说明已经排好序，直接退出循环
            break;
    }
}


/*******************************************************************************
 函数名: mid_filter_per_data_for_phase
 描述:   单点中值滤波
 输入参数:   dat:新数据，len,滤波长度，奇数
 输出参数:   无
 函数返回值:
 历史修改记录:
 <作者><日期><描述>
 ******************************************************************************/

int mid_filter_per_data_for_phase(int dat, unsigned int len, unsigned char reset)
{
    //缓存空间
    static int buf[20] = {0};
    int buf_tem[20] = {0}, min_index = 0;

    int min = 0;
    int i = 0, j = 0;
    if (reset == 1)
        memset(buf, 0, 21);//复制字符 0（一个无符号字符）到参数 buf 所指向的字符串的前 20个字符
    len = len < 20 ? len : 20;

    //移位数据,保存新数据
    for (i = 0; i < len - 1; i++)
    {
        buf[i] = buf[i + 1];
        buf_tem[i] = buf[i];
    }
    buf[i] = dat;
    buf_tem[i] = buf[i];

    //查找排序
    min = buf[0];
    min_index = len / 2;
    for (i = 0; i < min_index + 1; i++)
    {
        for (j = i + 1; j < len - 1; j++)
        {
            if (buf_tem[i] > buf_tem[j])
            {
                min = buf_tem[i];
                buf_tem[i] = buf_tem[j];
                buf_tem[j] = min;
            }
        }
    }

    return buf_tem[min_index];
}


// 函数定义：计算数组中除去最大值和最小值后的均值
float averageWithoutMinMax(float arr[], int size) {
    // 确保数组至少有五个元素
    if (size < 5) {
        printf("数组元素个数不足五个，无法计算\n");
        return 0.0;
    }

    // 寻找最大值和最小值的索引
//    int maxIndex = 0, minIndex = 0;
//    for (int i = 1; i < size; i++) {
//        if (arr[i] >arr[maxIndex]) {
//            maxIndex = i;
//        }
//        if (arr[i] < arr[minIndex]) {
//            minIndex = i;
//        }
//    }

    // 计算除去最大值和最小值后的三个数的和
    int sum = 0;
    for (int i = 0; i < size; ++i)
        {

            sum += (float)arr[i];
        }


    // 计算均值并返回
    return (float)sum / 5.0;
}

//返回电压有效值mv 放大1000倍 返回电压mV，pl端放大200，ps放大5
//在电路设计时候，电压缩小3x21=63，后与电流同时扩大2.6倍
//pl端计算的有效值是模长，即rms*根号2，而且iir滤波器有衰减，实际值要除以1.4，通带衰减系数3db=20log(1.412)
float get_voltage_dec(int num,int index)
{


	float data;

    ps_rd_bram_rms_u(num,index)    ;
   // printf(" 开始读电压有效值\t\n") ;

    data = (float) Calc_voltage_r(read_data_rms_u)*63/1.412;



    return data;
}

//返回电流有效值 mA
float get_curre_dec(int num,int index)
{
	float data;

    ps_rd_bram_rms_i(num,index)    ;
   // printf(" 开始读电流有效值\t\n") ;

    data =  (float)Calc_voltage_r(read_data_rms_i)/1.412;
  //  printf("BRAM_rms_i_actual  is %f\t\n", data) ;
    return data;
}


//定义一个fixdt（1,32,20）转float函数
float fixed_1_32_20_point_to_decimal( int fixed_point) {
	float a ;
	a = ((float) fixed_point / (2<<19) ) * 180 / PI;
    return a;
}

//返回相位差

float get_phase_dec(int num,int index)
{
	float data;

    ps_rd_bram_phase_sub(num,index)    ;
    //printf(" 开始读相位差\t\n") ;

   //因为采集得到的phase_sub是定点小数，所以需要转换为float
    data = fixed_1_32_20_point_to_decimal(read_data_phase_sub);
    //将采集到的数组转换成有效值
    if( (180<=data) && (data<=360) )
           {
    	data = data - 360 ;
           }
    else if ( (-360 <= data) && (data <= -180) )
           {
    	data = data + 360;
           }

   // printf("  %f\t\n",testdiff[i]) ;
   //printf("BRAM_PHASE_SUB_actual  is %f\t\n", data) ;
    return data;

}



//返回动态支路电流有效值mA
float get_current_dynamic_dec(int num,int index)
{
	float data;

    ps_rd_bram_rms_dynamic(num,index)    ;
   // printf(" 开始读动态支路电流有效值\t\n") ;

    data =(float)Calc_voltage_r(read_data_rms_dynamic)/1.412;
   // printf("BRAM_rms_dynamic_actual  is %f\t\n", data) ;
    return data;
}


//定义一个无符号fixdt（32，8）转float函数
float fixed_0_32_8_point_to_decimal(unsigned int fixed_point) {
	    float a ;
		a = ((float) fixed_point / (2<<7) ) ;
	    return a;
}


//返回模长商，I/U，pl传上来的值是fixdt（0,32,8）
float get_modulus_div_dec(int num,int index)
{
	float data;

    ps_rd_bram_modulus_div(num,index)    ;
   // printf(" 开始读模长商\t\n") ;

   //因为采集得到的modulus_div是定点小数，所以需要转换为float,实际电路u缩小了63倍
    data = fixed_0_32_8_point_to_decimal(read_data_modulus_div)/63;//i/(u*63)
   // printf("BRAM_modulus_div_actual  is %f\t\n", data) ;
    return data;
}


