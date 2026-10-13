/******************<Dependencies>*****************/
#include "communication/data_parser.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/safety_devices.h"
/******************</Dependencies>*****************/



/******************<Public Functions>*****************/
/*******rover_i2c_parse_data*******
 * Description
        > implementation of MIL data serialization protocol
        > reads a raw I2C packet received from master I2C read
        > validates sync bytes, extracts header info
        > extracts payload into an array
        > [TBD] executes Fletcher's checksum
 * Arguments
        > uint8_t* packet: pointer to received I2C packet
        > uint16_t packet_len: number of bytes in received packet
 * Returns
        > N/A
*/
bool rover_i2c_parse_data(uint8_t* packet, uint16_t packet_len){

    // define variables to store decoded data
        uint8_t payload_len_lo, payload_len_hi;
        uint16_t payload_length = 0;

    // make sure packet contains at least the 6-byte header
        if (packet == NULL || packet_len < 6){
            printf("\t> packet too short!\n");
            return false;
        }

    // validate sync bytes
        if (packet[0] != 0x37 || packet[1] != 0x01){
            printf("\t> invalid sync bits (0x%02X, 0x%02X)\n", packet[0], packet[1]);
            return false;
        }

    // get payload length
        payload_len_lo = packet[4];
        payload_len_hi = packet[5];
        payload_length = (uint16_t)(payload_len_hi) << 8 | (uint16_t)(payload_len_lo);

    // make sure packet contains the full payload
        if (packet_len < (6 + payload_length)){
            printf("\t> incomplete payload!\n");
            return false;
        }

        if (packet[2] != 0x30 || packet[3] != 0x00 || payload_length != 3 ||
            packet_len != 9 || (packet[6] >> 4) != 0x05 ||
            packet[7] > 100 || packet[8] > 100) {
            printf("Rejected motor packet: class=%02X subclass=%02X len=%u device=%u speeds=%u,%u\n",
                   packet[2], packet[3], payload_length, packet[6] >> 4,
                   packet[7], packet[8]);
            return false;
        }

    // payload starts after the 6-byte header
        uint8_t* payload = &packet[6];

    // This nine-byte ESP32 format has no checksum bytes.

    // proceed based on subclass ID
        uint8_t subclass_id = packet[3];
        switch(subclass_id){
            case 0x00: {
                rover_i2c_extract_motor_commands(payload);
                break;
            }

            default: {
                printf("\t> Invalid subclass ID!\n");
            }
        }
    return true;
}

void rover_stop_motors(void) {
    setMotorSpeed_all(0);
    setMotorMode('A', 'B');
    kill_relay();
}



/*******rover_i2c_extract_motor_commands*******
 * Description
        > extracts motor commands from received I2C payload
        > Subclass ID: 0x00
 * Arguments
        > uint8_t* payload: data extracted from rover_i2c_parse_data()
 * Returns
        > N/A
*/
void rover_i2c_extract_motor_commands(uint8_t* payload){
    // define variables to store data after extraction
        bool right_motor_dir, left_motor_dir, brake_state, relay_state;
        uint8_t device_id, left_motor_speed, right_motor_speed;

    // extract values from first byte
        right_motor_dir = (payload[0] >> 0) & 0x01;
        left_motor_dir  = (payload[0] >> 1) & 0x01;
        brake_state     = (payload[0] >> 2) & 0x01;
        relay_state     = (payload[0] >> 3) & 0x01;
        device_id       = (payload[0] >> 4) & 0x0F;

    // extract motor speed data
        left_motor_speed  = payload[1];
        right_motor_speed = payload[2];

    // Limit USB output so logging does not dominate command processing.
        static uint64_t last_log_us = 0;
        const uint64_t now = time_us_64();
        if (now - last_log_us >= 200000) {
            last_log_us = now;
            printf("Motor RX: device=%u left=%u/%s right=%u/%s relay=%u brake=%u\n",
                   device_id, left_motor_speed, left_motor_dir ? "F" : "R",
                   right_motor_speed, right_motor_dir ? "F" : "R",
                   relay_state, brake_state);
        }

    // Remove PWM before changing direction or braking.
        setMotorSpeed_all(0);
        if (!relay_state) {
            rover_stop_motors();
        } else if (brake_state) {
            setMotorMode('A', 'B');
            if (!get_relay_state()) enable_relay();
        } else {
            setMotorMode('L', left_motor_dir ? 'F' : 'R');
            setMotorMode('R', right_motor_dir ? 'F' : 'R');
            if (!get_relay_state()) enable_relay();
            setMotorSpeed_side('L', left_motor_speed);
            setMotorSpeed_side('R', right_motor_speed);
        }

    // return
        return;
}
/******************</Public Functions>*****************/
