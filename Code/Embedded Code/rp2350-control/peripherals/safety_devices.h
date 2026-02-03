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

#include "system_general.h"

#include "hardware/gpio.h"
/******************</Dependencies>*****************/


/******************<Defines>*****************/
/******************</Defines>*****************/


/******************<Functions>*****************/
void initRelay();
bool getRelayState();
void enableRelay();
void killRelay();
/******************</Functions>*****************/



#endif // SAFETY_DEVICES_H