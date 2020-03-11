#pragma once

#include <Arduino.h>
#include <functional>

#include "util/logger.h"
#include "util/singletimer.h"

class ADC {
 public:
  typedef std::function<void(uint8_t channel, uint16_t value)> ConversionCallback_t;

  ADC() : LOG("ADC"), timer_("ADC"), currentChannel_(0) {}

  virtual bool begin(uint32_t periodMs = 5) {
    timer_.startMs(periodMs, false, true, std::bind(&ADC::onTimer, this));
    return true;
  }
  virtual void loop() { timer_.loop(); }
  virtual uint8_t getChannelCount() = 0;

  void onResult(ConversionCallback_t cb) { conversionCallback_ = cb; }

 protected:
  virtual void triggerConversion(uint8_t channel) = 0;
  void publishValue(uint8_t channel, uint16_t value) {
    if (conversionCallback_) {
      conversionCallback_(channel, value);
    }
  }

 private:
  void onTimer() {
    uint8_t c = getChannelCount();
    if (c > 0) {
      triggerConversion(currentChannel_++);
      if (currentChannel_ >= c) {
        currentChannel_ = 0;
      }
    }
  };
  Logger LOG;
  SingleTimer timer_;
  uint8_t currentChannel_;
  ConversionCallback_t conversionCallback_;
};