#include "Adafruit_VL53L1X.h"
#include "hardware/gpio.h"
#include "pico/stdlib.h"

Adafruit_VL53L1X::Adafruit_VL53L1X(uint8_t shutdown_pin, uint8_t irq_pin)
    : VL53L1X(irq_pin) {
  _shutdown_pin = shutdown_pin;
  _irq_pin = irq_pin;
}

bool Adafruit_VL53L1X::begin(uint8_t i2c_addr, bool debug) {
  if (_shutdown_pin != -1) {
    gpio_init(_shutdown_pin);
    gpio_set_dir(_shutdown_pin, GPIO_OUT);
    gpio_put(_shutdown_pin, 1);
    gpio_put(_shutdown_pin, 0);
    sleep_ms(5);
    gpio_put(_shutdown_pin, 1);
  }
  sleep_ms(5);

  vl_status = InitSensor(i2c_addr * 2);
  if (vl_status != VL53L1X_ERROR_NONE) {
    return false;
  }

  if (sensorID() != 0xEACC) {
    return false;
  }

  return true;
}

uint16_t Adafruit_VL53L1X::sensorID(void) {
  uint16_t sensorID = 0;
  vl_status = VL53L1X_GetSensorId(&sensorID);
  return sensorID;
}

int16_t Adafruit_VL53L1X::distance(void) {
  uint16_t distance;
  uint8_t rangeStatus;

  vl_status = VL53L1X_GetRangeStatus(&rangeStatus);
  if ((vl_status != VL53L1X_ERROR_NONE) || (rangeStatus != 0x0)) {
    return -1;
  }

  vl_status = VL53L1X_GetDistance(&distance);
  if (vl_status != VL53L1X_ERROR_NONE) {
    return -1;
  }
  return (int16_t)distance;
}

bool Adafruit_VL53L1X::clearInterrupt(void) {
  vl_status = VL53L1X_ClearInterrupt();
  return (vl_status == VL53L1X_ERROR_NONE);
}

bool Adafruit_VL53L1X::setIntPolarity(bool polarity) {
  vl_status = VL53L1X_SetInterruptPolarity(polarity);
  return (vl_status == VL53L1X_ERROR_NONE);
}

bool Adafruit_VL53L1X::getIntPolarity(void) {
  uint8_t x = 0;
  vl_status = VL53L1X_GetInterruptPolarity(&x);
  return (bool)x;
}

bool Adafruit_VL53L1X::startRanging(void) {
  vl_status = VL53L1X_StartRanging();
  return (vl_status == VL53L1X_ERROR_NONE);
}

bool Adafruit_VL53L1X::stopRanging(void) {
  vl_status = VL53L1X_StopRanging();
  return (vl_status == VL53L1X_ERROR_NONE);
}

bool Adafruit_VL53L1X::dataReady(void) {
  uint8_t x = 0;
  vl_status = VL53L1X_CheckForDataReady(&x);
  return (bool)x;
}

bool Adafruit_VL53L1X::setTimingBudget(uint16_t ms) {
  vl_status = VL53L1X_SetTimingBudgetInMs(ms);
  return (vl_status == VL53L1X_ERROR_NONE);
}

uint16_t Adafruit_VL53L1X::getTimingBudget(void) {
  uint16_t ms = 0;
  vl_status = VL53L1X_GetTimingBudgetInMs(&ms);
  if (vl_status == VL53L1X_ERROR_NONE) {
    return ms;
  }
  return 0;
}