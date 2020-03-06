#pragma once

#include <Arduino.h>
#include <functional>

class Incrementor {
 public:
  /**
   * @param indexDiff +1 or -1
   */
  typedef std::function<void(int8_t indexDiff)> IndexChangeCallback_t;

  Incrementor(uint16_t sensitivity) : sensitivity_(sensitivity), pos_(0) {}

  void reset() { pos_ = 0; }
  int16_t getIndex() { return pos_ / sensitivity_; }

  void onIndexChange(IndexChangeCallback_t cb) { indexChangeCallback_ = cb; }

  bool incPos() {
    int16_t oldIndex = getIndex();
    pos_++;
    int16_t newIndex = getIndex();
    bool changed = oldIndex != newIndex;
    if (changed) {
      pos_ -= sensitivity_;
      if (indexChangeCallback_) {
        indexChangeCallback_(+1);
      }
    }
    return changed;
  }

  bool decPos() {
    int16_t oldIndex = getIndex();
    pos_--;
    int16_t newIndex = getIndex();
    bool changed = oldIndex != newIndex;
    if (changed) {
      pos_ += sensitivity_;
      if (indexChangeCallback_) {
        indexChangeCallback_(-1);
      }
    }
    return changed;
  }

  void add(int16_t diff) {
    if (diff > 0) {
      for (int16_t i = 0; i < diff; ++i) {
        incPos();
      }
    } else if (diff < 0) {
      for (int16_t i = 0; i > diff; --i) {
        decPos();
      }
    }
  }

 private:
  int16_t sensitivity_;
  int16_t pos_;
  IndexChangeCallback_t indexChangeCallback_;
};