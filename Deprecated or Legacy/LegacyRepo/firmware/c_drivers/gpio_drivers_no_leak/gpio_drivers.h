#ifndef MYCLASS_H
#include <cstdint>
#define MYCLASS_H

class gpio_drivers
{
int fd;
uint32_t* gpio_mem;
public:
gpio_drivers();
void set_output(int selected_pin);
void set_input(int selected_pin);
void set_high(int selected_pin);
void set_low(int selected_pin);
~gpio_drivers();
};
#endif
