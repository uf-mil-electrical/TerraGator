#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "board_config.h"

void board_config()
{
    stdio_init_all(); 

    // ---------------- CURRENT SENSING [ADC] --------------- //
    
    adc_init();                 // initialize the ADC HW
    adc_gpio_init(CS_ADC);      // initialize GPIO for ADC usage
    adc_select_input(ADC_CH);   // select the ADC channel to be used

    // ---------------- VOLTAGE SENSING [I2C] --------------- //
    
    i2c_init(VS_I2C_PORT, I2C_BAUDRATE);
    gpio_set_function(VS_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(VS_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(VS_I2C_SDA);
    gpio_pull_up(VS_I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // ---------------- 7SEG DRIVER [SPI] ------------------- //
    
    spi_init(SPI_PORT, SPI_BAUDRATE);
    gpio_set_function(CS_7SEG,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_dir(CS_7SEG, GPIO_OUT); // CS is active low, so pull high when not in use 
    gpio_put(CS_7SEG, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // ------------ RPi5 COMMUNICATION [UART] -------------- //
    
    uart_init(UART_ID, UART_BAUDRATE);
    // Set the TX and RX pins by using the function select on the GPIO
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
}