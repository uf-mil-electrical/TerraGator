#ifndef TEST_PICO_STDLIB_H
#define TEST_PICO_STDLIB_H
#include <stdbool.h>
#include <stdint.h>
typedef unsigned int uint;
#define GPIO_OUT 1
#define GPIO_FUNC_PWM 4
#define GPIO_FUNC_I2C 3
void gpio_init(uint pin);
void gpio_set_dir(uint pin, bool output);
void gpio_put(uint pin, bool value);
bool gpio_get_out_level(uint pin);
void gpio_set_function(uint pin, uint function);
void gpio_pull_up(uint pin);
void stdio_init_all(void);
void sleep_ms(uint32_t ms);
uint64_t time_us_64(void);
#endif
