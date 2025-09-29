#ifndef MYCLASS_H
#define MYCLASS_H

class gpio_drivers
{
public:
static void set_output(int selected_pin);
static void set_input(int selected_pin);
static void set_high(int selected_pin);
static void set_low(int selected_pin);
};
#endif
