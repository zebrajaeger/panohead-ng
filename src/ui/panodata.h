#pragma once

#include <Arduino.h>

#include "pano/view.h"
#include "pano/picture.h"
#include "pano/raster.h"
#include "pano/shot.h"

class PanoData {
 public:
  PanoData() : view_(), picture_(), raster_(NULL), shots_(NULL), delayAfterMove_(0), focusTime_(0), triggerTime_(0) {}

  const View& getView() const { return view_; }
  void setView(const View& view) { view_ = view; }

  const Picture& getPicture() const { return picture_; }
  Picture& getPicture() { return picture_; }
  void setPicture(const Picture& picture) { picture_ = picture; }

  Raster* getRaster() { return raster_; }
  const Raster* getRaster() const { return raster_; }
  void setRaster(Raster* raster) { raster_ = raster; }

  const pano::Shots* getShots() const { return shots_; }
  void setShots(pano::Shots* shots) { shots_ = shots; }

  int32_t getDelayAfterMove() { return delayAfterMove_; }
  void setDelayAfterMove(int32_t delayAfterMove) { delayAfterMove_ = delayAfterMove; }

  int32_t getFocusTime() { return focusTime_; }
  void setFocusTime(int32_t focusTime) { focusTime_ = focusTime; }

  int32_t getTriggerTime() { return triggerTime_; }
  void setTriggerTime(int32_t triggerTime) { triggerTime_ = triggerTime; }

  void toJson(String& json) const;

 private:
  View view_;
  Picture picture_;
  Raster* raster_;
  pano::Shots* shots_;
  int32_t delayAfterMove_;
  int32_t focusTime_;
  int32_t triggerTime_;
};