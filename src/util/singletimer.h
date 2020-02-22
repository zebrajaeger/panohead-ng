#pragma once

#include <Arduino.h>
#include <functional>

#include "util/logger.h"

class SingleTimer {
 public:
  SingleTimer(const String& name) : LOG("SingleTimer(" + name + ")"), running_(false) {}

  void start(uint64_t periodUs, bool strict = false, bool recurrent = false) {
    periodUs_ = periodUs;
    strict_ = strict;
    recurrent_ = recurrent;
    nextEvent_ = micros() + periodUs;
    running_ = true;
  }

  void stop() { running_ = false; }
  void onTimer(std::function<void()> cb) { cb_ = cb; }

  bool isRunning() { return running_; }

  uint32_t getMsToNextEvent() { return (nextEvent_ - micros()) / 1000; }

  void loop() {
    if (!running_) {
      return;
    }

    uint64_t now = micros();
    if (now >= nextEvent_) {
      if (recurrent_) {
        if (strict_) {
          nextEvent_ = nextEvent_ + periodUs_;
          trigger();
          if (nextEvent_ < micros()) {
            LOG.w("Event handler needs more time than timer period length is");
          }
        } else {
          trigger();
          nextEvent_ = micros() + periodUs_;
        }
      } else {
        running_ = false;
        trigger();
      }
    }
  }

 protected:
  void trigger() {
    if (cb_) {
      cb_();
    }
  }

 private:
  Logger LOG;
  bool running_;
  bool recurrent_;
  bool strict_;
  uint64_t periodUs_;
  uint64_t nextEvent_;
  std::function<void()> cb_;
};