#include "../include/led.h"
#include "xparameters.h"
#include "xgpiops.h"
#include "../include/uart0.h"
XGpioPs Gpio_Led;
#define DDS_PWM_EN   63

#define LED0_PIN     57
#define LED1_PIN     58
#define LED2_PIN     59
#define LED3_PIN     60 //54 1 55 2 56 3 57 4 58 5  59 6 60 7
extern void gettempvalue();
unsigned int LED_Run_Time;
void LEDRun()
{
	unsigned char buf[10] = {0x01,0x02,0x03,0x04,0x05};
	LED_Run_Time++; //10k
	if (LED_Run_Time > 2000) {
		LED_Run_Time = 0;
		Waterlamp(); //Á÷Ë®µÆ
		//SendMcuRegOneByte(0x01, 0x01);
		buf[0] = 1;
		//SendDgusRegMulByte(0x01,&buf[0],0x01);
		//test();
		//gettempvalue();
	}
}

void led_init(void) {
	XGpioPs_Config *GpioSpiConfigPtr;

	GpioSpiConfigPtr = XGpioPs_LookupConfig(XPAR_XGPIOPS_0_DEVICE_ID);

	XGpioPs_CfgInitialize(&Gpio_Led, GpioSpiConfigPtr,
			GpioSpiConfigPtr->BaseAddr);

	XGpioPs_SetDirectionPin(&Gpio_Led, LED0_PIN, 1);
	XGpioPs_SetDirectionPin(&Gpio_Led, LED1_PIN, 1);
	XGpioPs_SetDirectionPin(&Gpio_Led, LED2_PIN, 1);
	XGpioPs_SetDirectionPin(&Gpio_Led, LED3_PIN, 1);


	XGpioPs_SetDirectionPin(&Gpio_Led, DDS_PWM_EN, 1);


	XGpioPs_SetOutputEnablePin(&Gpio_Led, LED0_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio_Led, LED1_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio_Led, LED2_PIN, 1);
	XGpioPs_SetOutputEnablePin(&Gpio_Led, LED3_PIN, 1);

	XGpioPs_SetOutputEnablePin(&Gpio_Led, DDS_PWM_EN, 1);
	XGpioPs_WritePin(&Gpio_Led, DDS_PWM_EN, 1);
}

void led_turn_state(led_t led, led_sta_t sta) {
	switch (led) {
	case LED0:
		XGpioPs_WritePin(&Gpio_Led, LED0_PIN, sta);
		break;
	case LED1:
		XGpioPs_WritePin(&Gpio_Led, LED1_PIN, sta);
		break;
	case LED2:
		XGpioPs_WritePin(&Gpio_Led, LED2_PIN, sta);
		break;
	case LED3:
		XGpioPs_WritePin(&Gpio_Led, LED3_PIN, sta);
		break;
	default:
		break;
	}
}

void led_blingbling(led_t led) {
	static led_sta_t sta = OFF;

	sta = (sta == OFF) ? ON : OFF;

	switch (led) {
	case LED0:
		XGpioPs_WritePin(&Gpio_Led, LED0_PIN, sta);
		break;
	case LED1:
		XGpioPs_WritePin(&Gpio_Led, LED1_PIN, sta);
		break;
	case LED2:
		XGpioPs_WritePin(&Gpio_Led, LED2_PIN, sta);
		break;
	case LED3:
		XGpioPs_WritePin(&Gpio_Led, LED3_PIN, sta);
		break;

	default:
		break;
	}

}

void Waterlamp(void) {
	static int LEDNUM = 0;
	LEDNUM++;
	if (LEDNUM > 7)
		LEDNUM = 0;
	switch (LEDNUM) {
	case 0:
	//	led_turn_state(LED0, 0);
	//	XGpioPs_WritePin(&Gpio_Led, DDS_PWM_EN, 0);
		break;
	case 1:
		led_turn_state(LED1, 0);
		break;
	case 2:
		led_turn_state(LED2, 0);
		break;
	case 3:
		led_turn_state(LED3, 0);
		break;
	case 4:
		led_turn_state(LED3, 1);
		break;
	case 5:
		led_turn_state(LED2, 1);
		break;
	case 6:
		led_turn_state(LED1, 1);
		break;
	case 7:
	//	XGpioPs_WritePin(&Gpio_Led, DDS_PWM_EN, 1);
	//	led_turn_state(LED0, 1);
		break;

	}
}
