

/******************<Dependencies>*****************/
#include "control/RemoteXY_control.h"
/******************</Dependencies>*****************/



/******************<Temp debug>*****************/
uint8_t debug_count = 0;
/******************</Temp debug>*****************/



/******************<Public Functions>*****************/

/*******runRover_RemoteXYControl()*******
 * Description
        > reads data from ESP32 running the RemoteXY GUI over I2C
        > updates RP2350 outputs accordingly
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void runRover_RemoteXYControl(){
    char mode = ' ';
	uint8_t left_steering_multiplier = 0;
	uint8_t right_steering_multiplier = 0;

	uint8_t left_motor_velocity = 0;
	uint8_t right_motor_velocity = 0;

    // First, read values from ESP32
        // i: define structure to hold received values
            uint8_t i2c_data[4];
        
        // ii: read data from ESP32 over I2C
            i2c_read_esp32(i2c_data, 4);

        // iii: parse and print received data
			int8_t steering    =   (int8_t)i2c_data[0];
            int8_t velocity    =   (int8_t)i2c_data[1];
            uint8_t relay_state =   i2c_data[1];
            uint8_t brake_state =   i2c_data[2];

            //printf("I2C Data received: steering = %d, velocity = %d, relay_state = %u, brake_state = %u\n", steering, velocity, relay_state, brake_state);

    // Second, validate received values
		// i: check if steering is in valid range (-100 <= steering <= 100)
			if ((steering < -100) || (steering > 100)) {
				printf("INVALID STEERING\n");
				return;
			}

        // ii: check if velocity is in valid range (-100 <= velocity <= 100)
            if ((velocity < -100) || (velocity > 100)) {
                printf("INVALID VELOCITY\n");
                return;
            }
        
        // iii: check if relay_state is in valid range (0 <= relay_state <= 1)
            if (relay_state > 1) {
                printf("INVALID RELAY STATE: %u\n", relay_state);
                return;
            }

        // iv: check if brake_state is in valid range (0 <= brake_state <= 1)
            if (brake_state > 1) {
                printf("INVALID BRAKE STATE: %u\n", brake_state);
                return;
            }

    // Third, engage/disengage relay if needed
        // i: get current relay state
            bool current_relay_state = getRelayState();

        // ii: if new relay state is different from current relay state, change relay state
            if (relay_state != current_relay_state){
                if(relay_state){enableRelay();}
                else {killRelay();}
            }
            
            
    // Fourth, if brake is engaged, stop rover immediately
        if (brake_state == 1){
            // i: print debug message
                printf("[!] BRAKE ENGAGED\n");

            // ii: halt motors
                setMotorSpeed_all(0);

            // iii: delay a bit (give motors time to spin down)
                sleep_ms(500);
            
            // iv: engage brakes if not already engaged
                mode = 'B';
                if (mode != getMotorMode()){
                    setMotorMode('A', 'B');     // set (A)ll motors to (B)rake
                }

            // v: skip rest of this function
                return;
        }
    

    // Fifth, update motor speed, mode, based on velocity and steering
        // i: determine rover motor mode based on velocity
            if (velocity > 0){      // if velocity > 0, go forwards
                mode = 'F';
            }
            else if (velocity < 0){ // if velocity < 0, go backwards
                mode = 'R';
            }
            else {                  // if velocity == 0 (or something else), neutral
                mode = 'N';
            }

		// ii: determine direction to move
			if (steering > 0){ // turn right
				left_steering_multiplier = steering;
				right_steering_multiplier = 100-steering;
			}
			else if (steering < 0){ // turn left
				steering = -1 * steering;
				left_steering_multiplier = 100 - steering;
				right_steering_multiplier = steering;			
			}
			else {	// don't turn
				left_steering_multiplier = 100;
				right_steering_multiplier = 100;
			}

		// iii: get velocity, normalize if needed
			if (velocity < 0){          // set velocity to be 0 <= velocity < 100
                velocity = velocity * -1;
            }

            if (velocity > 100){        // if overflow occurs, reset to 100
                velocity = 100;
            }

		// iv: update mode for all motors
            if (mode != getMotorMode()){
                setMotorMode('A', mode);
            }

		// v: set motor speeds
			left_motor_velocity = (velocity * left_steering_multiplier);
			right_motor_velocity = (velocity * right_steering_multiplier);

			//setMotorSpeed_side('L', left_motor_velocity);
			//setMotorSpeed_side('R', right_motor_velocity);

			debug_count++;
			if (debug_count >= 5){
				printf("overall velo: %u, L steer: %u, R steer: %u, L velo: %u, R velo: %u\n", velocity, left_steering_multiplier, right_steering_multiplier, left_motor_velocity, right_motor_velocity);
				debug_count = 0;
			}

    // Lastly, return to main program
        return;
}

/******************<Public Functions>*****************/