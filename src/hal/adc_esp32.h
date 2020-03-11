#pragma once

#include "adc.h"

#include <vector>

class ADC_ESP32 : public ADC {
 public:
  ADC_ESP32() : ADC(), gpioPins_() {}

  bool begin(std::vector<uint8_t> gpioPins, uint32_t periodMs = 5) {
    gpioPins_ = gpioPins;
    analogReadResolution(12);
    return ADC::begin(periodMs);
  }
  virtual uint8_t getChannelCount() override { return gpioPins_.size(); }

 protected:
  virtual void triggerConversion(uint8_t channel) override { publishValue(channel, analogRead(gpioPins_[channel])); }

 private:
  std::vector<uint8_t> gpioPins_;
};