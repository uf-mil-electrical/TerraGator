#include "board_config.h"
#include "hardware/spi.h"
#include <math.h> 
#include "sseg.h"

void max7219_write(uint8_t reg, uint8_t data)
{
    // LED driver has the following SPI protocol
    // | D15 - D12 | D11 - D8 |  D7  -  D0   |
    // |  X X X X  |  ADDRESS | MSB DATA LSB | 

    uint16_t reg_config = (reg << REG_ADDR_MAP_BP) | (data); 
    // assert CS
    gpio_put(CS_7SEG, 0); 

    spi_write16_blocking(SPI_PORT, &reg_config, 1); 
    // deassert CS
    gpio_put(CS_7SEG, 1);
}

// initializes the SSEG for proper function 
void sseg_init()
{
    // configure for normal operation
    max7219_write(MAX7219_SHUTDOWN, 1 << SHUTDOWN_NORMAL_OPERATION_BP);  

    // set decode mode for digits 0-3
    max7219_write(MAX7219_DECODE_MODE, DECODE_MODE_0_3_DECODE_MODE_GC); 

    // set for medium LED intensity 
    max7219_write(MAX7219_INTENSITY, 0x08); 

    // configure test mode to be off 
    max7219_write(MAX7219_DISPLAY_TEST, 0x00); 

    // set first 4 display digits in scan-limit register
    max7219_write(MAX7219_SCAN_LIMIT, SCAN_LIMIT_DISPLAY_DIG_0_3_GC);   
}

// writes the voltage and current values to the LEDs 
void max7219_voltage_current_write(float voltage, float current)
{
    // for values less than 10, use a 1 decimal precision display (i.e., display ones and tenths place)
    // for values greater than 10, use a 0 decimal precision display (i.e., display tens and ones place)
   
    uint8_t dig0_value; 
    uint8_t dig1_value; 
    uint8_t dig2_value; 
    uint8_t dig3_value; 

    // voltage conversion 

    if (voltage < 10)
    {   
        dig0_value = (uint8_t)(voltage); // integer portion
        dig1_value = (uint8_t)roundf((voltage - (float)(dig0_value)) * 10); //fractional portion
        if(dig1_value >= 10) // round up 
        {
            dig0_value += 1;
            dig1_value = 0; 
        }  
        dig0_value |= (1 << DP_BP); // add decimal point    
    }   
    else 
    {
        float rounded_voltage = roundf(voltage); 
        dig0_value = (uint8_t)(rounded_voltage/10); // tens place 
        dig1_value = (uint8_t)fmodf(rounded_voltage, 10); // ones place
    }

    // current conversion 
    if (current < 10)
    {
        dig2_value = (uint8_t)(current); // ones place 
        dig3_value = (uint8_t)roundf((current - (float)(dig2_value)) * 10); //tenths place 
        if(dig3_value >= 10) //round up 
        {
            dig2_value += 1;
            dig3_value = 0; 
        }  
        dig2_value |= (1 << DP_BP); // add decimal point 
    }
    else
    {
        float rounded_current = roundf(current); 
        dig2_value = (uint8_t)(rounded_current/10); // tens place 
        dig3_value = (uint8_t)fmodf(rounded_current, 10); // ones place
    }

    // write digits to each 7seg display 
    max7219_write(MAX7219_DIG0, dig0_value); 
    max7219_write(MAX7219_DIG1, dig1_value);
    max7219_write(MAX7219_DIG2, dig2_value);
    max7219_write(MAX7219_DIG3, dig3_value);
    
}