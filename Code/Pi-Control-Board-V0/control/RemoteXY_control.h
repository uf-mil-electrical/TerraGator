#ifndef REMOTEXY_CONTROL_H
#define REMOTEXY_CONTROL_H

/******************<Summary>*****************
 * Name: RemoteXY_control.h
 * Purpose:
 *      > Defines functions for use when rover should be controlled
 *          by RemoteXY app
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "system_general.h"
#include "peripherals/rover_i2c.h"
#include "peripherals/ESP32.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/safety_devices.h"
/******************</Dependencies>*****************/

/******************<Defines>*****************/
/******************</Defines>*****************/


/******************<Functions>*****************/
void runRover_RemoteXYControl();
/******************</Functions>*****************/


#endif // REMOTEXY_CONTROL_H