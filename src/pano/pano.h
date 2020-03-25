#pragma once

#include <Arduino.h>

#include "./shot/shot_row.h"
#include "./strategy/strategy.h"
#include "data/picture.h"
#include "data/view.h"
#include "movement_timing.h"

class Pano {
 public:
  Pano() : view_(), picture_(), shotRow_(), movementTiming_(), strategy_(NULL) {}
  Pano(const View& view, const Picture& panoPicture, const ShotRow& shotRow, MovementTiming movementTiming, const Strategy* strategy)
      : view_(view), picture_(panoPicture), movementTiming_(movementTiming), strategy_(strategy) {}

  // uint32_t getPositionCount() const { return strategy_->getPositionCount(view_, picture_); }
  Position getPosition(uint32_t imageIndex) const { return strategy_->getPosition(view_, picture_, imageIndex); }

  // uint32_t getShotCount() const { return shotRow_.getShotCount(); }
  const Shot& getShot(uint32_t shotIndex) { return *(shotRow_.getShot(shotIndex)); }

  const MovementTiming& getMovementTiming() const { return movementTiming_; }

  // TODO log somewhere....

  // void statistic() {
  //   if (view_.isPartial()) {
  //     LOG.i("View: partial, y1:%f, y2:%f", view_.getY1(), view_.getY2());
  //   } else {
  //     LOG.i("View: full, x1:%f, x2:%f / y1:%f, y2:%f", view_.getX1(), view_.getX2(), view_.getY1(), view_.getY2());
  //   }
  //   LOG.i("Picture: w:%f, h:%f, overlapW:%f, overklaH:%f", picture_.getWidth(), picture_.getHeight(), picture_.getOverlapWidth(),
  //         picture_.getOverlapHeight());

  //   // TODO shotrow_

  //   LOG.i("Strategy: %s", strategy_->getName());
  // }

  // Pano& operator=(const Pano& other) {
  //   view_ = other.view_;
  //   picture_ = other.picture_;
  //   shotRow_ = other.shotRow_;
  //   movementTiming_ = other.movementTiming_;
  //   return *this;
  // }

 private:
  View view_;
  Picture picture_;
  ShotRow shotRow_;
  MovementTiming movementTiming_;
  const Strategy* strategy_;
};