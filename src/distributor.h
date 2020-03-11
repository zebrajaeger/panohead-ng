#pragma once

#include "data/position.h"
#include "data/value.h"
#include "data/view.h"
#include "pano/picture.h"
#include "pano/raster.h"
#include "pano/shot.h"

class Distributor {
 public:
  static Distributor& getInstance();

  Distributor() {}

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

 private:
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
};