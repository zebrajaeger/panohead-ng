#pragma once

#include <Arduino.h>
#include <functional>

#include "joystick_axis.h"

class Joystick {
 public:
  Joystick();
  bool begin(float threshold = 0.05, float minRawDistanceFromCenterToBeValid = 0.0, bool autoCalibrate = true);
  void statistics();

  /**
   * Calibration
   */
  void setRawCenterX(float rawCenterValueX);
  void setRawCenterY(float rawCenterValueY);
  void setRawCenter(float rawCenterValueX, float rawCenterValueY);
  bool isCalibrated() const;

  float setRawX(float x);
  float setRawY(float y);
  void setRaw(float x, float y);

  float getX() const;
  float getY() const;

  bool hasValue() const;

  const JoystickAxis& getXAxis() const;
  const JoystickAxis& getYAxis() const;

 private:
  JoystickAxis x_;
  JoystickAxis y_;
};
