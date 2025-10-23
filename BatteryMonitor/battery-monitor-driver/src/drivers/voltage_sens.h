#ifndef VOLTAGE_SENS_H_
#define VOLTAGE_SENS_H_

// Minimum voltage sensor header for the INA226 Power Monitor

#include "board_config.h"
#include "hardware/i2c.h"

// -------------------- REGISTER MAP ------------------------ //
#define VS_SLAVE_ADDR 0x40 // A0 and A1 are tied to GND
// register map for the INA226-Q1 power monitor
#define INA226_CONFIG          0x00
#define INA226_SHUNT_VOLTAGE   0x01
#define INA226_BUS_VOLTAGE     0x02
#define INA226_POWER           0x03
#define INA226_CURRENT         0x04
#define INA226_CALIBRATION     0x05
#define INA226_MASK_EN         0x06
#define INA226_ALERT_LIMIT     0x07
#define INA226_MANUFACTURER_ID 0xFE
#define INA226_DIE_ID          0xFF

// ----------- BITMASKS, BITPOSITIONS, GROUP CONFIGS -------- //

// [0x00] Configuration Register 
typedef enum avg_mode 
{
    INA226_AVERAGING_MODE_1,
    INA226_AVERAGING_MODE_4, 
    INA226_AVERAGING_MODE_16,
    INA226_AVERAGING_MODE_64,
    INA226_AVERAGING_MODE_128,
    INA226_AVERAGING_MODE_256,
    INA226_AVERAGING_MODE_512, 
    INA226_AVERAGING_MODE_1024
} ina226_avg_mode_t; 

typedef enum vbus_conv_time
{
    INA226_VBUSCT_140us, 
    INA226_VBUSCT_204us,
    INA226_VBUSCT_332us,
    INA226_VBUSCT_588us,
    INA226_VBUSCT_1_1ms,
    INA226_VBUSCT_2_116ms,
    INA226_VBUSCT_4_156ms,
    INA226_VBUSCT_8_244ms 
} ina226_vbus_conv_time_t; 

typedef enum vshct_conv_time
{
    INA226_VSHTCT_140us, 
    INA226_VSHCT_204us,
    INA226_VSHCT_332us,
    INA226_VSHCT_588us,
    INA226_VSHCT_1_1ms, 
    INA226_VSHCT_2_116ms,
    INA226_VSHCT_4_156ms,
    INA226_VSHCT_8_244ms 
} ina226_vshct_conv_time_t; 

typedef enum mode
{
    INA226_POWERDOWN, 
    INA226_VSHUNT_TRIG,
    INA226_VBUS_TRIG,
    INA226_VSHUNT_VBUS_TRIG,
    INA226_POWERDOWN_2,
    INA226_VSHUNT_CONT,
    INA226_VBUS_CONT,
    INA226_VSHUNT_VBUS_CONT 
} ina226_mode_t; 

#define INA226_CONFIG_RST         15
#define INA226_CONFIG_AVG0_BP     9
#define INA226_CONFIG_VBUSTCT0_BP 6
#define INA226_CONFIG_VSHCT0_BP   3
#define INA226_CONFIG_MODE0_BP    0

// [0x02] Bus Voltage Register
#define INA226_VBUS_FSR 40.96f // [V]
#define INA226_VBUS_LSB 1.25e-3f // [V/LSB]
#define INA226_VBUS_FLOAT_TO_BIN(x_f) (uint16_t)(x_f/INA226_VBUS_LSB);   

// [0x06] Mask/Enable Register
#define INA226_MASK_EN_SOL_BP  15
#define INA226_MASK_EN_SUL_BP  14
#define INA226_MASK_EN_BOL_BP  13 // configures alert pin to be asserted if bus voltage measurement exceeds value programming in Alert limit register
#define INA226_MASK_EN_BUL_BP  12 // configures alert pin to be asserted if bus voltage measurement drops below value programmed in the Alert limit register 
#define INA226_MASK_EN_POL_BP  11 
#define INA226_MASK_EN_CNVR_BP 10
#define INA226_MASK_EN_AFF_BP  4
#define INA226_MASK_EN_CVRF_BP 3
#define INA226_MASK_EN_OVF_BP  2
#define INA226_MASK_EN_APOL_BP 1 // Sets alert polarity (1 = active-high open collector; 0 = active-low open collector)
#define INA226_MASK_EN_LEN_BP  0

// [0x07] Alert Limit Register


// [0x08] Manufacturer ID register
#define INA226_MANUFACTURER_ID_VAL 0b0101010001001001

// --------------------- FUNCTION PROTOTYPES ------------------- //

// general i2c function to write to the 16-bit addresses of the INA226
void ina226_write_register(uint8_t reg, uint16_t value);

// general i2c function to read from the 16-bit addresses of the INA226
uint16_t ina226_read_register(uint8_t reg);

// initialize the voltage sensing IC over I2C
// includes configuring configuration register, mask enable, and alert limit register
void voltage_sens_init(); 

// read the bus voltage from voltage sensor bus voltage register 
float read_bus_voltage(); 

// poll alert limit connected to GPIO on RPi4
// returns 1 if alert limit is asserted, and 0 if alert limit is not asserted
bool poll_alert_limit();

#endif