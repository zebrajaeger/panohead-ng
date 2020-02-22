#pragma once

#include <Arduino.h>
#include <functional>

#include "hal/ads1115.h"
#include "util/logger.h"
#include "util/singletimer.h"

class ADC {
 public:
  ADC() : LOG("ADC"), adc_(), timer_("ADC"), currentChannel_(0) {}

  bool begin(uint32_t periodMs = 5) {
    bool result = adc_.begin();
    if (result) {
      timer_.start(periodMs * 1000, false, true);
      timer_.onTimer(std::bind(&ADC::onTimer, this));
    }
    return result;
  }

  void loop() {
    adc_.loop();
    timer_.loop();
  }

  void onResult(ADS1115::conversionCallback_t cb) { adc_.onResult(cb); }

 protected:
  void onTimer() {
    adc_.triggerConversion(currentChannel_);
    currentChannel_++;
    if (currentChannel_ > 3) {
      currentChannel_ = 0;
    }
  }

 private:
  Logger LOG;
  ADS1115 adc_;
  SingleTimer timer_;
  uint8_t currentChannel_;
};