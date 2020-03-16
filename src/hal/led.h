#pragma once

#include <Arduino.h>

#include "util/singletimer.h"
#include "hal/digitalio.h"

class Led {
 public:
  Led();

  bool begin(DigitalIO *io, uint8_t pin, float blinkSpeedHz = 2.0);
  void loop();

  void set(bool state);
  void toggle();
  void blink();
  void setBlinkSpeed(float speedHz);
  void blink(float speedHz);

 private:
  uint64_t getPeriodUs();

  DigitalIO *io_;
  uint8_t pin_;
  SingleTimer timer_;
  float blinkSpeedHz_;
};