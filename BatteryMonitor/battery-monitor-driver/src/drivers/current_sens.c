#include "current_sens.h"
#include "board_config.h"
#include "hardware/adc.h"

float get_current_sens_reading()
{
    // => input current = (V_OUT (analog) - V_REF) / sens
    uint16_t adc_out = adc_read(); 
    float output_voltage = (float) adc_out * ADC_VREF / ((1 << ADC_RESOLUTION) - 1); 
    float current = ((float)(adc_out - CS_VREF)) / CS_sens; 
    return current; 
}