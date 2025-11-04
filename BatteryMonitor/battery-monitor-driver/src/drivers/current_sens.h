#ifndef CURRENT_SENS_H_
#define CURRENT_SENS_H_

#include "board_config.h"
#include "hardware/i2c.h"

#define NUM_SAMPLES 15

// Reads ADC value available at the DATA register and converts 
// it to the value to be output by the 7SEG displays 
float get_current_sens_reading(); 

// structure to store 15-point running average information 
struct runningAverageFilter 
{
        float buffer[NUM_SAMPLES];
        int val_count; 
        int index; 
        float sum;  
}; 

// initializes running average structure
void running_average_init(struct runningAverageFilter *filt);

// add new data point to running average filter and returns the average of the RA buffer
float running_average_update(struct runningAverageFilter *filt, float new_sample);

#endif