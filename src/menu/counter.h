#pragma once

#include <Arduino.h>
#include <functional>

class Counter {
 public:
  typedef std::function<void(int16_t from, int16_t to)> IndexChangeCallback_t;

  Counter(uint16_t sensitivity, int16_t initialIndex, int16_t minIndex, int16_t maxIndex, bool circular)
      : sensitivity_(sensitivity), pos_(initialIndex), indexChangeCallback_(), min_(minIndex), max_(maxIndex), isCircular_(circular) {}
  bool add(int16_t diff) {
    int16_t oldIndex = getIndex();
    pos_ += diff;
    int16_t newIndex = getIndex();
    if (oldIndex != newIndex) {
      checkIndex(oldIndex, newIndex);
    }
    newIndex = getIndex();
    if (indexChangeCallback_) {
      indexChangeCallback_(oldIndex, newIndex);
    }
    return oldIndex != newIndex;
  }
  uint16_t getIndex() { return pos_ / sensitivity_; }
  void onIndexChange(IndexChangeCallback_t cb) { indexChangeCallback_ = cb; }
  uint16_t getMin() const { return min_; }
  void setMin(uint16_t min) { min_ = min; }
  uint16_t getMax() const { return max_; }
  void setMax(uint16_t max) { max_ = max; }

 protected:
  virtual void checkIndex(int16_t oldIndex, int16_t newIndex) {
    if (newIndex > max_) {
      if (isCircular_) {
        setIndex(min_);
      } else {
        setIndex(max_);
      }
    } else if (newIndex < min_) {
      if (isCircular_) {
        setIndex(max_);
      } else {
        setIndex(min_);
      }
    }
  };
  uint16_t getPos() { return pos_; }
  void setIndex(uint16_t index) {
    uint16_t sub = pos_ % sensitivity_;
    pos_ = index * sensitivity_ + sub;
  }

 private:
  uint16_t sensitivity_;
  int16_t pos_;
  IndexChangeCallback_t indexChangeCallback_;
  int16_t min_;
  int16_t max_;
  bool isCircular_;
};
