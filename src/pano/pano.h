#pragma once

#include <Arduino.h>

#include "./shot/shot_row.h"
#include "./strategy/strategy.h"
#include "data/picture.h"
#include "data/view.h"
#include "util/logger.h"

class Pano {
 public:
  Pano() : LOG(LoggerFactory::getLogger("Pano")), view_(), picture_(), shotRow_(), strategy_(NULL) {}
  Pano(const View& view, const Picture& panoPicture, const ShotRow& shotRow, const Strategy* strategy)
      : LOG(LoggerFactory::getLogger("Pano")), view_(view), picture_(panoPicture), strategy_(strategy) {}

  uint32_t getPositionCount() const { return strategy_->getPositionCount(view_, picture_); }
  Position getPosition(uint32_t imageIndex) const { return strategy_->getPosition(view_, picture_, imageIndex); }

  uint32_t getShotCount() const { return shotRow_.getShotCount(); }
  const Shot& getShot(uint32_t shotIndex) { return *(shotRow_.getShot(shotIndex)); }

  void statistic() {
    if (view_.isPartial()) {
      LOG.i("View: partial, y1:%f, y2:%f", view_.getY1(), view_.getY2());
    } else {
      LOG.i("View: full, x1:%f, x2:%f / y1:%f, y2:%f", view_.getX1(), view_.getX2(), view_.getY1(), view_.getY2());
    }
    LOG.i("Picture: w:%f, h:%f, overlapW:%f, overklaH:%f", picture_.getWidth(), picture_.getHeight(), picture_.getOverlapWidth(),
          picture_.getOverlapHeight());

    // TODO shotrow_

    LOG.i("Strategy: %s", strategy_->getName());
  }

 private:
  Logger& LOG;
  View view_;
  Picture picture_;
  ShotRow shotRow_;
  const Strategy* strategy_;
};