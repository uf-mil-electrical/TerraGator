#include "BME280.h"
#include <wiringPi.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(){

    BME280 bme = BME280();

    if(!bme.init()){
        printf("BME280 init failed\n");
        return 1;
    }

    while(1){
        int32_t T = bme.get_temp();
        printf("Temperature is: %d.%d C\n", T/100, T - (T/100)*100);
        printf("Humidity: %3f %RH\n", bme.get_hum());
        printf("Pressure: %2f Pa \n", bme.get_press());
        delay(1000);
    }

    return 0;
}