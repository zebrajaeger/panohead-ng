#pragma once

#include <Arduino.h>

#include <functional>
#include <string>
#include <vector>

#include "incrementor.h"
#include "selector.h"
#include "util/loggerfactory.h"

// =====================================================================
class MenuItem;
class MenuItem {
 public:
  typedef std::function<void(MenuItem& self)> RenderCallback_t;
  /**
   * @return false to prevent the change of active item
   */
  typedef std::function<bool(MenuItem& self)> ButtonPushCallback_t;
  /**
   * @return false to prevent the change of active item
   */
  typedef std::function<bool(MenuItem& self, int16_t diff)> EncoderPushCallback_t;
  typedef std::function<void(MenuItem& self, Incrementor::Step upDown)> IndexChangedCallback_t;
  typedef std::function<bool(MenuItem& self, int16_t from, int16_t to)> SelectionChangedCallback_t;
  typedef std::function<void(MenuItem& self)> EnterItemCallback_t;

  MenuItem(const std::string& name);
  ~MenuItem();

  MenuItem* add(MenuItem* newItem);
  MenuItem* addAsActive(MenuItem* newItem);
  bool isLeaf() const { return items_.empty(); }

  void goUp(bool callEnterNotification = true);
  bool setActiveItem(int16_t index, bool callEnterNotification = true);
  bool setActiveItem(std::string name, bool callEnterNotification = true);
  MenuItem* getActiveItem() { return active_; }
  const MenuItem* getActiveItem() const { return active_; }
  bool isActive() const { return this == active_; }
  std::string getActivePath();
  MenuItem* getActivePathItem();

  const std::string& getName() const { return name_; }
  const std::vector<MenuItem*>& getItems() const { return items_; }
  const Selector& getSelector() const { return selector_; }
  Selector& getSelector() { return selector_; }
  uint16_t getSelectedIndex() { return selector_.getIndex(); }

  void encoderChanged(int16_t diff);
  void buttonPushed();

  void loop();
  MenuItem* onRender(RenderCallback_t cb);
  MenuItem* onButtonPushed(ButtonPushCallback_t cb);
  MenuItem* onIndexChanged(IndexChangedCallback_t cb);
  MenuItem* onSelectionChanged(SelectionChangedCallback_t cb);
  MenuItem* onEnter(EnterItemCallback_t cb);
  void requireRepaint() { requireRepaint_ = true; }
  void requireRepaintParent() {
    if (parent_) {
      parent_->requireRepaint();
    }
  }

  bool isEnabled() const { return enabled_; }
  MenuItem* setEnabled(bool enabled);
  MenuItem* enable();
  MenuItem* disable();

  bool isSubMenuOnly() { return subMenuOnly_; }
  MenuItem* subMenuOnly() { return setSubMenuOnly(true); }
  MenuItem* setSubMenuOnly(bool subMenuOnly);

  MenuItem* getParent() { return parent_; }
  const MenuItem* getParent() const { return parent_; }

  MenuItem* operator[](int16_t index);
  MenuItem* operator[](std::string name);

 protected:
  Logger& LOG;

 private:
  void setParent(MenuItem* parent) { parent_ = parent; }
  void propagateEnter();

  std::string name_;
  bool subMenuOnly_;
  bool enabled_;
  MenuItem* parent_;
  std::vector<MenuItem*> items_;
  MenuItem* active_;
  bool requireRepaint_;
  RenderCallback_t renderCallback_;
  ButtonPushCallback_t buttonPushCallback_;
  // EncoderPushCallback_t encoderPushCallback_;
  IndexChangedCallback_t indexChangedCallback_;
  Incrementor incrementor_;
  SelectionChangedCallback_t selectionChangedCallback_;
  Selector selector_;
  EnterItemCallback_t enterItemCallback_;
};
