#pragma once

#include <Arduino.h>
#include <functional>

class Counter {
 public:
  typedef std::function<void(int16_t from, int16_t to)> IndexChangeCallback_t;
  typedef std::function<bool(int16_t index)> IsEnabledCallback_t;

  Counter(uint16_t sensitivity, int16_t initialIndex, int16_t minIndex, int16_t maxIndex, bool circular);

  bool addToPos(int16_t diff);
  uint16_t getPos();
  bool incPos();
  bool decPos();

  uint16_t getIndex();
  bool incIndex();
  bool decIndex();
  void setIndex(uint16_t index);
  void setIndexToMin();
  void setIndexToMax();

  bool isEnabled(int16_t index);
  bool isEnabled();

  bool goToNextIndex();
  bool goToPreviousIndex();
  bool goToNextEnabledIndex();
  bool goToPreviousEnabledIndex();

  void onIndexChange(IndexChangeCallback_t cb);
  void onIsEnabled(IsEnabledCallback_t cb);

  uint16_t getMin() const;
  void setMin(uint16_t min);
  uint16_t getMax() const;
  void setMax(uint16_t max);

 private:
  uint16_t sensitivity_;
  int16_t pos_;
  IndexChangeCallback_t indexChangeCallback_;
  IsEnabledCallback_t isEnabledCallback_;
  int16_t min_;
  int16_t max_;
  bool isCircular_;
};
