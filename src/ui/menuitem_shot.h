#pragma once

#include <functional>

#include "distributor.h"
#include "menuitem_base.h"

class MenuItemShot : public MenuItemBase {
 public:
  MenuItemShot(Display *display, const std::string &name) : MenuItemBase(display, name) {
    add(new MenuItem("Ok"));
    add(new MenuItem("Cancel"));
    onRender(std::bind(&MenuItemShot::render, this, std::placeholders::_1));
    onButtonPushed([this](MenuItem &self) {
      goUp();
      return false;
    });
  }

  void setShotIndex(uint8_t shotIndex) { shotIndex_ = shotIndex; }

 private:
  void render(MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();

    u8g2->clearBuffer();

    uint16_t selected = getSelector().getIndex();

    // ok
    DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 60, selected == 0, 0x73);

    // cancel
    u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
    DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, selected == 1, 0x11b);

    u8g2->sendBuffer();
  }

  uint8_t shotIndex_;
};