#ifndef MOTOR_SPEED_CONTROL_H
#define MOTOR_SPEED_CONTROL_H

/******************<Summary>*****************
 * Name: motor_speed_control.h
 * Purpose:
 *      > creates functions that can adjust motor speed (PWM period)
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/



/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "system_general.h"
#include "hardware/pwm.h"
/******************</Dependencies>*****************/



/******************<Defines>*****************/
/******************</Defines>*****************/



/******************<Global variables>*****************/
// Struct to store information on each motor
typedef struct motor_str {
    uint8_t motor_ID;           // motor number (e.g., 1,2,etc. )
    uint8_t side;               // side this motor is on (left=0, right=1)
    uint8_t GPIO_pin;           // GPIO pin for this motor's PWM control
    uint8_t PWM_slice;          // PWM slice for this motor
    uint8_t PWM_channel;        // PWM channel for this motor (0=A, 1=B)
} motor_str;
/******************</Global variables>*****************/



/******************<Functions>*****************/
void motor_init();
void printMotorDetails();
void setMotorMode(char motor_set, char mode);
char getMotorMode();
void setMotorSpeed(uint8_t ID, uint8_t speed);
void setMotorSpeed_all(uint8_t speed);
void setMotorSpeed_side(char side, uint8_t speed);
/******************</Functions>*****************/

#endif // MOTOR_SPEED_CONTROL_H