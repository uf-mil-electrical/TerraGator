#ifndef BME280_H
#define BME280_H

#include <stdio.h>
#include <string.h>
#include <cstdint>
#include <wiringPi.h>

using namespace std;

class BME280 {
    public:
        const uint8_t ADDRESS = 0x77;
        
        const uint8_t RESET = 0xE0;
        const uint8_t STATUS = 0xF3;
        const uint8_t CTRL = 0xF5;

        const uint8_t CTRL_HUM = 0xF2; //write before ctrl meas
        const uint8_t CTRL_MEAS = 0xF4;

        const uint8_t TEMP_LSB = 0xFB;
        const uint8_t TEMP_MSB = 0xFA;
        const uint8_t TEMP_XLSB = 0xFC;

        const uint8_t PRESS_LSB = 0xF8;
        const uint8_t PRESS_MSB = 0xF7;
        const uint8_t PRESS_XLSB = 0xF9;

        const uint8_t HUM_LSB = 0xFE;
        const uint8_t HUM_MSB = 0xFD;


        //calibration registers
        const uint8_t DIG_T1 = 0x88;
        const uint8_t DIG_T2 = 0x8A;
        const uint8_t DIG_T3 = 0x8C;

        const uint8_t DIG_P1 = 0x8E;
        const uint8_t DIG_P2 = 0x90;
        const uint8_t DIG_P3 = 0x92;
        const uint8_t DIG_P4 = 0x94;
        const uint8_t DIG_P5 = 0x96;
        const uint8_t DIG_P6 = 0x98;
        const uint8_t DIG_P7 = 0x9A;
        const uint8_t DIG_P8 = 0x9C;
        const uint8_t DIG_P9 = 0x9E;

        const uint8_t DIG_H1 = 0xA1;
        const uint8_t DIG_H2 = 0xE1;
        const uint8_t DIG_H3 = 0xE3;
        const uint8_t DIG_H4 = 0xE4;
        const uint8_t DIG_H5 = 0xE5;
        const uint8_t DIG_H6 = 0xE7;

        //calibration values
        uint16_t dig_T1;
        int16_t dig_T2;
        int16_t dig_T3;
        
        uint16_t dig_P1;
        int16_t dig_P2;
        int16_t dig_P3;
        int16_t dig_P4;
        int16_t dig_P5;
        int16_t dig_P6;
        int16_t dig_P7;
        int16_t dig_P8;
        int16_t dig_P9;

        uint8_t dig_H1;
        int16_t dig_H2;
        uint8_t dig_H3;
        int16_t dig_H4;
        int16_t dig_H5;
        int8_t dig_H6;
        
        int fd;
        int32_t t_fine;


        BME280(void);
        bool init(void);
        void set_reg(uint8_t reg, uint8_t val);
        uint8_t read_reg(uint8_t reg);
        int32_t get_temp(void);
        float get_hum(void);
        float get_press(void);


};

#endif
