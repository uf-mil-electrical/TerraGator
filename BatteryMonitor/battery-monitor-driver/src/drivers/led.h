#ifndef LED_H_
#define LED_H_

#include "board_config.h"
#include "hardware/gpio.h"
#define NUM_LED 3

// -------------- VOLTAGE / CURRENT STATUS CODES ---------- //

#define BMS_STATUS_OK 0x00
#define BMS_STATUS_VOLTAGE_OVERCHARGED 0x01
#define BMS_STATUS_VOLTAGE_NEAR_AIR_LANDING 0x02
#define BMS_STATUS_VOLTAGE_NEAR_DEEP_DISCHARGE 0x04
#define BMS_STATUS_CURRENT_WARNING 0x08
#define BMS_STATUS_CURRENT_CRITICAL 0x10

// ----------------- FUNCTION PROTOTYPES --------------- //

// sets direction, initial state of LEDs, and timer callback 
void led_init(); 

// changes the LED state based on voltage and current values provided
uint8_t change_led_indicators (float voltage, float current); 

#endif