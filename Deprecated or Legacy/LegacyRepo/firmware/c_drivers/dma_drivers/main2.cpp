#include "dma_drivers.h"
//#include <ctime>
#include <unistd.h>
#include "gpio_drivers.h"
#include <iostream>

#define MOTOR1_EN 5
#define MOTOR2_EN 6
#define MOTOR3_EN 13
#define MOTOR4_EN 19
#define MOTOR5_EN 26
#define MOTOR6_EN 12

#define L_IN1   8
#define L_IN2   7
#define R_IN1   14
#define R_IN2   15

void move_forward(uint8_t power,dma_handler& dma_object)
{

gpio_drivers::set_high(L_IN1);
gpio_drivers::set_low(L_IN2);
gpio_drivers::set_high(R_IN1);
gpio_drivers::set_low(R_IN2);



dma_object.modify_blocks(power,250,MOTOR1_EN);
dma_object.modify_blocks(power,250,MOTOR2_EN);
dma_object.modify_blocks(power,250,MOTOR3_EN);
dma_object.modify_blocks(power,250,MOTOR4_EN);
dma_object.modify_blocks(power,250,MOTOR5_EN);
dma_object.modify_blocks(power,250,MOTOR6_EN);

}

void move_backward(uint8_t power,dma_handler& dma_object)
{
gpio_drivers::set_low(L_IN1);
gpio_drivers::set_high(L_IN2);
gpio_drivers::set_low(R_IN1);
gpio_drivers::set_high(R_IN2);

dma_object.modify_blocks(power,250,MOTOR1_EN);
dma_object.modify_blocks(power,250,MOTOR2_EN);
dma_object.modify_blocks(power,250,MOTOR3_EN);
dma_object.modify_blocks(power,250,MOTOR4_EN);
dma_object.modify_blocks(power,250,MOTOR5_EN);
dma_object.modify_blocks(power,250,MOTOR6_EN);

}

void print_all_thing(dma_handler& dma_object)
{
        /*std::cout << "\n";

        for (int i=0;i<20;i++)
        {
                std::cout << " reserved " << dma_object.reserved[i].reserved_me;
                std::cout << " pin: " << dma_object.reserved[i].pin;
                std::cout << " start " << dma_object.reserved[i].start;
                std::cout << " end " << dma_object.reserved[i].end;
        }
        return;
        */

}

int main()
{

gpio_drivers::set_output(MOTOR1_EN);
gpio_drivers::set_output(MOTOR2_EN);
gpio_drivers::set_output(MOTOR3_EN);
gpio_drivers::set_output(MOTOR4_EN);
gpio_drivers::set_output(MOTOR5_EN);
gpio_drivers::set_output(MOTOR6_EN);

dma_handler dma_object = dma_handler(125,250,0,26);

while(1)
{
//std::cout << "\n after turn off start moving forward\n";
print_all_thing(dma_object);
move_forward(50,dma_object);
sleep(1);
//std::cout << "\n after moving forward\n";
print_all_thing(dma_object);
dma_object.turn_off();
sleep(3);
//std::cout << "\nafter turn all off\n";
print_all_thing(dma_object);
move_backward(50,dma_object);
sleep(1);
//std::cout << "\n after move backward\n";
print_all_thing(dma_object);
dma_object.turn_off();
sleep(3);
}


return 0;
}