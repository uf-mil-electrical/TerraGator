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
#include "drivers/status.h"
#include "drivers/payload.h"

int main()
{
    // initialization procedures 
    board_config();  
    sseg_init(); 
    voltage_sens_init(); 
    // current sens is just ADC, which is configured in board_config();
    led_init();
    // initialize running average filter
    struct runningAverageFilter current_filter;
    struct runningAverageFilter voltage_filter; 
    running_average_init(&current_filter); 
    running_average_init(&voltage_filter); 

    // ADDITIONS BEGIN HERE
    
    // TO-DO : fill this in with our battery parameters!
    BatteryConfig config = {
        .cell_count = 0, // [Cells]
        .capacity_mAh = 0, // [mAh]
        .max_voltage = 0.0f, // [V]
        .min_voltage = 0.0f, // [V]
        .max_discharge_current = 0.0f, // [A]
        .max_charge_current = 0.0f, // [A]
        .expected_current = 0.0f, // [A]
    };
    set_battery_config(config);
    // Sending battery config once upon startup
    send_battery_config_payload();
    // ADDITIONS END HERE
    
    //printf("Beginning serial output..."); 

    // small delay before beginning to read to allow initializations time to settle 
    sleep_ms(10);
    while(1)
    {
        // get voltage and current data 
        float raw_voltage = read_bus_voltage(); 
        float raw_current = get_current_sens_reading(); 
        float filtered_voltage = running_average_update(&voltage_filter, raw_voltage);
        float filtered_current = running_average_update(&current_filter, raw_current); 
        
        /* serial outputs for debugging. Commented for future debugging use
        printf("Voltage reading: %f\n", raw_voltage); 
        printf("Current reading: %f\n", raw_current);
        printf("Filtered voltage: %f\n", filtered_voltage); 
        printf("Filtered current: %f\n", filtered_current);
        printf("--RA current structure attributes--\n"); 
        printf("val_count, %d\t", current_filter.val_count); 
        printf("index, %d\t", current_filter.index);
        printf("sum, %f\n", current_filter.sum); 
        printf("--RA voltage structure attributes--\n"); 
        printf("val_count, %d\t", voltage_filter.val_count); 
        printf("index, %d\t", voltage_filter.index);
        printf("sum, %f\n", voltage_filter.sum); 
        */
       
        // output voltage and current values to LED driver over SPI every ~100 ms
        sleep_ms(100); 
        max7219_voltage_current_write(filtered_voltage, filtered_current); 

        // update status LEDs onboard battery monitor
        uint8_t status = change_led_indicators(filtered_voltage, filtered_current);
        
        // sending every ~100 ms using according to MIL UART 
        send_payload(status, filtered_voltage, filtered_current);
    }
}

