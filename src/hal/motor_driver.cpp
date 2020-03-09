#include "motor_driver.h"

//------------------------------------------------------------------------------
MotorDriver::MotorDriver()
    : LOG(LoggerFactory::getLogger("MotorDriver")),
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
    timer_.startMs(50, false, true, std::bind(&MotorDriver::updateState, this));

    return true;
  }

  return false;
}

//------------------------------------------------------------------------------
void MotorDriver::loop()
//------------------------------------------------------------------------------
{
  timer_.loop();
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
    LOG.d("#    -POS curr: %d -> target: %d, off: %d", tmc429_.getActualPosition(i), tmc429_.getTargetPosition(i), axisOffset_[i]);
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
    // LOG.d("Go %i from %i to %i", axisIndex, currentPos, (int32_t)stepsPos);
    tmc429_.setSoftMode(axisIndex);
    tmc429_.setTargetPosition(axisIndex, stepsPos);
    updateAxis(axisIndex, true);
  } else {
    LOG.e("Cannot go to pos. Translator for axis %u is NULL", axisIndex);
  }
}

//------------------------------------------------------------------------------
void MotorDriver::jogTo(uint8_t axisIndex, double delta)
//------------------------------------------------------------------------------
{
  if (isAtTargetPos(axisIndex)) {
    // not moving.
    goTo(axisIndex, delta);
  } else {
    // already moving. take current pos and add jogging delta
    Translator* t = translators_[axisIndex];
    if (t) {
      int32_t newPos = tmc429_.getActualPosition(axisIndex) + t->revolutionToSteps(delta);
      tmc429_.setTargetPosition(axisIndex, newPos);
    } else {
      LOG.e("Cannot jog to pos. Translator for axis %u is NULL", axisIndex);
    }
  }
}

//------------------------------------------------------------------------------
void MotorDriver::jogV(uint8_t axisIndex, double revPerSecond)
//------------------------------------------------------------------------------
{
  Translator* t = translators_[axisIndex];
  if (t) {
    tmc429_.setVelocityMode(axisIndex);
    tmc429_.setTargetVelocityInHz(axisIndex, t->revolutionToSteps(revPerSecond));
  } else {
    LOG.e("Cannot jog to pos. Translator for axis %u is NULL", axisIndex);
  }
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
bool MotorDriver::isAtTargetPos(uint8_t axisIndex)
//------------------------------------------------------------------------------
{
  TMC429::Status s = tmc429_.getStatus();
  switch (axisIndex) {
    case 0:
      return s.at_target_position_0;
    case 1:
      return s.at_target_position_1;
    case 2:
      return s.at_target_position_2;
    default:
      return true;
  }
}

//------------------------------------------------------------------------------
void MotorDriver::updateAxis(uint8_t axisIndex, bool isMoving)
//------------------------------------------------------------------------------
{
  // if state has changes...
  if (axisMoving_[axisIndex] != isMoving) {
    // and motor has reached target pos
    if (!isMoving) {
      // check for no velocity before send stop message
      if (tmc429_.getActualVelocityInHz(axisIndex) == 0) {
        tmc429_.stop(axisIndex);  // velocity mode, speed 0

        axisMoving_[axisIndex] = false;
        // reset internal pos add it to offset
        axisOffset_[axisIndex] += tmc429_.getActualPosition(axisIndex);
        tmc429_.setActualPosition(axisIndex, 0);
        tmc429_.setTargetPosition(axisIndex, 0);
        // LOG.d("Axis %d movement STOPPED", axisIndex);
        // callback
        if (movementStatusChangeCallback_) {
          movementStatusChangeCallback_(axisIndex, axisMoving_);
        }
      }
    } else {
      // not @ target pos
      axisMoving_[axisIndex] = true;
      // LOG.d("Axis %d movement STARTED", axisIndex);
      if (movementStatusChangeCallback_) {
        movementStatusChangeCallback_(axisIndex, axisMoving_);
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