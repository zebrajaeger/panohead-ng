#pragma once

#include <Arduino.h>
#include <functional>

#include "util/logger.h"
#include "util/singletimer.h"

#include "pano/pano.h"
#include "pano/shot.h"

class PanoAutomat {
 public:
  typedef std::function<void(bool focus, bool trigger)> cameraCallback_t;
  typedef std::function<void(uint32_t columnIndex, uint32_t rowIndex, uint32_t shotIndex)> pictureCallback_t;
  typedef std::function<void(pano::Position position)> moveCallback_t;
  typedef std::function<void(double progress)> progressCallback_t;
  typedef std::function<void()> panoFinishCallback_t;

  typedef enum {
    UNINITIALIZED,
    INITAILIZED,
    MOVE,
    WAIT_AFTER_MOVE,
    FOCUS,
    TRIGGER,
    WAIT_BETWEEN_SHOTS,
    WAIT_AFTER_LAST_SHOT,
    FINISH
  } state_t;

  PanoAutomat()
      : LOG("PanoAutomat"),
        raster_(NULL),
        shots_(0, 0, 0),
        timer_("PanoAutomat"),
        state_(UNINITIALIZED),
        currentColumn_(0),
        currentRow_(0),
        currentShot_(0),
        cameraCallback_([this](bool, bool) { LOG.d("cameraCallback_ uninitialized"); }),
        beforePictureCallback_([this](uint32_t, uint32_t, uint32_t) {}),
        afterPictureCallback_([this](uint32_t, uint32_t, uint32_t) {}),
        moveCallback_([this](pano::Position) { LOG.d("moveCallback_ uninitialized"); }),
        progressCallback_([this](double) {}),
        panoFinishCallback_([this]() {}) {}

  bool begin() {
    timer_.onTimer(std::bind(&PanoAutomat::onTimer, this));
    timer_.start(1);
    setState(INITAILIZED);
    return true;
  }

  bool start(pano::Raster* raster, const pano::Shots& shots) {
    if (raster_) {
      delete raster_;
    }
    raster_ = raster;
    shots_ = shots;

    resetPosition();
    resetShot();

    setState(MOVE);
    triggerMove();

    return true;
  }

  bool moveDone() {
    if (state_ == MOVE) {
      setState(WAIT_AFTER_MOVE);
      LOG.i("Wait for %dms", shots_.getDelayAfterMoveMs());
      triggerTimerMs(shots_.getDelayAfterMoveMs());
      return true;
    }
    return false;
  }

  void onCamera(cameraCallback_t cb) { cameraCallback_ = cb; }
  void onBeforePicture(pictureCallback_t cb) { beforePictureCallback_ = cb; }
  void onAfterPicture(pictureCallback_t cb) { afterPictureCallback_ = cb; }
  void onMove(moveCallback_t cb) { moveCallback_ = cb; }
  void onProgress(progressCallback_t cb) { progressCallback_ = cb; }
  void onPanoFinish(panoFinishCallback_t cb) { panoFinishCallback_ = cb; }

  void loop() { timer_.loop(); }

  void statistic() {
    raster_->statistics();
    LOG.d("# State: %s", stateToName(state_));
    LOG.d("# Col: %d", currentColumn_);
    LOG.d("# Row: %d", currentRow_);
    LOG.d("# Sht: %d", currentShot_);
  }

 protected:
  state_t setState(state_t newState) {
    state_t temp = state_;
    state_ = newState;
    LOG.i("State change %s -> %s", stateToName(temp), stateToName(newState));
    return temp;
  }

  void onTimer() {
    switch (state_) {
      case WAIT_AFTER_MOVE:
      case WAIT_BETWEEN_SHOTS: {
        LOG.i("###################################################");
        LOG.i("##### @[%u,%u] take shot(%u/%u) #####", currentColumn_, currentRow_, currentShot_, shots_.getShotCount());
        LOG.i("###################################################");
        beforePictureCallback_(currentColumn_, currentRow_, currentShot_);
        setState(FOCUS);
        cameraCallback_(true, false);
        // LOG.e("XXX %d %d", currentShot_, shots_[currentShot_].getFocusTimeMs());
        triggerTimerMs(shots_[currentShot_].getFocusTimeMs());
      } break;

      case FOCUS: {
        setState(TRIGGER);
        cameraCallback_(true, true);  // or false,true ?
        triggerTimerMs(shots_[currentShot_].getTriggerTimeMs());
      } break;

      case TRIGGER: {
        cameraCallback_(false, false);
        afterPictureCallback_(currentColumn_, currentRow_, currentShot_);
        if (prepareNextShot()) {
          setState(WAIT_BETWEEN_SHOTS);
          triggerTimerMs(shots_.getDelayBetweenShotsMs());
        } else {
          resetShot();
          setState(WAIT_AFTER_LAST_SHOT);
          triggerTimerMs(shots_.getDelayAfterLastShotMs());
        }
      } break;

      case WAIT_AFTER_LAST_SHOT: {
        if (prepareNextPosition()) {
          setState(MOVE);
          triggerMove();
        } else {
          resetPosition();
          setState(FINISH);
          panoFinishCallback_();
        }
      } break;

      default:
        break;
    }
  }

  void resetShot() { currentShot_ = 0; }

  bool prepareNextShot() {
    currentShot_++;
    LOG.d("prepareNextShot() ->  current:%d max:%d", currentShot_, shots_.getShotCount());
    if (currentShot_ >= shots_.getShotCount()) {
      currentShot_ = 0;
      return false;
    } else {
      return true;
    }
  }

  void resetPosition() {
    currentColumn_ = 0;
    currentRow_ = 0;
  }

  bool prepareNextPosition() {
    currentColumn_++;
    if (currentColumn_ >= raster_->getColumnsForRowIndex(currentRow_)) {
      currentColumn_ = 0;
      currentRow_++;

      if (currentRow_ >= raster_->getRows()) {
        return false;
      }
    }
    return true;
  }

  void triggerTimerMs(uint32_t ms) {
    LOG.i("Timer startet for %dms", ms);
    timer_.start(ms * 1000);
  }

  void triggerMove() {
    pano::Position p = raster_->getPositionFor(currentColumn_, currentRow_);
    moveCallback_(p);
  }

  const char* stateToName(state_t state) {
    switch (state) {
      case UNINITIALIZED:
        return "UNINITIALIZED";
        break;
      case INITAILIZED:
        return "INITAILIZED";
        break;
      case MOVE:
        return "MOVE";
        break;
      case WAIT_AFTER_MOVE:
        return "WAIT_AFTER_MOVE";
        break;
      case FOCUS:
        return "FOCUS";
        break;
      case TRIGGER:
        return "TRIGGER";
        break;
      case WAIT_BETWEEN_SHOTS:
        return "WAIT_BETWEEN_SHOTS";
        break;
      case WAIT_AFTER_LAST_SHOT:
        return "WAIT_AFTER_LAST_SHOT";
        break;
      case FINISH:
        return "FINISH";
        break;
    }
    return "?unknown?";
  }

 private:
  Logger LOG;
  pano::Raster* raster_;
  pano::Shots shots_;
  SingleTimer timer_;
  state_t state_;
  uint32_t currentColumn_;
  uint32_t currentRow_;
  uint32_t currentShot_;
  cameraCallback_t cameraCallback_;
  pictureCallback_t beforePictureCallback_;
  pictureCallback_t afterPictureCallback_;
  moveCallback_t moveCallback_;
  progressCallback_t progressCallback_;
  panoFinishCallback_t panoFinishCallback_;
};