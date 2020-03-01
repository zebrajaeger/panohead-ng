#pragma once

#include <Arduino.h>
#include <functional>
#include <string>
#include <vector>

#include "counter.h"

// =====================================================================
class MenuItem;
class MenuItem {
 public:
  typedef std::function<void(MenuItem& self, RangeCounter&counter)> RenderCallback_t;

  MenuItem(std::string name);
  virtual void add(MenuItem* newItem);
  bool isLeaf() const;

  bool setActiveItem(int16_t index);
  bool setActiveItem(std::string name);
  MenuItem* getActiveItem();
  const MenuItem* getActiveItem() const;
  bool isActive() const;
  std::string getActivePath();

  const std::string& getName() const;
  const std::vector<MenuItem*>& getItems() const;

  void encoderChanged(int16_t diff);
  void buttonPushed();

  void loop();
  void onRender(RenderCallback_t cb);
  void requireRepaint();

 private:
  void setParent(MenuItem* parent);

  std::string name_;
  MenuItem* parent_;
  std::vector<MenuItem*> items_;
  MenuItem* active_;
  bool requireRepaint_;
  RenderCallback_t renderCallback_;
  RangeCounter counter_;
};

// class LambdaMenuitem : public MenuItem {
//  public:
//   typedef std::function<void()> LoopCallback_t;
//   typedef std::function<void()> ButtonCallback_t;
//   typedef std::function<void(int16_t diff)> EncoderCallback_t;

//   LambdaMenuitem(std::string name) : MenuItem(name) {}
//   virtual void onLoop(LoopCallback_t cb) { loopCallback_ = cb; }
//   virtual void onButton(ButtonCallback_t cb) { buttonCallback_ = cb; }
//   virtual void onEncoder(EncoderCallback_t cb) { encoderCallback_ = cb; }

//  protected:
//   virtual void onEncoder(int16_t diff) {
//     if (encoderCallback_) {
//       encoderCallback_(diff);
//     }
//   }
//   virtual void onButton() {
//     if (buttonCallback_) {
//       buttonCallback_();
//     }
//   }
//   virtual void onLoop() {
//     if (loopCallback_) {
//       loopCallback_();
//     }
//   }

//  private:
//   LoopCallback_t loopCallback_;
//   ButtonCallback_t buttonCallback_;
//   EncoderCallback_t encoderCallback_;
// };