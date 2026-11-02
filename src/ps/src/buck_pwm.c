#include "../include/buck_pwm.h"
#include <stdio.h>
#include "axi_pwm.h"
#include "xil_io.h"
#include "xparameters.h"
#include "sleep.h"

void buck_pwm_set_duty( float duty_cycle)
{
   uint32_t duty;
   uint64_t N = 4294967296;//1U << 32;//2^32
   duty = N * (1 - (duty_cycle)) - 1; //duty cycle为占空比，duty为计数器目标值

   AXI_PWM_mWriteReg(XPAR_AXI_PWM_0_S00_AXI_BASEADDR,AXI_PWM_S00_AXI_SLV_REG1_OFFSET, duty);

}


void buck_pwm_set_fre(unsigned int fre)
{
   uint64_t N = 4294967296;//2^32
   uint32_t fc = 57000000;//采样率50mhz
   uint32_t period;
   period = fre * N / fc;//period为计数器步进值
   AXI_PWM_mWriteReg(XPAR_AXI_PWM_0_S00_AXI_BASEADDR,AXI_PWM_S00_AXI_SLV_REG0_OFFSET, period);//40khz

}

