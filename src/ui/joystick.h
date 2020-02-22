#pragma once

#include <Arduino.h>
#include "util/logger.h"

class JoystickAxis {
 public:
  JoystickAxis(const String& name) : LOG(name), threshold_(0), center_(0), r1_min(0), r1_max(0), r2_min(0), r2_max(0), isCalibrated_(false) {}
  void begin(float threshold = 0.1) { threshold_ = threshold; }

  void setCenter(float v) {
    r1_min = v * threshold_;
    r1_max = v * (1 - threshold_);
    r2_min = v * (1 + threshold_);
    r2_max = v * (2 - threshold_);
    LOG.d("calibrated: v: %f, r1_min: %f, r1_max: %f, r2_min: %f, r2_max: %f", v, r1_min, r1_max, r2_min, r2_max);
    isCalibrated_ = true;
  }

  float getValue() { return currentValue_; }
  float setRawValue(float v) {
    if (v < r1_max) {
      if (v < r1_min) {
        currentValue_ = -1;
      } else {
        currentValue_ = map(v, r1_min, r1_max, -1, 0);
      }
    } else if (v > r2_min) {
      if (v > r2_max) {
        currentValue_ = 1;
      } else {
        currentValue_ = map(v, r2_min, r2_max, 0, 1);
      }
    } else {
      currentValue_ = 0;
    }
    return currentValue_;
  }

  bool isCalibrated() const { return isCalibrated_; }

 private:
  float map(float x, float in_min, float in_max, float out_min, float out_max) {
    long divisor = (in_max - in_min);
    if (divisor == 0) {
      return -1;  // AVR returns -1, SAM returns 0
    }
    return (x - in_min) * (out_max - out_min) / divisor + out_min;
  }

  Logger LOG;
  float threshold_;
  float center_;
  float r1_min;
  float r1_max;
  float r2_min;
  float r2_max;
  float currentValue_;
  bool isCalibrated_;
};

class Joystick {
 public:
  Joystick() : x_("JOX.X"), y_("JOX.Y") {}

  bool begin(float threshold = 0.05) {
    x_.begin(threshold);
    y_.begin(threshold);
    return true;
  }
  /**
   * Calibration
   */
  void setCenterX(float centerValueX) { x_.setCenter(centerValueX); }
  void setCenterY(float centerValueY) { y_.setCenter(centerValueY); }
  void setCenter(float centerValueX, float centerValueY) {
    x_.setCenter(centerValueX);
    y_.setCenter(centerValueY);
  }
  float setRawX(float x) { return x_.setRawValue(x); }
  float setRawY(float y) { return y_.setRawValue(y); }
  void setRaw(float x, float y) {
    x_.setRawValue(x);
    y_.setRawValue(y);
  }
  float getX() { return x_.getValue(); }
  float getY() { return y_.getValue(); }
  bool isCalibrated() const { return x_.isCalibrated() && y_.isCalibrated(); }

  const JoystickAxis& getXAxis() const { return x_; }

  const JoystickAxis& getYAxis() const { return y_; }

 private:
  JoystickAxis x_;
  JoystickAxis y_;
};
