#pragma once

#include <functional>

#include "menuitem_base.h"
#include "displayutils.h"

class MenuItemBoot : public MenuItemBase {
 public:
  //------------------------------------------------------------------------------
  MenuItemBoot(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name)
  //------------------------------------------------------------------------------
  {
    using namespace std;
    using namespace std::placeholders;

    onRender(bind(&MenuItemBoot::render, this, _1));
  }

 private:
  //------------------------------------------------------------------------------
  void render(MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    u8g2_->clearBuffer();
    u8g2_->setFont(u8g2_font_timR10_tf);

    DisplayUtils::drawStringAt(u8g2_, 0, 0, false, false, "Booting...");

    u8g2_->sendBuffer();
  }
};