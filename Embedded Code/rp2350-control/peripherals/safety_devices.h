#ifndef SAFETY_DEVICES_H
#define SAFETY_DEVICES_H

/******************<Summary>*****************
 * Name: safety_devices.h
 * Purpose:
 *      > Functions for safety-related devices for the rover
 *      > Primarily, relay and buzzer control
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "pins.h"

#include "hardware/gpio.h"
/******************</Dependencies>*****************/


/******************<Defines>*****************/
/******************</Defines>*****************/


/******************<Functions>*****************/
void init_relay();
bool get_relay_state();
void enable_relay();
void kill_relay();

void init_leds();
void set_led(uint8_t led, bool state);
void set_all_leds(bool state);
/******************</Functions>*****************/



#endif // SAFETY_DEVICES_H