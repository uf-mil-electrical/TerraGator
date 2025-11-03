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
#include "drivers/led.h"

int main()
{
    // initialization procedures 
    board_config();  
    sseg_init(); 
    voltage_sens_init(); 
    // current sens is just ADC, which is configured in board_config();
    led_init();

    // small delay before beginning to read to allow initializations time to settle 
    sleep_ms(10);
    while(1)
    {
        // get voltage and current data 
        float voltage = read_bus_voltage(); 
        float current = get_current_sens_reading(); 

        // output voltage and current values to LED driver over SPI every ~100 ms
        sleep_ms(1000); 
        max7219_voltage_current_write(voltage, current); 

        // update status LEDs onboard battery monitor
        uint8_t status = change_led_indicators(voltage, current);
        
        // send LED status to RPi5 
        uart_putc_raw(uart0, status);
    }
}

