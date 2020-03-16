#pragma once

#include <Arduino.h>

class DigitalIO {
 public:
  virtual void set(uint8_t pin, bool value) = 0;
  virtual bool get(uint8_t pin) = 0;
  virtual void toggle(uint8_t pin) { set(pin, !get(pin)); };
};