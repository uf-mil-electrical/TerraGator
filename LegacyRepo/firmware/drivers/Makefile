all:
	g++ -o sensors -lstdc++ ISM330DHCX/ISM330DHCX.cpp BME280/BME280.cpp sensors.cpp -lwiringPi `pkg-config --cflags --libs gtk+-3.0` `pkg-config --cflags --libs opencv4`
