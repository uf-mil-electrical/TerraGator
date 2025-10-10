#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include "pico/stdlib.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"

// ------------------------------------------------------------------
// Name: BOARD CONFIGURATION
// Purpose: Define all pin mappings, configurations, and device-specific
// parameters for the Battery Monitor Board
// ------------------------------------------------------------------

// ------------------------ UART ------------------------------------
// UART for communication with RPi5
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart1
#define UART_BAUDRATE 115200
#define UART_TX_PIN 16
#define UART_RX_PIN 17

// ------------------------- I2C ------------------------------------
// I2C defines for the voltage sensing (VS) IC
#define VS_I2C_PORT i2c0
#define VS_I2C_SDA 10
#define VS_I2C_SCL 11
#define VS_ALERT   9 
#define I2C_BAUDRATE 100000 // 100kbps for standard operation mode

// ------------------------- SPI ------------------------------------
// 7SEG SPI Communication defines with SPI0 
#define SPI_PORT spi0
#define CS_7SEG  5
#define PIN_SCK  2
#define PIN_MOSI 3
#define SPI_BAUDRATE 1000000 // 1MHz operation

// ------------------------- ADC ------------------------------------
// ADC1 for current sensing IC
#define CS_ADC 27
#define ADC_CH 1
#define VS_SLAVE_ADDR 0x40 // A0 and A1 are tied to GND

// V_OUT (ADC) = sensitivity * input current + V_REF
#define CS_VREF 0
#define ADC_VREF 3.3f
#define ADC_RESOLUTION 12
// The TMCS1100A1 has a sensitivity of 50mV / A
const float CS_sens = .05; // V/A

// ------------------------ GPIO ------------------------------------
// LEDs connected for operating region indication
#define RED_LED 21
#define YELLOW_LED 20
#define GREEN_LED 19

// ------------------ FUNCTION PROTOTYPES --------------------------
// initializes all modules used on the Battery Monitor Board
void board_config(); 

#endif