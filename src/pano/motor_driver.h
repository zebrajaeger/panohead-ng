#pragma once

#include <Arduino.h>
#include <functional>

#include <TMC429.h>

#include "hal/clocksource.h"
#include "pano/translator.h"
#include "util/logger.h"
#include "util/singletimer.h"

// class Actor;
class MotorDriver {
 public:
  typedef std::function<void(uint8_t axisIndex, const std::array<bool, 3>& axisMoving)> movementStatusChangeCallback_t;

  MotorDriver();
  bool begin(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, std::array<Translator*, 3> translators);
  void statistic();
  void goTo(uint8_t axisIndex, double pos);
  void loop();
  void onStatusChange(movementStatusChangeCallback_t cb);

 protected:
  void initMotor(uint8_t axisIndex);
  void updateState();
  void updateAxis(uint8_t axis, bool isMoving);

 private:
  Logger LOG;
  ClockSource clockSource_;
  TMC429 tmc429_;
  std::array<Translator*, 3> translators_;
  std::array<int64_t, 3> axisOffset_;
  SingleTimer timer_;
  std::array<bool, 3> axisMoving_;
  movementStatusChangeCallback_t movementStatusChangeCallback_;
};