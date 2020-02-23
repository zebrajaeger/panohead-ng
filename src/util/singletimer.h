#pragma once

#include <Arduino.h>
#include <functional>

#include "util/logger.h"

class SingleTimer {
 public:
  SingleTimer(const String& name);

  void startS(double periodS, bool strict = false, bool recurrent = false);
  void startS(double periodS, bool strict, bool recurrent, std::function<void()> cb);

  void startMs(uint64_t periodMs, bool strict = false, bool recurrent = false);
  void startMs(uint64_t periodMs, bool strict, bool recurrent, std::function<void()> cb);

  void startUs(uint64_t periodUs, bool strict = false, bool recurrent = false);
  void startUs(uint64_t periodUs, bool strict, bool recurrent, std::function<void()> cb);

  void stop();
  void onTimer(std::function<void()> cb);
  bool isRunning();

  uint32_t getMsToNextEvent();

  void loop();

 protected:
  void trigger();

 private:
  Logger LOG;
  bool running_;
  bool recurrent_;
  bool strict_;
  uint64_t periodUs_;
  uint64_t nextEvent_;
  std::function<void()> cb_;
};