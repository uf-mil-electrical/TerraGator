#include "led.h"
#include "board_config.h"
#include "hardware/irq.h"
#include "hardware/gpio.h"

const int LED_pins[NUM_LED] = {GREEN_LED, YELLOW_LED, RED_LED}; 
bool GYR_led_toggle_flag[NUM_LED] = {false, false, false}; // for voltage indication
bool GYR_led_state[NUM_LED] = {false, false, false};  
bool GYR_current_flag[NUM_LED] = {false, false, false}; // for current indication

bool led_timer_callback(repeating_timer_t *rt)
{
    // toggle LEDs to be flashed
    for (int i = 0; i < NUM_LED; i++)
    {
        // if enabled, toggle the state and write
        if(GYR_led_toggle_flag[i])
        {
            // invert state
            GYR_led_state[i] = !GYR_led_state[i];
            gpio_put(LED_pins[i], GYR_led_state[i]); 
        }
    }
    return true; 

}

void led_init()
{
    // enable IO 
    gpio_init(RED_LED);
    gpio_init(YELLOW_LED);
    gpio_init(GREEN_LED);

    // set GPIOs for LEDs as outputs and assert low 
    gpio_set_dir(RED_LED, GPIO_OUT);
    gpio_set_dir(YELLOW_LED, GPIO_OUT);
    gpio_set_dir(GREEN_LED, GPIO_OUT);

    gpio_put(RED_LED, 0);
    gpio_put(YELLOW_LED, 0);
    gpio_put(GREEN_LED, 0);

    // add a timer callback for approximately 0.5 second flashing intervals 
    static repeating_timer_t timer; 
    add_repeating_timer_ms(500, led_timer_callback, NULL, &timer); 
}

uint8_t change_led_indicators (float voltage, float current)
{
    uint8_t status = BMS_STATUS_OK;
    // change LED state based on voltage and current measurements 
    // FLASH voltage indicators and use SOLID current indicators (voltage has priority over current for LiPo protection)
    // check if we're outside acceptable range(s) 
    // } VOLTAGE RANGES
    // With LiPo, air landing voltage is 3.7V/cell, max is 4.1V/cell
    // |    > 24.6     |  flash RED   | overcharged (>4.1V / cell)
    // | [22.8, 24.6V] | flash GREEN  | ideal operating region (<= 4.1V/cell && >= 3.8V/cell)
    // | [21.0, 22.8)  | flash YELLOW | approaching air landing voltage or lower, disconnect soon (>= 3.5V/cell && < 3.8V/cell )
    // |    < 21.0     |  flash RED   | approaching deep discharge, disconnect now (<3.5V/cell)
    
    bool VGREEN; 
    bool VYELLOW; 
    bool VRED; 

    if (voltage <= 24.6 && voltage >= 22.8)    
    {
        GYR_led_toggle_flag[0] = true;
        GYR_led_toggle_flag[1] = false;
        GYR_led_toggle_flag[2] = false; 
    }
    else if (voltage < 22.8 && voltage >= 21.0)
    {
        GYR_led_toggle_flag[0] = false;
        GYR_led_toggle_flag[1] = true;
        GYR_led_toggle_flag[2] = false; 
        status |= BMS_STATUS_VOLTAGE_NEAR_AIR_LANDING;   
    }   
    else if (voltage > 24.6 || voltage < 21.0)
    {
        GYR_led_toggle_flag[0] = false;
        GYR_led_toggle_flag[1] = false;
        GYR_led_toggle_flag[2] = true;  
        status |= BMS_STATUS_VOLTAGE_NEAR_DEEP_DISCHARGE;  
    }

    // } CURRENT RANGES
    // | may be changed later, just for testing now  
    // |
    // | 
    if (current < 10)
    {
        GYR_current_flag[0] = true; 
        GYR_current_flag[1] = false; 
        GYR_current_flag[2] = false; 
    }
    else if (current >= 10 && current < 15)
    {
        GYR_current_flag[0] = false; 
        GYR_current_flag[1] = true; 
        GYR_current_flag[2] = false;
        status |= BMS_STATUS_CURRENT_WARNING; 
    }
    else 
    {
        GYR_current_flag[0] = false; 
        GYR_current_flag[1] = false; 
        GYR_current_flag[2] = true;
        status |= BMS_STATUS_CURRENT_CRITICAL;
    }

    for (int i = 0; i < NUM_LED; i++)
    {
        // if voltage flag is enabled, leave as flashing indication 
        // (current at same LED state is implied) so do nothing 

        // if voltage flag is not enabled and current flag is enabled, turn on LED
        // to indicate current
        if (GYR_current_flag[i] && !GYR_led_toggle_flag[i])
        {
            gpio_put(LED_pins[i], true);
        }
        else if (!GYR_current_flag[i] && !GYR_led_toggle_flag[i])
        {
            gpio_put(LED_pins[i], false);
        }
        
    }
    return status;
}