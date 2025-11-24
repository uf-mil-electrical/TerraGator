#ifndef SYSTEM_GENERAL_H
#define SYSTEM_GENERAL_H

/******************<Summary>*****************
 * Name: system_general.h
 * Purpose:
 *      > Defines values and functions for use throughout other parts of the rover code
 *      > Define values for peripheral and GPIO assignment HERE
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "system_general.h"
/******************</Dependencies>*****************/


/******************<Motor Setup>*****************/
#define NUM_MOTORS              6           // Number of (controllable) motors
#define PWM_WRAP                6000        // PWM wrap value (PWM period = clock_sys / wrap value)
/******************</Motor Setup>*****************/


/******************<I2C Setup>*****************/
#define I2C_BAUDRATE            400*1000    // initialize I2C at 400 kHz
#define I2C_PORT                i2c1        // I2C port to use for I2C communication (i2c0 or i2c1, depends on which GPIO pins are used)
#define RP2350_I2C_ADDRESS      0x40        // I2C address for this board
#define ESP32_I2C_ADDRESS       0x20        // I2C address for ESP32
/******************</I2C Setup>*****************/


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

// I2C pins
#define I2C_SDA                 10
#define I2C_SCL                 11
/******************</GPIO Definitions>*****************/



/******************<Functions>*****************/
uint8_t read_digit_serial(void);
void runMenu(void);
/******************</Functions>*****************/


#endif // SYSTEM_GENERAL_H