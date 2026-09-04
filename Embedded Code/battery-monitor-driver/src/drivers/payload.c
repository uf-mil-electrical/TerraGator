#include "hardware/uart.h"
#include <stdint.h>
#include "payload.h"

// Active battery config stored as module so both send_payload()
// & status.c (through get_battery_config()) can access.

// DEFAULTS ALL 0 initially
// OVERRIDE with set_battery_config()

static BatteryConfig deployed_config = {
    .cell_count             = 0,
    .capacity_mAh           = 0,
    .max_voltage            = 0.0f,
    .min_voltage            = 0.0f,
    .max_discharge_current  = 0.0f,
    .max_charge_current     = 0.0f,
    .expected_current       = 0.0f,
};

// Computes a Fletcher 16 checksum over buf[start..end] inclusive.
// Per the MIL UART protocol, checksum STARTS at the CLASS ID byte (indexex at 2)
// and runs through the LAST data byte — SYNC BYTES ARE EXCLUDED!
// Refer here for additional specs: https://milwiki.cbrxyz.com/wiki/Software-electrical_data_serialization_protocol
static void fletcher16(uint8_t *buf, int start, int end, uint8_t *sum1_out, uint8_t *sum2_out)
{
    uint8_t sum1 = 0;
    uint8_t sum2 = 0;
    for (int i = start; i <= end; i++)
    {
        sum1 = (sum1 + buf[i]) % 255;
        sum2 = (sum2 + sum1)  % 255;
    }
    *sum1_out = sum1;
    *sum2_out = sum2;
}

// only needed here in payload.c, so no need for header in payload.h
// sends payload over UART0
static void uart_send_payload(uint8_t *payload, int len){
    for (int i = 0; i < len; i++){
        uart_putc_raw(uart0, payload[i]);
    }
}

// Reference: https://milwiki.cbrxyz.com/wiki/Software-electrical_data_serialization_protocol
// Formatting of the Payload is as follows:
// | status code (1 byte) | voltage (2 bytes) | current (2 bytes) | 
void send_payload(uint8_t status, float voltage, float current) {
    // Values come in as milivolts and miliamps (floats) 
    // So I convert to integers for the payload.
    int16_t v_int = (int16_t)(voltage * 1000);
    int16_t c_int = (int16_t)(current * 1000);
    // Constructing the payload!
    uint8_t payload[13];
    // Sync Characters and Class/Subclass IDs
    payload[0] = SYNC_CHAR_ONE;
    payload[1] = SYNC_CHAR_TWO;
    payload[2] = ROVER_CLASS_ID;
    payload[3] = ROVER_SUBCLASS_ID_STATUS;
    // Sending 5 Bytes of for the payload (1 for status code, 2 for voltage, 2 for current)
    payload[4] = 0x05;                  // Low Byte of payload Length
    payload[5] = 0x00;                  // High Byte of payload length
    payload[6] = status;                // Status Code
    payload[7] = (v_int) & 0xFF;        // Low Byte of voltage
    payload[8] = (v_int >> 8) & 0xFF;   // High Byte of voltage
    payload[9] = (c_int) & 0xFF;        // Low Byte of current
    payload[10] = (c_int >> 8) & 0xFF;  // High Byte of current

    // Checksum covers classID through to LAST data byte (index 2 to 10)
    fletcher16(payload, 2, 10, &payload[11], &payload[12]);
    uart_send_payload(payload,13);
}

void send_battery_config_payload(void){
    // converting floats to integers
    int16_t max_v  = (int16_t)(deployed_config.max_voltage * 1000); // [mV]
    int16_t min_v  = (int16_t)(deployed_config.min_voltage * 1000); // [mV]
    int16_t max_dc = (int16_t)(deployed_config.max_discharge_current * 1000); // [mA]
    int16_t max_cc = (int16_t)(deployed_config.max_charge_current * 1000); // [mA]
    int16_t exp_c  = (int16_t)(deployed_config.expected_current * 1000); // [mA]

    uint8_t payload[21]; // 6 bytes for header data + 11 bytes payload + 2 bytes checksum
    payload[0] = SYNC_CHAR_ONE;
    payload[1] = SYNC_CHAR_TWO;
    payload[2] = ROVER_CLASS_ID;
    payload[3] = ROVER_SUBCLASS_ID_CONFIG;
    payload[4] = 0x0B; // Low Byte of payload Length
    payload[5] = 0x00; // High Byte of payload length
    payload[6] = deployed_config.cell_count;                 // Deployed Cell Count
    payload[7] = deployed_config.capacity_mAh & 0xFF;        // Low Byte Capacity
    payload[8] = (deployed_config.capacity_mAh >> 8) & 0xFF; // High Byte Capacity
    payload[9]  = max_v & 0xFF;         // Low Byte Max Voltage
    payload[10] = (max_v >> 8) & 0xFF;  // High Byte Max Voltage
    payload[11] = min_v & 0xFF;         // Low Byte Min Voltage
    payload[12] = (min_v >> 8) & 0xFF;  // High Byte Max Voltage
    payload[13] = max_dc & 0xFF;        // Low Byte of Max Discharge Current
    payload[14] = (max_dc >> 8) & 0xFF; // High Byte of Max Discharge Current
    payload[15] = max_cc & 0xFF;        // Low Byte of Max Charge Current
    payload[16] = (max_cc >> 8) & 0xFF; // High Byte of Max Charge Current
    payload[17] = exp_c & 0xFF;         // Low Byte of Expected Operating Currents
    payload[18] = (exp_c >> 8) & 0xFF;  // High Byte of Expected Operating Currents
 
    // Checksum covers classID through to LAST data byte (index 2 to 18)
    fletcher16(payload, 2, 18, &payload[19], &payload[20]);
    uart_send_payload(payload, 21);
}

void set_battery_config(BatteryConfig config){
    deployed_config = config;
}

BatteryConfig get_battery_config(void){
    return deployed_config;
}