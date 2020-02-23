#include "motor_driver.h"

//------------------------------------------------------------------------------
MotorDriver::MotorDriver()
    : LOG("MotorDriver"),
      clockSource_(),
      tmc429_(),
      translators_({NULL, NULL, NULL}),
      axisOffset_({0, 0, 0}),
      timer_("MotorDriver"),
      axisMoving_({false, false, false})
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool MotorDriver::begin(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, std::array<Limit_t*, 3> limits,
                        std::array<Translator*, 3> translators)
//------------------------------------------------------------------------------
{
  translators_ = translators;

  clockSource_.begin(pinClockSource, 1000000 * clockSpeedMHz);
  delay(100);  // give tmc some time to initialize

  tmc429_.setup(pinCS, clockSpeedMHz);
  if (tmc429_.communicating()) {
    tmc429_.initialize();
    tmc429_.disableRightSwitches();
    tmc429_.setSwitchesActiveLow();

    for (uint8_t i = 0; i < 3; ++i) {
      initMotor(i, limits[i]);
    }

    // axis watcher timer
    timer_.onTimer(std::bind(&MotorDriver::updateState, this));
    timer_.start(1000 * 50, false, true);

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void MotorDriver::initMotor(uint8_t axisIndex, Limit_t* limit)
//------------------------------------------------------------------------------
{
  LOG.d("Init axis %d", axisIndex);
  tmc429_.stop(axisIndex);  // velocity mode, speed 0
  tmc429_.setLimitsInHz(axisIndex, limit->velocityMinHz, limit->velocityMaxHz, limit->acceleration_max_hz_per_s);
  tmc429_.setActualPosition(axisIndex, 0);
  tmc429_.setTargetPosition(axisIndex, 0);
  tmc429_.disableLeftSwitchStop(axisIndex);
  tmc429_.disableRightSwitchStop(axisIndex);
  tmc429_.disableSwitchSoftStop(axisIndex);

  tmc429_.setSoftMode(axisIndex);
  LOG.d("   - ACCmax:%d", tmc429_.getAccelerationMaxInHzPerS(axisIndex));
  LOG.d("   - Vmin: %d Vmax:%d VmaxUL:%d", tmc429_.getVelocityMinInHz(axisIndex), tmc429_.getVelocityMaxInHz(axisIndex),
        tmc429_.getVelocityMaxUpperLimitInHz());
}

//------------------------------------------------------------------------------
void MotorDriver::statistic()
//------------------------------------------------------------------------------
{
  TMC429::Status s = tmc429_.getStatus();
  for (uint8_t i = 0; i < 3; ++i) {
    LOG.d("# AXIS: %d", i);
    if (i == 0) {
      LOG.d("#    @TargetPos: %d ", s.at_target_position_0);
    } else if (i == 1) {
      LOG.d("#    @TargetPos: %d ", s.at_target_position_1);
    } else if (i == 2) {
      LOG.d("#    @TargetPos: %d ", s.at_target_position_2);
    }
    LOG.d("#    -POS(0) curr: %d -> target: %d", tmc429_.getActualPosition(0), tmc429_.getTargetPosition(0));
    LOG.d("#    -V   curr: %d", tmc429_.getActualVelocityInHz(i));
  }
}

//------------------------------------------------------------------------------
void MotorDriver::goTo(uint8_t axisIndex, double pos)
//------------------------------------------------------------------------------
{
  Translator* t = translators_[axisIndex];
  if (t) {
    int64_t stepsPos = t->revolutionToSteps(pos);
    int64_t relPos = stepsPos - axisOffset_[axisIndex];
    int32_t currentPos = tmc429_.getActualPosition(axisIndex);

    LOG.d("Go %i from %i to %i", axisIndex, currentPos, (int32_t)relPos);
    tmc429_.setSoftMode(axisIndex);
    tmc429_.setTargetPosition(axisIndex, relPos);
    updateAxis(axisIndex, true);
  } else {
    LOG.e("Cannot go to pos. Translator for axis %u is NULL", axisIndex);
  }
}

//------------------------------------------------------------------------------
void MotorDriver::loop()
//------------------------------------------------------------------------------
{
  timer_.loop();
}

//------------------------------------------------------------------------------
void MotorDriver::updateState()
//------------------------------------------------------------------------------
{
  TMC429::Status s = tmc429_.getStatus();
  updateAxis(0, !s.at_target_position_0);
  updateAxis(1, !s.at_target_position_1);
  updateAxis(2, !s.at_target_position_2);
}

//------------------------------------------------------------------------------
void MotorDriver::updateAxis(uint8_t axis, bool isMoving)
//------------------------------------------------------------------------------
{
  // if state has changes...
  if (axisMoving_[axis] != isMoving) {
    // and motor has reached target pos
    if (!isMoving) {
      // check for no velocity before send stop message
      if (tmc429_.getActualVelocityInHz(axis) == 0) {
        tmc429_.stop(axis);  // velocity mode, speed 0

        axisMoving_[axis] = false;
        // reset internal pos add it to offset
        axisOffset_[axis] += tmc429_.getActualPosition(axis);
        tmc429_.setActualPosition(axis, 0);
        tmc429_.setTargetPosition(axis, 0);
        LOG.d("Axis %d movement STOPPED", axis);
        // callback
        if (movementStatusChangeCallback_) {
          movementStatusChangeCallback_(axis, axisMoving_);
        }
      }
    } else {
      // not @ target pos
      axisMoving_[axis] = true;
      LOG.d("Axis %d movement STARTED", axis);
      if (movementStatusChangeCallback_) {
        movementStatusChangeCallback_(axis, axisMoving_);
      }
    }
  }
}

//------------------------------------------------------------------------------
void MotorDriver::onStatusChange(movementStatusChangeCallback_t cb)
//------------------------------------------------------------------------------
{
  movementStatusChangeCallback_ = cb;
}