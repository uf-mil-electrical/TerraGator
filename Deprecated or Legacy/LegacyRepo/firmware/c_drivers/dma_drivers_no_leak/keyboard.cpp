#include "gpio_drivers.h"
#include "dma_drivers.h"
#include <iostream>
#include <fstream>
#include <string>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>

#define SONAR3 22

#define MOTOR1_EN 5
#define MOTOR2_EN 6
#define MOTOR3_EN 13
#define MOTOR4_EN 19
#define MOTOR5_EN 26
#define MOTOR6_EN 12

#define L_IN1 8
#define L_IN2 7
#define R_IN1 14
#define R_IN2 15

void move_forward(uint8_t power, dma_handler& dma_object)
{
    gpio_drivers::set_high(L_IN1);
    gpio_drivers::set_low(L_IN2);
    gpio_drivers::set_low(R_IN1);
    gpio_drivers::set_high(R_IN2);

    dma_object.modify_blocks(power, 250, MOTOR1_EN);
    dma_object.modify_blocks(power, 250, MOTOR2_EN);
    dma_object.modify_blocks(power, 250, MOTOR3_EN);
    dma_object.modify_blocks(power, 250, MOTOR4_EN);
    dma_object.modify_blocks(power, 250, MOTOR5_EN);
    dma_object.modify_blocks(power, 250, MOTOR6_EN);
}

void move_backward(uint8_t power, dma_handler& dma_object)
{
    gpio_drivers::set_low(L_IN1);
    gpio_drivers::set_high(L_IN2);
    gpio_drivers::set_high(R_IN1);
    gpio_drivers::set_low(R_IN2);

    dma_object.modify_blocks(power, 250, MOTOR1_EN);
    dma_object.modify_blocks(power, 250, MOTOR2_EN);
    dma_object.modify_blocks(power, 250, MOTOR3_EN);
    dma_object.modify_blocks(power, 250, MOTOR4_EN);
    dma_object.modify_blocks(power, 250, MOTOR5_EN);
    dma_object.modify_blocks(power, 250, MOTOR6_EN);
}

void turn_right(uint8_t power, dma_handler& dma_object)
{
    gpio_drivers::set_low(L_IN1);
    gpio_drivers::set_high(L_IN2);
    gpio_drivers::set_high(R_IN1);
    gpio_drivers::set_low(R_IN2);

    dma_object.modify_blocks(power, 250, MOTOR1_EN);
    dma_object.modify_blocks(power, 250, MOTOR2_EN);
    dma_object.modify_blocks(power, 250, MOTOR3_EN);
    dma_object.modify_blocks(power, 250, MOTOR4_EN);
    dma_object.modify_blocks(power, 250, MOTOR5_EN);
    dma_object.modify_blocks(power, 250, MOTOR6_EN);
}

void turn_left(uint8_t power, dma_handler& dma_object)
{
    gpio_drivers::set_high(L_IN1);
    gpio_drivers::set_low(L_IN2);
    gpio_drivers::set_low(R_IN1);
    gpio_drivers::set_high(R_IN2);

    dma_object.modify_blocks(power, 250, MOTOR1_EN);
    dma_object.modify_blocks(power, 250, MOTOR2_EN);
    dma_object.modify_blocks(power, 250, MOTOR3_EN);
    dma_object.modify_blocks(power, 250, MOTOR4_EN);
    dma_object.modify_blocks(power, 250, MOTOR5_EN);
    dma_object.modify_blocks(power, 250, MOTOR6_EN);
}

int main()
{
    // GPIO setup
    gpio_drivers::set_output(MOTOR1_EN);
    gpio_drivers::set_output(MOTOR2_EN);
    gpio_drivers::set_output(MOTOR3_EN);
    gpio_drivers::set_output(MOTOR4_EN);
    gpio_drivers::set_output(MOTOR5_EN);
    gpio_drivers::set_output(MOTOR6_EN);

    gpio_drivers::set_output(L_IN1);
    gpio_drivers::set_output(L_IN2);
    gpio_drivers::set_output(R_IN1);
    gpio_drivers::set_output(R_IN2);

    dma_handler dma_object = dma_handler(125, 250, 0, 26);

    // Controller input setup
    const char* device_path = "/dev/input/event4";
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open input device: " << device_path << std::endl;
        return 1;
    }

    struct input_event ev;
    while (true) {
        ssize_t bytes = read(fd, &ev, sizeof(ev));
        if (bytes < (ssize_t)sizeof(ev)) {
            std::cerr << "Error reading input event" << std::endl;
            break;
        }

        if (ev.type == EV_KEY) {
            if (ev.value == 1) { // Key pressed
                switch (ev.code) {
                    case KEY_W: // Forward
                        move_forward(175, dma_object);
                        break;
                    case KEY_S: // Backward
                        move_backward(175, dma_object);
                        break;
                    case KEY_A: // Left
                        turn_left(175, dma_object);
                        break;
                    case KEY_D: // Right
                        turn_right(175, dma_object);
                        break;
                }
            } else if (ev.value == 0) { // Key released
                dma_object.turn_off();
            }
        }
    }

    close(fd);
    return 0;
}
