#ifndef __GIC_H
#define __GIC_H

#include "../include/data_dec.h"
#include "xscugic.h"
#include "xstatus.h"
#include "bram_wr_rd.h"
#include "xil_exception.h"

typedef void (*IntrHandler_ptr)(void *CallbackRef);


#define INTC_DEVICE_ID	                       XPAR_SCUGIC_SINGLE_DEVICE_ID //通用中断控制器 ID

#define INTR_ID_PHASE_SUB_ID                   XPAR_FABRIC_PL_BRAM_WR_LITE_PHASE_SUB_INTR_INTR
#define INTR_ID_MODULUS_DIV_ID                 XPAR_FABRIC_PL_BRAM_WR_LITE_MODULUS_DIV_INTR_INTR
#define INTR_ID_RMS_I_ID                       XPAR_FABRIC_PL_BRAM_WR_LITE_RMS_I_INTR_INTR
#define INTR_ID_RMS_U_ID                       XPAR_FABRIC_PL_BRAM_WR_LITE_RMS_U_INTR_INTR
#define INTR_ID_RMS_DYNAMIC_ID                 XPAR_FABRIC_PL_BRAM_WR_LITE_RMS_DYNAMIC_INTR_INTR

float   phase_sub ;
float   modulus_div ;
float   rms_i ;
float   rms_u ;
float   rms_dynamic ;

int Intr_flag_bram_rd;
int Intr_flag_phase_sub;
int Intr_flag_modulus_div;
int Intr_flag_rms_i;
int Intr_flag_rms_u;
int Intr_flag_rms_dynamic;
//定义GIC驱动实例,这些都没有用到
XScuGic gic_ins_ptr_phase_sub;
XScuGic gic_ins_ptr_modulus_div;
XScuGic gic_ins_ptr_rms_i;
XScuGic gic_ins_ptr_rms_u;
XScuGic gic_ins_ptr_rms_dynamic;
 /************************** Function Prototypes ******************************/
 void gic_get_object(uint32_t* addr,XScuGic gic_ins);
 int IntrInitFuntion(XScuGic *gic_ins,u16 INTR_ID,int priority,IntrHandler_ptr IntrHandler_ptr);//接收PL端的intr中断
 void IntrHandler_phase_sub(void *CallbackRef);//中断服务函数
 void IntrHandler_modulus_div(void *CallbackRef);//中断服务函数
 void IntrHandler_rms_i(void *CallbackRef);
 void IntrHandler_rms_u(void *CallbackRef);
 void IntrHandler_rms_dynamic(void *CallbackRef);
 void IntrHandler(void *CallbackRef);

 /**************************Global Variable Definitions ***********************/

#endif
