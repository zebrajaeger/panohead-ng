#pragma once

#include <Arduino.h>
#include <functional>

class Counter {
 public:
  typedef std::function<void(uint16_t from, uint16_t to)> IndexChangeCallback_t;

  Counter(uint16_t sensitivity, uint16_t initialIndex = 0) : sensitivity_(sensitivity), pos_(initialIndex), indexChangeCallback_() {}
  void add(int16_t diff) {
    uint16_t oldIndex = getIndex();
    pos_ += diff;
    uint16_t newIndex = getIndex();
    if (oldIndex != newIndex) {
      checkIndex(oldIndex, newIndex);
    }
    if (indexChangeCallback_) {
      newIndex = getIndex();
      indexChangeCallback_(oldIndex, newIndex);
    }
  }
  uint16_t getIndex() { return pos_ / sensitivity_; }
  void onIndexChange(IndexChangeCallback_t cb) { indexChangeCallback_ = cb; }

 protected:
  virtual void checkIndex(uint16_t from, uint16_t to){};
  uint16_t getPos() { return pos_; }
  void setIndex(uint16_t index) {
    uint16_t sub = pos_ % sensitivity_;
    pos_ = index * sensitivity_ + sub;
  }

 private:
  uint16_t sensitivity_;
  uint16_t pos_;
  IndexChangeCallback_t indexChangeCallback_;
};

class RangeCounter : public Counter {
 public:
  RangeCounter(uint16_t sensitivity, uint16_t initialIndex, uint16_t minIndex, uint16_t maxIndex, bool circular)
      : Counter(sensitivity, initialIndex), min_(minIndex), max_(maxIndex), isCircular_(circular) {}

  virtual void checkIndex(uint16_t from, uint16_t to) {
    if (to > max_) {
      if (isCircular_) {
        setIndex(min_);
      } else {
        setIndex(max_);
      }
    } else if (to < min_) {
      if (isCircular_) {
        setIndex(max_);
      } else {
        setIndex(min_);
      }
    }
  };

 private:
  uint16_t min_;
  uint16_t max_;
  bool isCircular_;
};
