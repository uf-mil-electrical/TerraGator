/******************<Dependencies>*****************/
#include "peripherals/safety_devices.h"
/******************</Dependencies>*****************/



/******************<Private variables>*****************/
bool relayEnabled;                          // keeps track of whether or not relay is enabled/killed
/******************</Private variables>*****************/



/******************<Public variables>*****************/
/******************</Public variables>*****************/



/******************<Relay Control>*****************/

/*******init_relay*******
 * Description
        > initializes the GPIO pin that connects to the relay's control NMOS
        > this must be called before enableRelay or killRelay
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void init_relay(){
    // First, init relay control pin, set relay control pin to OUTPUT
        gpio_init(RELAY_CONTROL);
        gpio_set_dir(RELAY_CONTROL, GPIO_OUT);

    // Second, set relay pin LOW (turn relay off)
        gpio_put(RELAY_CONTROL, GPIO_SET_LOW);

    // Third, indicate that the relay is OFF
        relayEnabled = false;

    // Lastly, return to main program
        return;
}

/*******get_relay_state*******
 * Description
        > used to get state of relay
        > false=relay off, true=relay on
 * Arguments
        > N/A
 * Returns
        > bool (relayEnabled)
*/
bool get_relay_state(){
    // Return value of relayEnabled
    return relayEnabled;
}


/*******enable_relay*******
 * Description
        > sets the relay control pin HIGH
        > note that relay will not turn on if other safety devices have killed the relay (e.g., E-Stop button)
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void enable_relay(){
    // set relay control pin HIGH
        gpio_put(RELAY_CONTROL, GPIO_SET_HIGH);

    // indicate that the relay is ON
        relayEnabled = true;
        set_led(2, true);

    // print debug message
        printf("> enableRelay: relay ENABLED\n");

    // return to main program
        return;
}


/*******kill_relay*******
 * Description
        > sets the relay control pin LOW
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void kill_relay(){
    // set relay control pin LOW
        gpio_put(RELAY_CONTROL, GPIO_SET_LOW);

    // indicate that the relay is OFF
        relayEnabled = false;
        set_led(2, false);

    // Third, print debug message
        printf("> enableRelay: relay KILLED\n");

    // Lastly, return to main program
        return;
}

/******************</Relay Control>*****************/


/******************<LEDs>*****************/

/*******init_leds*******
 * Description
        > initializes LEDs onboard control board
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void init_leds(){

    // initialize relevant GPIO
        gpio_init(LED_1_PIN);
        gpio_init(LED_2_PIN);
        gpio_init(LED_3_PIN);
        gpio_init(LED_4_PIN);
        gpio_init(LED_5_PIN);

    // set GPIO as outputs
        gpio_set_dir(LED_1_PIN, GPIO_OUT);
        gpio_set_dir(LED_2_PIN, GPIO_OUT);
        gpio_set_dir(LED_3_PIN, GPIO_OUT);
        gpio_set_dir(LED_4_PIN, GPIO_OUT);
        gpio_set_dir(LED_5_PIN, GPIO_OUT);

    // set all LEDs low
        gpio_put(LED_1_PIN, GPIO_SET_LOW);
        gpio_put(LED_2_PIN, GPIO_SET_LOW);
        gpio_put(LED_3_PIN, GPIO_SET_LOW);
        gpio_put(LED_4_PIN, GPIO_SET_LOW);
        gpio_put(LED_5_PIN, GPIO_SET_LOW);
}


/*******set_led*******
 * Description
        > sets one LED to on or off
 * Arguments
        > uint8_t led: target LED
        > bool state: false=off, true=on
 * Returns
        > N/A
*/
void set_led(uint8_t led, bool state){

    // adjust targeted LED
        switch (led){
            case 1: {gpio_put(LED_1_PIN, state); break;}
            case 2: {gpio_put(LED_2_PIN, state); break;}
            case 3: {gpio_put(LED_3_PIN, state); break;}
            case 4: {gpio_put(LED_4_PIN, state); break;}
            case 5: {gpio_put(LED_5_PIN, state); break;}
            default: {printf("set_led(): invalid LED\n");}
        }    
}


/*******set_all_leds*******
 * Description
        > sets all LED to on or off
 * Arguments
        > bool state: false=off, true=on
 * Returns
        > N/A
*/
void set_all_leds(bool state){
    for (int i = 0; i < 5; i++){
        set_led(i, state);
    }
}

/******************</LEDs>*****************/