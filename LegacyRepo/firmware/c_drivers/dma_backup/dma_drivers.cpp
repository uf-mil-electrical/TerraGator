#define gpiomem "/dev/gpiomem"
#define gen_mem "/dev/mem"
#define GPIOSET0 0x1c/4
#define GPCLR0 0x28/4
#define DMA_OFFSET 0xfe007000


#include <cstdint>
#include <sys/mman.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <cstring>

#include <iostream>
#include <bitset>   // Include this for std::bitset
#include "dma_drivers.h"
//#include "dma_mem.h"
#include <fstream>


/*

Control Blocks (CB) are 8 words (256 bits) in length and must start at a 256-bit aligned address.

*/

/*
uint32_t block_one[] = 
{
0x03E00008, //0 TI - flags  0x03E00000
0x00000000, //1 SOURCE AD This is dynamically allocated, let the compiler handle it, will assign to buff>
0x7e20001c, //2 DEST AD  This is a constant as it is legacy addressed - should be GPIOSET0
0x00000004, //3 TXFR_LEM Only ever want to transfer 32 bits
0x00000000, //4 N/A
0x00000000, //5 NEXTCONBK
0x00000000, //6 reserved
0x00000000 //7 reserved
};*/


/*void dma_handler::output_active_gpio(uint32_t first_set, uint32_t second_set)
{
std::ofstream outfile("gpio_toggle_state");
outfile << first_set;
outfile << "\n";
outfile << second_set;
return;
}

void dma_handler::check_active_gpio(uint32_t* two_buffer)
{
std::ifstream infile("gpio_toggle_state");
infile >> *(two_buffer);
infile >> *(two_buffer + 1);
return;
}
*/

dma_handler::dma_handler(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num,uint32_t pin_number)
{

std::cout << "This is runnning";
pwm_dma(on_block,num_blocks_f,dma_num,pin_number);

}


void dma_handler::print_all_cbs(uint32_t num_control_blocks,uint32_t* cb_start)
{
for (int i = 0; i<num_control_blocks;i++)
{
	for (int j = 0; j<8; j++)
	{
		std::cout << "\n" <<std::bitset<32>(cb_start[i*8+j]) << "\t" << cb_start[i*8+j];
	}
	std::cout << "\n\n";
}
return;
}

void dma_handler::reset_dma(uint32_t* dma_mem, uint32_t dma_num)
{

*(dma_mem+(dma_num*0x100/4)) = (1 << 31);//may need to do masking stuff here

}


void dma_handler::initiate_dma_transfer(uint32_t* dma_mem ,uint32_t first_cb_address, uint32_t dma_num)
{

*(dma_mem+1+(dma_num*0x100/4)) = first_cb_address;
*(dma_mem+(dma_num*0x100/4)) = 0b1;//may need to do masking stuff here

}

