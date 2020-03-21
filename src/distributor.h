#pragma once

#include <Arduino.h>

#include "data/position.h"
#include "data/power.h"
#include "data/value.h"
#include "data/view.h"
#include "pano/picture.h"
#include "pano/raster.h"
#include "pano/shot.h"
#include "util/loggerfactory.h"

class Distributor {
 public:
  typedef std::function<void(bool complete)> PanoCompleteCallback_t;

  static Distributor& getInstance();
  // operators connot be static so this is the shorter form of getInstance
  static Distributor& i;

  // prevent accidential instance
  Distributor(Distributor const&) = delete;
  void operator=(Distributor const&) = delete;

  Distributor() : LOG(LoggerFactory::getLogger("Distributor")) {
    view_.addListener([this](const Value<View>& v) { check(); });
    picture_.addListener([this](const Value<Picture>& v) { check(); });
    picture_.addListener([this](const Value<Picture>& v) { check(); });
    focusTime_.addListener([this](const Value<int32_t>& v) { check(); });
    triggerTime_.addListener([this](const Value<int32_t>& v) { check(); });
    panoComplete_.addListener([this](const Value<bool>& complete) { LOG.i("Pano complete: %d", complete); });
  }

  Value<Power>& getPower() { return power_; }
  Value<View>& getView() { return view_; }
  Value<Picture>& getPicture() { return picture_; }
  Value<int32_t>& getDelayAfterMove() { return delayAfterMove_; }
  Value<int32_t>& getFocusTime() { return focusTime_; }
  Value<int32_t>& getTriggerTime() { return triggerTime_; }
  Value<Raster*>& getRaster() { return raster_; }
  Value<Shots*>& getShots() { return shots_; }
  Value<Position>& getPosition() { return position_; }
  Value<Position>& getLevel() { return level_; }
  Value<Position>& getJoystick() { return joystick_; }
  Value<bool>& getAxisMovingX() { return axisMovingX_; }
  Value<bool>& getAxisMovingY() { return axisMovingY_; }
  Value<bool>& getCameraFocus() { return cameraFocus_; }
  Value<bool>& getCameraTrigger() { return cameraTrigger_; }
  Value<const char*>& getPanoAutomatStatus() { return panoAutomatStatus_; }
  Value<uint32_t>& getPanoAutomatColumn() { return panoAutomatColumn_; }
  Value<uint32_t>& getPanoAutomatRow() { return panoAutomatRow_; }
  Value<uint32_t>& getPanoAutomatShot() { return panoAutomatShot_; }

  Value<bool>& getPanoComplete() { return panoComplete_; }

 private:
  void check() {
    bool viewComplete = (*view_).isComplete();
    bool pictureComplete = (*picture_).isComplete();
    bool focusTimeComplete = *focusTime_ > 0;
    bool triggerTimeComplete = *triggerTime_ > 0;

    bool complete = viewComplete && pictureComplete && focusTimeComplete && triggerTimeComplete;
    LOG.i("CHECK view:%d pic:%d foc:%d, trig:%d -> %d", viewComplete, pictureComplete, focusTimeComplete, triggerTimeComplete, complete);
    panoComplete_ = complete;
  }

  Logger& LOG;
  Value<Power> power_;
  Value<View> view_;
  Value<Picture> picture_;
  Value<int32_t> delayAfterMove_;
  Value<int32_t> focusTime_;
  Value<int32_t> triggerTime_;
  Value<Raster*> raster_;
  Value<Shots*> shots_;
  Value<Position> position_;
  Value<Position> level_;
  Value<Position> joystick_;
  Value<bool> axisMovingX_;
  Value<bool> axisMovingY_;
  Value<bool> cameraFocus_;
  Value<bool> cameraTrigger_;
  Value<const char*> panoAutomatStatus_;
  Value<uint32_t> panoAutomatColumn_;
  Value<uint32_t> panoAutomatRow_;
  Value<uint32_t> panoAutomatShot_;

  Value<bool> panoComplete_;
};