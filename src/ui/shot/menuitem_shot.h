#pragma once

#include <functional>

#include "distributor.h"
#include "ui/menuitem_base.h"
#include "ui/menuitem_number.h"

class MenuItemShot : public MenuItemBase {
 public:
  MenuItemShot(Display *display, const std::string &name)
      : MenuItemBase(display, name), shotIndex_(0), shot_(NULL), focusItem_(NULL), triggerItem_(NULL), focusTimeMs_(0), triggerTimeMs_(0) {
    focusItem_ = new MenuItemNumber(display, "Focus", "s", 30000);
    focusItem_->onSave([this](const MenuItemNumber &self) { focusTimeMs_ = self.getValue(); });
    add(focusItem_);

    triggerItem_ = new MenuItemNumber(display, "Trigger", "s", 300000);
    triggerItem_->onSave([this](const MenuItemNumber &self) { triggerTimeMs_ = self.getValue(); });
    add(triggerItem_);

    add(new MenuItem("Ok"));

    add(new MenuItem("Cancel"));

    onRender(std::bind(&MenuItemShot::render, this, std::placeholders::_1));
    onButtonPushed([this](MenuItem &self) {
      switch (getSelectedIndex()) {
        case 0:
          focusItem_->setValue(focusTimeMs_);
          setActiveItem(0);
          break;
        case 1:
          triggerItem_->setValue(triggerTimeMs_);
          setActiveItem(1);
          break;
        case 2:
          // TODO SAVE
          goUp();
          break;
        case 3:
          goUp();
          break;
      }
      return false;
    });
  }

  void setShotIndex(uint16_t shotIndex) {
    shotIndex_ = shotIndex;
    shot_ = &(Distributor::i.getShots().get().getShotRaw(shotIndex_));
    if (shot_) {
      focusTimeMs_ = shot_->getFocusTimeMs();
      triggerTimeMs_ = shot_->getTriggerTimeMs();
    } else {
      focusTimeMs_ = 0;
      triggerTimeMs_ = 0;
    }
  }

 private:
  void render(MenuItem &menu) {
    U8G2 *u8g2 = getU8g2();

    u8g2->clearBuffer();

    u8g2->setFont(u8g2_font_timR10_tf);
    uint16_t selected = getSelectedIndex();

    uint8_t y;
    // headline
    y = 0;
    DisplayUtils::drawStringAt(u8g2, 0, y, false, false, getName().c_str());

    // focus
    y = 14;
    DisplayUtils::drawStringAt(u8g2, 10, y, false, false, "Focus ");
    DisplayUtils::drawNumberAt(u8g2, 50, y, DisplayUtils::NONE, false, focusTimeMs_);
    if (selected == 0) {
      u8g2->drawFrame(0, y - 1, 127, 16);
    }

    // trigger
    y = 28;
    DisplayUtils::drawStringAt(u8g2, 10, y, false, false, "Shot ");
    DisplayUtils::drawNumberAt(u8g2, 50, y, DisplayUtils::NONE, false, triggerTimeMs_);
    if (selected == 1) {
      u8g2->drawFrame(00, y - 1, 127, 16);
    }

    // ok
    DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 60, selected == 2, 0x73);

    // cancel
    u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
    DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, selected == 3, 0x11b);

    u8g2->sendBuffer();
  }

  uint16_t shotIndex_;
  Shot *shot_;
  MenuItemNumber *focusItem_;
  MenuItemNumber *triggerItem_;
  uint32_t focusTimeMs_;
  uint32_t triggerTimeMs_;
};