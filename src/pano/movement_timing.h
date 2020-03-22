#pragma once

#include <Arduino.h>

class MovementTiming {
 public:
  MovementTiming() : delayAfterMoveMs_(0), delayBetweenShotsMs_(0), delayAfterLastShotMs_(0) {}
  MovementTiming(uint32_t delayAfterMoveMs, uint32_t delayBetweenShotsMs, uint32_t delayAfterLastShotMs)
      : delayAfterMoveMs_(delayAfterMoveMs), delayBetweenShotsMs_(delayBetweenShotsMs), delayAfterLastShotMs_(delayAfterLastShotMs) {}

  uint32_t getDelayAfterMoveMs() { return delayAfterMoveMs_; }
  uint32_t getDelayBetweenShotsMs() { return delayBetweenShotsMs_; }
  uint32_t getDelayAfterLastShotMs() { return delayAfterLastShotMs_; }

 private:
  uint32_t delayAfterMoveMs_;
  uint32_t delayBetweenShotsMs_;
  uint32_t delayAfterLastShotMs_;
};