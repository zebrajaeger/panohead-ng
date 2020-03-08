#pragma once

#include <Arduino.h>
#include <functional>
#include <string>
#include <vector>

#include "selector.h"
#include "incrementor.h"

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

  MenuItem(const std::string& name);

  MenuItem* add(MenuItem* newItem);
  MenuItem* addAsActive(MenuItem* newItem);
  bool isLeaf() const;

  void goUp();
  bool setActiveItem(int16_t index);
  bool setActiveItem(std::string name);
  MenuItem* getActiveItem();
  const MenuItem* getActiveItem() const;
  bool isActive() const;
  std::string getActivePath();
  MenuItem* getActivePathItem();

  const std::string& getName() const;
  const std::vector<MenuItem*>& getItems() const;
  const Selector& getSelector() const;
  Selector& getSelector();

  void encoderChanged(int16_t diff);
  void buttonPushed();

  void loop();
  MenuItem* onRender(RenderCallback_t cb);
  MenuItem* onButtonPushed(ButtonPushCallback_t cb);
  MenuItem* onIndexChanged(IndexChangedCallback_t cb);
  MenuItem* onSelectionChanged(SelectionChangedCallback_t cb);
  void requireRepaint();

  bool isEnabled() const;
  MenuItem* setEnabled(bool enabled);
  MenuItem* enable();
  MenuItem* disable();

  bool isSubMenuOnly();
  MenuItem* subMenuOnly();
  MenuItem* setSubMenuOnly(bool subMenuOnly);

  MenuItem* getParent();
  const MenuItem* getParent() const;

  MenuItem* operator[](int16_t index);
  MenuItem* operator[](std::string name);

 private:
  void setParent(MenuItem* parent);

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
};
