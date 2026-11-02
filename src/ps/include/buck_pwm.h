#ifndef BUCK_PWM_H
#define BUCK_PWM_H

#define START_ADDR_AXI_PWM_0                       XPAR_AXI_PWM_0_S00_AXI_BASEADDR
#define END_ADDR_AXI_PWM_0                         XPAR_AXI_PWM_0_S00_AXI_HIGHADDR
#include "xil_printf.h"
#include "xparameters.h"
#include "xil_io.h"
typedef            void        CPU_VOID;
typedef            char        CPU_CHAR;                        /*  8-bit character                                     */
typedef  unsigned  char        CPU_BOOLEAN;                     /*  8-bit boolean or logical                            */
typedef  unsigned  char        CPU_INT08U;                      /*  8-bit unsigned integer                              */
typedef    signed  char        CPU_INT08S;                      /*  8-bit   signed integer                              */
typedef  unsigned  short       CPU_INT16U;                      /* 16-bit unsigned integer                              */
typedef    signed  short       CPU_INT16S;                      /* 16-bit   signed integer                              */
typedef  unsigned  int         CPU_INT32U;                      /* 32-bit unsigned integer                              */
typedef    signed  int         CPU_INT32S;                      /* 32-bit   signed integer                              */
typedef  unsigned  long  long  CPU_INT64U;                      /* 64-bit unsigned integer                              */
typedef    signed  long  long  CPU_INT64S;                      /* 64-bit   signed integer                              */

typedef            float       CPU_FP32;                        /* 32-bit floating point                                */
typedef            double      CPU_FP64;                        /* 64-bit floating point                                */


typedef  volatile  CPU_INT08U  CPU_REG08;                       /*  8-bit register                                      */
typedef  volatile  CPU_INT16U  CPU_REG16;                       /* 16-bit register                                      */
typedef  volatile  CPU_INT32U  CPU_REG32;                       /* 32-bit register                                      */
typedef  volatile  CPU_INT64U  CPU_REG64;                       /* 64-bit register                                      */

typedef enum
{
    FORCE_LOW = 0,
    FORCE_HIGH,
    PWM,
} PWM_MODE;




#define Brige_BUCK_BASSADDR       XPAR_AXI_IP_CON_0_S00_AXI_BASEADDR
#define Brige_DUTY_OFFSET         AXI_IP_CON_S00_AXI_SLV_REG1_OFFSET
//º¯ÊýÉùÃ÷
void buck_pwm_set_duty(float duty);
void buck_pwm_set_fre (unsigned int fre);



#endif
