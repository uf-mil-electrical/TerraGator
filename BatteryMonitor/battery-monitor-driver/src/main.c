#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "board_config.h"

// Questions for Russell
// (1) you have the VREF on the current sensing IC configured s.t. 
// the linear measurement range is +- 11.5A (bidirectional). How do we want to indicate 
// negative currents on the 7SEGs? 
// >  program assuming A1 chip configured for unidirectional measurement 1A - 96A
// > 
// (2) The issue mentioned acceptable and unacceptable ranges. What 
// explicitly are these ranges? 
// > V: 22.4-24.6 V (GREEN); YELLOW slightly outside; RED widely outside  
// > I: determine on own 

int main()
{
    board_config(); 

    // test 1: just LEDs to ensure proper display 


    // get ADC data 
    

    // get voltage data 

    // check if we're outside acceptable range(s) 

    // change LED state based on voltage and current measurements 

    // output voltage and current values to LED driver over SPI every ~100 ms

    // communicate with RPi5 over UART regarding non-ideal ranges 

    // repeat
    
}

