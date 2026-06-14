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
#define I2C_TIMEOUT_DELAY               1000    // for I2C read: wait 1000us, then proceed if no data is captured

#define ROVER_I2C_MAX_RX_BUF_SIZE       64      // max bytes possibly received for one I2C transaction
/******************</Defines>*****************/


/******************<Functions>*****************/
void init_rover_i2c(i2c_inst_t* i2c_bus, uint8_t sda_pin, uint8_t scl_pin, uint baudrate, bool master_mode);
void rover_i2c_read(i2c_inst_t* i2c_bus, uint8_t read_address, uint8_t* data, uint8_t num_bytes);
void rover_i2c_get_num_bytes_rx(i2c_inst_t* i2c_bus, uint8_t* num_bytes);
void rover_i2c_write(i2c_inst_t* i2c_bus, uint8_t write_address, uint8_t* data, uint8_t num_bytes);
bool rover_i2c_check_for_rx(i2c_inst_t* i2c_bus, uint8_t* out_buf, uint16_t* out_len);
/******************</Functions>*****************/



#endif // ROVER_I2C_H