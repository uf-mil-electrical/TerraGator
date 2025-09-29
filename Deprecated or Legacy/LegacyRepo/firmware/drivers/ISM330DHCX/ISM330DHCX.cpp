#include "ISM330DHCX.h"
#include <wiringPiI2C.h>

ISM330DHCX::ISM330DHCX(void) {}

bool ISM330DHCX::init(void) {
    fd = wiringPiI2CSetup(ADDRESS);
    if(fd == -1) {
        return false;
    }   

    //accelerometer control: 416 Hz (high performance), 2g, high-resultion = 0
    set_reg(CTRL1_XL, 0x60);
    //gyroscope control: 416 Hz (high performance), 2000 dps, high-resultion = 0
    set_reg(CTRL2_G, 0x60);

    
    return true;
}

void ISM330DHCX::set_reg(uint8_t reg, uint8_t val) {
    uint8_t data[2];
    data[0] = reg;
    data[1] = val;
    wiringPiI2CWriteReg8(fd, reg, val);
}

int8_t ISM330DHCX::read_reg(uint8_t reg) {
    return wiringPiI2CReadReg8(fd, reg);
}


ISM330DHCX::GyroData ISM330DHCX::get_gyro(void) {
    while(!(read_reg(STATUS) & 0x02)) {};
    GyroData gyro;

    int16_t raw_x = (read_reg(OUTX_L_G) | (read_reg(OUTX_H_G) << 8));
    int16_t raw_y = (read_reg(OUTY_L_G) | (read_reg(OUTY_H_G) << 8));
    int16_t raw_z = (read_reg(OUTZ_L_G) | (read_reg(OUTZ_H_G) << 8));

    // Convert raw data to degrees per second (°/s)
    float gyro_x_dps = raw_x * gyro_range;
    float gyro_y_dps = raw_y * gyro_range;
    float gyro_z_dps = raw_z * gyro_range;

    // Convert degrees per second to radians per second (rad/s)
    gyro.x = gyro_x_dps * (3.14159 / 180);
    gyro.y = gyro_y_dps * (3.14159 / 180);
    gyro.z = gyro_z_dps * (3.14159 / 180);

    return gyro;
}

ISM330DHCX::AccelData ISM330DHCX::get_accel(void) {
    while(!(read_reg(STATUS) & 0x01)) {};
    AccelData accel;
    accel.x = (float)(read_reg(OUTX_L_A) | (read_reg(OUTX_H_A) << 8)) * accel_range * milli_g_to_accel;
    accel.y = (float)(read_reg(OUTY_L_A) | (read_reg(OUTY_H_A) << 8)) * accel_range * milli_g_to_accel;
    accel.z = (float)(read_reg(OUTZ_L_A) | (read_reg(OUTZ_H_A) << 8)) * accel_range * milli_g_to_accel;
    return accel;
}