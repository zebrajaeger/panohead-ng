#pragma once

#include <Arduino.h>

#include <functional>

template <typename T>
class Debouncer {
 public:
  typedef std::function<void(T newValue)> StateChangeCallback_t;

  Debouncer() : lockTimeUs_(0), startTimeUs_(0), locked_(false), pendingNotification_(false) {}

  bool begin(uint32_t lockTimeMs) {
    lockTimeUs_ = lockTimeMs * 1000;
    return true;
  }

  void loop() {
    // send notification if pending
    if (pendingNotification_) {
      pendingNotification_ = false;
      if (stateChangeCallback_) {
        stateChangeCallback_(notificationValue_);
      }
    }

    // unlock on timeout
    if (locked_) {
      if (startTimeUs_ + lockTimeUs_ <= micros()) {
        locked_ = false;
      }
    }
  }

  void triggerChange(T newState) {
    if (!locked_) {
      locked_ = true;
      pendingNotification_ = true;
      notificationValue_ = newState;
      startTimeUs_ = micros();
    }
  }

  void onChange(StateChangeCallback_t cb) { stateChangeCallback_ = cb; }

 private:
  uint64_t lockTimeUs_;

  uint64_t startTimeUs_;
  bool locked_;
  bool pendingNotification_;
  T notificationValue_;

  StateChangeCallback_t stateChangeCallback_;
};