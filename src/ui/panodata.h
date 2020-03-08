#pragma once

#include <Arduino.h>

#include "pano/view.h"
#include "pano/picture.h"
#include "pano/raster.h"
#include "pano/shot.h"

class PanoData {
 public:
  PanoData() : view_(), picture_(), raster_(NULL), shots_(NULL), delayAfterMove_(0), focusTime_(0), triggerTime_(0) {}

  void setView(const View& view) { view_ = view; }
  const View& getView() const { return view_; }

  void setPicture(const Picture& picture) { picture_ = picture; }
  const Picture& getPicture() const { return picture_; }

  void setRaster(const Raster* raster) { raster_ = raster; }
  const Raster* getRaster() const { return raster_; }

  void setShots(const pano::Shots* shots) { shots_ = shots; }
  const pano::Shots* getShots() const { return shots_; }

  void setDelayAfterMove(int32_t delayAfterMove) { delayAfterMove_ = delayAfterMove; }
  int32_t getDelayAfterMove() { return delayAfterMove_; }

  void setFocusTime(int32_t focusTime) { focusTime_ = focusTime; }
  int32_t getFocusTime() { return focusTime_; }
  
  void setTriggerTime(int32_t triggerTime) { triggerTime_ = triggerTime; }
  int32_t getTriggerTime() { return triggerTime_; }

  void toJson(String& json) const;

 private:
  View view_;
  Picture picture_;
  const Raster* raster_;
  const pano::Shots* shots_;
  int32_t delayAfterMove_;
  int32_t focusTime_;
  int32_t triggerTime_;
};