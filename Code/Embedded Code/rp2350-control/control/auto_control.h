#ifndef AUTO_CONTROL_H
#define AUTO_CONTROL_H

/******************<Summary>*****************
 * Name: auto_control.h
 * Purpose:
 *      > Primary rover control
 *      > Operates rover in response to commands received
 *          over I2C
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "communication/rover_i2c.h"

#include "peripherals/motor_speed_control.h"
#include "peripherals/safety_devices.h"

#include "pins.h"
/******************</Dependencies>*****************/


/******************<Functions>*****************/
uint8_t read_digit_serial(void);
void runMenu(void);
/******************</Functions>*****************/



#endif // AUTO_CONTROL_H