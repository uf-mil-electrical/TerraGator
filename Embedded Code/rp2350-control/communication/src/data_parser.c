/******************<Dependencies>*****************/
#include "communication/data_parser.h"
/******************</Dependencies>*****************/



/******************<Public Functions>*****************/
/*******rover_i2c_parse_data*******
 * Description
        > implementation of MIL data serialization protocol
        > reads a raw I2C packet from RX FIFO (slave mode)
        > validates sync bytes, extracts header info
        > extracts payload into an array
        > [TBD] executes Fletcher's checksum
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use (i2c0 or i2c1)
 * Returns
        > N/A
*/
void rover_i2c_parse_data(i2c_inst_t* i2c_bus){

    printf("rover_i2c_parse_data(): reading data...\n");

    // define variables to store decoded data
        uint8_t header[6];
        uint8_t payload_len_lo, payload_len_hi;
        uint16_t payload_length = 0;

    // read 6-byte header (sync bits, class, subclass, payload length)
        i2c_read_raw_blocking(i2c_bus, header, 6);

    // validate sync bytes
        if (header[0] != 0x37 || header[1] != 0x01){
            printf("\t> invalid sync bits (0x%02X, 0x%02X)\n", header[0], header[1]);
        }

    // print class ID and subclass ID
        printf("\t> Class ID: 0x%02X\n", header[2]);
        printf("\t> Subclass ID: 0x%02X\n", header[3]);

    // get payload length
        payload_len_lo = header[4];
        payload_len_hi = header[5];
        payload_length = (uint16_t)(payload_len_hi) << 8 | (uint16_t)(payload_len_lo);
        printf("\t> Payload length: %u\n", payload_length);

    // read payload
        uint8_t payload[payload_length];
        i2c_read_raw_blocking(i2c_bus, payload, payload_length);

    // read checksum (last two bytes) to validate data

    // proceed based on subclass ID
        uint8_t subclass_id = header[3];
        switch(subclass_id){
            case 0x00: {
                rover_i2c_extract_motor_commands(payload);
                break;
            }

            default: {
                printf("\t> Invalid subclass ID!\n");
            }
        }
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

    // print values
        printf("rover_i2c_extract_motor_commands(): data received\n");
        printf("\t> device_id: %u\n", device_id);
        printf("\t> right_motor_direction: %u\n", right_motor_dir);
        printf("\t> left_motor_direction: %u\n", left_motor_dir);
        printf("\t> brake_state: %u\n", brake_state);
        printf("\t> relay_state: %u\n", relay_state);
        printf("\t> right_motor_speed: %u\n", right_motor_speed);
        printf("\t> left_motor_speed: %u\n", left_motor_speed);

    // return
        return;
}
/******************</Public Functions>*****************/
