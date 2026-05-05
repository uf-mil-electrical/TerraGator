#ifndef PINS_H
#define PINS_H

/******************<Summary>*****************
 * Name: pins.h
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

/******************</Dependencies>*****************/



/******************<General macros>*****************/
#define GPIO_SET_LOW            0           // for use with gpio_put
#define GPIO_SET_HIGH           1           // for use with gpio_put
/******************</General macros>*****************/



/******************<Motors>*****************/
// Motor PWM pins
#define MOTOR1_PWM              9
#define MOTOR2_PWM              8
#define MOTOR3_PWM              7
#define MOTOR4_PWM              6
#define MOTOR5_PWM              5
#define MOTOR6_PWM              4

// Motor direction pins
#define LEFT_MOTOR_CONTROL_1    3
#define LEFT_MOTOR_CONTROL_2    2
#define RIGHT_MOTOR_CONTROL_1   1
#define RIGHT_MOTOR_CONTROL_2   0

// Motor PWM Setup
#define NUM_MOTORS              6           // Number of (controllable) motors
#define PWM_WRAP                6000        // PWM wrap value (PWM period = clock_sys / wrap value)
/******************</Motors>*****************/


/******************<I2C0 Communication>*****************/
#define I2C0_BAUDRATE           100*1000    // initialize I2C at 1 kHz
#define RP2350_I2C_ADDRESS      0x40        // I2C address for this board
#define ESP32_I2C_ADDRESS       0x20        // I2C address for ESP32

// Pins
#define I2C0_SDA                37
#define I2C0_SCL                36
/******************</I2C0 Communication>*****************/


/******************<Safety Devices>*****************/
// Relay pins
#define RELAY_CONTROL           39
#define RELAY_FB                38

// LEDs
#define LED_1_PIN               21
#define LED_2_PIN               22
#define LED_3_PIN               23
#define LED_4_PIN               24
#define LED_5_PIN               25
/******************</Safety Devices>*****************/


#endif // PINS_H