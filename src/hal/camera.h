#pragma once

#include <Arduino.h>

#include "hal/digitalio.h"
#include "util/loggerfactory.h"

struct Camera {
 public:
  Camera() : LOG(LoggerFactory::getLogger("Camera")), digitalIO_(NULL), focusPin_(0), triggerPin_(0) {}

  bool begin(DigitalIO *digitalIO, uint8_t focusPin, uint8_t triggerPin) {
    digitalIO_ = digitalIO;
    focusPin_ = focusPin;
    triggerPin_ = triggerPin;

    return true;
  }

  void setFocus(boolean focus) {
    if (digitalIO_) {
      digitalIO_->set(focusPin_, !focus);
    }
  }

  void setTrigger(boolean trigger) {
    if (digitalIO_) {
      digitalIO_->set(triggerPin_, !trigger);
    }
  }

  void set(boolean focus, boolean trigger) {
    setFocus(focus);
    setTrigger(trigger);
  }

 private:
  Logger &LOG;
  DigitalIO *digitalIO_;
  uint8_t focusPin_;
  uint8_t triggerPin_;
};