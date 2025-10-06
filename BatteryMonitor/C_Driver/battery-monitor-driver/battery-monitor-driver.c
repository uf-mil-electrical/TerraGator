#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// ---------------- PIN MAP ---------------------- / 

// questions for Russell
// (1) How should I program the level-shifter, I see that /CS is on GPIO 5?
// (2) Is the system flash memory automatically configured? 

// 7SEG SPI Communication defines with SPI0 
#define SPI_PORT spi0
#define PIN_CS   5
#define PIN_SCK  2
#define PIN_MOSI 3
#define ChS_7SEG  5

// I2C defines for the voltage sensing (VS) IC
#define VS_I2C_PORT i2c0
#define VS_I2C_SDA 10
#define VS_I2C_SCL 11
#define VS_ALERT   9  

// UART for communication with RPi5
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define BAUD_RATE 115200
#define UART_TX_PIN 16
#define UART_RX_PIN 17

// ADC1 for current sensing IC
#define CS_ADC 27
#define ADC_CH 1

// LEDs connected for outputs 
#define RED_LED 21
#define YELLOW_LED 20
#define GREEN_LED 19

int main()
{
    stdio_init_all(); 

    // -------------------- CURRENT SENSING ------------------- //
    // ADC function
    // initialize the ADC HW
    adc_init(); 
    // initialize GPIO for ADC usage
    adc_gpio_init(CS_ADC); 
    // select the ADC channel to be used
    adc_select_input(ADC_CH); 

    // ADC_d = Vin * (2^12- 1) / VREF; => Vin = ADC_d * VREF / (1 << 12)
    const float DtoA_conv_factor = 3.3f / (1 << 12); 
    uint16_t result = adc_read(); 
    float cs_voltage = result * DtoA_conv_factor; 

    // The TMCS1100A3 has a sensitivity of 200mV / A
    const float CS_sens = .2; // V/A
    float cs_current = cs_voltage / CS_sens; 
    // -------------------------------------------------------- //

    // SPI initialisation. This example will use SPI at 1MHz.
    spi_init(SPI_PORT, 1000*1000);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    
    // Chip select is active-low, so we'll initialise it to a driven-high state
    gpio_set_dir(PIN_CS, GPIO_OUT);
    gpio_put(PIN_CS, 1);
    // For more examples of SPI use see https://github.com/raspberrypi/pico-examples/tree/master/spi

    // I2C Initialisation. Using it at 400Khz.
    i2c_init(VS_I2C_PORT, 400*1000);
    
    gpio_set_function(VS_I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(VS_I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(VS_I2C_SDA);
    gpio_pull_up(VS_I2C_SCL);
    // For more examples of I2C use see https://github.com/raspberrypi/pico-examples/tree/master/i2c

    // Set up our UART
    uart_init(UART_ID, BAUD_RATE);
    // Set the TX and RX pins by using the function select on the GPIO
    // Set datasheet for more information on function select
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    
    // Use some the various UART functions to send out data
    // In a default system, printf will also output via the default UART
    
    // Send out a string, with CR/LF conversions
    uart_puts(UART_ID, " Hello, UART!\n");
    
    // For more examples of UART use see https://github.com/raspberrypi/pico-examples/tree/master/uart

    while (true) {
        printf("Hello, world!\n");
        sleep_ms(1000);
    }
}
