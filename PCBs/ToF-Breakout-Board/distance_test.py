from machine import Pin, I2C
import time
from vl53l1x import VL53L1X

# ToF sensor allows up to 400kHz for i2c frequency 
i2c = I2C(0, scl=Pin(17), sda=Pin(16), freq = 400000)

sensor = VL53L1X(i2c) 

sensor.start_ranging()
while True: 
    if sensor.data_ready:
        dist = sensor.distance
        if dist is not None:
            print("Distance:", dist, "in")
        sensor.clear_interrupt()
    time.sleep(0.05)


