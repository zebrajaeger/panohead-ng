#pragma once

#include <Arduino.h>

class PanoPosition {
 public:
  PanoPosition() : shotCount_(0), shotIndex_(0), positionCount_(0), positionIndex_(0) {}

  uint32_t getShotCount() const { return shotCount_; }
  uint32_t getShotIndex() const { return shotIndex_; }

  uint32_t getPositionCount() const { return positionCount_; }
  uint32_t getPositionIndex() const { return positionIndex_; }

  void reset() { positionIndex_ = 0; }

  void reset(uint32_t positionCount, uint32_t shotCount) {
    positionCount_ = positionCount;
    positionIndex_ = 0;

    shotCount_ = shotCount;
    shotIndex_ = 0;
  }

  PanoPosition& operator++() {
    next();
    return *this;
  }

  PanoPosition& next() {
    if (shotIndex_++ >= shotCount_) {
      shotIndex_ = 0;
    }
    return *this;
  }

  bool isFirstShot() const { return shotIndex_ == 0; }
  bool isLastShot() const { return shotIndex_ == (shotCount_ - 1); }
  bool hasMoreShots() const { return !isLastShot(); }

  bool isFirstPosition() const { return positionIndex_ == 0; }
  bool isLastPosition() const { return positionIndex_ == (positionCount_ - 1); }
  bool hasMorePositions() const { return !isLastPosition(); }

  operator bool() const { return positionIndex_ < positionCount_; }

 private:
  uint32_t shotCount_;
  uint32_t shotIndex_;

  uint32_t positionCount_;
  uint32_t positionIndex_;
};