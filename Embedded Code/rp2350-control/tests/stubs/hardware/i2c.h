#ifndef TEST_I2C_H
#define TEST_I2C_H
#include "pico/stdlib.h"
typedef struct { int unused; } i2c_inst_t;
extern i2c_inst_t test_i2c1;
#define i2c1 (&test_i2c1)
void i2c_init(i2c_inst_t *bus, uint baud);
void i2c_set_slave_mode(i2c_inst_t *bus, bool slave, uint8_t address);
int i2c_read_timeout_us(i2c_inst_t *bus, uint8_t address, uint8_t *data, uint length, bool nostop, uint timeout);
int i2c_write_timeout_us(i2c_inst_t *bus, uint8_t address, const uint8_t *data, uint length, bool nostop, uint timeout);
#endif
