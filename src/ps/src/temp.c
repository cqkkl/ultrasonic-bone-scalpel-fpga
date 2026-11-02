/*
 * temp.c
 *
 *  Created on: 2023年11月21日
 *      Author: yanghang
 */

#include "xadcps.h"
#include "../include/temp.h"
#define XADC_DEVICE_ID XPAR_XADCPS_0_DEVICE_ID //PS XADC 器件 ID
static XAdcPs     xadc_inst; //XADC 驱动实例
u32 temp_rawdata; //温度 原始数据

u32 vcc_pint_rawdata; //PS 内核电压 原始数据
 u32 vcc_paux_rawdata; //PS 辅助电压 原始数据
 u32 vcc_pddr_rawData; //PS DDR 电压 原始数据
 u32 vcc_int_rawdata; //PL 内核电压 原始数据
 u32 vcc_aux_rawdata; //PL 辅助电压 原始数据
 u32 vcc_bram_rawData; //PL BRAM 电压 原始数据
 float temp; //温度
 float vcc_pint; //PS 内核电压
 float vcc_paux; //PS 辅助电压
 float vcc_pddr; //PS DDR 电压
 float vcc_int; //PL 内核电压
 float vcc_aux; //PL 辅助电压
 float vcc_bram; //PL BRAM 电压
 void getpstempInit(void)
 {
     XAdcPs_Config *ConfigPtr; //XADC 配置指针
	 ConfigPtr = XAdcPs_LookupConfig(XADC_DEVICE_ID);
	 XAdcPs_CfgInitialize(&xadc_inst, ConfigPtr, ConfigPtr->BaseAddress);

	 //设置 XADC 操作模式为“默认安全模式”
	  XAdcPs_SetSequencerMode(&xadc_inst, XADCPS_SEQ_MODE_SAFE);

 }
 void gettempvalue(void)
 {
	 //获取原始温度传感器数据
	 temp_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_TEMP);
	 //转换成温度信息
	 temp = XAdcPs_RawToTemperature(temp_rawdata);

//	 //获取 VCCPINT 传感器数据，并转换成电压信息
//	 vcc_pint_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VCCPINT);
//	 vcc_pint = XAdcPs_RawToVoltage(vcc_pint_rawdata);
//
//	 //获取 VCCPAUX 传感器数据，并转换成电压信息
//	 vcc_paux_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VCCPAUX);
//	 vcc_paux = XAdcPs_RawToVoltage(vcc_paux_rawdata);
//
//	 //获取 VCCPDRO 传感器数据，并转换成电压信息
//	 vcc_pddr_rawData = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VCCPDRO);
//	 vcc_pddr = XAdcPs_RawToVoltage(vcc_pddr_rawData);
//
//	 //获取 VCCINT 传感器数据，并转换成电压信息
//	 vcc_int_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VCCINT);
//	 vcc_int = XAdcPs_RawToVoltage(vcc_int_rawdata);
//
//	 //获取 VCCAUX 传感器数据，并转换成电压信息
//	 vcc_aux_rawdata = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VCCAUX);
//	 vcc_aux = XAdcPs_RawToVoltage(vcc_aux_rawdata);
//
//	 //获取 VBRAM 传感器数据，并转换成电压信息
//	 vcc_bram_rawData = XAdcPs_GetAdcData(&xadc_inst, XADCPS_CH_VBRAM);
//	 vcc_bram = XAdcPs_RawToVoltage(vcc_bram_rawData);

	 //打印温度、电压信息
//	 printf("Raw VccPInt %lu, Real VccPInt %fV \n", vcc_pint_rawdata, vcc_pint);
//	 printf("Raw Temp %lu, Real Temp %fC \n", temp_rawdata, temp);
//	 printf("Raw VccPAux %lu, Real VccPAux %fV \n", vcc_paux_rawdata, vcc_paux);
//	 printf("Raw VccPDDR %lu, Real VccPDDR %fV \n", vcc_pddr_rawData, vcc_pddr);
//	 printf("Raw VccInt %lu, Real VccInt %fV \n", vcc_int_rawdata, vcc_int);
//	 printf("Raw VccAux %lu, Real VccAux %fV \n", vcc_aux_rawdata, vcc_aux);
//	 printf("Raw VccBram %lu, Real VccBram %fV \n\r", vcc_bram_rawData, vcc_bram);

 }
