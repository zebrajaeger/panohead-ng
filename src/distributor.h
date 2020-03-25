#pragma once

#include <Arduino.h>

#include "data/picture.h"
#include "data/position.h"
#include "data/power.h"
#include "data/value.h"
#include "data/view.h"
#include "pano/pano.h"
#include "pano/pano_state.h"
#include "pano/shot/shot_row.h"
#include "util/loggerfactory.h"

class Distributor {
 public:
  typedef std::function<void(bool complete)> PanoCompleteCallback_t;

  static Distributor& i;

  // prevent new instances
  Distributor(Distributor const&) = delete;
  void operator=(Distributor const&) = delete;

  Distributor() : LOG(LoggerFactory::getLogger("Distributor")) {
    view_.addListener([this](const Value<View>& v) { check(); });
    picture_.addListener([this](const Value<Picture>& v) { check(); });
    picture_.addListener([this](const Value<Picture>& v) { check(); });
  }

  // physical values in
  Value<Power>& getPower() { return power_; }
  Value<Position>& getPosition() { return position_; }
  Value<Position>& getLevel() { return level_; }
  Value<Position>& getJoystick() { return joystick_; }
  Value<bool>& getAxisMovingX() { return axisMovingX_; }
  Value<bool>& getAxisMovingY() { return axisMovingY_; }
  Value<bool>& getCameraFocus() { return cameraFocus_; }

  // physical values out
  Value<bool>& getCameraTrigger() { return cameraTrigger_; }

  // pano configuration (preparation)
  Value<View>& getView() { return view_; }
  Value<Picture>& getPicture() { return picture_; }
  Value<MovementTiming>& getMovementTiming() { return movementTiming_; }
  Value<ShotRow>& getShots() { return shots_; }
  Value<bool>& getPanoConfigurationComplete() { return panoConfigurationComplete_; }

  // pano automat (execution)
  Value<Pano>& getPano() { return pano_; }
  Value<PanoState>& getPanoState() { return panoState_; }

 private:
  // TODO is the distributor a good place for this logic? I dont think so. Maybe it is better to refactor it to a place where it is more expected.
  void check() {
    bool viewComplete = (*view_).isComplete();
    bool pictureComplete = (*picture_).isComplete();
    bool shotsComplete = (*shots_).isComplete();

    bool complete = viewComplete && pictureComplete && shotsComplete;
    LOG.i("CHECK view:%d pic:%d shots:%d -> %d", viewComplete, pictureComplete, shotsComplete, complete);
    panoConfigurationComplete_ = complete;
  }

  Logger& LOG;

  // physical values in
  Value<Power> power_;
  Value<Position> position_;
  Value<Position> level_;
  Value<Position> joystick_;
  Value<bool> axisMovingX_;
  Value<bool> axisMovingY_;
  Value<bool> cameraFocus_;

  // physical values out
  Value<bool> cameraTrigger_;

  // pano configuration (preparation)
  Value<View> view_;
  Value<Picture> picture_;
  Value<MovementTiming> movementTiming_;
  Value<ShotRow> shots_;
  Value<bool> panoConfigurationComplete_;

  // pano automat (execution)
  Value<Pano> pano_;
  Value<PanoState> panoState_;
};