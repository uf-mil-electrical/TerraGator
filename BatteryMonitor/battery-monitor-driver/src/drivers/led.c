#include "led.h"
#include "board_config.h"
#include "hardware/gpio.h"


void led_init()
{
    // set GPIOs for LEDs as outputs and assert low 
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    gpio_put(RED_LED, 0); 
    gpio_put(YELLOW_LED, 0); 
    gpio_put(GREEN_LED, 0); 
}

void change_led_indicators (float voltage, float current)
{
    // change LED state based on voltage and current measurements 
    // FLASH voltage indicators and use SOLID current indicators (voltage has priority over current)
    // check if we're outside acceptable range(s) 
    // } VOLTAGE RANGES
    // With LiPo, air landing voltage is 3.7V/cell, max is 4.1V/cell
    // |    > 24.6     |  flash RED   | overcharged 
    // | [22.8, 24.6V] | flash GREEN  | ideal operating region
    // | [21.0, 22.8)  | flash YELLOW | approaching air landing voltage or lower, disconnect soon 
    // |    < 21.0     |  flash RED   | approaching deep discharge, disconnect now 
    // } CURRENT RANGES
    // 

    bool VGREEN; 
    bool VYELLOW; 
    bool VRED; 
    if (voltage <= 24.6 && voltage >= 22.2)    
    {
        bool VGREEN = true; 
        bool VYELLOW = false;
        bool VRED = false;   
    }
    else if (voltage < 22.2 && voltage >= 21.0)
    {
        bool VGREEN = false; 
        bool VYELLOW = true;
        bool VRED = false;
    
    }   
    else if (voltage > 24.6 || voltage < 21.0)
    {
        bool VGREEN = false; 
        bool VYELLOW = false;
        bool VRED = true;
    }

    gpio_put(RED_LED, VRED); 
    gpio_put(YELLOW_LED, VYELLOW); 
    gpio_put(GREEN_LED, VGREEN); 
}