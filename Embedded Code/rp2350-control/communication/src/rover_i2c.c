/******************<Dependencies>*****************/
#include "communication/rover_i2c.h"
/******************</Dependencies>*****************/



/******************<Private Functions>*****************/

/* no long using for master design
// buffer for in-progress I2C transaction
static volatile uint8_t rx_buf[ROVER_I2C_MAX_RX_BUF_SIZE];
static volatile uint16_t rx_len = 0;

// buffer for previous I2C transaction
static volatile uint8_t packet_buf[ROVER_I2C_MAX_RX_BUF_SIZE];
static volatile uint16_t packet_len = 0;
static volatile bool packet_ready = false;
*/
/******************</Private Functions>*****************/



/******************<Private Functions>*****************/

/******************<Private Functions>*****************/


/******************<Public Functions>*****************/

/*******init_rover_i2c*******
 * Description
        > initializes an I2C bus for communicating with other devices
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to init (i2c0 or i2c1s)
        > uint8_t sda_pin: valid GPIO pin for use as SDA
        > uint8_t scl_pin: valid GPIO pin for use as SCL
        > uint baudrate: baudrate for this I2C bus
        > bool master_mode: true=init I2C bus in master mode, false=init in slave mode
 * Returns
        > N/A
*/
void init_rover_i2c(i2c_inst_t* i2c_bus, uint8_t sda_pin, uint8_t scl_pin, uint baudrate, bool master_mode){

    // initialialize I2C bus
        i2c_init(i2c_bus, baudrate);
        gpio_set_function(sda_pin, GPIO_FUNC_I2C);
        gpio_set_function(scl_pin, GPIO_FUNC_I2C);
        gpio_pull_up(sda_pin);
        gpio_pull_up(scl_pin);

    // if not in master mode
        if (!master_mode){
            i2c_set_slave_mode(i2c_bus, true, RP2350_I2C_ADDRESS);
        }

    // lastly, return
        return;
}


/*******rover_i2c_read*******
 * Description
        > reads data from an I2C bus
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use
        > uint8_t read_address: address from which to read
        > uint8_t* data: pointer to location to store received data
        > uint8_t num_bytes: number of bytes to read
 * Returns
        > int num_bytes_read
*/
int rover_i2c_read(i2c_inst_t* i2c_bus, uint8_t read_address, uint8_t* data, uint8_t num_bytes){
    int num_bytes_read = 0;

    // read data from I2C buffer
        num_bytes_read = i2c_read_timeout_us(
            i2c_bus,
            read_address,
            data,
            num_bytes,
            false,         // nostop = false (If true, master retains control of the bus at the end of the transfer (no Stop is issued), and the next transfer will begin with a Restart rather than a Start)
            I2C_TIMEOUT_DELAY
        );

    // Second, print debug message
        if (num_bytes_read < 0){
            printf("rover_i2c_read(): I2C error (%d)\n", num_bytes_read);
        }
        else if (num_bytes_read != num_bytes){
            printf("rover_i2c_read(): expected %u bytes, received %d\n", num_bytes, num_bytes_read);
        }

    // Lastly, return to main program
        return num_bytes_read;
}

/*******rover_i2c_get_num_bytes_rx*******
 * Description
        > determines how many bytes of data have been received over I2C
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use
        > uint8_t* num_bytes: pointer to variable in which to store num bytes received
 * Returns
        > N/A
*/
/* no longer use in the master design
void rover_i2c_get_num_bytes_rx(i2c_inst_t* i2c_bus, uint8_t* num_bytes){
    *num_bytes = i2c_get_read_available(i2c_bus);
}
    */


/*******rover_i2c_write*******
 * Description
        > write data to an I2C bus
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use
        > uint8_t write_address: address to which to write
        > uint8_t* data: pointer to data array
        > num_bytes: number of bytes to write
 * Returns
        > N/A
*/
int rover_i2c_write(i2c_inst_t* i2c_bus, uint8_t write_address, uint8_t* data, uint8_t num_bytes){

        int result;

    // First, write data to I2C bus
      result = i2c_write_timeout_us(i2c_bus, write_address, data, num_bytes,
                                   false, I2C_TIMEOUT_DELAY);

    /*
        i2c_write_timeout_us(
            I2C_PORT,
            ESP32_I2C_ADDRESS,
            data,
            num_bytes,
            false,                  // nostop = false (If true, master retains control of the bus at the end of the transfer (no Stop is issued), and the next transfer will begin with a Restart rather than a Start)
            I2C_TIMEOUT_DELAY
        );
    */

    // Second, print debug message
        if (result != num_bytes) {
            static uint64_t last_error_us = 0;
            const uint64_t now = time_us_64();
            if (now - last_error_us >= 1000000) {
                last_error_us = now;
                printf("I2C write FAILED: address=0x%02X expected=%u result=%d; check SDA/SCL/GND and ESP32 slave init\n",
                       write_address, num_bytes, result);
            }
        }

    // Lastly, return to main program
        return result; // capture the return value
}


// new function added polling from esp32
/*******rover_i2c_poll_esp32*******
 * Description
        > polls ESP32 to check if new RemoteXY motor data exists
        > if new data exists, requests the full motor command packet
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use
        > uint8_t* packet: pointer to location to store received motor packet
 * Returns
        > bool: true=new motor packet received, false=no new packet or I2C error
*/
bool rover_i2c_poll_esp32(i2c_inst_t* i2c_bus, uint8_t* packet){

    uint8_t request = I2C_STATUS_REQUEST;
    uint8_t status = 0;

    int num_bytes_written = 0;
    int num_bytes_read = 0;


    // ask ESP32 for status
        num_bytes_written = rover_i2c_write(
            i2c_bus,
            ESP32_I2C_ADDRESS,
            &request,
            1 // number of bytes of 1, single command
        );

        if (num_bytes_written != 1){
            return false;
        }


    // ESP32 handles the request in a FreeRTOS task and preloads the reply.
    // A STOP alone does not guarantee that callback has run before the read.
        sleep_ms(2);

    // read status byte from ESP32
        num_bytes_read = rover_i2c_read(
            i2c_bus,
            ESP32_I2C_ADDRESS,
            &status,
            1
        );

        if (num_bytes_read != 1){
            return false;
        }


    // no new RemoteXY data
        if (status != 1){
            return false;
        }


    // new data exists, request motor packet
        request = I2C_MOTOR_DATA_REQUEST;

        num_bytes_written = rover_i2c_write(
            i2c_bus,
            ESP32_I2C_ADDRESS,
            &request,
            1
        );

        if (num_bytes_written != 1){
            return false;
        }


        sleep_ms(2);

    //do a second request read full motor command packet
        num_bytes_read = rover_i2c_read(
            i2c_bus,
            ESP32_I2C_ADDRESS,
            packet,
            MOTOR_PACKET_SIZE   // actual data
        );

        if (num_bytes_read != MOTOR_PACKET_SIZE){
            return false;
        }


    // new packet successfully received
        return true;
}

/*******rover_i2c_check_for_rx*******
 * Description
        > checks to see if an I2C packet has been received
        > returns true if packet received
        > fills a buffer if a complete I2C transaction has been received
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to uses (i2c0 or i2c1)
        > uint8_t* out_buf: pointer to buffer in which to store data
        > uint16_t* out_len: length of buffer in which to store data
 * Returns
        > bool: true=packet received
*/ /* already have the polling function
bool rover_i2c_check_for_rx(i2c_inst_t* i2c_bus, uint8_t* out_buf, uint16_t* out_len){
    //
}
*/
/******************</Public Functions>*****************/
