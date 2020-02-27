#pragma once

#include <Arduino.h>

class Selection {
 public:
  Selection(int16_t max, uint16_t stepsPerIndex_, bool circular = false)
      : stepsPerIndex_(stepsPerIndex_), index_(0), min_(0 * stepsPerIndex_), max_(((max + 1) * stepsPerIndex_) - 1), circular_(circular) {}

  bool add(int16_t delta) { return setIndex(index_ + delta); }

  bool setIndex(int16_t v) {
    int16_t oldIndex = getIndex();

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

    return oldIndex != getIndex();
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
