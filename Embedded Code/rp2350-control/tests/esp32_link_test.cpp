#include <assert.h>
#include <stdio.h>
#include "Wire.h"
// Arduino's sketch preprocessor normally supplies these declarations.
void receive_i2c_request(int);
void send_i2c_data();
void prepare_i2c_reply();
void write_i2c_reply(const uint8_t *, size_t);
#include "../../ESP32-code/esp32-code/esp32-code.ino"

static std::vector<uint8_t> master_read(uint8_t request, size_t length) {
    Wire.request = request;
    receive_i2c_request(1); // Master gives the deferred RX task time to run.
    assert(Wire.fifo.size() == length); // Reply must exist BEFORE the read.
    auto result = Wire.fifo;
    Wire.fifo.clear(); // Hardware read consumes the reply.
    send_i2c_data(); // Original ESP32 fires onRequest AFTER that read.
    assert(Wire.fifo.empty()); // Must not preload a stale reply afterward.
    return result;
}
int main() {
    setup(); assert(i2c_ready);
    RemoteXY.connect_flag = 1;
    RemoteXY.joystick_x = 0; RemoteXY.joystick_y = 50;
    update_drive_controls(); check_for_new_commands();
    assert(master_read(0, 1)[0] == 1);
    const std::vector<uint8_t> forward = {0x37,1,0x30,0,3,0,0x5B,50,50};
    assert(master_read(1, 9) == forward);
    assert(master_read(0, 1)[0] == 0);
    // Straight travel: every usable forward/reverse position commands both
    // sides identically, including small horizontal noise in the dead zone.
    for (int x = -4; x <= 4; x++) {
        for (int y = -100; y <= 100; y++) {
            RemoteXY.joystick_x = x;
            RemoteXY.joystick_y = y;
            update_drive_controls();
            check_for_new_commands();
            const auto command = master_read(1, 9);
            const int speed = abs(y) < 5 ? 0 : abs(y);
            const uint8_t flags = speed == 0 ? 0x57 : (y > 0 ? 0x5B : 0x58);
            assert(command[6] == flags);
            assert(command[7] == speed && command[8] == speed);
        }
    }
    RemoteXY.joystick_x = 100; RemoteXY.joystick_y = 0;
    update_drive_controls(); check_for_new_commands();
    assert(master_read(0, 1)[0] == 1);
    const std::vector<uint8_t> turn = {0x37,1,0x30,0,3,0,0x5A,100,100};
    assert(master_read(1, 9) == turn);
    RemoteXY.connect_flag = 0;
    update_drive_controls(); check_for_new_commands();
    assert(master_read(0, 1)[0] == 1);
    const std::vector<uint8_t> stop = {0x37,1,0x30,0,3,0,0x57,0,0};
    assert(master_read(1, 9) == stop);
    assert(Wire.buffered_writes == 0 && i2c_reply_errors == 0);
    puts("PASS: ESP32 reply timing, all straight forward/reverse joystick positions, center dead zone, turning and disconnect packet bytes");
}
