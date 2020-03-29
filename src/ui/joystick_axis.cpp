#include "joystick_axis.h"

#include <limits>

//------------------------------------------------------------------------------
JoystickAxis::JoystickAxis(const std::string& name)
    : LOG(LoggerFactory::getLogger(name)),
      autoCalibrate_(true),
      isCalibrated_(false),
      preliminaryDistanceFormCenter_(0.0),
      rawCenter_(0.0),
      rawMin_(std::numeric_limits<float>::max()),     // yes, min gets the most positive value to reduce later
      rawMax_(std::numeric_limits<float>::lowest()),  // yes, max gets the most negative value to increase later
      threshold_(0),
      center_(0),
      minCenter_(0),
      minBorder_(0),
      maxCenter_(0),
      maxBorder_(0),
      hasValue_(false)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
void JoystickAxis::begin(float threshold, float preliminaryDistanceFormCenter, bool autoCalibrate)
//------------------------------------------------------------------------------
{
  threshold_ = threshold;
  autoCalibrate_ = autoCalibrate;
  preliminaryDistanceFormCenter_ = preliminaryDistanceFormCenter;
}

//------------------------------------------------------------------------------
void JoystickAxis::statistics()
//------------------------------------------------------------------------------
{
  LOG.s("AC:%d Raw(min:%f, center:%f, max:%f) border(minB:%f, minC:%f, maxC:%f, maxB:%f)", autoCalibrate_, rawMin_, rawCenter_, rawMax_,
        minBorder_, minCenter_, maxCenter_, maxBorder_);
}

//------------------------------------------------------------------------------
void JoystickAxis::setRawBorder(float rawMin, float rawMax)
//------------------------------------------------------------------------------
{
  rawMin_ = rawMin_;
  rawMax_ = rawMax_;
}

//------------------------------------------------------------------------------
void JoystickAxis::reCalculateBorders()
//------------------------------------------------------------------------------
{
  float thresholdMin = (rawCenter_ - rawMin_) * threshold_;
  minCenter_ = rawCenter_ - thresholdMin;
  minBorder_ = rawMin_;

  float thresholdMax = (rawMax_ - rawCenter_) * threshold_;
  maxCenter_ = rawCenter_ + thresholdMax;
  maxBorder_ = rawMax_;
}

//------------------------------------------------------------------------------
void JoystickAxis::setRawCenter(float rawCenter)
//------------------------------------------------------------------------------
{
  rawCenter_ = rawCenter;
  rawMin_ = rawCenter - preliminaryDistanceFormCenter_;
  rawMax_ = rawCenter + preliminaryDistanceFormCenter_;
  reCalculateBorders();
  isCalibrated_ = true;
}

//------------------------------------------------------------------------------
float JoystickAxis::setRawValue(float v)
//------------------------------------------------------------------------------
{
  if (autoCalibrate_) {
    bool recalculationRequired = false;
    if (rawMin_ > v) {
      recalculationRequired = true;
      rawMin_ = v;
    }
    if (rawMax_ < v) {
      recalculationRequired = true;
      rawMax_ = v;
    }
    if (recalculationRequired) {
      reCalculateBorders();
    }
  }

  if (v <= minCenter_) {
    hasValue_ = true;
    currentValue_ = map(v, minCenter_, minBorder_, 0.0, -1.0);
  } else if (v >= maxCenter_) {
    hasValue_ = true;
    currentValue_ = map(v, maxCenter_, maxBorder_, 0.0, 1.0);
  } else {
    currentValue_ = 0.0;
    hasValue_ = false;
  }

  return currentValue_;
}

//------------------------------------------------------------------------------
float JoystickAxis::map(float x, float in_min, float in_max, float out_min, float out_max)
//------------------------------------------------------------------------------
{
  float inRange = (in_max - in_min);
  float in_pos = x - in_min;
  float outRange = out_max - out_min;
  return ((in_pos / inRange) * outRange ) + out_min;

  // if (divisor == 0) {
    // return -1;  // AVR returns -1, SAM returns 0
  // }
  // return (x - in_min) * (out_max - out_min) / divisor + out_min;
}
