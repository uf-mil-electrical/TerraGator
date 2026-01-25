/******************<Dependencies>*****************/
#include "peripherals/motor_speed_control.h"
/******************</Dependencies>*****************/


/******************<Private variables>*****************/
bool motors_initialized = false;           // keeps track of whether or not initMotors has been called
char current_motor_mode = ' ';             // keeps track of motor mode (only updated when mode for both sides is updated)
/******************</Private variables>*****************/


/******************<Public variables>*****************/
motor_str motors[NUM_MOTORS];               // Global array of motor structs
/******************</Public variables>*****************/


/******************<Function definitions>*****************/

/*******motor_init*******
 * Description
        > initializes all motor PWM stuff
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void motor_init() {

    // First, set GPIO pins for each motor
        motors[0].motor_ID = 1;                         // Motor #1
        motors[0].side = 0;                             // left side of rover
        motors[0].GPIO_pin = MOTOR1_PWM;

        motors[1].motor_ID = 2;                         // Motor #2
        motors[1].side = 0;                             // left side of rover
        motors[1].GPIO_pin = MOTOR2_PWM;

        motors[2].motor_ID = 3;                         // Motor #3
        motors[2].side = 0;                             // left side of rover
        motors[2].GPIO_pin = MOTOR3_PWM;

        motors[3].motor_ID = 4;                         // Motor #4
        motors[3].side = 1;                             // right side of rover
        motors[3].GPIO_pin = MOTOR4_PWM;

        motors[4].motor_ID = 5;                         // Motor #5
        motors[4].side = 1;                             // right side of rover
        motors[4].GPIO_pin = MOTOR5_PWM;

        motors[5].motor_ID = 6;                         // Motor #6
        motors[5].side = 1;                             // right side of rover
        motors[5].GPIO_pin = MOTOR6_PWM;

    // Second, set configuration for PWM pins
        pwm_config config = pwm_get_default_config();   // get default PWM configuration
        pwm_config_set_clkdiv(&config, 4.f);            // set divider, reduces counter clock to sysclock/this value

    
    // Third, run for loop to initialize each motor PWM slice & channel
        uint8_t initialized_slices[NUM_MOTORS];
        bool slice_initialized = false;

        for (uint8_t i = 0; i < NUM_MOTORS; i++){
            initialized_slices[i] = 100;                // Set all values of this array to something other than 0. Prevents Slice 0 from going uninitialized.
        }

        for (uint8_t i = 0; i < NUM_MOTORS; i++){
            // i: set GPIO pin to be PWM
                gpio_set_function(motors[i].GPIO_pin, GPIO_FUNC_PWM);

            // ii: get slice and channel for this pin
                motors[i].PWM_slice   = pwm_gpio_to_slice_num(motors[i].GPIO_pin);
                motors[i].PWM_channel = pwm_gpio_to_channel(motors[i].GPIO_pin);      // 0=A, 1=B

            // iii: initialize slice, or skip if this slice has already been initialized
                slice_initialized = false;

                for (uint8_t j = 0; j < NUM_MOTORS; j++){   // check if this slice has been initialized
                    if (initialized_slices[j] == motors[i].PWM_slice){
                        slice_initialized = true;
                        break;
                    }
                }

                if (slice_initialized == false){            // initialize slice if not yet initialized
                    pwm_config_set_wrap(&config, PWM_WRAP);
                    pwm_init(motors[i].PWM_slice, &config, true);
                }
        }

    // Fourth, initialize control logic pins
        gpio_init(LEFT_MOTOR_CONTROL_1);                    // initialize pins
        gpio_init(LEFT_MOTOR_CONTROL_2);
        gpio_init(RIGHT_MOTOR_CONTROL_1);
        gpio_init(RIGHT_MOTOR_CONTROL_2);

        gpio_set_dir(LEFT_MOTOR_CONTROL_1, GPIO_OUT);       // set control logic pins as outputs
        gpio_set_dir(LEFT_MOTOR_CONTROL_2, GPIO_OUT);
        gpio_set_dir(RIGHT_MOTOR_CONTROL_1, GPIO_OUT);
        gpio_set_dir(RIGHT_MOTOR_CONTROL_2, GPIO_OUT);

    // Fifth, indicate that initMotors() has been run
        motors_initialized = true;

    // Sixth, print debug message
        printf("Motors initialized\n");
}


/*******printMotorDetails*******
 * Description
        > prints out information about each motor's PWM settings
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void printMotorDetails() {

    // Prepare variables for this function
        uint8_t ID = 0;
        uint8_t side = 0;
        uint8_t GPIO_pin = 0;
        uint8_t slice = 0;
        uint8_t channel = 0;

        char side_ch = ' ';
        char channel_ch = ' ';

    // Print motor data
    printf("*****Printing motor PWM settings*****\n");

    for (uint8_t i = 0; i < NUM_MOTORS; i++){

        // i: get data from motor struct
            ID          = motors[i].motor_ID;
            side        = motors[i].side;
            GPIO_pin    = motors[i].GPIO_pin;
            slice       = motors[i].PWM_slice;
            channel     = motors[i].PWM_channel;

        // ii: format data as needed
            if (side == 0){side_ch = 'L';}
            else {side_ch = 'R';}

            if (channel == 0){channel_ch = 'A';}
            else {channel_ch = 'B';}
            
        // iii: print data for this motor
            printf("\t> Motor %u\n", ID);
            printf("\t\t- Side: %c\n", side_ch);
            printf("\t\t- GPIO: %u\n", GPIO_pin);
            printf("\t\t- PWM Slice: %u\n", slice);
            printf("\t\t- PWM Channel: %c\n", channel_ch);

    }
}


/*******setMotorMode*******
 * Description
        > sets motor mode for all left motors OR all right motors, or all motors
 * Arguments
        > motor_set
            'L': left motors affected only
            'R': right motors affected only
            'A': all motors affected
        > mode
            'F': forward
            'R': reverse
            'B': brake
            'N': neutral
 * Returns
        > N/A
*/
void setMotorMode(char motor_set, char mode){
    // First, validate inputs
        if ((motor_set != 'L') && (motor_set != 'R') && (motor_set != 'A')){
            printf("setMotorMode(): invalid motor_set argument (%c)", motor_set);
            return;
        }

        if ((mode != 'F') && (mode != 'R') && (mode != 'B') && (mode != 'N')){
            printf("setMotorMode(): invalid mode argument (%c)", mode);
            return;
        }

        if ((mode == 'B') && (motor_set != 'A')){
            printf("setMotorMode(): unsafe request, braking should affect all motors simultaneously");
            return;
        }

    // Second, determine which logic pins should be affected
        bool affect_left = false;
        bool affect_right = false;

        if ((motor_set == 'L') || (motor_set == 'A')){
            affect_left = true;
        }
        if ((motor_set == 'R') || (motor_set == 'A')){
            affect_right = true;
        }
    
    // Third, update affected logic pins
        switch(mode){
            case 'F': { // IN1 = 1, IN2 = 0

                if (affect_left){
                    gpio_put(LEFT_MOTOR_CONTROL_1, true);
                    gpio_put(LEFT_MOTOR_CONTROL_2, false);
                    printf("setMotorMode(): left motors set to forward\n");
                }

                if (affect_right){
                    gpio_put(RIGHT_MOTOR_CONTROL_1, false);
                    gpio_put(RIGHT_MOTOR_CONTROL_2, true);
                    printf("setMotorMode(): right motors set to forward\n");
                }

                if (motor_set == 'A'){current_motor_mode = 'F';}

                break;
            }
            case 'R': { // IN1 = 0, IN2 = 1 

                if (affect_left){
                    gpio_put(LEFT_MOTOR_CONTROL_1, false);
                    gpio_put(LEFT_MOTOR_CONTROL_2, true);
                    printf("setMotorMode(): left motors set to reverse\n");
                }

                if (affect_right){
                    gpio_put(RIGHT_MOTOR_CONTROL_1, true);
                    gpio_put(RIGHT_MOTOR_CONTROL_2, false);
                    printf("setMotorMode(): right motors set to reverse\n");
                }

                if (motor_set == 'A'){current_motor_mode = 'R';}

                break;
            }
            case 'B': { // IN1 = 0, IN2 = 0

                if (affect_left){
                    gpio_put(LEFT_MOTOR_CONTROL_1, false);
                    gpio_put(LEFT_MOTOR_CONTROL_2, false);
                    printf("setMotorMode(): left motors set to brake\n");
                }

                if (affect_right){
                    gpio_put(RIGHT_MOTOR_CONTROL_1, false);
                    gpio_put(RIGHT_MOTOR_CONTROL_2, false);
                    printf("setMotorMode(): right motors set to brake\n");
                }

                if (motor_set == 'A'){current_motor_mode = 'B';}

                break;
            }
            case 'N': { // IN1 = 1, IN2 = 1

                if (affect_left){
                    gpio_put(LEFT_MOTOR_CONTROL_1, true);
                    gpio_put(LEFT_MOTOR_CONTROL_2, true);
                    printf("setMotorMode(): left motors set to neutral\n");
                }

                if (affect_right){
                    gpio_put(RIGHT_MOTOR_CONTROL_1, true);
                    gpio_put(RIGHT_MOTOR_CONTROL_2, true);
                    printf("setMotorMode(): right motors set to neutral\n");
                }

                if (motor_set == 'A'){current_motor_mode = 'N';}

                break;
            }
        }

    // Lastly, return to main program
        return;
}

