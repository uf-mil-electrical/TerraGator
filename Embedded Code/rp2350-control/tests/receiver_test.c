// Compile the production receiver, motor driver and main loop against mock SDK IO.
#include <assert.h>
#include <setjmp.h>
#include <stdio.h>
#include <string.h>
#include "communication/data_parser.h"
#include "communication/rover_i2c.h"
#include "peripherals/motor_speed_control.h"
#include "peripherals/safety_devices.h"

static bool pins[48];
static uint16_t levels[12][2];
static uint init_count[12];
static uint64_t clock_us = 2000000;
static uint8_t packet[9] = {0x37, 1, 0x30, 0, 3, 0, 0x5B, 50, 50};
static uint8_t request;
static int status = 1, write_failure = 0, short_read = 0;
static bool running_main = false, saw_drive = false;
static unsigned loops = 0;
static jmp_buf main_exit;
i2c_inst_t test_i2c1;

void gpio_init(uint pin) { assert(pin < 48); }
void gpio_set_dir(uint pin, bool output) { (void)pin; (void)output; }
void gpio_put(uint pin, bool value) { pins[pin] = value; }
bool gpio_get_out_level(uint pin) { return pins[pin]; }
void gpio_set_function(uint pin, uint function) { (void)pin; (void)function; }
void gpio_pull_up(uint pin) { (void)pin; }
void stdio_init_all(void) {}
uint64_t time_us_64(void) { return clock_us; }
void sleep_ms(uint32_t ms) {
    clock_us += (uint64_t)ms * 1000;
    if (running_main && ms == 10) {
        if (loops == 0) {
            saw_drive = get_relay_state() && levels[4][1] > 0;
            assert(saw_drive);
            status = 0; // Drop packets after one valid command.
        }
        if (++loops == 65) longjmp(main_exit, 1);
    }
}
pwm_config pwm_get_default_config(void) { return (pwm_config){1, 0}; }
void pwm_config_set_clkdiv(pwm_config *c, float d) { c->divider = d; }
void pwm_config_set_wrap(pwm_config *c, uint16_t w) { c->wrap = w; }
uint pwm_gpio_to_slice_num(uint pin) { return (pin >> 1) % 8; }
uint pwm_gpio_to_channel(uint pin) { return pin & 1; }
void pwm_init(uint slice, const pwm_config *config, bool enabled) {
    assert(config->wrap == PWM_WRAP && config->divider == 4 && enabled);
    init_count[slice]++;
    levels[slice][0] = levels[slice][1] = 0;
}
void pwm_set_chan_level(uint slice, uint channel, uint16_t level) { levels[slice][channel] = level; }
void i2c_init(i2c_inst_t *bus, uint baud) { assert(bus == i2c1 && baud == 100000); }
void i2c_set_slave_mode(i2c_inst_t *bus, bool slave, uint8_t address) { (void)bus; (void)slave; (void)address; }
int i2c_write_timeout_us(i2c_inst_t *bus, uint8_t address, const uint8_t *data, uint length, bool nostop, uint timeout) {
    assert(bus == i2c1 && address == 0x20 && length == 1 && !nostop && timeout == 10000);
    request = data[0];
    return write_failure ? -1 : 1;
}
int i2c_read_timeout_us(i2c_inst_t *bus, uint8_t address, uint8_t *data, uint length, bool nostop, uint timeout) {
    assert(bus == i2c1 && address == 0x20 && !nostop && timeout == 10000);
    if (request == 0) { assert(length == 1); data[0] = status; }
    else { assert(request == 1 && length == 9); memcpy(data, packet, 9); }
    return short_read ? 0 : (int)length;
}

// Exercise the actual infinite firmware loop with a finite mock clock.
#define main rover_firmware_main
#include "../main.c"
#undef main

