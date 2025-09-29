#include "dma_drivers.h"
//#include <ctime>
#include <unistd.h>
#include "gpio_drivers.h"
#include <iostream>
#include <fstream>
#include <string>

#define SONAR3 22

#define MOTOR1_EN 5
#define MOTOR2_EN 6
#define MOTOR3_EN 13
#define MOTOR4_EN 19
#define MOTOR5_EN 26
#define MOTOR6_EN 12

#define L_IN1	8
#define L_IN2	7
#define R_IN1	14
#define R_IN2	15

void move_forward(uint8_t power,dma_handler& dma_object)
{

gpio_drivers::set_high(L_IN1);
gpio_drivers::set_low(L_IN2);
gpio_drivers::set_low(R_IN1);
gpio_drivers::set_high(R_IN2);


dma_object.modify_blocks(power,250,MOTOR1_EN);
dma_object.modify_blocks(power,250,MOTOR2_EN);
dma_object.modify_blocks(power,250,MOTOR3_EN);
dma_object.modify_blocks(power,250,MOTOR4_EN);
dma_object.modify_blocks(power,250,MOTOR5_EN);
dma_object.modify_blocks(power,250,MOTOR6_EN);

}

void move_backward(uint8_t power, dma_handler& dma_object)
{
gpio_drivers::set_low(L_IN1);
gpio_drivers::set_high(L_IN2);
gpio_drivers::set_high(R_IN1);
gpio_drivers::set_low(R_IN2);

dma_object.modify_blocks(power,250,MOTOR2_EN);
dma_object.modify_blocks(power,250,MOTOR3_EN);
dma_object.modify_blocks(power,250,MOTOR4_EN);
dma_object.modify_blocks(power,250,MOTOR5_EN);
dma_object.modify_blocks(power,250,MOTOR6_EN);

}

void turn_right(uint8_t power, dma_handler& dma_object)
{
gpio_drivers::set_low(L_IN1);
gpio_drivers::set_high(L_IN2);
gpio_drivers::set_high(R_IN1);
gpio_drivers::set_low(R_IN2);

dma_object.modify_blocks(power,250,MOTOR2_EN);
dma_object.modify_blocks(power,250,MOTOR3_EN);
dma_object.modify_blocks(power,250,MOTOR4_EN);
dma_object.modify_blocks(power,250,MOTOR5_EN);
dma_object.modify_blocks(power,250,MOTOR6_EN);

}

void turn_left(uint8_t power, dma_handler& dma_object)
{
gpio_drivers::set_low(L_IN1);
gpio_drivers::set_high(L_IN2);
gpio_drivers::set_high(R_IN1);
gpio_drivers::set_low(R_IN2);

dma_object.modify_blocks(power, 250, MOTOR2_EN);
dma_object.modify_blocks(power, 250, MOTOR3_EN);
dma_object.modify_blocks(power, 250, MOTOR4_EN);
dma_object.modify_blocks(power, 250, MOTOR5_EN);
dma_object.modify_blocks(power, 250, MOTOR6_EN);
}

void print_all_thing(dma_handler& dma_object)
{
	std::cout << "\n";

	for (int i=0;i<20;i++)
	{
		std::cout << " " << dma_object.reserved[i].reserved_me;
	}

}

int main()
{
//uint32_t two_buffer[2];
//check_active_gpio(two_buffer);
//std::cout << *two_buffer;
//std::cout << "\n";
//std::cout << *(two_buffer + 1);
//output_active_gpio(100,200);


//pwm_dma(125,250,0); // pwm_dma(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num)//rate should be a number between>
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

gpio_drivers::set_output(SONAR3);

dma_handler dma_object = dma_handler(125,250,0,26);

/*
while(1)
{
print_all_thing(dma_object);
move_forward(175,dma_object);
sleep(1);
print_all_thing(dma_object);
dma_object.turn_off();
sleep(3);
print_all_thing(dma_object);
move_backward(175,dma_object);
sleep(1);
print_all_thing(dma_object);
dma_object.turn_off();
sleep(3);
}
*/

std::ifstream inputFile("/home/raspberry/Mars-Rover/firmware/sonar3.txt");
if (!inputFile) {
	std::cerr << "Unable to open file example.txt";
	return 1; // Exit with error code
}

std::string line;
int distance;
do{
	//trigger PWM
	gpio_drivers::set_high(SONAR3);
	sleep(0.00001);
	gpio_drivers::set_low(SONAR3);

	//get interrupt info (Distance)
	std::getline(inputFile, line);
	int distance = std::atoi(line.c_str());
	
	move_forward(175, dma_object);
	//sleep(1);

	
}while(distance>15); 

dma_object.turn_off();


return 0;
}