void dma_handler::pwm_dma(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num,uint32_t pin_number)//rate should be a number between 50-100 that is % of time on
{

std::cout << "This is running";


if (on_block >= num_blocks_f)
{
	std::cout << "This is not a valid rate number, please provide a value between 0-100; no code exectued" << "\n";
	return;
}

//memory mapping dma memory:
//int dma_num = 0;

char file_name[] = gen_mem;
int flags = 0x101002;
int fd = openat(0,file_name,flags);
std::cout << fd;

uint32_t* dma_mem = reinterpret_cast<uint32_t*>(mmap(0,4096,3,1,fd,DMA_OFFSET));
//end of memory mapping

reset_dma(dma_mem,0);


num_control_blocks = num_blocks_f;


uint32_t all_on = (1<<pin_number);//0b00001001001001001001001001001001;
uint32_t all_off = (1<<pin_number);//0;

//below is virtual mailbox setup to get persistant contigous memory setup
//DMAMemHandle *dma_cbs = dma_malloc(sizeof(block_one)*num_control_blocks);
//DMAMemHandle *source_info = dma_malloc(sizeof(all_on));
//DMAMemHandle *source_info2 = dma_malloc(sizeof(all_off));

this->dma_cbs = dma_malloc(sizeof(block_one)*num_control_blocks);
this->source_info = dma_malloc(sizeof(all_on));
this->source_info2 = dma_malloc(sizeof(all_off));

uint32_t* all_on_pointer = reinterpret_cast<uint32_t*>(source_info->virtual_addr);
*(all_on_pointer) = all_on;

uint32_t* all_off_pointer = reinterpret_cast<uint32_t*>(source_info2->virtual_addr);
*(all_off_pointer) = all_off;

bus_address_on = (uint32_t)source_info->bus_addr;
block_one[1] = bus_address_on;

bus_address_off = (uint32_t)source_info2->bus_addr;
//second block [1] = bus_address_off  --- done in a lower line


//This code can be cleaned up but I do not care right now
uint32_t* point_virt = reinterpret_cast<uint32_t*>(dma_cbs->virtual_addr);
for (int i = 0; i<num_control_blocks; i++)
{
        point_virt[i*8] = block_one[0];
	if(i < on_block)
	{
	point_virt[i*8 + 1] = bus_address_on;
	point_virt[i*8 + 2] = block_one[2];
	}
	else
	{
	std::cout << "GOOD\n\n";
	point_virt[i*8 + 1] = bus_address_off;
	point_virt[i*8 + 2] = 0x7e200028;
	}
	//point_virt[i*8 + 2] = block_one[2];
        point_virt[i*8 + 3] = block_one[3];
	point_virt[i*8 + 4] = block_one[4];
        point_virt[i*8 + 5] = dma_cbs->bus_addr + 8*sizeof(uint32_t)*(i+1); //address of first block + offset of a block*(current block # + 1) -- gives us address of next block
        point_virt[i*8 + 6] = block_one[6];
	point_virt[i*8 + 7] = block_one[7];
}

//point_virt[5] = (dma_cbs->bus_addr) + 8 * sizeof(uint32_t);
//point_virt[1+8] = bus_address_on;
//point_virt[2+8] = 0x7e200028;
//point_virt[5+8] = (dma_cbs->bus_addr);

//point_virt[1] = bus_address_off;
point_virt[2] = 0x7e200028;
point_virt[(num_control_blocks-1)*8 +5] = dma_cbs->bus_addr; //creating the infinite loop.

std::cout << "\n" << "last block address should be " << dma_cbs->bus_addr << "\n";


print_all_cbs(num_control_blocks,point_virt);

initiate_dma_transfer(dma_mem,dma_cbs->bus_addr,0);

std::cout << "\nThe on address is " << bus_address_on;
std::cout << "\nThe off address is " << bus_address_off;

return;

}


void dma_handler::modify_blocks(uint8_t on_block, uint8_t num_blocks_f)
{

uint32_t* point_virt = reinterpret_cast<uint32_t*>(dma_cbs->virtual_addr);
for (int i = 0; i<num_control_blocks-1; i++)
{
        point_virt[i*8] = block_one[0];
        if(i < on_block)
	{
	 point_virt[i*8 + 1] = bus_address_on;
	 point_virt[i*8 + 2] = block_one[2];
	}
	else
	{
	std::cout << "GOOD\n\n";
	point_virt[i*8 + 1] = bus_address_off;
        point_virt[i*8 + 2] = 0x7e200028;
	}
        point_virt[i*8 + 4] = block_one[4];
        point_virt[i*8 + 5] = dma_cbs->bus_addr + 8*sizeof(uint32_t)*(i+1); //address of first block + offset of a bloc>
        point_virt[i*8 + 6] = block_one[6];
        point_virt[i*8 + 7] = block_one[7];
}

}

/*int main()
{
//uint32_t two_buffer[2];
//check_active_gpio(two_buffer);
//std::cout << *two_buffer;
//std::cout << "\n";
//std::cout << *(two_buffer + 1);
//output_active_gpio(100,200);


pwm_dma(125,250,0); // pwm_dma(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num)//rate should be a number between 50-100 that is % of time on


return 0;
}*/
/*int main()
{
//uint32_t two_buffer[2];
//check_active_gpio(two_buffer);
//std::cout << *two_buffer;
//std::cout << "\n";
//std::cout << *(two_buffer + 1);
//output_active_gpio(100,200);


pwm_dma(125,250,0); // pwm_dma(uint8_t on_block,uint8_t num_blocks_f,uint32_t dma_num)//rate should be a number between>


return 0;
}*/
