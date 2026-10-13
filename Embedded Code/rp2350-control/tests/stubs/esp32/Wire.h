#pragma once
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include <algorithm>
#include <vector>
using std::max;
#define CONFIG_IDF_TARGET_ESP32 1
#define PROGMEM
#define OUTPUT 1
#define LOW 0
#define HIGH 1
typedef int portMUX_TYPE;
#define portMUX_INITIALIZER_UNLOCKED 0
#define portENTER_CRITICAL(x) ((void)(x))
#define portEXIT_CRITICAL(x) ((void)(x))
inline int constrain(int v, int lo, int hi) { return std::max(lo, std::min(v, hi)); }
inline unsigned long millis() { return 1000; }
inline void delay(int) {}
inline void pinMode(int, int) {}
inline void digitalWrite(int, int) {}
struct MockSerial {
    void begin(int) {}
    void println(const char *) {}
    template<class... T> void printf(const char *, T...) {}
} Serial;
struct MockWire {
    int request = -1;
    std::vector<uint8_t> fifo;
    unsigned buffered_writes = 0;
    void onReceive(void (*)(int)) {}
    void onRequest(void (*)()) {}
    bool begin(uint8_t addr, int sda, int scl, uint32_t freq) {
        return addr == 0x20 && sda == 21 && scl == 22 && freq == 100000;
    }
    bool available() { return request >= 0; }
    int read() { int result = request; request = -1; return result; }
    size_t slaveWrite(const uint8_t *data, size_t length) {
        fifo.assign(data, data + length); return length;
    }
    size_t write(const uint8_t *, size_t length) { buffered_writes++; return length; }
} Wire;
