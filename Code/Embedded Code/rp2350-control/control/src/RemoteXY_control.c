

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

    // First, read values from ESP32
        // i: define structure to hold received values
            uint8_t i2c_data[4];
        
        // ii: read data from ESP32 over I2C
            i2c_read_esp32(i2c_data, 4);

        // iii: parse and print received data
			int8_t steering    =   (int8_t)i2c_data[0];
            int8_t velocity    =   (int8_t)i2c_data[1];
            uint8_t relay_state =   i2c_data[2];
            uint8_t brake_state =   i2c_data[3];

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

            // ii: halt motors
                setMotorSpeed_all(0);

            // iii: delay a bit (give motors time to spin down)
                sleep_ms(1000);
            
            // iv: engage brakes if not already engaged
                mode = 'B';
                if (mode != getMotorMode()){
                    setMotorMode('A', 'B');     // set (A)ll motors to (B)rake
                }

            // v: skip rest of this function
                return;
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


			debug_count++;
			if (debug_count >= 5){
				printf("steering: %u\n", steering);
				debug_count = 0;
			}

    // Lastly, return to main program
        return;
}

/******************<Public Functions>*****************/