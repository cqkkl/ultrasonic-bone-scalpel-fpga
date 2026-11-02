/*
 * Alarm.h
 *
 *  Created on: 2023Äê11ÔÂ6ÈÕ
 *      Author: yanghang
 */
#ifndef __ALARM_H
#define __ALARM_H
#include "buck_pwm.h"
#include "data_dec.h"
#include "freq_track.h"
#include "uart0.h"


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


#define NO_CURRENT_DATA   50
#define LOW_VOLT_DATA  20
#define HIGH_TUNERES_DATA   2000
#define HIGH_TUNEVOLT_DATA   1500


CPU_BOOLEAN UTDriver_Alarm(UT_PARAM* v_UtParam);
void initAlarmValue(void);
CPU_BOOLEAN UTDrive_Scan_Alarm();
void test(void);
void UTDrive_Scan_State(CPU_INT08U state);
void UTDrive_ScanFault_Clear(void);
#endif

