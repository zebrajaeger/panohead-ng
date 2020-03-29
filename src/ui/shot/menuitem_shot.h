#pragma once

#include <functional>

#include "distributor.h"
#include "ui/menuitem_base.h"
#include "ui/menuitem_number.h"

class MenuItemShot : public MenuItemBase {
 public:
  enum Items { FOCUS = 0, TRIGGER = 1, ENABLE = 2, OK = 3, CANCEL = 4 };
  MenuItemShot(Display *display, const std::string &name);
  void setShotIndex(uint16_t shotIndex);

 private:
  void render(MenuItem &menu);

  uint16_t shotIndex_;
  Shot *shot_;
  MenuItemNumber *focusItem_;
  MenuItemNumber *triggerItem_;
  uint32_t focusTimeMs_;
  uint32_t triggerTimeMs_;
  bool enabled_;
};