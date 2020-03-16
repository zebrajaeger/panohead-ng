#pragma once

#include "digitalio.h"

#include <pcf8574_esp.h>

class PCF8574 : public DigitalIO {
 public:
  PCF8574() : DigitalIO(), pcf_(NULL) {}
  ~PCF8574() {
    if (pcf_) {
      delete pcf_;
    }
  }

  bool begin(uint8_t i2cAddress = 0x20) {
    bool result = false;
    if (!pcf_) {
      Wire.beginTransmission(i2cAddress);
      if (Wire.endTransmission() == 0) {
        pcf_ = new PCF857x(i2cAddress, &Wire);
        pcf_->begin();
        result = true;
      }
    }
    return result;
  }

  virtual void set(uint8_t pin, bool value) {
    if (pcf_) {
      pcf_->write(pin, value);
    }
  };

  virtual bool get(uint8_t pin) {
    if (pcf_) {
      return pcf_->read(pin) != 0;
    }
    return false;
  }

  virtual void toggle(uint8_t pin) {
    if (pcf_) {
      pcf_->toggle(pin);
    }
  };

 private:
  PCF857x *pcf_;
};