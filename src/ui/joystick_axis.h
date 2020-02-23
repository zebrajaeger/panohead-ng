#pragma once

#include <Arduino.h>

#include "util/logger.h"

class JoystickAxis {
 public:

  JoystickAxis(const String& name);

  void begin(float threshold = 0.1, float minRawDistanceFromCenterToBeValid = 0.0, bool autoCalibrate = true);
  void statistics();

  void setRawCenter(float rawCenter);
  bool isCalibrated() const;
  void setRawBorder(float rawMin, float rawMax);

  float setRawValue(float v);
  float getValue() const ;
  bool hasValue() const;

 private:
  void reCalculateBorders();
  static float map(float x, float in_min, float in_max, float out_min, float out_max);

  Logger LOG;

  bool autoCalibrate_;
  bool isCalibrated_;

  float minRawDistanceFromCenterToBeValid_;
  float minValid_;
  float maxValid_;

  float rawCenter_;
  float rawMin_;
  float rawMax_;

  float threshold_;
  float center_;
  float minCenter_;
  float minBorder_;
  float maxCenter_;
  float maxBorder_;

  float currentValue_;
  bool hasValue_;
};