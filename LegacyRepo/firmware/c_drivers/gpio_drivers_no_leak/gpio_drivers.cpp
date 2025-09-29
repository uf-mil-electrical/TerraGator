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


gpio_drivers::gpio_drivers()
{

char file_name[] = gpiomem;
int flags = 0x101002;
fd = openat(0, file_name, flags);
gpio_mem = reinterpret_cast<uint32_t *>(mmap(0, 4096, 3, 1, fd, 0));

}

void gpio_drivers::set_output(int selected_pin)
{

	int fsel = selected_pin / 10;
	int offset = selected_pin % 10;
	int desired_set = 1 << (offset * 3);

	uint32_t current = *(gpio_mem + fsel);
	uint32_t mask = ~(0b111 << (offset * 3));
	*(gpio_mem + fsel) = (current & mask) | desired_set;
}

void gpio_drivers::set_input(int selected_pin)
{

	int fsel = selected_pin / 10;
	int offset = selected_pin % 10;
	// int desired_set = 1<<(offset*3);

	uint32_t current = *(gpio_mem + fsel);
	uint32_t mask = ~(0b111 << (offset * 3));
	*(gpio_mem + fsel) = (current & mask); //|desired_set;
	return;
}

void gpio_drivers::set_high(int selected_pin)
{

	int set_clr_sel = selected_pin / 32;
	int set_clr_bit = selected_pin % 32;
	int mem_shift = GPIOSET0 + 4 * set_clr_sel;
	int mask = 0b1 << set_clr_bit;


	*(gpio_mem + mem_shift) = mask;
	return;
}

void gpio_drivers::set_low(int selected_pin)
{

	int set_clr_sel = selected_pin / 32;
	int set_clr_bit = selected_pin % 32;
	int mem_shift = GPCLR0 + set_clr_sel;
	int mask = 0b1 << (set_clr_bit); // because 0 is one


	//print_32(mem_shift);
	//print_32(mask);
	//print_32(mem_shift);
	*(gpio_mem + mem_shift) = mask;
	return;
}

gpio_drivers::~gpio_drivers()
{
close(fd);
munmap(gpio_mem, 4096);
}

