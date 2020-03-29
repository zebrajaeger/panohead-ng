#include "menuitem_shot.h"

//------------------------------------------------------------------------------
MenuItemShot::MenuItemShot(Display *display, const std::string &name)
    : MenuItemBase(display, name),
      shotIndex_(0),
      shot_(NULL),
      focusItem_(NULL),
      triggerItem_(NULL),
      focusTimeMs_(0),
      triggerTimeMs_(0),
      enabled_(false)
//------------------------------------------------------------------------------
{
  focusItem_ = new MenuItemNumber(display, "Focus", "s", 30000);
  focusItem_->onSave([this](const MenuItemNumber &self) { focusTimeMs_ = self.getValue(); });
  add(focusItem_);

  triggerItem_ = new MenuItemNumber(display, "Trigger", "s", 300000);
  triggerItem_->onSave([this](const MenuItemNumber &self) { triggerTimeMs_ = self.getValue(); });
  add(triggerItem_);

  add(new MenuItem("ENABLE"));

  add(new MenuItem("Ok"));

  add(new MenuItem("Cancel"));

  onEnter([](MenuItem &self) { self.getSelector().first(); });
  onRender(std::bind(&MenuItemShot::render, this, std::placeholders::_1));
  onButtonPushed([this](MenuItem &self) {
    switch (getSelectedIndex()) {
      case Items::FOCUS:
        focusItem_->setValue(focusTimeMs_);
        setActiveItem(0);
        break;
      case Items::TRIGGER:
        triggerItem_->setValue(triggerTimeMs_);
        setActiveItem(1);
        break;
      case Items::ENABLE:
        enabled_ = !enabled_;
        requireRepaint();
        break;
      case Items::OK:
        if (shot_) {
          shot_->setEnabled(enabled_);
          shot_->setFocusTimeMs(focusTimeMs_);
          shot_->setTriggerTimeMs(triggerTimeMs_);
          Distributor::i.getShots().propagateChange();
        }
        goUp();
        break;
      case Items::CANCEL:
        goUp();
        break;
    }
    return false;
  });
}

//------------------------------------------------------------------------------
void MenuItemShot::setShotIndex(uint16_t shotIndex)
//------------------------------------------------------------------------------
{
  shotIndex_ = shotIndex;
  shot_ = &(Distributor::i.getShots().get().getShotRaw(shotIndex_));
  if (shot_) {
    enabled_ = shot_->getEnabled();
    focusTimeMs_ = shot_->getFocusTimeMs();
    triggerTimeMs_ = shot_->getTriggerTimeMs();
  } else {
    enabled_ = false;
    focusTimeMs_ = 0;
    triggerTimeMs_ = 0;
  }
}

//------------------------------------------------------------------------------
void MenuItemShot::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
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
  if (selected == Items::FOCUS) {
    u8g2->drawFrame(0, y - 1, 127, 16);
  }

  // trigger
  y = 28;
  DisplayUtils::drawStringAt(u8g2, 10, y, false, false, "Shot ");
  DisplayUtils::drawNumberAt(u8g2, 50, y, DisplayUtils::NONE, false, triggerTimeMs_);
  if (selected == Items::TRIGGER) {
    u8g2->drawFrame(00, y - 1, 127, 16);
  }

  // enable
  DisplayUtils::drawSymbolAt(u8g2, 0 + 8, 60, selected == Items::ENABLE,
                             enabled_ ? DisplayUtils::Symbol::CHECKED : DisplayUtils::Symbol::UNCHECKED);

  // ok
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 60, selected == Items::OK, DisplayUtils::Symbol::OK);

  // cancel
  u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, selected == Items::CANCEL, DisplayUtils::Symbol::CANCEL);

  u8g2->sendBuffer();
}
