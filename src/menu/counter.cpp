#include "counter.h"

Counter::Counter(uint16_t sensitivity, int16_t initialIndex, int16_t minIndex, int16_t maxIndex, bool circular)
    : sensitivity_(sensitivity),
      pos_(initialIndex),
      indexChangeCallback_(),
      isEnabledCallback_(),
      min_(minIndex),
      max_(maxIndex),
      isCircular_(circular) {}

void Counter::addToPos(int16_t diff) {
  if (diff > 0) {
    for (int16_t i = 0; i < diff; ++i) {
      int16_t oldIndex = getIndex();
      if (incPos()) {
        if (goToNextEnabledIndex()) {
          int16_t newIndex = getIndex();
          if (indexChangeCallback_) {
            indexChangeCallback_(oldIndex, newIndex);
          }
          oldIndex = newIndex;
        }
      } 
    }

  } else if (diff < 0) {
    for (int16_t i = 0; i > diff; --i) {
      int16_t oldIndex = getIndex();
      if (decPos()) {
        if (goToPreviousEnabledIndex()) {
          int16_t newIndex = getIndex();
          if (indexChangeCallback_) {
            indexChangeCallback_(oldIndex, newIndex);
          }
          oldIndex = newIndex;
        }
      }
    }
  }
}

uint16_t Counter::getPos() { return pos_; }

/**
 * returns true if index has been changed
 */
bool Counter::incPos() {
  int16_t oldIndex = getIndex();
  pos_++;
  int16_t newIndex = getIndex();

  if (newIndex > max_) {
    if (isCircular_) {
      setIndexToMin();
    } else {
      setIndexToMax();
    }
    newIndex = getIndex();
  }

  return oldIndex != newIndex;
}

/**
 * returns true if index has been changed
 */
bool Counter::decPos() {
  int16_t oldIndex = getIndex();
  pos_--;
  int16_t newIndex = getIndex();

  if (newIndex < min_) {
    if (isCircular_) {
      setIndexToMax();
    } else {
      setIndexToMin();
    }
    newIndex = getIndex();
  }

  return oldIndex != newIndex;
}

uint16_t Counter::getIndex() { return pos_ / sensitivity_; }

/**
 * returns true if index has been changed
 */
bool Counter::incIndex() {
  pos_ += sensitivity_;
  return getIndex() > max_;
}

/**
 * returns true if index has been changed
 */
bool Counter::decIndex() {
  pos_ -= sensitivity_;
  return getIndex() < min_;
}

void Counter::setIndex(uint16_t index) {
  uint16_t sub = pos_ % sensitivity_;
  pos_ = index * sensitivity_ + sub;
}

void Counter::setIndexToMin() { setIndex(min_); }

void Counter::setIndexToMax() { setIndex(max_); }

bool Counter::isEnabled(int16_t index) {
  if (isEnabledCallback_) {
    return isEnabledCallback_(index);
  } else {
    return true;
  }
}

bool Counter::isEnabled() { return isEnabled(getIndex()); }

/**
 * @return true if new index could be found
 * */
bool Counter::goToNextIndex() {
  if (incIndex()) {
    if (isCircular_) {
      setIndexToMin();
    } else {
      setIndexToMax();
      return false;
    }
  }
  return true;
}

/**
 * @return true if new index could be found
 * */
bool Counter::goToPreviousIndex() {
  if (decIndex()) {
    if (isCircular_) {
      setIndexToMax();
    } else {
      setIndexToMin();
      return false;
    }
  }
  return true;
}

/**
 * return true if new index could be found
 * */
bool Counter::goToNextEnabledIndex() {
  if (isEnabled()) {
    return true;
  }

  int16_t oldPos = pos_;
  for (int16_t i = 0; i < max_ && goToNextIndex(); ++i) {
    if (isEnabled()) {
      return true;
    }
  }
  pos_ = oldPos;
  return false;
}

/**
 * return true if new index could be found
 * */
bool Counter::goToPreviousEnabledIndex() {
  if (isEnabled()) {
    return true;
  }

  int16_t oldPos = pos_;
  for (int16_t i = 0; i < max_ && goToPreviousIndex(); ++i) {
    if (isEnabled()) {
      return true;
    }
  }
  pos_ = oldPos;
  return false;
}

void Counter::onIndexChange(IndexChangeCallback_t cb) { indexChangeCallback_ = cb; }

void Counter::onIsEnabled(IsEnabledCallback_t cb) { isEnabledCallback_ = cb; }

uint16_t Counter::getMin() const { return min_; }

void Counter::setMin(uint16_t min) { min_ = min; }

uint16_t Counter::getMax() const { return max_; }

void Counter::setMax(uint16_t max) { max_ = max; }
