#pragma once

#include <functional>

#include "distributor.h"
#include "menuitem_shot.h"
#include "ui/menuitem_base.h"

class MenuItemShots : public MenuItemBase {
 public:
  MenuItemShots(Display *display, const std::string &name);

 private:
  void render(MenuItem &menu);

  uint8_t selectionIndexSort_;
  uint8_t selectionIndexOk_;
  MenuItemShot *shotItem_;
};