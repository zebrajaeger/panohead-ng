#pragma once

#include <Arduino.h>
#include <functional>
#include <string>
#include <vector>

class MenuItem {
 public:
  MenuItem(std::string name) : name_(name), items_(), active_(this) {}
  MenuItem& add(MenuItem* newItem) { items_.push_back(newItem); }

  void setActiveItem(int8_t index) {
    if (index == -1) {
      active_ = this;
    } else {
      active_ = items_[index];
    }
  }

  MenuItem* getActiveItem() { return active_; }
  bool isActive() { return this == active_; }

  void pushEncoder(int16_t diff) {
    if (isActive()) {
      onEncoder(diff);
    } else {
      active_->pushEncoder(diff);
    }
  }

  void pushButton() {
    if (isActive()) {
      onButton();
    } else {
      active_->pushButton();
    }
  }

  void pushLoop() {
    if (isActive()) {
      onLoop();
    } else {
      active_->pushLoop();
    }
  }

 protected:
  virtual void onEncoder(int16_t diff) = 0;
  virtual void onButton() = 0;
  virtual void onLoop() = 0;

 private:
  std::string name_;
  std::vector<MenuItem*> items_;
  MenuItem* active_;
};


class LambdaMenuitem : public MenuItem {
 public:
  typedef std::function<void()> LoopCallback_t;
  typedef std::function<void()> ButtonCallback_t;
  typedef std::function<void(int16_t diff)> EncoderCallback_t;

  LambdaMenuitem(std::string name) : MenuItem(name) {}
  virtual void onLoop(LoopCallback_t cb) { loopCallback_ = cb; }
  virtual void onButton(ButtonCallback_t cb) { buttonCallback_ = cb; }
  virtual void onEncoder(EncoderCallback_t cb) { encoderCallback_ = cb; }

 protected:
  virtual void onEncoder(int16_t diff) {
    if (encoderCallback_) {
      encoderCallback_(diff);
    }
  }
  virtual void onButton() {
    if (buttonCallback_) {
      buttonCallback_();
    }
  }
  virtual void onLoop() {
    if (loopCallback_) {
      loopCallback_();
    }
  }

 private:
  LoopCallback_t loopCallback_;
  ButtonCallback_t buttonCallback_;
  EncoderCallback_t encoderCallback_;
};