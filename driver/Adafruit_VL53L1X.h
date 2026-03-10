#ifndef ADAFRUIT_VL53L1X_H
#define ADAFRUIT_VL53L1X_H

#include "vl53l1x_class.h"

#define VL53L1X_I2C_ADDR 0x29

class Adafruit_VL53L1X : public VL53L1X {
public:
  Adafruit_VL53L1X(uint8_t shutdown_pin = -1, uint8_t irq_pin = -1);

  bool begin(uint8_t i2c_addr = VL53L1X_I2C_ADDR, bool debug = false);
  uint16_t sensorID(void);

  bool startRanging(void);
  bool stopRanging(void);
  bool setTimingBudget(uint16_t ms);
  uint16_t getTimingBudget(void);

  bool dataReady(void);
  int16_t distance(void);

  bool clearInterrupt(void);
  bool setIntPolarity(bool polarity);
  bool getIntPolarity(void);

  VL53L1X_ERROR vl_status; 

private:
  int8_t _irq_pin, _shutdown_pin;
};

#endif