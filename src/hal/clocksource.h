#pragma once

#include <Arduino.h>
#include <driver/ledc.h>

class ClockSource {
 public:
  ClockSource() : pin_(-1), speedHz_(-1) {}

  bool begin(uint8_t pin = 22, uint32_t speedHz = 10000000) {
    pin_ = pin;
    speedHz_ = speedHz;

    ledc_timer_config_t tmr;
    tmr.duty_resolution = LEDC_TIMER_1_BIT;
    tmr.freq_hz = speedHz_;
    tmr.speed_mode = LEDC_HIGH_SPEED_MODE;
    tmr.timer_num = LEDC_TIMER_0;
    ledc_timer_config(&tmr);

    ledc_channel_config_t ch;
    ch.channel = LEDC_CHANNEL_0;
    ch.duty = 1;
    ch.gpio_num = pin;
    ch.speed_mode = LEDC_HIGH_SPEED_MODE;
    ch.hpoint = 0;
    ch.timer_sel = LEDC_TIMER_0;
    ledc_channel_config(&ch);

    return true;
  }

  int8_t pin_;
  int32_t speedHz_;
};