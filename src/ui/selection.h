#pragma once

#include <Arduino.h>

class Selection {
 public:
  Selection(int16_t max, uint16_t stepsPerIndex_, bool circular = false)
      : stepsPerIndex_(stepsPerIndex_), index_(0), min_(0 * stepsPerIndex_), max_(((max + 1) * stepsPerIndex_) - 1), circular_(circular) {}

  void add(int16_t delta) { setIndex(index_ + delta); }

  void setIndex(int16_t v) {
    if (v < min_) {
      if (circular_) {
        v = v % getSize();
      } else {
        v = min_;
      }
    }
    if (v > max_) {
      if (circular_) {
        v = v % getSize();
      } else {
        v = max_;
      }
    }
    index_ = v;
  }

  int16_t getIndex() const { return index_ / stepsPerIndex_; }
  bool isCircular() const { return circular_; }

 private:
  int16_t getSize() const { return max_ - min_; }
  uint16_t stepsPerIndex_;
  int16_t index_;
  int16_t min_;
  int16_t max_;
  bool circular_;
};