#include "board_config.h"
#include "hardware/i2c.h"

// initialize the voltage sensing IC over I2C
void voltage_sens_init()
{
    // set the configuration register to that only bus voltage is converted 
    
    // choose continuous or triggered (single-shot) operating mode
    // for single-shot, must write to config register each time we want a reading

    // do we want a power-down mode for the VS? 

    // when reading, could use conversion ready flag bit in the mask/enable register
    // interrupt automatically clears after reading the mask/enable register

    // set alert limit register for bus voltage over-limit (BOL)
    // BOL is set if mask/enable register
    
}