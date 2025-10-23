#include <stdint.h>
#include "pico/stdlib.h"
#include "board_config.h"
#include "hardware/i2c.h"
#include "voltage_sens.h"

void ina226_write_register(uint8_t reg, uint16_t value)
{
    // INA226 expects the following in an I2C write data frame: 
    // | ADDR|R/!W | ACK | DATA_MSB | ACK | DATA_LSB | ACK | 
    uint8_t data[3]; 
    data[0] = reg; 
    data[1] = (value >> 8); 
    data[2] = (value & 0xFF); 

    i2c_write_blocking(VS_I2C_PORT, VS_SLAVE_ADDR, data, 3, false); 
}

uint16_t ina226_read_register(uint8_t reg)
{
    uint8_t data[2]; 
    // INA226 has the following read word format
    // |ADDR| R/!W | ACK | DATA_MSB | ACK | DATA_LSB | ACK | 

    // write register address 
    i2c_write_blocking(VS_I2C_PORT, VS_SLAVE_ADDR, &reg, 1, true); 

    // read from the register over the SDA line 
    i2c_read_blocking(VS_I2C_PORT, VS_SLAVE_ADDR, data, 2, false);

    // MSB sent first 
    return ((data[0] << 8) | data[1]); 
} 

// initialize the voltage sensing IC over I2C
void voltage_sens_init()
{
    // set the configuration register for 1 point averaging, 
    // 1.1ms updates, and continuous operating mode for the bus voltage 
    uint16_t configuration_register_config = (INA226_AVERAGING_MODE_1 << INA226_CONFIG_AVG0_BP) |\
                                             (INA226_VBUSCT_1_1ms << INA226_CONFIG_VBUSTCT0_BP) |\
                                             (INA226_VSHCT_1_1ms << INA226_CONFIG_VSHCT0_BP) |\
                                             (INA226_VBUS_CONT << INA226_CONFIG_MODE0_BP);
    ina226_write_register(INA226_CONFIG, configuration_register_config); 
    
    // write to mask enable register to set alert pin for bus voltage under-voltage
    uint16_t mask_enable_config = (1 << INA226_MASK_EN_BOL_BP) |\
                                  (0 << INA226_MASK_EN_POL_BP); 
    ina226_write_register(INA226_MASK_EN, mask_enable_config); 
    // leave alert latch enable as transparent so that flag is not cleared until the fault is resolved. 
    
    // set alert limit register for bus voltage over-limit (BOL)
    uint16_t vbus_undervoltage_limit = (uint16_t)(VS_VBUS_UNDER_VOLTAGE_LIMIT / INA226_VBUS_LSB);
    ina226_write_register(INA226_ALERT_LIMIT, vbus_undervoltage_limit);
    
}

float read_bus_voltage()
{
    uint16_t raw_vbus_voltage = ina226_read_register(INA226_BUS_VOLTAGE); 
    return raw_vbus_voltage * INA226_VBUS_LSB; 
}

bool poll_alert_limit()
{
    // get GPIO state 
    // active low, so if low, we have reached the alert 
    if(gpio_get(VS_ALERT)) // returns state (high == true)
    {
        return false; 
    }
    return true; 
}