/********************************************************

$$$$$$$$\                                      $$$$$$\             $$\                         
\__$$  __|                                    $$  __$$\            $$ |                        
   $$ | $$$$$$\   $$$$$$\   $$$$$$\  $$$$$$\  $$ /  \__| $$$$$$\ $$$$$$\    $$$$$$\   $$$$$$\  
   $$ |$$  __$$\ $$  __$$\ $$  __$$\ \____$$\ $$ |$$$$\  \____$$\\_$$  _|  $$  __$$\ $$  __$$\ 
   $$ |$$$$$$$$ |$$ |  \__|$$ |  \__|$$$$$$$ |$$ |\_$$ | $$$$$$$ | $$ |    $$ /  $$ |$$ |  \__|
   $$ |$$   ____|$$ |      $$ |     $$  __$$ |$$ |  $$ |$$  __$$ | $$ |$$\ $$ |  $$ |$$ |      
   $$ |\$$$$$$$\ $$ |      $$ |     \$$$$$$$ |\$$$$$$  |\$$$$$$$ | \$$$$  |\$$$$$$  |$$ |      
   \__| \_______|\__|      \__|      \_______| \______/  \_______|  \____/  \______/ \__|                                                  

    Pi Control Board V0
    RP2350A/B (Pico 2)
    Written by Russell MacGregor
********************************************************/                                                                                               
                       


//*****************<Includes>***************** */
#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/time.h"
#include "hardware/irq.h"

#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"

#include "pins.h"

#include "peripherals/ESP32.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/rover_i2c.h"

#include "control/RemoteXY_control.h"
#include "control/rover_cli.h"
//*****************</Includes>*****************



int main()
{
    //**************<Peripheral Init>**************
    stdio_init_all();

    init_rover_i2c();
    init_relay();
    init_leds();

    sleep_ms(50);

    init_motors();
    printf("\n\n\n");
    printf("> Initializing rover...\n");

    sleep_ms(2000);
    //**************</Peripheral Init>**************

    
    // Run main program
    while (true) {
        //runMenu();
        //runRover_RemoteXYControl();
		//printf("meow\n");


        
        uint8_t i2c_data[4];
        
        // read data from ESP32 over I2C
        i2c_read_esp32(i2c_data, 4);

        // parse and print received data
        int8_t steering    =   (int8_t)i2c_data[0];
        int8_t velocity    =   (int8_t)i2c_data[1];
        uint8_t relay_state =   i2c_data[2];
        uint8_t brake_state =   i2c_data[3];

        printf("data read!\n");
        printf("\tsteering: %d\n", steering);
        printf("\tvelocity: %d\n", velocity);
        printf("\trelay_state: %u\n", relay_state);
        printf("\tbrake_state: %u\n", brake_state);

        sleep_ms(50);


        // Second, validate received values
		// i: check if steering is in valid range (-100 <= steering <= 100)
			if ((steering < -100) || (steering > 100)) {
				printf("INVALID STEERING\n");
			}

        // ii: check if velocity is in valid range (-100 <= velocity <= 100)
            if ((velocity < -100) || (velocity > 100)) {
                printf("INVALID VELOCITY\n");
            }
        
        // iii: check if relay_state is in valid range (0 <= relay_state <= 1)
            if (relay_state > 1) {
                printf("INVALID RELAY STATE: %u\n", relay_state);
            }

        // iv: check if brake_state is in valid range (0 <= brake_state <= 1)
            if (brake_state > 1) {
                printf("INVALID BRAKE STATE: %u\n", brake_state);
            }

    // Third, engage/disengage relay if needed
        // i: get current relay state
            bool current_relay_state = get_relay_state();

        // ii: if new relay state is different from current relay state, change relay state
            if (relay_state != current_relay_state){
                if(relay_state){enable_relay();}
                else {kill_relay();}
            }
            
            
    // Fourth, if brake is engaged, stop rover immediately
        if (brake_state == 1){
            // i: print debug message
                printf("[!] BRAKE ENGAGED\n");
        }
    

    // Fifth, update motor modes based on steering
		if ( (steering < -1 * TURN_STEERING_THRESHOLD) || (steering > TURN_STEERING_THRESHOLD) ){ // is turning?
			// if turning right
				if (steering > TURN_STEERING_THRESHOLD) {
					if (velocity > 0){
						setMotorMode('L', 'F');	// left motors forward
						setMotorMode('R', 'R');	// right motors backward
					}
					else {
						setMotorMode('L', 'R');	// left motors backward
						setMotorMode('R', 'F');	// right motors forward
					}
				}

			// if turning left
				if (steering < -1 * TURN_STEERING_THRESHOLD) {
					if (velocity > 0){
						setMotorMode('L', 'R');	// left motors backward
						setMotorMode('R', 'F');	// right motors forward
					}
					else {
						setMotorMode('L', 'F');	// left motors forward
						setMotorMode('R', 'R');	// right motors backward
					}
				}
		}
		else {
			printf("not turning\n");
			if (velocity >= 0){
				setMotorMode('A', 'F');	// all motors forward
			}
			else {
				setMotorMode('A', 'R');	// all motors backward
			}
		}

	// Sixth, set motor velocities
		// i: get velocity, normalize if needed
			if (velocity < 0){          // set velocity to be 0 <= velocity < 100
				velocity = velocity * -1;
			}

			if (velocity > 100){        // if overflow occurs, reset to 100
				velocity = 100;
			}

		// ii: set motor speeds
			setMotorSpeed_all(velocity);
    }


}