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
  void setRawCenterX(float rawCenterValueX){x_.setRawCenter(rawCenterValueX);}
  void setRawCenterY(float rawCenterValueY){y_.setRawCenter(rawCenterValueY);}
  void setRawCenter(float rawCenterValueX, float rawCenterValueY);
  bool isCalibrated() const{return x_.isCalibrated() && y_.isCalibrated();}

  float setRawX(float x){return x_.setRawValue(x);}
  float setRawY(float y){return y_.setRawValue(y);}
  void setRaw(float x, float y);

  float getX() const{return x_.getValue();}
  float getY() const{return y_.getValue();}

  bool hasValue() const{return x_.hasValue() || y_.hasValue();}

  const JoystickAxis& getXAxis() const{return x_;}
  const JoystickAxis& getYAxis() const{return y_;}

 private:
  JoystickAxis x_;
  JoystickAxis y_;
};
