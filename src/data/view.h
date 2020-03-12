#pragma once

#include <Arduino.h>

#include "optional.h"

class View {
 public:
  View() : x1_(0.0), x2_(0.0), y1_(0.0), y2_(0.0), isPartial_(true) {}

  View(double x1, double x2, double y1, double y2) : x1_(x1), x2_(x2), y1_(y1), y2_(y2), isPartial_(true) {}
  View(double y1, double y2) : x1_(0.0), x2_(0.0), y1_(y1), y2_(y2), isPartial_(false) {}

  double getX1() const { return *x1_; }
  void setX1(double v) { x1_ = v; }

  double getX2() const { return *x2_; }
  void setX2(double v) { x2_ = v; }

  double getY1() const { return *y1_; }
  void setY1(double v) { y1_ = v; }

  double getY2() const { return *y2_; }
  void setY2(double v) { y2_ = v; }

  double calculateWidth() const { return isPartial_ ? *x2_ - *x1_ : 1.0; }
  double calculateHeight() const { return *y2_ - *y1_; }

  bool isPartial() const { return isPartial_; }
  void setPartial(bool partial) { isPartial_ = partial; }
  bool togglePartial() {
    isPartial_ = !isPartial_;
    return isPartial_;
  }

  bool isComplete() {
    if (isPartial_) {
      return y1_ && y2_;
    } else {
      return x1_ && x2_ && y1_ && y2_;
    }
  }

  bool operator==(const View &b) const { return x1_ == b.x1_ && x2_ == b.x2_ && y1_ == b.y1_ && y2_ == b.y2_; }

 private:
  Optional<double> x1_;
  Optional<double> x2_;
  Optional<double> y1_;
  Optional<double> y2_;
  bool isPartial_;
};