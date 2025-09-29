#include <unistd.h>
#include <sys/mman.h>
#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>

#define GPFSEL2_OFFSET 2 //the below values are found on page 69 of BCM2711 ARM Peripherals
#define GPIO_OUTPUT 1

#define LEGACY_ADDRESS_BASE 0x7C00_0000
#define GPIO_LEGACY_BASE 0x7e20_0000
#define DMA_LEGACE_BASE 0x7e00_7000
#define PHYSICAL_ADDRESS_BASE  0x4_0000_0000//47c really but for math

int main()
{
	int fd = open("/dev/gpiomem",O_RDWR);
	if(fd == -1)
	{
	perror("open");
	}

	int fd_2 = open("/dev/mem",O_RDWR);
	if(fd_2 == -1)
	{
	perror("open");
	}

	uint32_t * gpio_pointer = mmap(NULL,4096,PROT_READ|PROT_WRITE,MAP_SHARED,fd,0);
	if(gpio_pointer == MAP_FAILED)
	{
	printf("Bad gpio mapping");
	}



	*(gpio_pointer+2) = GPIO_OUTPUT << 3;





	close(fd);
	return 0;
}
