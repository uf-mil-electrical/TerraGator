/******************<Dependencies>*****************/
#include "peripherals/safety_devices.h"
/******************</Dependencies>*****************/



/******************<Private variables>*****************/
bool relayEnabled;                          // keeps track of whether or not relay is enabled/killed
/******************</Private variables>*****************/



/******************<Public variables>*****************/
/******************</Public variables>*****************/



/******************<Public Functions>*****************/

/*******initRelay*******
 * Description
        > initializes the GPIO pin that connects to the relay's control NMOS
        > this must be called before enableRelay or killRelay
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void initRelay(){
    // First, init relay control pin, set relay control pin to OUTPUT
        gpio_init(RELAY_CONTROL);
        gpio_set_dir(RELAY_CONTROL, SET_GPIO_OUTPUT);

    // Second, set relay pin LOW (turn relay off)
        gpio_put(RELAY_CONTROL, GPIO_SET_LOW);

    // Third, indicate that the relay is OFF
        relayEnabled = false;

    // Lastly, return to main program
        return;
}

/*******getRelayState*******
 * Description
        > used to get state of relay
        > false=relay off, true=relay on
 * Arguments
        > N/A
 * Returns
        > bool (relayEnabled)
*/
bool getRelayState(){
    // Return value of relayEnabled
    return relayEnabled;
}

/*******enableRelay*******
 * Description
        > sets the relay control pin HIGH
        > note that relay will not turn on if other safety devices have killed the relay (e.g., E-Stop button)
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void enableRelay(){
    // First, set relay control pin HIGH
        gpio_put(RELAY_CONTROL, GPIO_SET_HIGH);

    // Second, indicate that the relay is ON
        relayEnabled = true;

    // Third, print debug message
        printf("> enableRelay: relay ENABLED\n");

    // Lastly, return to main program
        return;
}

/*******killRelay*******
 * Description
        > sets the relay control pin LOW
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void killRelay(){
    // First, set relay control pin LOW
        gpio_put(RELAY_CONTROL, GPIO_SET_LOW);

    // Second, indicate that the relay is OFF
        relayEnabled = false;

    // Third, print debug message
        printf("> enableRelay: relay KILLED\n");

    // Lastly, return to main program
        return;
}
/******************</Public Functions>*****************/