static void expect_side(bool forward_left, bool forward_right, uint16_t left, uint16_t right) {
    assert(pins[3] == forward_left && pins[2] == !forward_left);
    assert(pins[1] == !forward_right && pins[0] == forward_right);
    for (uint pin = 7; pin <= 9; pin++) assert(levels[pin >> 1][pin & 1] == left);
    for (uint pin = 4; pin <= 6; pin++) assert(levels[pin >> 1][pin & 1] == right);
    assert(get_relay_state() && pins[RELAY_CONTROL]);
}
static void expect_stopped(void) {
    for (uint pin = 4; pin <= 9; pin++) assert(levels[pin >> 1][pin & 1] == 0);
    assert(!get_relay_state() && !pins[RELAY_CONTROL]);
    assert(!pins[0] && !pins[1] && !pins[2] && !pins[3]);
}
int main(void) {
    init_relay(); init_leds(); init_motors();
    assert(init_count[2] == 1 && init_count[3] == 1 && init_count[4] == 1);
    uint8_t received[9];
    assert(rover_i2c_poll_esp32(i2c1, received));
    assert(memcmp(packet, received, 9) == 0);
    assert(rover_i2c_parse_data(received, 9));
    expect_side(true, true, 3001, 3001);
    // Check all direction combinations with zero, small and full PWM values.
    const uint8_t speeds[] = {0, 1, 50, 99, 100};
    for (uint direction = 0; direction < 4; direction++) {
        for (uint l = 0; l < 5; l++) for (uint r = 0; r < 5; r++) {
            packet[6] = 0x58 | direction;
            packet[7] = speeds[l]; packet[8] = speeds[r];
            assert(rover_i2c_parse_data(packet, 9));
            uint16_t left = (speeds[l] * 6001u + 50u) / 100u;
            uint16_t right = (speeds[r] * 6001u + 50u) / 100u;
            if (left > 6000) left = 6000;
            if (right > 6000) right = 6000;
            expect_side((direction & 2) != 0, (direction & 1) != 0, left, right);
        }
    }
    packet[6] = 0x58; packet[7] = 20; packet[8] = 80;
    assert(rover_i2c_parse_data(packet, 9)); expect_side(false, false, 1200, 4801);
    packet[6] = 0x5A;
    assert(rover_i2c_parse_data(packet, 9)); expect_side(true, false, 1200, 4801);
    packet[6] = 0x5D;
    assert(rover_i2c_parse_data(packet, 9));
    for (uint pin = 4; pin <= 9; pin++) assert(levels[pin >> 1][pin & 1] == 0);
    assert(get_relay_state() && !pins[0] && !pins[1] && !pins[2] && !pins[3]);
    packet[6] = 0x57;
    assert(rover_i2c_parse_data(packet, 9)); expect_stopped();
    assert(!rover_i2c_parse_data(NULL, 9));
    for (uint length = 0; length < 9; length++) assert(!rover_i2c_parse_data(packet, length));
    const uint8_t bad[9] = {0, 0, 0, 1, 4, 1, 0x67, 101, 101};
    for (uint i = 0; i < 9; i++) {
        uint8_t saved = packet[i]; packet[i] = bad[i];
        assert(!rover_i2c_parse_data(packet, 9)); packet[i] = saved;
        expect_stopped();
    }
    status = 0; assert(!rover_i2c_poll_esp32(i2c1, received));
    status = 255; assert(!rover_i2c_poll_esp32(i2c1, received));
    status = 1; write_failure = 1; assert(!rover_i2c_poll_esp32(i2c1, received));
    write_failure = 0; short_read = 1; assert(!rover_i2c_poll_esp32(i2c1, received));
    short_read = 0;
    set_all_leds(true); assert(pins[17] && pins[18] && pins[19] && pins[20] && pins[21]);
    set_all_leds(false); assert(!pins[17] && !pins[18] && !pins[19] && !pins[20] && !pins[21]);
    packet[6] = 0x5B; packet[7] = packet[8] = 50;
    running_main = true;
    if (setjmp(main_exit) == 0) rover_firmware_main();
    assert(saw_drive); expect_stopped();
    puts("PASS: production I2C polling, packet validation, forward/reverse/turn, PWM, brake/relay, LEDs and main-loop timeout");
    return 0;
}
