/******************<Dependencies>*****************/
#include "peripherals/rover_i2c.h"
/******************</Dependencies>*****************/


/******************<Public Functions>*****************/

/*******init_rover_i2c*******
 * Description
        > initializes I2C bus for communicating with other devices
 * Arguments
        > N/A
 * Returns
        > N/A
*/
void init_rover_i2c(void){
    // First, initialialize I2C
        i2c_init(I2C_PORT, I2C_BAUDRATE);
        gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
        gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
        gpio_pull_up(I2C_SDA);
        gpio_pull_up(I2C_SCL);

    // Second, set I2C to slave mode
        i2c_set_slave_mode(I2C_PORT, true, RP2350_I2C_ADDRESS);     //true = slave mode, false = master mode

    // Lastly, return to main program
        return;
}


/*******i2c_read_remoteXY*******
 * Description
        > read data from ESP32 which outputs data from RemoteXY app
 * Arguments
        > num_bytes: number of bytes to read from ESP32
 * Returns
        > N/A
*/
void i2c_read_remoteXY(uint8_t* data, uint8_t num_bytes){
    int num_bytes_read = 0;

    // First, read data from I2C buffer
        num_bytes_read = i2c_read_timeout_us(
            I2C_PORT,
            ESP32_I2C_ADDRESS,
            data,
            num_bytes,
            false,                  // nostop = false (If true, master retains control of the bus at the end of the transfer (no Stop is issued), and the next transfer will begin with a Restart rather than a Start)
            I2C_TIMEOUT_DELAY);

    // Second, print debug message
        if (num_bytes_read < 0){
            printf("i2c_read_remoteXY(): I2C error (%d)\n", num_bytes_read);
        }
        else if (num_bytes_read != num_bytes){
            printf("i2c_read_remoteXY(): expected %u bytes, received %d\n", num_bytes, num_bytes_read);
        }
        

    // Third, return to main program
        return;
}



/******************</Public Functions>*****************/
