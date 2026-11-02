 #include "../include/bram_wr_rd.h"

 #include "xil_printf.h"
 #include "stdio.h"
 #include "pl_bram_wr_lite.h"
 #include "rd_wheel_search.h"
 #include "xbram.h"



 #define START_ADDR_WR_PS            0                                          //从第一个开始写
 #define START_ADDR_RD_PS            0                                          //从第一个开始读
 #define BRAM_DATA_BYTE_32BITS       4                                          //BRAM 数据字节个数
 #define BRAM_DATA_BYTE_64BITS       8                                          //BRAM 数据字节个数



                                          /*PS写PL读-fword*/
 //将FWORD写入 BRAM_FWORD
 //data_len为几个32位数
 int ps_wr_bram_fword(int data_len, int data)
 {
   int wr_cnt=0;
   int i = 0;
   int Read_Data;
   //每次循环向 BRAM 中写入 1 个字符
   for(i = BRAM_DATA_BYTE_32BITS*START_ADDR_WR_PS ; i < BRAM_DATA_BYTE_32BITS*(START_ADDR_WR_PS + data_len) ;i += BRAM_DATA_BYTE_32BITS)
   {
     XBram_WriteReg(START_ADDR_AXI_BRAM_CTRL_FWORD,i,data) ;
     wr_cnt++;

   }
   //printf("完成PS写入BRAM\t\n");
   		for(i = 0 ; i < BRAM_DATA_BYTE_32BITS*(START_ADDR_WR_PS + data_len); i += 4)
   			{
   				Read_Data = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_FWORD , i) ;
   				usleep(1000);
   				//printf("fword is %d\t Read data is %d\t\n",  i/4 ,Read_Data) ;
   			}
   //设置 BRAM 写入的字符串长度，slv_reg2
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_FWORD, PL_BRAM_LEN_RD , BRAM_DATA_BYTE_32BITS*(data_len-1)) ;//4
   //设置 BRAM 的起始地址，slv_reg1
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_FWORD, PL_BRAM_START_ADDR_RD, START_ADDR_WR_PS) ;//0
   //拉高 BRAM 开始读信号，slv_reg0[0]
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_FWORD, PL_BRAM_START_RD , 1) ;
   //拉低 BRAM 开始读信号
   //PL_BRAM_RD_mWriteReg(START_ADDR_FWORD, PL_BRAM_START_RD , 0) ;
   return wr_cnt;
 }

                                      /*PS写PL读-pword*/
 //将PWORD写入 BRAM_PWORD
 //data_len为几个32位数
 int ps_wr_bram_pword(int data_len, int data)
 {
	 int wr_cnt=0;
	 int i = 0;
    //每次循环向 BRAM 中写入 1 个字符
   for(i = BRAM_DATA_BYTE_32BITS*START_ADDR_WR_PS ; i < BRAM_DATA_BYTE_32BITS*(START_ADDR_WR_PS + data_len) ;i += BRAM_DATA_BYTE_32BITS)
   {
      XBram_WriteReg(START_ADDR_AXI_BRAM_CTRL_PWORD,i,data) ;
      wr_cnt++;

   }

   //设置 BRAM 写入的字符串长度，slv_reg2
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_PWORD, PL_BRAM_LEN_RD , BRAM_DATA_BYTE_32BITS*(data_len-1)) ;//4
      //设置 BRAM 的起始地址，slv_reg1
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_PWORD, PL_BRAM_START_ADDR_RD, START_ADDR_WR_PS) ;//0
      //拉高 BRAM 开始读信号，slv_reg0[0]
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_PWORD, PL_BRAM_START_RD , 1) ;
      //拉低 BRAM 开始读信号
      //PL_BRAM_RD_mWriteReg(START_ADDR_PWORD, PL_BRAM_START_RD , 0) ;
      return wr_cnt;
 }


                                               /*PS写PL读-c0*/
 //将PWORD写入 BRAM_c0
 //data_len为几个32位数
 int ps_wr_bram_c0(int data_len, int data)
 {
	 int wr_cnt=0;
	 int i = 0;
   //每次循环向 BRAM 中写入 1 个字符
   for(i = BRAM_DATA_BYTE_32BITS*START_ADDR_WR_PS ; i < BRAM_DATA_BYTE_32BITS*(START_ADDR_WR_PS + data_len) ;i += BRAM_DATA_BYTE_32BITS)
   {
     XBram_WriteReg(START_ADDR_AXI_BRAM_CTRL_C0,i,data) ;
     wr_cnt++;

   }

   //设置 BRAM 写入的字符串长度，slv_reg2
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_C0, PL_BRAM_LEN_RD , BRAM_DATA_BYTE_32BITS*(data_len-1)) ;//4
   //设置 BRAM 的起始地址，slv_reg1
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_C0, PL_BRAM_START_ADDR_RD, START_ADDR_WR_PS) ;//0
   //拉高 BRAM 开始读信号，slv_reg0[0]
   RD_WHEEL_SEARCH_mWriteReg(START_ADDR_C0, PL_BRAM_START_RD , 1) ;
   //拉低 BRAM 开始读信号
   //PL_BRAM_RD_mWriteReg(START_ADDR_PWORD, PL_BRAM_START_RD , 0) ;
   return wr_cnt;
 }









                                                /*PS读PL写-phase_sub*/
 /*******************************************************************************
  *
   *  读取pl传上来的数据
   * 输入参数: data_len：pl写多少个数据发一个中断       index：取第几个点
   * 返回参数: 无
   * 历史修改记录:
  * <作者><日期><描述>
 ******************************************************************************/
 //从 BRAM_PHASE_SUB 中读出数据
  void ps_rd_bram_phase_sub(int data_len,int index)
 {




   //start_addr
   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_PHASE_SUB,PL_BRAM_WR_LITE_S00_AXI_SLV_REG1_OFFSET,0);
   //wr_len
   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_PHASE_SUB,PL_BRAM_WR_LITE_S00_AXI_SLV_REG2_OFFSET,BRAM_DATA_BYTE_32BITS*(data_len-1));
 //从 BRAM 中读出数据

	   read_data_phase_sub = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_PHASE_SUB,4*index) ;

   // printf("BRAM_PHASE_SUB address is %d\t,Read data is %d\n",i/BRAM_DATA_BYTE_32BITS ,read_data[j]) ;

 }

                                                 /*PS读PL写-modulus_div*/
 //从 BRAM_MODULUS_DIV 中读出数据
  void ps_rd_bram_modulus_div(int data_len,int index)
  {



	 //start_addr
	    PL_BRAM_WR_LITE_mWriteReg(START_ADDR_MODULUS_DIV,PL_BRAM_WR_LITE_S00_AXI_SLV_REG1_OFFSET,0);
	    //wr_len
	    PL_BRAM_WR_LITE_mWriteReg(START_ADDR_MODULUS_DIV,PL_BRAM_WR_LITE_S00_AXI_SLV_REG2_OFFSET,BRAM_DATA_BYTE_32BITS*(data_len-1));
  //从 BRAM 中读出数据

    	read_data_modulus_div = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_MODULUS_DIVISION,4*index) ;//4x100,第100个数

    // printf("BRAM_MODULUS_DIV address is %d\t,Read data is %d\n",i/BRAM_DATA_BYTE_64BITS ,read_data[i]) ;


  }

                                    /*PS读PL写-rms_i*/
