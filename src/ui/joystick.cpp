#include "joystick.h"

//------------------------------------------------------------------------------
Joystick::Joystick()
    : x_("JOX.X"),
      y_("JOX.Y")
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Joystick::begin(float threshold, float minRawDistanceFromCenterToBeValid, bool autoCalibrate)
//------------------------------------------------------------------------------
{
  x_.begin(threshold, minRawDistanceFromCenterToBeValid, autoCalibrate);
  y_.begin(threshold, minRawDistanceFromCenterToBeValid, autoCalibrate);
  return true;
}

//------------------------------------------------------------------------------
void Joystick::statistics()
//------------------------------------------------------------------------------
{
  x_.statistics();
  y_.statistics();
}

//------------------------------------------------------------------------------
void Joystick::setRawCenter(float rawCenterValueX, float rawCenterValueY)
//------------------------------------------------------------------------------
{
  x_.setRawCenter(rawCenterValueX);
  y_.setRawCenter(rawCenterValueY);
}

//------------------------------------------------------------------------------
void Joystick::setRaw(float x, float y)
//------------------------------------------------------------------------------
{
  x_.setRawValue(x);
  y_.setRawValue(y);
}
