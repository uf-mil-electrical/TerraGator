#include "BME280.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(){

    BME280 bme = BME280();
    bme.begin(0x77);



    while(1){
        printf("Temperature: " +  bme.temperature());
        printf("Humidity: " + bme.humidity());
        printf("Pressure: %f hPa \n", bme.pressure()/100.0);
        delay(1000);
    }

    return 0;
}