//从 BRAM_RMS_I 中读出数据
void ps_rd_bram_rms_i(int data_len,int index)
{



	//start_addr
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_I,PL_BRAM_WR_LITE_S00_AXI_SLV_REG1_OFFSET,0);
	   //wr_len
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_I,PL_BRAM_WR_LITE_S00_AXI_SLV_REG2_OFFSET,BRAM_DATA_BYTE_32BITS*(data_len-1));
  //从 BRAM 中读出数据

	  read_data_rms_i = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_RMS_I,4*index) ;
	//  usleep(100);
   // printf("BRAM_RMS_I address is %d\t,Read data is %d\n",i/BRAM_DATA_BYTE_32BITS ,read_data[i]) ;


}

                                      /*PS读PL写-rms_u*/
//从 BRAM_RMS_U 中读出数据
void ps_rd_bram_rms_u(int data_len,int index)
{



	//start_addr
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_U,PL_BRAM_WR_LITE_S00_AXI_SLV_REG1_OFFSET,0);
	   //wr_len
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_U,PL_BRAM_WR_LITE_S00_AXI_SLV_REG2_OFFSET,BRAM_DATA_BYTE_32BITS*(data_len-1));
  //从 BRAM 中读出数据

	  read_data_rms_u = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_RMS_U,4*index) ;//第100个数

  //  printf("BRAM_RMS_U address is %d\t,Read data is %d\n",i/BRAM_DATA_BYTE_32BITS ,read_data[i]) ;



}

                                       /*PS读PL写-rms_dynamic*/
//从 BRAM_RMS_DYNAMIC 中读出数据
void ps_rd_bram_rms_dynamic(int data_len,int index)
{



	//start_addr
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_DYNAMIC,PL_BRAM_WR_LITE_S00_AXI_SLV_REG1_OFFSET,0);
	   //wr_len
	   PL_BRAM_WR_LITE_mWriteReg(START_ADDR_RMS_DYNAMIC,PL_BRAM_WR_LITE_S00_AXI_SLV_REG2_OFFSET,BRAM_DATA_BYTE_32BITS*(data_len-1));
  //从 BRAM 中读出数据

	  read_data_rms_dynamic = XBram_ReadReg(START_ADDR_AXI_BRAM_CTRL_RMS_DYNAMIC,4*index) ;

   // printf("BRAM_RMS_DYNAMIC address is %d\t,Read data is %d\n",i/BRAM_DATA_BYTE_32BITS ,read_data[i]) ;


}





