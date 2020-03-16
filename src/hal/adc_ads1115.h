#pragma once

#include "adc.h"

#include "hal/ads1115.h"

class ADC_ADS1115 : public ADC {
 public:
  ADC_ADS1115() : ADC(), adc_() {
    adc_.onResult([this](uint8_t channel, uint16_t value) { ADC::publishValue(channel, value); });
  }

  bool begin(int8_t i2caddres, uint32_t periodMs = 5) { return adc_.begin(i2caddres) && ADC::begin(periodMs); }
  virtual bool begin(uint32_t periodMs = 5) override { return adc_.begin() && ADC::begin(periodMs); }

  virtual void loop() override {
    ADC::loop();
    adc_.loop();
  }
  virtual uint8_t getChannelCount() override { return 4; }

 protected:
  virtual void triggerConversion(uint8_t channel) override { adc_.triggerConversion(channel); }

 private:
  ADS1115 adc_;
};