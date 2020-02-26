#pragma once

#include <Arduino.h>
#include <functional>

#include <TMC429.h>

#include "hal/clocksource.h"
#include "hal/translator.h"
#include "util/logger.h"
#include "util/singletimer.h"

// requires https://platformio.org/lib/show/5822/TMC429
class MotorDriver {
 public:
  typedef std::function<void(uint8_t axisIndex, const std::array<bool, 3>& axisMoving)> movementStatusChangeCallback_t;
  typedef struct {
    uint32_t velocityMinHz;
    uint32_t velocityMaxHz;
    uint32_t acceleration_max_hz_per_s;
  } Limit_t;

  MotorDriver();
  bool begin(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, std::array<Limit_t*, 3> limits, std::array<Translator*, 3> translators);
  void loop();
  void statistic();

  void goTo(uint8_t axisIndex, double pos);
  void jogTo(uint8_t axisIndex, double delta);
  void jogV(uint8_t axisIndex, double revPerSecond);
  bool isAtTargetPos(uint8_t axisIndex);

  void onStatusChange(movementStatusChangeCallback_t cb);

 protected:
  void initMotor(uint8_t axisIndex,Limit_t* limit);
  void updateState();
  void updateAxis(uint8_t axisIndex, bool isMoving);

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