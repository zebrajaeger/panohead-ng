#pragma once

#include <Arduino.h>

#include "pano/view.h"

class PanoData {
 public:
  void setView(const View& view) { view_ = view; }
  const View& getView() const { return view_; }

  void setDelayAfterMove(int32_t delayAfterMove) { delayAfterMove_ = delayAfterMove; }
  int32_t getDelayAfterMove() { return delayAfterMove_; }
  void setFocusTime(int32_t focusTime) { focusTime_ = focusTime; }
  int32_t getFocusTime() { return focusTime_; }
  void setTriggerTime(int32_t triggerTime) { triggerTime_ = triggerTime; }
  int32_t getTriggerTime() { return triggerTime_; }

 private:
  View view_;
  int32_t delayAfterMove_;
  int32_t focusTime_;
  int32_t triggerTime_;
};