#ifndef SYSTEM_GENERAL_H
#define SYSTEM_GENERAL_H

/******************<Summary>*****************
 * Name: system_general.h
 * Purpose:
 *      > Defines values and functions for use throughout other parts of the rover code
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
/******************</Dependencies>*****************/


/******************<Values for Code>*****************/
#define NUM_MOTORS              6       // Number of (controllable) motors
#define PWM_WRAP                6000    // PWM wrap value (PWM period = clock_sys / wrap value)
/******************</Values for Code>*****************/



/******************<GPIO Definitions>*****************/
// Motor PWM pins
#define MOTOR1_PWM              0
#define MOTOR2_PWM              1
#define MOTOR3_PWM              2
#define MOTOR4_PWM              3
#define MOTOR5_PWM              4
#define MOTOR6_PWM              5

// Motor direction pins
#define LEFT_MOTOR_CONTROL_1    6
#define LEFT_MOTOR_CONTROL_2    7
#define RIGHT_MOTOR_CONTROL_1   8
#define RIGHT_MOTOR_CONTROL_2   9
/******************</GPIO Definitions>*****************/



/******************<Functions>*****************/
uint8_t read_digit_serial(void);
void runMenu(void);
/******************</Functions>*****************/


#endif // SYSTEM_GENERAL_H