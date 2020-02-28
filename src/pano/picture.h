#pragma once

#include <Arduino.h>

class Picture {
 public:
  //------------------------------------------------------------------------------
  Picture(double width, double height, double overlapWidth, double overlapHeight)
      : width_(width),
        height_(height),
        overlapWidth_(overlapWidth),
        overlapHeight_(overlapHeight)
  //------------------------------------------------------------------------------
  {}

  //------------------------------------------------------------------------------
  double width() const
  //------------------------------------------------------------------------------
  {
    return width_;
  }

  //------------------------------------------------------------------------------
  double height() const
  //------------------------------------------------------------------------------
  {
    return height_;
  }

  //------------------------------------------------------------------------------
  double widthWithinPano() const
  //------------------------------------------------------------------------------
  {
    return width_ - overlapWidth_;
  }

  //------------------------------------------------------------------------------
  double heightWithinPano() const
  //------------------------------------------------------------------------------
  {
    return height_ - overlapHeight_;
  }

  //------------------------------------------------------------------------------
  double overlapWidth() const
  //------------------------------------------------------------------------------
  {
    return overlapWidth_;
  }

  //------------------------------------------------------------------------------
  double overlapHeight() const
  //------------------------------------------------------------------------------
  {
    return overlapHeight_;
  }

 private:
  double width_;
  double height_;
  double overlapWidth_;
  double overlapHeight_;
};