#pragma once

#include <Arduino.h>

#include <array>

#include "shot.h"

class ShotRow {
 public:
  ShotRow() : shots_() {}

  uint32_t getCapacity() const { return (uint32_t)(shots_.size()); }
  uint32_t getShotCount() const {
    uint32_t count = 0;
    for (uint32_t i = 0; i < getCapacity(); ++i) {
      if (shots_[(std::size_t)i]) {
        ++count;
      }
    }
    return count;
  }

  const Shot* getShot(uint32_t index) const { return const_cast<ShotRow*>(this)->getShot(index); }

  Shot* getShot(uint32_t index) {
    uint32_t count = 0;
    uint32_t size = getCapacity();
    for (uint32_t i = 0; i < size; ++i) {
      if (shots_[(std::size_t)i]) {
        if (count++ == index) {
          return &shots_[(std::size_t)i];
        }
      }
    }
    return NULL;
  }

  const Shot* operator[](uint32_t index) const { return getShot(index); }

  void compact() {
    uint32_t target = findNextDisabled(0);
    uint32_t source = findNextEnabled(target + 1);
    while (source != -1 && target != -1) {
      move(source, target);
      target = findNextDisabled(target + 1);
      source = findNextEnabled(source + 1);
    }
  }

  bool operator==(const ShotRow& b) const {
    if (getCapacity() != b.getCapacity()) {
      return false;
    }

    if (getShotCount() != b.getShotCount()) {
      return false;
    }

    uint32_t size = getShotCount();
    for (uint32_t i = 0; i < size; ++i) {
      if (getShot(i) != b.getShot(i)) {
        return false;
      }
    }
    return true;
  }

  bool isComplete() {
    uint32_t size = getCapacity();
    for (uint32_t i = 0; i < size; ++i) {
      if (!shots_[(std::size_t)i].isComplete()) {
        return false;
      }
    }
    return true;
  }

 private:
  void move(uint32_t from, uint32_t to) {
    shots_[(std::size_t)from] = shots_[(std::size_t)to];
    shots_[(std::size_t)from].disable();
  }
  void swap(uint32_t a, uint32_t b) {
    Shot temp = shots_[(std::size_t)a];
    shots_[(std::size_t)a] = shots_[(std::size_t)b];
    shots_[(std::size_t)b] = temp;
  }

  /**
   * @return -1 if nothing found, otherwise index within shots_
   */
  int32_t findNextEnabled(uint32_t from) const {
    uint32_t size = getCapacity();
    for (uint32_t i = from; i < size; ++i) {
      if (shots_[(std::size_t)i]) {
        return i;
      }
    }
    return -1;
  }

  /**
   * @return -1 if nothing found, otherwise index within shots_
   */
  int32_t findNextDisabled(uint32_t from) const {
    uint32_t size = getCapacity();
    for (uint32_t i = from; i < size; ++i) {
      if (!shots_[(std::size_t)i]) {
        return i;
      }
    }
    return -1;
  }

  std::array<Shot, 10> shots_;
};