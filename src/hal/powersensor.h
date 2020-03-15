#pragma once

#include <Arduino.h>
#include <functional>

#include <Adafruit_INA219.h>

#include "util/loggerfactory.h"
#include "util/singletimer.h"

#include "data/power.h"

class PowerSensor {
 public:
  typedef std::function<void(const Power&)> ConversionCallback_t;

  PowerSensor();
  bool begin(uint32_t periodMs = 50);
  void loop(){timer_.loop();}
  void onData(ConversionCallback_t cb) { conversionCallback_ = cb; }
  void statistics();

 private:
  void onTimer();

  Logger& LOG;
  SingleTimer timer_;
  ConversionCallback_t conversionCallback_;
  Adafruit_INA219 ina219_;
};