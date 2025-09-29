#define gpiomem "/dev/gpiomem"
#define GPIOSET0 0x1c / 4
#define GPCLR0 0x28 / 4

#include <cstdint>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>

#include <iostream>
#include <bitset> // Include this for std::bitset
#include "gpio_drivers.h"

/*void print_32(uint32_t to_print)
{
	std::cout << "\n"
			  << std::bitset<32>(to_print);
}*/

void gpio_drivers::set_output(int selected_pin)
{

	int fsel = selected_pin / 10;
	int offset = selected_pin % 10;
	int desired_set = 1 << (offset * 3);

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0, file_name, flags);

	uint32_t *gpio_mem = reinterpret_cast<uint32_t *>(mmap(0, 4096, 3, 1, fd, 0));
	uint32_t current = *(gpio_mem + fsel);
	uint32_t mask = ~(0b111 << (offset * 3));
	*(gpio_mem + fsel) = (current & mask) | desired_set;
	close(fd);
}

void gpio_drivers::set_input(int selected_pin)
{

	int fsel = selected_pin / 10;
	int offset = selected_pin % 10;
	// int desired_set = 1<<(offset*3);

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0, file_name, flags);

	uint32_t *gpio_mem = reinterpret_cast<uint32_t *>(mmap(0, 4096, 3, 1, fd, 0));
	uint32_t current = *(gpio_mem + fsel);
	uint32_t mask = ~(0b111 << (offset * 3));
	*(gpio_mem + fsel) = (current & mask); //|desired_set;
	close(fd);
	return;
}

void gpio_drivers::set_high(int selected_pin)
{

	int set_clr_sel = selected_pin / 32;
	int set_clr_bit = selected_pin % 32;
	int mem_shift = GPIOSET0 + 4 * set_clr_sel;
	int mask = 0b1 << set_clr_bit;

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0, file_name, flags);

	uint32_t *gpio_mem = reinterpret_cast<uint32_t *>(mmap(0, 4096, 3, 1, fd, 0));
	*(gpio_mem + mem_shift) = mask;
	close(fd);
	return;
}

void gpio_drivers::set_low(int selected_pin)
{

	int set_clr_sel = selected_pin / 32;
	int set_clr_bit = selected_pin % 32;
	int mem_shift = GPCLR0 + set_clr_sel;
	int mask = 0b1 << (set_clr_bit); // because 0 is one

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0, file_name, flags);

	uint32_t *gpio_mem = reinterpret_cast<uint32_t *>(mmap(0, 4096, 3, 1, fd, 0));
	//print_32(mem_shift);
	//print_32(mask);
	//print_32(mem_shift);
	*(gpio_mem + mem_shift) = mask;
	close(fd);
	return;
}

// see page 77 for info on alternative function assignments
//int main()
//{

//	for (int i = 20; i < 28; i++)
//	{
		// set_high(i);
//	}

	// set_output(21);
	// int selected_pin = 21;
	// set_output(21);
	// set_high(21);

	/*for (int i = 20; i<28; i++)
	{
	set_input(i);
	}*/
	//sonar
//	set_output(21);
	//set_high(21);
	//sleep(0.000010);
	//set_low(21);
	//sonar

//	set_output(20);
	// set_output(1);
	// set_output(21);
	// set_output(20);
	// set_output(25);
	// set_output(24);
	// set_output(23);
	// set_output(22);
	// set_output(27);
	// set_output(26);

	/*int set_clr_sel = selected_pin/32;
	int set_clr_bit = selected_pin%32;
	int mem_shift = GPCLR0 + set_clr_sel;
	int mask = 0b1 << (set_clr_bit); //because 0 is one

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0,file_name,flags);

	uint32_t* gpio_mem = reinterpret_cast<uint32_t*>(mmap(0,4096,3,1,fd,0));
	print_32(mem_shift);
	print_32(mask);
	print_32(mem_shift);
	*(gpio_mem+mem_shift) = mask;
	*/

	/*int set_clr_sel = selected_pin/32;
	int set_clr_bit = selected_pin%32;
	int mem_shift = GPIOSET0 + 4*set_clr_sel;
	int mask = 0b1 << set_clr_bit;

	char file_name[] = gpiomem;
	int flags = 0x101002;
	int fd = openat(0,file_name,flags);

	uint32_t* gpio_mem = reinterpret_cast<uint32_t*>(mmap(0,4096,3,1,fd,0));
	(gpio_mem+mem_shift)* = mask;*/

//	return 0;
//}
