#pragma once

#include <Arduino.h>

struct Camera {
 public:
  Camera() : focusPin_(0), triggerPin_(0) {}

  bool begin(uint8_t focusPin, uint8_t triggerPin) {
    focusPin_ = focusPin;
    pinMode(focusPin_, OUTPUT);

    triggerPin_ = triggerPin;
    pinMode(triggerPin_, OUTPUT);

    return true;
  }

  void setFocus(boolean focus) { digitalWrite(focusPin_, focus); }

  void setTrigger(boolean trigger) { digitalWrite(triggerPin_, trigger); }

  void set(boolean focus, boolean trigger) {
    digitalWrite(focusPin_, focus);
    digitalWrite(triggerPin_, trigger);
  }

 private:
  uint8_t focusPin_;
  uint8_t triggerPin_;
};