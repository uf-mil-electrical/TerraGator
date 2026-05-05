/******************<Dependencies>*****************/
#include "control/rover_cli.h"
/******************<Dependencies>*****************/



/******************<Public Functions>*****************/
/*******read_digit_serial*******
 * Description
        > helper function for reading from serial monitor
        > will read an input and convert it from ASCII to uint8_t
        > ONLY FOR USE WITH DIGITS, NOT REGULAR CHARACTERS
 * Arguments
        > N/A
 * Returns
        > uint8_t value of digit input
*/
uint8_t read_digit_serial(void){
    uint8_t result = 0;

    while(true){
        // i: get input char (blocking)
            char c_input = getchar();

        // ii: validate input, also check for backspace
            // check for enter key (end of input)
            if (c_input == '\r' || c_input == '\n'){break;}

            // check for backspace or delete key (clear input)
            if (c_input == 0x08 || c_input == 0x7F){
                result = 0;
                printf("\t<cleared>\t");
                continue;
            }

            // ignore non-digit values
            if (c_input < '0' || c_input > '9') {continue;}

        // iii: add new input to result
            result = result * 10 + (c_input-'0');           // convert ASCII to uint8_t
            printf("%c", c_input);                          // print value
    }

    return result;
}


/*******runMenu*******
 * Description
        > reads inputs from serial monitor
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void runMenu(void){
    // First, print menu
        printf("\n\n\n\n\n\n\n");
        printf("**********TerraGator Debug Menu**********\n");
        printf("1: change motor mode\n");
        printf("2: change motor speed\n");
        printf("3: print motor details\n");
        printf("4: read values from ESP32\n");
        printf("5: toggle relay control pin\n");
        printf("Input: ");

        uint8_t input = read_digit_serial();

        printf("\n\n");

    // Second, execute based off input
        switch(input){
            case 1: {   // change motor mode

                printf("> Side to adjust: ");
                char side = getchar();
                printf("%c\n", side);

                printf("> New mode: ");
                char mode = getchar();
                printf("%c\n", mode);

                setMotorMode(side, mode);
                
                break;
            }

            case 2: {   // change motor speed

                printf("> Motor to adjust: ");
                uint8_t target_motor = read_digit_serial();
                printf("\n");

                printf("> New speed: ");
                uint8_t target_speed = read_digit_serial();
                printf("\n");

                setMotorSpeed(target_motor, target_speed);
                break;
            }

            case 3: {   // print motor data
                printMotorDetails();
                break;
            }

            case 4: {   // read values from ESP32
                // i: define structure to hold result
                    uint8_t i2c_data[2];

                // ii: read data from ESP32 over I2C
                    i2c_read_esp32(i2c_data, 2);

                // iii: parse and print received data
                    int8_t velocity = (int8_t)i2c_data[0];
                    uint8_t mode =    i2c_data[1];

                    printf("> Data from ESP32:\n");
                    printf("\tvelocity: %d\n", velocity);
                    printf("\tmode: %u\n", mode);

                break;
            }

            case 5: {   // toggle value of relay control pin
                // i: get relay state
                    bool relay_state = get_relay_state();
                
                // ii: toggle relay pin, print message
                    if (relay_state == false){
                        enable_relay();
                        printf("> Relay is now ON");
                    }
                    else {
                        kill_relay();
                        printf("> Relay is now OFF");
                    }

                break;
            }

            default: {
                printf("Invalid input");
                break;
            }
        }
}
/******************</Public Functions>*****************/