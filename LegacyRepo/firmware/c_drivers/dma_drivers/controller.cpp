#include "gpio_drivers.h"
#include "dma_drivers.h"
#include <iostream>
#include <fstream>
#include <string>
#include <linux/input.h>
#include <fcntl.h>
#include <unistd.h>
using namespace std;





int main()
{
    // Controller input setup
	// ls /dev/input/event*
    const char* device_path = "/dev/input/event10"; // Adjust path as necessary for your Xbox controller
    int fd = open(device_path, O_RDONLY);
    if (fd == -1) {
        std::cerr << "Failed to open input device: " << device_path << std::endl;
        return 1;
    }

    struct input_event ev;
    uint8_t power = 100; // Default power level for motors
    while (true) {
        ssize_t bytes = read(fd, &ev, sizeof(ev));
        if (bytes < (ssize_t)sizeof(ev)) {
            std::cerr << "Error reading input event" << std::endl;
            break;
        }

        if (ev.type == EV_KEY) {
            if (ev.value == 1) { // Key pressed
                switch (ev.code) {
                    case BTN_A: // A button: Move forward
                        //move_forward(power, dma_object);
                        cout << "Button A pressed: Moving forward" << endl;
                        break;
                    case BTN_B: // B button: Move backward
                        //move_backward(power, dma_object);
                        cout << "Button B pressed: Moving backward" << endl;
                        break;
                    case BTN_X: // X button: Turn left
                        //turn_left(power, dma_object);
                        cout << "Button X pressed: Moving left" << endl;
                        break;
                    case BTN_Y: // Y button: Turn right
                        //turn_right(power, dma_object);
                        cout << "Button Y pressed: Moving right" << endl;
                        break;
                }
            } else if (ev.value == 0) { // Key released
                //dma_object.turn_off();
            }
        } else if (ev.type == EV_ABS) {
            // Handle joystick movement for turning
            if (ev.code == ABS_X) {
                if (ev.value < 128) {
                    // Joystick moved left
                    //turn_left(power, dma_object);
                    cout << "Joystick moved left: Moving left" << endl;
                } else if (ev.value > 128) {
                    // Joystick moved right
                    //turn_right(power, dma_object);
                    cout << "Joystick moved right: Moving right" << endl;
                } else {
                    // Joystick centered
                    //dma_object.turn_off();
                }
            }
        }
    }

    close(fd);
    return 0;
}
