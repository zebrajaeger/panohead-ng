#pragma once

// #include <Arduino.h>
// #include <functional>

// #include <TMC429.h>

// #include "util/singletimer.h"

// class MovementMonitor {
//  public:
//   typedef std::function<void(uint8_t axisIndex, bool atTargetPos)> movementStatusChangeCallback_t;
//   MovementMonitor()
//       : tmc429_(NULL),
//         timer_("MovementMonitor"),
//         movementStatusChangeCallback_(NULL),
//         axis0Moving_(false),
//         axis1Moving_(false),
//         axis2Moving_(false) {}

//   bool begin(TMC429* tmc429, uint32_t periodMs) {
//     tmc429_ = tmc429;
//     updateState(false);
//     timer_.onTimer([this]() {
//       updateState(true);
//     });
//     timer_.start(periodMs * 1000, false, true);
//     return true;
//   }

//   void loop() { timer_.loop(); }

//   void onStatusChange(movementStatusChangeCallback_t cb) { movementStatusChangeCallback_ = cb; }

//  protected:
//   void updateState(bool sendUpdate) {
//     TMC429::Status s = tmc429_->getStatus();
//     if (sendUpdate) {
//       if (axis0Moving_ != s.at_target_position_0) {
//         movementStatusChangeCallback_(0, s.at_target_position_0);
//       }
//       if (axis1Moving_ != s.at_target_position_1) {
//         movementStatusChangeCallback_(1, s.at_target_position_0);
//       }
//       if (axis2Moving_ != s.at_target_position_2) {
//         movementStatusChangeCallback_(2, s.at_target_position_0);
//       }
//     }
//     axis0Moving_ = s.at_target_position_0;
//     axis1Moving_ = s.at_target_position_1;
//     axis2Moving_ = s.at_target_position_2;
//   }

//  private:
//   TMC429* tmc429_;
//   SingleTimer timer_;
//   movementStatusChangeCallback_t movementStatusChangeCallback_;
//   bool axis0Moving_;
//   bool axis1Moving_;
//   bool axis2Moving_;
// };
