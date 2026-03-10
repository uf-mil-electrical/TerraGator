#include <stdio.h>
#include <stdint.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"
#include "hardware/timer.h"
#include "driver/Adafruit_VL53L1X.h" // Updated header

#define TRIG_PIN 0
#define ECHO_PIN 1
#define I2C_SDA_PIN 28
#define I2C_SCL_PIN 29
#define x_shut_pin 25 

#define TOF_THRESHOLD_MM 150
#define ULTRA_THRESHOLD_CM 15.0f

#define I2C_PORT i2c0

// Instantiate the Adafruit VL53L1X object with the shutdown pin
Adafruit_VL53L1X tofSensor(x_shut_pin, -1);

float read_ultrasonic_cm() {
    gpio_put(TRIG_PIN, 1);
    sleep_us(10);
    gpio_put(TRIG_PIN, 0);

    uint32_t timeout = 26100;
    uint32_t wait_start = time_us_32();
    while (gpio_get(ECHO_PIN) == 0) {
        if (time_us_32() - wait_start > timeout) return -1.0f;
    }

    uint32_t start_time = time_us_32();
    while (gpio_get(ECHO_PIN) == 1) {
        if (time_us_32() - start_time > timeout) return -1.0f;
    }
    uint32_t end_time = time_us_32();

    uint32_t duration = end_time - start_time;
    return (float)duration * 0.0343f / 2.0f;
}

int main() {
    stdio_init_all();
    sleep_ms(3000); 
    
    printf("\n--- Starting Dual Sensor Setup ---\n");

    // Initialize Ultrasonic Pins
    gpio_init(TRIG_PIN);
    gpio_set_dir(TRIG_PIN, GPIO_OUT);
    gpio_put(TRIG_PIN, 0); 
    gpio_init(ECHO_PIN);
    gpio_set_dir(ECHO_PIN, GPIO_IN);

    // Initialize I2C hardware (Pico SDK)
    i2c_init(I2C_PORT, 400 * 1000);
    gpio_set_function(I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(I2C_SDA_PIN);
    gpio_pull_up(I2C_SCL_PIN);

    printf("wait for tofSensor init \n");
    
    // Initialize the ToF sensor (Removed Arduino Wire parameter)
    if (!tofSensor.begin(0x29)) {
        printf("ERROR: VL53L1X failed to boot! Check wiring.\n");
        while(1) sleep_ms(100); 
    }
    
    // Start ranging mode
    if (!tofSensor.startRanging()) {
        printf("ERROR: VL53L1X failed to start ranging!\n");
        while(1) sleep_ms(100);
    }
    
    printf("VL53L1X successfully booted.\n");
    printf("Sensors ready. Monitoring...\n");

    while (true) {
        float ultrasonic = read_ultrasonic_cm();
        int16_t tof = -1;

        // Check if new measurement data is available
        if (tofSensor.dataReady()) {
            tof = tofSensor.distance();
            tofSensor.clearInterrupt(); // Clear the interrupt to allow the next reading
        }

        printf("ToF: %d mm | Ultrasonic: %.2f cm\n", tof, ultrasonic);

        bool too_close = false;

        // The Adafruit library returns -1 on an error or invalid range
        if (tof != -1 && tof < TOF_THRESHOLD_MM && tof > 0) {
            too_close = true;
        }
        if (ultrasonic > 0 && ultrasonic < ULTRA_THRESHOLD_CM) {
            too_close = true;
        }

        if (too_close) {
            printf("WARNING: TOO CLOSE!\n");
        }

        sleep_ms(100);
    }
}