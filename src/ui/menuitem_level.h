#pragma once

#include <functional>

#include "menuitem_base.h"

class MenuItemLevel : public MenuItemBase {
 public:
  MenuItemLevel(Display *display, const std::string &name);
  virtual void setLeveling(float x, float y) override;

 private:
  void render(MenuItem &menu);

  float levelX_;
  float levelY_;
};