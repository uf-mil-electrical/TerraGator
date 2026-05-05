#ifndef ROVER_I2C_H
#define ROVER_I2C_H

/******************<Summary>*****************
 * Name: rover_i2c.h
 * Purpose:
 *      > Functions for I2C communication
 * Written / updated by:
 *      > Russell
******************</Summary>*****************/


/******************<Dependencies>*****************/
#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"

#include "hardware/i2c.h"

#include "pins.h"
/******************</Dependencies>*****************/


/******************<Defines>*****************/
#define I2C_TIMEOUT_DELAY       1000     // for I2C read: wait 1000us, then proceed if no data is captured
/******************</Defines>*****************/


/******************<Functions>*****************/
void init_rover_i2c(void);
void i2c_read_esp32(uint8_t* data, uint8_t num_bytes);
void i2c_write_esp32(uint8_t* data, uint8_t num_bytes);
/******************</Functions>*****************/



#endif // ROVER_I2C_H