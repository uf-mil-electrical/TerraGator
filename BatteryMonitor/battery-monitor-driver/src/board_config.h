#ifndef BOARD_CONFIG_H
#define BOARD_CONFIG_H

#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/gpio.h"
#include "hardware/spi.h"
#include "hardware/i2c.h"
#include "hardware/uart.h"
#include "hardware/adc.h"
#include "board_config.h"

extern const float CS_sens;

// ------------------------------------------------------------------
// Name: BOARD CONFIGURATION
// Purpose: Define all pin mappings, configurations, and device-specific
// parameters for the Battery Monitor Board
// ------------------------------------------------------------------

// ------------------------ UART ------------------------------------
// UART for communication with RPi5
// By default the stdout UART is `uart0`, so we will use the second one
#define UART_ID uart0
#define UART_BAUDRATE 115200
#define UART_TX_PIN 16
#define UART_RX_PIN 17

// ------------------------- I2C ------------------------------------
// I2C defines for the voltage sensing (VS) IC
#define VS_I2C_PORT i2c1
#define VS_I2C_SDA 10
#define VS_I2C_SCL 11
#define VS_ALERT   9  
#define VS_VBUS_UNDER_VOLTAGE_LIMIT 21.0f
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

// V_OUT (ADC) = sensitivity * input current + V_REF
#define CS_VREF 0
#define ADC_VREF 3.3f
#define ADC_RESOLUTION 12

// ------------------------ GPIO ------------------------------------
// LEDs connected for operating region indication
#define RED_LED 21
#define YELLOW_LED 20
#define GREEN_LED 19

// ------------------ FUNCTION PROTOTYPES --------------------------
// initializes all modules used on the Battery Monitor Board
void board_config(); 

#endif