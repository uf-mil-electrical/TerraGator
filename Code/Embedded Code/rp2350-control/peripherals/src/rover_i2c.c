/******************<Dependencies>*****************/
#include "peripherals/rover_i2c.h"
/******************</Dependencies>*****************/


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

    // initialialize I2C in master mode
        i2c_init(i2c_bus, baudrate);
        gpio_set_function(sda_pin, GPIO_FUNC_I2C);
        gpio_set_function(scl_pin, GPIO_FUNC_I2C);
        gpio_pull_up(sda_pin);
        gpio_pull_up(scl_pin);

    // Lastly, return to main program
        return;
}


/*******rover_read_esp32*******
 * Description
        > reads data from an I2C bus
 * Arguments
        > i2c_inst_t* i2c_bus: I2C bus to use
        > uint8_t read_address: address from which to read
        > uint8_t* data: pointer to location to store received data
        > uint8_t num_bytes: number of bytes to read
 * Returns
        > N/A
*/
void rover_i2c_read(i2c_inst_t* i2c_bus, uint8_t read_address, uint8_t* data, uint8_t num_bytes){
    int num_bytes_read = 0;

    // First, read data from I2C buffer
        num_bytes_read = i2c_read_timeout_us(
            i2c_bus,
            read_address,
            data,
            num_bytes,
            false,                  // nostop = false (If true, master retains control of the bus at the end of the transfer (no Stop is issued), and the next transfer will begin with a Restart rather than a Start)
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
        return;
}


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
void rover_i2c_write(i2c_inst_t* i2c_bus, uint8_t write_address, uint8_t* data, uint8_t num_bytes){

    // First, write data to I2C bus
        i2c_write_blocking(i2c1, ESP32_I2C_ADDRESS, data, num_bytes, false);

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
        printf("> i2c_write_esp32(): wrote %u bytes to I2c bus\n", num_bytes);

    // Lastly, return to main program
        return;
}


/******************</Public Functions>*****************/
