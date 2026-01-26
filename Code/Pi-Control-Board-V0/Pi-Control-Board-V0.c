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
                       


//*****************<Includes>***************** */
#include <stdio.h>
#include "pico/stdlib.h"

#include "pico/time.h"
#include "hardware/irq.h"

#include "hardware/spi.h"
#include "hardware/uart.h"
#include "hardware/pwm.h"

#include "system_general.h"

#include "peripherals/ESP32.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/rover_i2c.h"

#include "control/RemoteXY_control.h"
#include "control/rover_cli.h"
//*****************</Includes>*****************


/*
//*****************<Pin Definitions>*****************
// SPI
#define SPI_PORT spi1           // SPI Port 1
#define PIN_SCK  10             // GPIO18 = SCK
#define PIN_MOSI 11             // GPIO11 = MOSI
#define PIN_MISO 12             // GPIO12 = MISO
#define PIN_CS   13             // GPIO13 = CS

// UART
#define UART_ID uart1           // UART Port 1
#define BAUD_RATE 115200        // UART Baud rate
#define UART_TX_PIN 4           // GPIO4 = TX
#define UART_RX_PIN 5           // GPIO5 = RX
//*****************</Pin Definitions>*****************
*/



int main()
{
    //**************<Peripheral Init>**************
    stdio_init_all();

    /*

    // <SPI Init>
    spi_init(SPI_PORT, 1000*1000);                  // initialize SPI at 1MHz
    gpio_set_function(PIN_MISO, GPIO_FUNC_SPI);
    gpio_set_function(PIN_CS,   GPIO_FUNC_SIO);
    gpio_set_function(PIN_SCK,  GPIO_FUNC_SPI);
    gpio_set_function(PIN_MOSI, GPIO_FUNC_SPI);
    gpio_set_dir(PIN_CS, GPIO_OUT);                 // initialize CS to be high (active low)
    gpio_put(PIN_CS, 1);
    // </SPI Init>


    // <UART Init>
    uart_init(UART_ID, BAUD_RATE);
    gpio_set_function(UART_TX_PIN, GPIO_FUNC_UART);
    gpio_set_function(UART_RX_PIN, GPIO_FUNC_UART);
    uart_puts(UART_ID, " Hello, UART!\n");          // Print UART debug message
    // </UART Init>

    */

    //**************</Peripheral Init>**************
    


    //**************</Motor Init>**************
    sleep_ms(5000);
    printf("\n\n\n");
    printf("> Initializing rover...\n");
    motor_init();
    init_rover_i2c();
    initRelay();
    //**************</Motor Init>**************

    
    // Run main program
    while (true) {
        //runMenu();
        runRover_RemoteXYControl();
        sleep_ms(50);        
    }

}