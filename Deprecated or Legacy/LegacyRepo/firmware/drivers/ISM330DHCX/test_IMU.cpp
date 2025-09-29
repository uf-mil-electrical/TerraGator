#include "ISM330DHCX.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

int main(){

    ISM330DHCX imu = ISM330DHCX();
    imu.init();

    while(1){
        printf("Gyro: [%f, %f, %f] rads/s\n", imu.get_gyro().x, imu.get_gyro().y, imu.get_gyro().z);
        printf("Accel: [%f, %f, %f] m/s^2\n", imu.get_accel().x, imu.get_accel().y, imu.get_accel().z);
        delay(1000);
    }

    return 0;
}