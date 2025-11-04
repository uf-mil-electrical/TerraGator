#include "current_sens.h"
#include "board_config.h"
#include "hardware/adc.h"

// The TMCS1100A1 has a sensitivity of 50mV / A
const float CS_sens = 0.05;

#define NUM_SAMPLES 15

float get_current_sens_reading()
{       
    // => input current = (V_OUT (analog) - V_REF) / sens
    uint16_t adc_out = adc_read(); 
    float output_voltage = (float) adc_out * ADC_VREF / ((1 << ADC_RESOLUTION) - 1); 
    float current = ((float)(output_voltage - CS_VREF)) / CS_sens; 
    return current; 
}

void running_average_init(struct runningAverageFilter *filt)
{
    filt->val_count = 0; 
    filt->index = 0; 
    filt->sum = 0.0f; 
    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        filt->buffer[i] = 0.0f;
    }
}

float running_average_update(struct runningAverageFilter *filt, float new_sample)
{
    filt->sum = filt->buffer[filt->index];          // remove old sample at the index
    filt->buffer[filt->index] = new_sample;        // store new sample 
    filt->sum += new_sample;                      // increment sum
    filt->index = (filt->index + 1) % NUM_SAMPLES; // circularly update index

    // for the first N samples, use a counter to average over N values instead of whole buffer size
    if (filt->val_count < NUM_SAMPLES)
    {
            filt->val_count++;
    }

    return filt->sum/filt->val_count;
}