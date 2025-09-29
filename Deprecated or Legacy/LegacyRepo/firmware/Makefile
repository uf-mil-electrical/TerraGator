all:
	g++ -o test_sensors -lstdc++ BME280/BME280.cpp ISM330DHCX/ISM330DHCX.cpp sensors.cpp -lwiringPi
kernel:
	sudo insmod sonar_1/sonar_1.ko
	sudo insmod sonar_2/sonar_2.ko
	sudo insmod sonar_3/sonar_3.ko
