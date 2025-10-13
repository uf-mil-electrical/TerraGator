#ifndef CURRENT_SENS_H_
#define CURRENT_SENS_H_

#include "board_config.h"
#include "hardware/i2c.h"

// Reads ADC value available at the DATA register and converts 
// it to the value to be output by the 7SEG displays 
float get_current_sens_reading(); 

#endif