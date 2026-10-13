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
#include "peripherals/motor_speed_control.h"
#include "peripherals/safety_devices.h"
//*****************</Includes>*****************

// Temporary startup test: set to 0 to disable before normal operation.
// Runs on every boot. RELAY_CONTROL must match the actual board wiring.
#define RUN_FORWARD_STARTUP_TEST 1
#define FORWARD_TEST_SPEED_PERCENT 20
#define FORWARD_TEST_DURATION_MS 5000


int main()
{
    // peripheral init
        stdio_init_all();

        init_relay();
        init_leds();

        sleep_ms(50);

        init_motors();
        rover_stop_motors();

        // change the rp5 into master                   master  = true
        init_rover_i2c(i2c1, I2C1_SDA, I2C1_SCL, I2C1_BAUDRATE, true);

        printf("\n\n\n");
        printf("> Initializing rover...\n");

        sleep_ms(2000);

#if RUN_FORWARD_STARTUP_TEST
        // Keep the wheels raised: this test runs without a phone connection.
        printf("> Forward test: all motors at %u%% PWM for %u ms\n",
               (unsigned int)FORWARD_TEST_SPEED_PERCENT,
               (unsigned int)FORWARD_TEST_DURATION_MS);
        setMotorSpeed_all(0);
        setMotorMode('A', 'F');
        enable_relay();
        setMotorSpeed_all(FORWARD_TEST_SPEED_PERCENT);
        printMotorMovement();
        sleep_ms(FORWARD_TEST_DURATION_MS);
        rover_stop_motors();
        printf("> Forward test complete: motors stopped, relay off\n");
        printf("> Starting RemoteXY command polling\n");
#endif

        // buffer for packet received from ESP32
        uint8_t esp32_packet[MOTOR_PACKET_SIZE];
        uint64_t last_command_us = time_us_64();
        bool command_active = false;
        uint32_t valid_packets = 0;
        uint32_t rejected_packets = 0;
        uint64_t last_report_us = time_us_64();
        printf("> I2C master: ESP32=0x%02X SDA=GP%u SCL=GP%u; waiting for 9-byte motor packets\n",
               ESP32_I2C_ADDRESS, I2C1_SDA, I2C1_SCL);
    // Run main program
        while (true) {

            // poll ESP32 to check if new RemoteXY data exists
            // if new data exists, rover_i2c_poll_esp32 gets the motor packet
            if (rover_i2c_poll_esp32(i2c1, esp32_packet)) {

                // parse the packet received from ESP32
                if (rover_i2c_parse_data(
                    esp32_packet,
                    MOTOR_PACKET_SIZE
                )) {
                    last_command_us = time_us_64();
                    command_active = true;
                    valid_packets++;
                } else {
                    rejected_packets++;
                    rover_stop_motors();
                    command_active = false;
                }
            }

            if (command_active && time_us_64() - last_command_us > 500000) {
                rover_stop_motors();
                command_active = false;
                printf("> Command timeout: no valid motor packet for 500 ms; stopped\n");
            }

            if (time_us_64() - last_report_us >= 1000000) {
                last_report_us = time_us_64();
                printf("RemoteXY link: valid=%lu rejected=%lu command_fresh=%u relay=%u\n",
                       (unsigned long)valid_packets, (unsigned long)rejected_packets,
                       command_active, get_relay_state());
                printMotorMovement();
            }

            // small delay before checking devices again
            sleep_ms(10);
        }

}
