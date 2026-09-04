
/********************************************************

$$$$$$$$\                                      $$$$$$\             $$\                         
\__$$  __|                                    $$  __$$\            $$ |                        
   $$ | $$$$$$\   $$$$$$\   $$$$$$\  $$$$$$\  $$ /  \__| $$$$$$\ $$$$$$\    $$$$$$\   $$$$$$\  
   $$ |$$  __$$\ $$  __$$\ $$  __$$\ \____$$\ $$ |$$$$\  \____$$\\_$$  _|  $$  __$$\ $$  __$$\ 
   $$ |$$$$$$$$ |$$ |  \__|$$ |  \__|$$$$$$$ |$$ |\_$$ | $$$$$$$ | $$ |    $$ /  $$ |$$ |  \__|
   $$ |$$   ____|$$ |      $$ |     $$  __$$ |$$ |  $$ |$$  __$$ | $$ |$$\ $$ |  $$ |$$ |      
   $$ |\$$$$$$$\ $$ |      $$ |     \$$$$$$$ |\$$$$$$  |\$$$$$$$ | \$$$$  |\$$$$$$  |$$ |      
   \__| \_______|\__|      \__|      \_______| \______/  \_______|  \____/  \______/ \__|                                                  

    RP2350 Control Board V2
    RP2350B
    Written by Russell MacGregor
********************************************************/                                                                                               
                       


//*****************<Includes>***************** */
#include <stdio.h>
#include "pico/stdlib.h"

#include "communication/rover_i2c.h"
#include "control/rover_cli.h"

#include "communication/data_parser.h"
//*****************</Includes>*****************



int main()
{
    // peripheral init
        stdio_init_all();

        init_relay();
        init_leds();

        sleep_ms(50);

        init_motors();

        init_rover_i2c(i2c1, I2C1_SDA, I2C1_SCL, I2C1_BAUDRATE, false);

        printf("\n\n\n");
        printf("> Initializing rover...\n");

        sleep_ms(2000);

    
    // Run main program
        while (true) {
            rover_i2c_parse_data(i2c1);

            sleep_ms(500);
        }

}