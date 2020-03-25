#pragma once

#include <Arduino.h>

class Position {
 public:
  Position() : x_(), y_() {}
  Position(double x, double y) : x_(x), y_(y) {}

  void setX(double x) { x_ = x; }
  double getX() const { return x_; }

  void setY(double y) { y_ = y; }
  double getY() const { return y_; }

  bool operator==(const Position& b) const {
    return (x_ == b.x_) && (y_ == b.y_);
  }

 private:
  double x_;
  double y_;
};