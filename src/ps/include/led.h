#ifndef __LED_H
#define __LED_H

typedef enum{LED0=0,LED1,LED2,LED3}led_t;
typedef enum{ON=0,OFF}led_sta_t;

void led_init(void);
void led_turn_state(led_t,led_sta_t);
void led_blingbling(led_t led);
void Waterlamp(void);
void LEDRun();
#endif
