#ifndef SSEG_H_
#define SSEG_H_

// Minimum 7-seg header file for the MAX7219 LED driver

#include "board_config.h"
#include "hardware/spi.h"

// -------------------- REGISTER MAP ------------------------ //

#define MAX7219_NO_OP        0x00  
#define MAX7219_DIG0         0x01 
#define MAX7219_DIG1         0x02 
#define MAX7219_DIG2         0x03
#define MAX7219_DIG3         0x04
#define MAX7219_DIG4         0x05
#define MAX7219_DIG5         0x06
#define MAX7219_DIG6         0x07
#define MAX7219_DIG7         0x08 
#define MAX7219_DECODE_MODE  0x09
#define MAX7219_INTENSITY    0x0A
#define MAX7219_SCAN_LIMIT   0x0B
#define MAX7219_SHUTDOWN     0x0C
#define MAX7219_DISPLAY_TEST 0x0F

// ----------- BITMASKS, BITPOSITIONS, GROUP CONFIGS -------- //

#define REG_ADDR_MAP_BP 8 //The Register Address format has addresses from D15-D8
#define SHUTDOWN_NORMAL_OPERATION_BP 0 
#define DECODE_MODE_0_3_DECODE_MODE_GC 0x0F // uses code B decoding for DIG0-3
#define SCAN_LIMIT_DISPLAY_DIG_0_3_GC 0x03
#define DP_BP 7
// writing to the digit registers is equivalent to writing the BCD equivalents per digit

// --------------------- FUNCTION PROTOTYPES ------------------- //

// initializes the SSEG for proper function 
void sseg_init(); 

// writes the voltage and current values to the LEDs 
void max7219_voltage_current_write(float voltage, float current); 

// general SPI write based on MAX7219 protocol
void max7219_write(uint8_t reg, uint8_t data);

#endif