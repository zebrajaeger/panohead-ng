#include "selector.h"

Selector::Selector(int16_t initialIndex, int16_t minIndex, int16_t maxIndex, bool circular)
    : index_(initialIndex), selectionChangedCallback_(), isEnabledCallback_(), min_(minIndex), max_(maxIndex), isCircular_(circular) {}

bool Selector::up() {
  int16_t oldIndex = index_;
  int16_t index = index_;
  if (getNextIndex(index) && findEnabledIndexUp(index)) {
    index_ = index;
    if (selectionChangedCallback_) {
      selectionChangedCallback_(oldIndex, index_);
    }

    return true;
  }
  return false;
}

bool Selector::down() {
  int16_t oldIndex = index_;
  int16_t index = index_;
  if (getPreviousIndex(index) && findEnabledIndexDown(index)) {
    index_ = index;
    if (selectionChangedCallback_) {
      selectionChangedCallback_(oldIndex, index_);
    }
    return true;
  }
  return false;
}

uint16_t Selector::getIndex() { return index_; }

bool Selector::isEnabled(int16_t index) {
  bool result = true;
  if (isEnabledCallback_) {
    result = isEnabledCallback_(index);
  }
  return result;
}

bool Selector::getNextIndex(int16_t& index) {
  if (index + 1 > max_) {
    if (isCircular_) {
      index = min_;
      return true;
    } else {
      return false;
    }
  }
  index++;
  return true;
}

bool Selector::getPreviousIndex(int16_t& index) {
  if (index - 1 < min_) {
    if (isCircular_) {
      index = max_;
      return true;
    } else {
      return false;
    }
  }
  index--;
  return true;
}

bool Selector::findEnabledIndexUp(int16_t& index) {
  int16_t startPos = index;
  int16_t currentPos = index;

  do {
    if (isEnabled(currentPos)) {
      index = currentPos;
      return true;
    } else {
      if (!getNextIndex(currentPos)) {
        break;
      }
    }
  } while (currentPos != startPos);
  return false;
}

bool Selector::findEnabledIndexDown(int16_t& index) {
  int16_t startPos = index;
  int16_t currentPos = index;

  do {
    if (isEnabled(currentPos)) {
      index = currentPos;
      return true;
    } else {
      if (!getPreviousIndex(currentPos)) {
        break;
      }
    }
  } while (currentPos != startPos);
  return false;
}

void Selector::onSelectionChanged(SelectionChangedCallback_t cb) { selectionChangedCallback_ = cb; }

void Selector::onIsEnabled(IsEnabledCallback_t cb) { isEnabledCallback_ = cb; }

uint16_t Selector::getMin() const { return min_; }

void Selector::setMin(uint16_t min) { min_ = min; }

uint16_t Selector::getMax() const { return max_; }

void Selector::setMax(uint16_t max) { max_ = max; }
