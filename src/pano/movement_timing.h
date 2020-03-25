#pragma once

#include <Arduino.h>

class MovementTiming {
 public:
  MovementTiming() : delayAfterMoveMs_(0), delayBetweenShotsMs_(0), delayAfterLastShotMs_(0) {}
  MovementTiming(uint32_t delayAfterMoveMs, uint32_t delayBetweenShotsMs, uint32_t delayAfterLastShotMs)
      : delayAfterMoveMs_(delayAfterMoveMs), delayBetweenShotsMs_(delayBetweenShotsMs), delayAfterLastShotMs_(delayAfterLastShotMs) {}

  uint32_t getDelayAfterMoveMs() const { return delayAfterMoveMs_; }
  void setDelayAfterMoveMs(uint32_t delayAfterMoveMs) { delayAfterMoveMs_ = delayAfterMoveMs; }

  uint32_t getDelayBetweenShotsMs() const { return delayBetweenShotsMs_; }
  void setDelayBetweenShotsMs(uint32_t delaybetweenShotsMs) { delayBetweenShotsMs_ = delaybetweenShotsMs; }

  uint32_t getDelayAfterLastShotMs() const { return delayAfterLastShotMs_; }
  void setDelayAfterLastShotMs(uint32_t delayAfterLastShotMs) { delayAfterLastShotMs_ = delayAfterLastShotMs; }

  bool operator==(const MovementTiming& b) const {
    return delayAfterMoveMs_ == b.delayAfterMoveMs_ && delayBetweenShotsMs_ == b.delayBetweenShotsMs_ &&
           delayAfterLastShotMs_ == b.delayAfterLastShotMs_;
  }

 private:
  uint32_t delayAfterMoveMs_;
  uint32_t delayBetweenShotsMs_;
  uint32_t delayAfterLastShotMs_;
};