all:
	echo "This file needs sudo permissions and is auto-run by this makefile"
	c++ -g main.cpp dma_drivers.cpp mailbox.c dma_mem.cpp -I/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers -L/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers -lgpio_drivers -o dma_test2
controller: controller.cpp dma_drivers.cpp
	echo "This file needs sudo permissions and is auto-run by this makefile"
	c++ -g controller.cpp dma_drivers.cpp mailbox.c dma_mem.cpp -I/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers -L/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers -lgpio_drivers -o controller
unit_test: unit_test.cpp dma_drivers.cpp
	c++ -g unit_test.cpp dma_drivers.cpp mailbox.c dma_mem.cpp -I/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers_no_leak -L/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers_no_leak -lgpio_drivers -o unit_test
both: main_both.cpp dma_drivers.cpp
	c++ -g main_both.cpp dma_drivers.cpp mailbox.c dma_mem.cpp -I/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers_no_leak -L/home/raspberry/Mars-Rover/firmware/c_drivers/gpio_drivers_no_leak -lgpio_drivers -o both
