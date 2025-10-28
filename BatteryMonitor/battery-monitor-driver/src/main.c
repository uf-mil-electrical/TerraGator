#include <stdio.h>
#include <unistd.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "board_config.h"
#include "drivers/current_sens.h"
#include "drivers/voltage_sens.h"
#include "drivers/sseg.h"
#include "drivers/rpi5_comm.h"
#include "drivers/led.h"


// (2) The issue mentioned acceptable and unacceptable ranges. What 
// explicitly are these ranges? 

int main()
{
    while(1)
    {
        // initialization procedures 
        board_config();  
        sseg_init(); 
        voltage_sens_init(); 
        // current sens is just ADC, which is configured in board_config();
        led_init();
        //uart_init(); 
        
        // get voltage and durrent data 
        float voltage = read_bus_voltage(); 
        float current = get_current_sens_reading(); 

        // output voltage and current values to LED driver over SPI every ~100 ms
        sleep_ms(1000); 
        max7219_voltage_current_write(voltage, current); 

        // communicate with RPi5 over UART regarding non-ideal ranges 

        // repeat
    }
    
    
}

