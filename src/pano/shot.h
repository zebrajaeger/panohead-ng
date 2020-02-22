#pragma once

#include <Arduino.h>
#include <vector>

namespace pano {
class Shot {
 public:
  Shot(uint32_t focusTimeMs, uint32_t triggerTimeMs) : focusTimeMs_(focusTimeMs), triggerTimeMs_(triggerTimeMs) {}
  uint32_t getFocusTimeMs() const { return focusTimeMs_; }
  uint32_t getTriggerTimeMs() const { return triggerTimeMs_; }

 private:
  uint32_t focusTimeMs_;
  uint32_t triggerTimeMs_;
};

class Shots {
 public:
  Shots(uint32_t delayAfterMoveMs, uint32_t delayBetweenShotsMs, uint32_t delayAfterLastShotMs)
      : delayAfterMoveMs_(delayAfterMoveMs), delayBetweenShotsMs_(delayBetweenShotsMs), delayAfterLastShotMs_(delayAfterLastShotMs) {}

  uint16_t getShotCount() const { return shots_.size(); }
  Shots& operator+=(const Shot& shot) {
    shots_.push_back(shot);
    return *this;
  }
  const Shot& operator[](size_t index) const { return shots_[index]; }

  uint32_t getDelayAfterMoveMs() { return delayAfterMoveMs_; }
  uint32_t getDelayBetweenShotsMs() { return delayBetweenShotsMs_; }
  uint32_t getDelayAfterLastShotMs() { return delayAfterLastShotMs_; }

 private:
  uint32_t delayAfterMoveMs_;
  uint32_t delayBetweenShotsMs_;
  uint32_t delayAfterLastShotMs_;
  std::vector<Shot> shots_;
};
}