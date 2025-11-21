/********************************************************

$$$$$$$$\                                      $$$$$$\             $$\                         
\__$$  __|                                    $$  __$$\            $$ |                        
   $$ | $$$$$$\   $$$$$$\   $$$$$$\  $$$$$$\  $$ /  \__| $$$$$$\ $$$$$$\    $$$$$$\   $$$$$$\  
   $$ |$$  __$$\ $$  __$$\ $$  __$$\ \____$$\ $$ |$$$$\  \____$$\\_$$  _|  $$  __$$\ $$  __$$\ 
   $$ |$$$$$$$$ |$$ |  \__|$$ |  \__|$$$$$$$ |$$ |\_$$ | $$$$$$$ | $$ |    $$ /  $$ |$$ |  \__|
   $$ |$$   ____|$$ |      $$ |     $$  __$$ |$$ |  $$ |$$  __$$ | $$ |$$\ $$ |  $$ |$$ |      
   $$ |\$$$$$$$\ $$ |      $$ |     \$$$$$$$ |\$$$$$$  |\$$$$$$$ | \$$$$  |\$$$$$$  |$$ |      
   \__| \_______|\__|      \__|      \_______| \______/  \_______|  \____/  \______/ \__|                                                  

    Pi Control Board V0
    RP2350A/B (Pico 2)
    Written by Russell MacGregor
********************************************************/                                                                                               
                       


//*****************<Includes>*****************
#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "C:\Users\russe\.pico-sdk\sdk\2.2.0\src\rp2_common\hardware_pwm\include\hardware\pwm.h"
//*****************</Includes>*****************



//*****************<Pin Definitions>*****************
// SPI
#define SPI_PORT spi1           // SPI Port 1
#define PIN_SCK  10             // GPIO18 = SCK
#define PIN_MOSI 11             // GPIO11 = MOSI
#define PIN_MISO 12             // GPIO12 = MISO
#define PIN_CS   13             // GPIO13 = CS

// I2C
#define I2C_PORT i2c1           // I2C Port 1
#define I2C_SDA 2               // GPIO2 = SDA
#define I2C_SCL 3               // GPIO3 = SCL

// UART
#define UART_ID uart1           // UART Port 1
#define BAUD_RATE 115200        // UART Baud rate
#define UART_TX_PIN 4           // GPIO4 = TX
#define UART_RX_PIN 5           // GPIO5 = RX

// PWM
#define MOTOR1_PWM  0           // Pin 0 = PWM for Motor 1
#define MOTOR2_PWM  1           // Pin 1 = PWM for Motor 2
//*****************</Pin Definitions>*****************



int main()
{
    //**************<Peripheral Init>**************
    stdio_init_all();

    // <SPI Init>
    spi_init(SPI_PORT, 1000*1000);                  // initialize SPI at 1MHz
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_dir(PIN_CS, GPIO_OUT);                 // initialize CS to be high (active low)
    gpio_put(PIN_CS, 1);
    // </SPI Init>


    // <I2C Init>
    i2c_init(I2C_PORT, 400*1000);                   // initialize I2C at 400 kHz
    gpio_set_function(I2C_SDA, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA);
    gpio_pull_up(I2C_SCL);
    // </I2C Init>


    // <UART Init>
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_puts(UART_ID, " Hello, UART!\n");          // Print UART debug message
    // </UART Init>


    // <PWM Init>
    gpio_set_function(MOTOR1_PWM, GPIO_FUNC_PWM);                   // Init MOTOR1_PWM pin as PWM

    uint motor1_pwm_slice = 0;
    typedef pwm_chan;
    pwm_chan motor1_pwm_channel;
    pwm_set_gpio_level(motor1_pwm_slice, motor1_pwm_channel);       // Get PWM slice # and channel for MOTOR1_PWM

    pwm_set_wrap(motor1_pwm_slice, 3);                              // set period of 4 cycles (0 to 3 inclusive)
    pwm_set_chan_level(motor1_pwm_slice, motor1_pwm_channel, 1);    // set channel A output high for one cycle before dropping
    pwm_set_enabled(motor1_pwm_slice, true);                        // set PWM running
    // </PWM Init>
    //**************</Peripheral Init>**************
    
    
    // Run main program
    while (true) {
        main_program();
    }
}


void main_program(){
    //
}