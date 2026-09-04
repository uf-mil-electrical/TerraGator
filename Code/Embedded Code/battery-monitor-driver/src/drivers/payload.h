#ifndef PAYLOAD_H
#define PAYLOAD_H

#include <stdint.h>

// Sync and ID constants per MIL UART protocol:
// https://milwiki.cbrxyz.com/wiki/Software-electrical_data_serialization_protocol
#define SYNC_CHAR_ONE 0x37
#define SYNC_CHAR_TWO 0x01
#define ROVER_CLASS_ID 0x30
#define ROVER_SUBCLASS_ID_STATUS 0x10 // voltage + current + status payload
#define ROVER_SUBCLASS_ID_CONFIG 0x11 // for characteristics of battery payload

// LiPo unit-cell voltage thresholds used to calculate 
// unit level status.

// These are multiplied by cell_count AT RUNTIME to get
// the full pack thresholds.

// some values concflicted with what I (Jesse) read on the
// the issue so for now I worked some values backwards.

#define LIPO_CELL_MAX_V 4.1f // [Volts per Cell] Overcharged Battery; Disconnect Charger
#define LIPO_CELL_IDEAL_MIN_V 3.8f // [Volts per Cell] Floor of the ideal operating range
#define LIPO_CELL_AIR_LANDING_V 3.5f // [Volts per Cell] Deep discharge is near

typedef struct{
    uint8_t cell_count;             // [Cells] number of LiPo cells in series configuration (i.e. 6 for 6S pack)
    uint16_t capacity_mAh;          // [mAh] pack capacity
    float max_voltage;              // [Volts] maximum allowable voltage before overcharging
    float min_voltage;              // [Volts] minimum allowable voltage before deep discharge
    float max_discharge_current;    // [Amps] max current discharge allowed
    float max_charge_current;       // [Amps] max charge current allowed
    float expected_current;         // [Amps] expected draw during operation
} BatteryConfig;

// set or get the active battery configuration
void set_battery_config(BatteryConfig config);
BatteryConfig get_battery_config(void);

// Header for send_payload
void send_payload(uint8_t status, float voltage, float current);

// send battery configuartion payload (subclass 0x11) : full BatteryConfig struct
void send_battery_config_payload(void);
#endif // PAYLOAD_H