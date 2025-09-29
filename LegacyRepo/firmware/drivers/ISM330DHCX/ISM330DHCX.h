#ifndef ISM330DHCX_H
#define ISM330DHCX_H

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <wiringPi.h>

using namespace std;

class ISM330DHCX {
    public:
        const uint8_t ADDRESS = 0x6A;
        const uint8_t STATUS = 0x1E;

        const uint8_t CTRL1_XL = 0x10;
        const uint8_t CTRL2_G = 0x11;

        
        const uint8_t OUTX_L_G = 0x22;
        const uint8_t OUTX_H_G = 0x23;
        const uint8_t OUTY_L_G = 0x24;
        const uint8_t OUTY_H_G = 0x25;
        const uint8_t OUTZ_L_G = 0x26;
        const uint8_t OUTZ_H_G = 0x27;

        const uint8_t OUTX_L_A = 0x28;
        const uint8_t OUTX_H_A = 0x29;
        const uint8_t OUTY_L_A = 0x2A;
        const uint8_t OUTY_H_A = 0x2B;
        const uint8_t OUTZ_L_A = 0x2C;
        const uint8_t OUTZ_H_A = 0x2D;

        int fd;
        float accel_range = .061; //2g: .061, 4: .122, 8: .244, 16: .488
        float gyro_range = .061; //125: 4.375, 250: 8.75, 500: 17.5, 1000: 35, 2000: 70
        float milli_g_to_accel = .00980665;

        struct GyroData {
            float x;
            float y;
            float z;
        };

        struct AccelData {
            float x;
            float y;
            float z;
        };
        
        ISM330DHCX(void);
        bool init(void);
        void set_reg(uint8_t reg, uint8_t val);
        int8_t read_reg(uint8_t reg);
        GyroData get_gyro(void);
        AccelData get_accel(void);
    
};

#endif
