#ifndef TEST_PWM_H
#define TEST_PWM_H
#include "pico/stdlib.h"
typedef struct { float divider; uint16_t wrap; } pwm_config;
pwm_config pwm_get_default_config(void);
void pwm_config_set_clkdiv(pwm_config *config, float divider);
void pwm_config_set_wrap(pwm_config *config, uint16_t wrap);
uint pwm_gpio_to_slice_num(uint gpio);
uint pwm_gpio_to_channel(uint gpio);
void pwm_init(uint slice, const pwm_config *config, bool enabled);
void pwm_set_chan_level(uint slice, uint channel, uint16_t level);
#endif
