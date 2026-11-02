#ifndef BRAM_WR_RD_H
#define BRAM_WR_RD_H
#include "xparameters.h"


//函数声明
 int ps_wr_bram_fword(int data_len, int data);
 int ps_wr_bram_pword(int data_len, int data);
 int ps_wr_bram_c0(int data_len, int data);

void ps_rd_bram_modulus_div(int data_len,int index);
void ps_rd_bram_phase_sub(int data_len,int index);
void ps_rd_bram_rms_i(int data_len,int index);
void ps_rd_bram_rms_u(int data_len,int index);
void ps_rd_bram_rms_dynamic(int data_len,int index);

          int read_data_phase_sub;
 unsigned int read_data_modulus_div;
          int read_data_rms_i;
          int read_data_rms_u;
          int read_data_rms_dynamic;

                                    /*各个自定义pl_bram模块的基地址*/
//PS写入PL
 #define START_ADDR_FWORD                XPAR_RD_WHEEL_SEARCH_FWORD_S00_AXI_BASEADDR                                                 //RAM 起始地址 范围:0~1023
 #define END_ADDR_FWORD                  XPAR_RD_WHEEL_SEARCH_FWORD_S00_AXI_HIGHADDR

 #define START_ADDR_PWORD                XPAR_RD_WHEEL_SEARCH_PWORD_S00_AXI_BASEADDR
 #define END_ADDR_PWORD                  XPAR_RD_WHEEL_SEARCH_PWORD_S00_AXI_HIGHADDR

 #define START_ADDR_C0                   XPAR_RD_WHEEL_SEARCH_C0_S00_AXI_BASEADDR
 #define END_ADDR_C0                     XPAR_RD_WHEEL_SEARCH_C0_S00_AXI_HIGHADDR

 //PL写入PS
 #define START_ADDR_MODULUS_DIV          XPAR_PL_BRAM_WR_LITE_MODULUS_DIV_S00_AXI_BASEADDR
 #define END_ADDR_MODULUS_DIV            XPAR_PL_BRAM_WR_LITE_MODULUS_DIV_S00_AXI_HIGHADDR

 #define START_ADDR_PHASE_SUB            XPAR_PL_BRAM_WR_LITE_PHASE_SUB_S00_AXI_BASEADDR
 #define END_ADDR_PHASE_SUB              XPAR_PL_BRAM_WR_LITE_PHASE_SUB_S00_AXI_HIGHADDR

 #define START_ADDR_RMS_DYNAMIC          XPAR_PL_BRAM_WR_LITE_RMS_DYNAMIC_S00_AXI_BASEADDR
 #define END_ADDR_RMS_DYNAMIC            XPAR_PL_BRAM_WR_LITE_RMS_DYNAMIC_S00_AXI_HIGHADDR

 #define START_ADDR_RMS_I                XPAR_PL_BRAM_WR_LITE_RMS_I_S00_AXI_BASEADDR
 #define END_ADDR_RMS_I                  XPAR_PL_BRAM_WR_LITE_RMS_I_S00_AXI_HIGHADDR

 #define START_ADDR_RMS_U                XPAR_PL_BRAM_WR_LITE_RMS_U_S00_AXI_BASEADDR
 #define END_ADDR_RMS_U                  XPAR_PL_BRAM_WR_LITE_RMS_U_S00_AXI_HIGHADDR


                               /*各个bram模块start、start_addr、LEN  地址*/
//fword、pword、c0、
 #define PL_BRAM_START_RD        RD_WHEEL_SEARCH_S00_AXI_SLV_REG0_OFFSET          //RAM 读开始寄存器地址
 #define PL_BRAM_START_ADDR_RD   RD_WHEEL_SEARCH_S00_AXI_SLV_REG1_OFFSET          //RAM 起始地址寄存器地址
 #define PL_BRAM_LEN_RD          RD_WHEEL_SEARCH_S00_AXI_SLV_REG2_OFFSET          //PL 读 RAM 数据的地址长度
//phase_sub、rms_dynamic、rms_i、rms_u

//modulus_div

                                    /*各个axi_bram_ctrl模块的基地址*/
 #define START_ADDR_AXI_BRAM_CTRL_C0                XPAR_AXI_BRAM_CTRL_C0_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_C0                  XPAR_AXI_BRAM_CTRL_C0_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_MODULUS_DIVISION  XPAR_AXI_BRAM_CTRL_MODULUS_DIV_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_MODULUS_DIVISION    XPAR_AXI_BRAM_CTRL_MODULUS_DIV_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_PHASE_SUB         XPAR_AXI_BRAM_CTRL_PHASE_SUB_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_PHASE_SUB           XPAR_AXI_BRAM_CTRL_PHASE_SUB_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_PWORD             XPAR_AXI_BRAM_CTRL_PWORD_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_PWORD               XPAR_AXI_BRAM_CTRL_PWORD_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_FWORD             XPAR_AXI_BRAM_CTRL_FWORD_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_FWORD               XPAR_AXI_BRAM_CTRL_FWORD_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_RMS_I             XPAR_AXI_BRAM_CTRL_RMS_I_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_RMS_I               XPAR_AXI_BRAM_CTRL_RMS_I_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_RMS_U             XPAR_AXI_BRAM_CTRL_RMS_U_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_RMS_U               XPAR_AXI_BRAM_CTRL_RMS_U_S_AXI_HIGHADDR

 #define START_ADDR_AXI_BRAM_CTRL_RMS_DYNAMIC       XPAR_AXI_BRAM_CTRL_RMS_DYNAMIC_S_AXI_BASEADDR
 #define END_ADDR_AXI_BRAM_CTRL_RMS_DYNAMIC         XPAR_AXI_BRAM_CTRL_RMS_DYNAMIC_S_AXI_HIGHADDR











#endif
