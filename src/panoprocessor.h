#pragma once

// #include <Arduino.h>
// #include <functional>
// #include <vector>

// #include <util/logger.h>
// #include <util/singletimer.h>

// class PanoProcessor {
//  public:
//   typedef enum { IDLE, MOVING, DELAY_AFTER_MOVE, FOCUSSING, TRIGGERING, DELAY_BETWEEN_SHOTS, STOPPED } state_t;

//   typedef struct {
//     uint32_t focusTimeUs;
//     uint32_t triggerTimeUs;
//   } shot_t;

//   class World {
//    public:
//     World(uint32_t width, uint32_t heigth) : width_(width_), height_(heigth) {}
//     uint32_t getWidth() { return width_; }
//     uint32_t getHeight() { return height_; }

//    private:
//     uint32_t width_;
//     uint32_t height_;
//   };

//   class Scene {
//    public:
//     Scene(uint16_t countX, uint16_t countY, uint32_t top, uint32_t bottom, uint32_t left, uint32_t right)
//         : countX_(countX), countY_(countY), top_(top), bottom_(bottom), left_(left), right_(right) {}

//     uint16_t getCountX() { return countX_; }
//     uint16_t getCountY() { return countY_; }
//     uint32_t getTop() { return top_; }
//     uint32_t getBottom() { return bottom_; }
//     uint32_t getLeft() { return left_; }
//     uint32_t getRight() { return right_; }
//     uint32_t getWidth() { return right_ - left_; }
//     uint32_t getHeigth() { return bottom_ - top_; }

//    private:
//     uint16_t countX_;
//     uint16_t countY_;
//     uint32_t top_;
//     uint32_t bottom_;
//     uint32_t left_;
//     uint32_t right_;
//   };

//   typedef struct {
//     Scene scene;
//     std::vector<shot_t> shots;
//   } pano_t;

//   typedef struct {
//     uint32_t xPosIndex;
//     uint32_t yPosIndex;
//     uint8_t shotIndex;
//   } panoState_t;

//   class Movement {
//    public:
//     Movement(uint64_t delayAfterMoveUs = 0, uint64_t delayBetweenShotsUs = 0)
//         : delayAfterMoveUs_(delayAfterMoveUs), delayBetweenShotsUs_(delayBetweenShotsUs) {}
//     uint64_t getDelayAfterMoveUs() { return delayAfterMoveUs_; };
//     uint64_t getDelayBetweenShotsUs() { return delayBetweenShotsUs_; };

//    private:
//     uint64_t delayAfterMoveUs_;
//     uint64_t delayBetweenShotsUs_;
//   };

//   PanoProcessor() : LOG("PanoProcessor"), timer_("PanoProcessor"), state_(IDLE) {
//     timer_.onTimer([&]() {
//       if (state_ == DELAY_AFTER_MOVE) {
//         // focus
//         state_ = FOCUSSING;
//         cbCamera_(true, false);
//         timer_.start(pano_.shots[panoState_.shotIndex].focusTimeUs);

//       } else if (state_ == FOCUSSING) {
//         // trigger
//         state_ = TRIGGERING;
//         cbCamera_(true, true);
//         timer_.start(pano_.shots[panoState_.shotIndex].focusTimeUs);

//       } else if (state_ == TRIGGERING) {
//         cbCamera_(false, false);

//         panoState_.shotIndex++;
//         if (panoState_.shotIndex < pano_.shots.size()) {
//           // next shot
//           state_ = DELAY_BETWEEN_SHOTS;
//           timer_.start(movement_.getDelayBetweenShotsUs());

//         } else {
//           // next pos (all shots done)
//           panoState_.shotIndex = 0;
//           panoState_.xPosIndex++;
//           if (panoState_.xPosIndex < pano_.scene.getCountX()) {
//             // next column(x)

//             state_ = MOVING;
//             // TODO move
//           } else {
//             // next row(y)
//             panoState_.xPosIndex = 0; // TODO: except full pano
//             panoState_.yPosIndex++;
//             if (panoState_.yPosIndex < pano_.scene.getCountY()) {
//               // move to pos

//               state_ = MOVING;
//               // TODO move
//             } else {
//               // stop because we are done
//               state_ = STOPPED;
//               cbFinish_();
//             }
//           }
//         }

//         //   if (world_.getWidth() == pano_.scene.getWidth()){
//         //     // 360°
//         //     panoState_.xPosIndex = 0;
//         //     panoState_.yPosIndex++;
//         //     if(panoState_.yPosIndex<pano_.scene.getCountY()){
//         //         // TODO movexy
//         //
//         //     }else{
//         //         // TODO done
//         //     }
//         //   }else{
//         //       //partial
//         //
//         //       // TODO movexy
//         //   }

//         //   cbGotoPos_(pano_.scene.left_, pano_.scene.top_);
//       }
//     });
//   }

//   bool begin() { return true; }

//   bool setup(const World& world) {
//     world_ = world;
//     return true;
//   }

//   void setPano(pano_t& pano) { pano_ = pano; }

//   void setMovement(const Movement& movement) { movement_ = movement; }

//   void start() {
//     panoState_.xPosIndex = 0;
//     panoState_.yPosIndex = 0;
//     panoState_.shotIndex = 0;
//     state_ = MOVING;
//     cbGotoPos_(pano_.scene.getLeft(), pano_.scene.getTop());
//   }

//   void loop() { timer_.loop(); }

//   void triggerPosReached() {
//     if (state_ == MOVING) {
//       state_ = DELAY_AFTER_MOVE;
//       timer_.start(movement_.getDelayAfterMoveUs());
//     }
//   }

//   void onGoToPos(std::function<void(uint32_t, uint32_t)> cb) { cbGotoPos_ = cb; }
//   void onCamera(std::function<void(bool, bool)> cb) { cbCamera_ = cb; }
//   void onFinish(std::function<void()> cb) { cbFinish_ = cb; }

//  private:
//   Logger LOG;
//   SingleTimer timer_;
//   state_t state_;
//   World world_;
//   Movement movement_;
//   pano_t pano_;
//   panoState_t panoState_;

//   std::function<void(uint32_t, uint32_t)> cbGotoPos_;
//   std::function<void(bool, bool)> cbCamera_;
//   std::function<void()> cbFinish_;
// };