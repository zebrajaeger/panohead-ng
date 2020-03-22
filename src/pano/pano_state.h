#pragma once

#include <Arduino.h>

#include "automat_state.h"
#include "pano_position.h"
#include "util/logger.h"

class PanoState {
 public:
  PanoState() : LOG(LoggerFactory::getLogger("PanoState")), automatState_(AutomatState_t::UNINITIALIZED), panoPosition_() {}

  void reset() {
    automatState_ = AutomatState_t::UNINITIALIZED;
    panoPosition_.reset();
  }

  AutomatState_t setState(AutomatState_t newState) {
    AutomatState_t old = automatState_;
    automatState_ = newState;
    return old;
  }

  AutomatState_t getAutomatState() { return automatState_; }
  const PanoPosition& getPanoPosition() const { return panoPosition_; }
  PanoPosition& getPanoPosition() { return panoPosition_; }

  void statistic() {
    LOG.i("automatState_: %s", stateToName(automatState_));
    panoPosition_.statistic();
  }

 private:
  Logger& LOG;
  AutomatState_t automatState_;
  PanoPosition panoPosition_;
};
