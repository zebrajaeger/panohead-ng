#pragma once

#include <Arduino.h>
#include <functional>

#include "util/loggerfactory.h"

class SingleTimer {
 public:
  typedef std::function<void()> callback_t;

  SingleTimer(const std::string& name);

  void startS(double periodS, bool strict = false, bool recurrent = false);
  void startS(double periodS, bool strict, bool recurrent, callback_t cb);

  void startMs(uint64_t periodMs, bool strict = false, bool recurrent = false);
  void startMs(uint64_t periodMs, bool strict, bool recurrent, callback_t cb);

  void startUs(uint64_t periodUs, bool strict = false, bool recurrent = false);
  void startUs(uint64_t periodUs, bool strict, bool recurrent, callback_t cb);

  void stop();
  void onTimer(callback_t cb);
  bool isRunning();

  uint32_t getMsToNextEvent();

  void loop();

 protected:
  void trigger();

 private:
  Logger &LOG;
  bool running_;
  bool recurrent_;
  bool strict_;
  uint64_t periodUs_;
  uint64_t nextEvent_;
  callback_t cb_;
};