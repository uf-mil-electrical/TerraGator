# gpio example not used for anything

import lgpio
import time
import datetime

TRIGGER_PIN = 17
ECHO_PIN = 27

# Open the GPIO chip (usually chip 0 is used on the Raspberry Pi)
chip = lgpio.gpiochip_open(0)

# Set the GPIO pin as an output
lgpio.gpio_claim_output(chip, TRIGGER_PIN)
lgpio.gpio_claim_input(chip, ECHO_PIN)

def main():
    send_pulse()
    resp_in_delta = read_response()
    resp_in_seconds = resp_in_delta.total_seconds() * 10**6
    distance_cm = (resp_in_seconds*0.0343) /2
    print(distance_cm)

    time.sleep(100 *10**-3)

def read_response():
    while lgpio.gpio_read(chip, ECHO_PIN) == 0:
        pass

    before = datetime.datetime.now()

    while lgpio.gpio_read(chip, ECHO_PIN) == 1:
        pass

    after = datetime.datetime.now()

    return after-before

def send_pulse():
    lgpio.gpio_write(chip, TRIGGER_PIN, 1)
    time.sleep(10 * 10**-6)
    lgpio.gpio_write(chip, TRIGGER_PIN, 0)

if __name__ == "__main__":

    while True:
        main()

    lgpio.gpiochip_close(chip)