/*******getMotorMode*******
 * Description
        > returns the current rover mode
        > note: is only updated when all mode for both sides is modified in setMotorMode
 * Arguments
        > N/A
 * Returns
        > char: motor mode ('F', 'R', 'B', 'N')
*/
char getMotorMode(){
    return current_motor_mode;
}


/*******setMotorSpeed*******
 * Description
        > changes speed of one motor
 * Arguments
        > ID: motor ID of target motor
        > speed: value from 0-100, where 0 means off and 100 means max speed
 * Returns
        > N/A
*/
void setMotorSpeed(uint8_t ID, uint8_t speed) {

    // First, validate inputs, determine which motor's PWM to adjust
        if (speed > 100) {          // check if speed arg is valid
            printf("setMotorSpeed(): invalid speed input\n");
            return;
        }

        if (motors_initialized == false){   // check if initMotors has been called
            printf("setMotorSpeed(): motors not yet initialized, call initMotors() before attempting to call setMotorSpeed()\n");
            return;
        }

        uint8_t target_motor = 0;
        bool motor_found = false;
        for (uint8_t i = 0; i < NUM_MOTORS; i++){
            if (motors[i].motor_ID == ID){
                target_motor = i;
                motor_found = true;
                break;
            }
        }

        if (motor_found == false){
            printf("setMotorSpeed(): invalid motor ID\n");
            return;
        }

    // Second, get PWM slice, channel, and wrap value (period)
        uint8_t slice = motors[target_motor].PWM_slice;
        uint8_t channel = motors[target_motor].PWM_channel;

    // Third, cast speed (0-100) to period
        uint16_t level = (uint16_t)((speed*(PWM_WRAP+1u)+50u)/100u);
        if(level > PWM_WRAP){level = PWM_WRAP;}

    // Fourth, update duty cycle for this channel only
        pwm_set_chan_level(slice, channel, level);

    // Fifth, print update
        //printf("setMotorSpeed(): changed Motor %u's PWM duty cycle to %u%%\n", ID, speed);

    // Lastly, return to main program
        return;
}


/*******setMotorSpeed_all*******
 * Description
        > changes speed of all motors
 * Arguments
        > speed: value from 0-100, where 0 means off and 100 means max speed
 * Returns
        > N/A
*/
void setMotorSpeed_all(uint8_t speed){
    for (uint8_t i = 1; i <= NUM_MOTORS; i++){
        setMotorSpeed(i, speed);
    }
}


/*******setMotorSpeed_side*******
 * Description
        > changes speed of all motors on one side
 * Arguments
		> side: 'L' = all left motors, 'R' = all right motors
        > speed: value from 0-100, where 0 means off and 100 means max speed
 * Returns
        > N/A
*/
void setMotorSpeed_side(char side, uint8_t speed){

	switch(side){
		case 'L': {
			setMotorSpeed(1, speed);
			setMotorSpeed(2, speed);
			setMotorSpeed(3, speed);
			break;
		}
		case 'R': {
			setMotorSpeed(4, speed);
			setMotorSpeed(5, speed);
			setMotorSpeed(6, speed);
			break;
		}
		default: {
			printf("setMotorSpeed_side(): invalid side\n");
			break;
		}
	}
}

/******************</Function definitions>*****************/