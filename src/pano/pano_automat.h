#pragma once

#include <Arduino.h>

#include <functional>

#include "automat_state.h"
#include "data/position.h"
#include "movement_timing.h"
#include "pano.h"
#include "pano_state.h"
#include "util/loggerfactory.h"
#include "util/singletimer.h"

class PanoAutomat {
 public:
  typedef std::function<void(Position position)> MoveCallback_t;
  typedef std::function<void(bool focus, bool trigger)> CameraCallback_t;
  typedef std::function<void(const Pano& pano, const PanoState& panoState)> StatusCallback_t;

  PanoAutomat()
      : LOG(LoggerFactory::getLogger("PanoAutomat")),
        timer_("PanoAutomat"),
        pano_(),
        movementTiming_(),
        panoState_(),
        moveCallback_([this](Position) { LOG.d("moveCallback_ uninitialized"); }),
        cameraCallback_([this](bool, bool) { LOG.d("cameraCallback_ uninitialized"); }),
        statusCallback_([](const Pano& pano, const PanoState& panoState) {}) {}

  bool begin() {
    timer_.onTimer(std::bind(&PanoAutomat::onTimer, this));
    setState(AutomatState_t::INITAILIZED);
    return true;
  }
  void loop() { timer_.loop(); }

  void setMovementTiming(const MovementTiming& movementTiming) { movementTiming_ = movementTiming; }

  void start(Pano pano) {
    panoState_.reset();
    pano_ = pano;

    setState(MOVE);
    triggerMove();
  }

  bool triggerMoveDone() {
    if (getState() == MOVE) {
      setState(AutomatState_t::WAIT_AFTER_MOVE);
      triggerDelayAfterMove();
      return true;
    }
    return false;
  }

  void onMove(MoveCallback_t cb) { moveCallback_ = cb; }
  void onCamera(CameraCallback_t cb) { cameraCallback_ = cb; }
  void onStatus(StatusCallback_t cb) { statusCallback_ = cb; }

  void statistic() {
    LOG.i("Timing: delayAfterMove:%dms, delayBetweenShots:%dms, delayAfterLastShot:%dms", movementTiming_.getDelayAfterMoveMs(),
          movementTiming_.getDelayBetweenShotsMs(), movementTiming_.getDelayAfterLastShotMs());

    LOG.i("AutomatState: %s", stateToName(panoState_.getAutomatState()));

    const PanoPosition& panoPos = panoState_.getPanoPosition();
    LOG.i("PanoPos.shot: %d/%d", panoPos.getShotIndex(), panoPos.getShotCount());
    LOG.i("PanoPos.pos: %d/%d", panoPos.getPositionIndex(), panoPos.getPositionCount());
  }

 private:
  AutomatState_t setState(AutomatState_t newState) {
    AutomatState_t old = panoState_.setState(newState);
    statusCallback_(pano_, panoState_);
    LOG.i("State change %s -> %s", stateToName(old), stateToName(newState));
    return old;
  }

  AutomatState_t getState() { return panoState_.getAutomatState(); }

  void onTimer() {
    switch (getState()) {
      case AutomatState_t::WAIT_AFTER_MOVE:
      case AutomatState_t::WAIT_BETWEEN_SHOTS: {
        setState(AutomatState_t::FOCUS);
        cameraCallback_(true, false);
        triggerFocusTime();
      } break;

      case AutomatState_t::FOCUS: {
        setState(AutomatState_t::TRIGGER);
        cameraCallback_(true, true);  // or false,true ?
        triggerTriggerTime();
      } break;

      case AutomatState_t::TRIGGER: {
        cameraCallback_(false, false);
        if (panoState_.getPanoPosition().next().isFirstShot()) {
          setState(AutomatState_t::WAIT_AFTER_LAST_SHOT);
          triggerDelayAfterLastShot();
        } else {
          setState(AutomatState_t::WAIT_BETWEEN_SHOTS);
          triggerDelayBetwenShots();
        }
      } break;

      case AutomatState_t::WAIT_AFTER_LAST_SHOT: {
        if (panoState_.getPanoPosition()) {
          setState(AutomatState_t::MOVE);
          triggerMove();
        } else {
          setState(AutomatState_t::FINISH);
        }
      } break;

      default:
        break;
    }
  }

  void triggerDelayAfterMove() { triggerTimerMs(movementTiming_.getDelayAfterMoveMs()); }
  void triggerDelayBetwenShots() { triggerTimerMs(movementTiming_.getDelayBetweenShotsMs()); }
  void triggerDelayAfterLastShot() { triggerTimerMs(movementTiming_.getDelayAfterLastShotMs()); }

  void triggerFocusTime() {
    uint32_t shotIndex = panoState_.getPanoPosition().getShotIndex();
    uint32_t focusTime = pano_.getShot(shotIndex).getFocusTimeMs();
    triggerTimerMs(focusTime);
  }

  void triggerTriggerTime() {
    uint32_t shotIndex = panoState_.getPanoPosition().getShotIndex();
    uint32_t triggerTime = pano_.getShot(shotIndex).getTriggerTimeMs();
    triggerTimerMs(triggerTime);
  }

  void triggerTimerMs(uint32_t ms) {
    // LOG.i("Timer startet for %dms", ms);
    timer_.startMs(ms);
  }

  void triggerMove() {
    Position p = pano_.getPosition(panoState_.getPanoPosition().getPositionIndex());
    moveCallback_(p);
  }

  Logger& LOG;
  SingleTimer timer_;

  Pano pano_;
  MovementTiming movementTiming_;
  PanoState panoState_;

  MoveCallback_t moveCallback_;
  CameraCallback_t cameraCallback_;
  StatusCallback_t statusCallback_;
};