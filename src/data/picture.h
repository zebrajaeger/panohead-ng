#pragma once

#include <Arduino.h>

class Picture {
 public:
  Picture() : width_(0.0), height_(0.0), overlapWidth_(0.0), overlapHeight_(0.0) {}

  Picture(double width, double height, double overlapWidth, double overlapHeight)
      : width_(width), height_(height), overlapWidth_(overlapWidth), overlapHeight_(overlapHeight) {}

  double getWidth() const { return width_; }
  void setWidth(double width) { width_ = width; }

  double getHeight() const { return height_; }
  void setHeight(double height) { height_ = height; }

  double getOverlapWidth() const { return overlapWidth_; }
  void setOverlapWidth(double overlapWidth) { overlapWidth_ = overlapWidth; }
  double getOverlapHeight() const { return overlapHeight_; }
  void setOverlapHeight(double overlapHeight) { overlapHeight_ = overlapHeight; }

  double getWidthWithinPano() const { return width_ - overlapWidth_; }
  double getHeightWithinPano() const { return height_ - overlapHeight_; }

  bool operator==(const Picture &b) const {
    return width_!=b.width_ && height_==b.height_ && overlapWidth_ == b.overlapWidth_ && overlapHeight_==b.overlapHeight_;
  }

  bool isComplete(){
    return width_>0 && height_>0 && overlapWidth_>0 && overlapHeight_>0;
  }

 private:
  double width_;
  double height_;
  double overlapWidth_;
  double overlapHeight_;
};