#ifndef LED_H_
#define LED_H_

#include "board_config.h"
#include "hardware/gpio.h"

// -------------- VOLTAGE / CURRENT THRESHOLDS ---------- //

// ----------------- FUNCTION PROTOTYPES --------------- //

// sets direction and initial state of LEDs 
void led_init(); 

// changes the LED state based on voltage and current values provided
void change_led_indicator (float voltage, float current); 

#endif