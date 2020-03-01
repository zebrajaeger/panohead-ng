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
  typedef std::function<void(MenuItem& self)> RenderCallback_t;
  typedef std::function<void(MenuItem& self)> ButtonPushCallback_t;

  MenuItem(std::string name);
  virtual void add(MenuItem* newItem);
  bool isLeaf() const;

  void goUp();
  bool setActiveItem(int16_t index);
  bool setActiveItem(std::string name);
  MenuItem* getActiveItem();
  const MenuItem* getActiveItem() const;
  bool isActive() const;
  std::string getActivePath();

  const std::string& getName() const;
  const std::vector<MenuItem*>& getItems() const;
  const Counter& getCounter() const;
  Counter& getCounter();

  void encoderChanged(int16_t diff);
  void buttonPushed();

  void loop();
  void onRender(RenderCallback_t cb);
  void onButtonPushed(ButtonPushCallback_t cb);
  void requireRepaint();

 private:
  void setParent(MenuItem* parent);

  std::string name_;
  MenuItem* parent_;
  std::vector<MenuItem*> items_;
  MenuItem* active_;
  bool requireRepaint_;
  RenderCallback_t renderCallback_;
  ButtonPushCallback_t buttonPushCallback_;
  Counter counter_;
};
