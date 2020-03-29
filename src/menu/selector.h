#pragma once

#include <Arduino.h>

#include <functional>

class Selector {
 public:
  typedef std::function<void(int16_t from, int16_t to)> SelectionChangedCallback_t;
  typedef std::function<bool(int16_t index)> IsEnabledCallback_t;

  Selector(int16_t initialIndex, int16_t minIndex, int16_t maxIndex, bool circular);

  bool first();
  bool up();
  bool down();

  uint16_t getIndex() { return index_; }
  bool getNextIndex(int16_t& index);
  bool getPreviousIndex(int16_t& index);
  bool findEnabledIndexUp(int16_t& index);
  bool findEnabledIndexDown(int16_t& index);

  bool isEnabled(int16_t index);

  void onSelectionChanged(SelectionChangedCallback_t cb) { selectionChangedCallback_ = cb; }
  void onIsEnabled(IsEnabledCallback_t cb) { isEnabledCallback_ = cb; }

  uint16_t getMin() const { return min_; }
  void setMin(uint16_t min) { min_ = min; }
  uint16_t getMax() const { return max_; }
  void setMax(uint16_t max) { max_ = max; }

 private:
  int16_t index_;
  SelectionChangedCallback_t selectionChangedCallback_;
  IsEnabledCallback_t isEnabledCallback_;
  int16_t min_;
  int16_t max_;
  bool isCircular_;
};
