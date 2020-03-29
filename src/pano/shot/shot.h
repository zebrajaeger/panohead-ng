#pragma once

#include <Arduino.h>

class Shot {
 public:
  Shot() : isEnabled_(false), focusTimeMs_(0), triggerTimeMs_(0) {}
  Shot(uint32_t focusTimeMs, uint32_t triggerTimeMs) : isEnabled_(true), focusTimeMs_(focusTimeMs), triggerTimeMs_(triggerTimeMs) {}

  // enable
  bool getEnabled() const { return isEnabled_; }
  void setEnabled(bool isEnabled) { isEnabled_ = isEnabled; }
  void enable() { setEnabled(true); }
  void disable() { setEnabled(false); }

  // focus time
  uint32_t getFocusTimeMs() const { return focusTimeMs_; }
  void setFocusTimeMs(uint32_t focusTimeMs) { focusTimeMs_ = focusTimeMs; }

  // trigger time
  uint32_t getTriggerTimeMs() const { return triggerTimeMs_; }
  void setTriggerTimeMs(uint32_t triggerTimeMs) { triggerTimeMs_ = triggerTimeMs; }

  bool isComplete() { return isEnabled_ ? focusTimeMs_ > 0 && triggerTimeMs_ > 0 : true; }

  bool operator==(const Shot& b) const { return focusTimeMs_ == b.focusTimeMs_ && triggerTimeMs_ == b.triggerTimeMs_; }
  bool operator!=(const Shot& b) const { return focusTimeMs_ != b.focusTimeMs_ || triggerTimeMs_ != b.triggerTimeMs_; }
  operator bool() const { return isEnabled_; }

 private:
  bool isEnabled_;
  uint32_t focusTimeMs_;
  uint32_t triggerTimeMs_;
};