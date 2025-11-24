#ifndef ROVER_CLI_H
#define ROVER_CLI_H

/******************<Summary>*****************
 * Name: rover_cli.h
 * Purpose:
 *      > Functions for rover CLI and debug control
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "system_general.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/rover_i2c.h"
/******************</Dependencies>*****************/


/******************<Functions>*****************/
uint8_t read_digit_serial(void);
void runMenu(void);
/******************</Functions>*****************/



#endif // ROVER_CLI_H