#pragma once

#include <Arduino.h>

#include "automat_state.h"
#include "pano_position.h"

class PanoState {
 public:
  PanoState() : automatState_(AutomatState_t::UNINITIALIZED), panoPosition_() {}

  void reset() {
    automatState_ = AutomatState_t::UNINITIALIZED;
    panoPosition_.reset();
  }

  AutomatState_t setState(AutomatState_t newState) {
    AutomatState_t old = automatState_;
    automatState_ = newState;
    return old;
  }

  AutomatState_t getAutomatState() const { return automatState_; }
  const PanoPosition& getPanoPosition() const { return panoPosition_; }
  PanoPosition& getPanoPosition() { return panoPosition_; }

  bool operator==(const PanoState& b) const { return automatState_ == b.automatState_ && panoPosition_ == b.panoPosition_; }

 private:
  AutomatState_t automatState_;
  PanoPosition panoPosition_;
};
