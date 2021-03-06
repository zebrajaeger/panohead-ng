#include "menuitem_shots.h"

//------------------------------------------------------------------------------
MenuItemShots::MenuItemShots(Display *display, const std::string &name)
    : MenuItemBase(display, name),
      selectionIndexSort_(0),
      selectionIndexOk_(0),
      shotItem_(NULL)
//------------------------------------------------------------------------------
{
  // shots
  ShotRow &shots = *(Distributor::i.getShots());
  char buf[10];
  for (uint8_t i = 0; i < shots.getCapacity(); ++i) {
    sprintf((char *)&buf, "Shot %d", i);
    add(new MenuItem((char *)&buf));
  }

  // Sort
  selectionIndexSort_ = shots.getCapacity();
  add(new MenuItem("Sort"));

  // Ok
  selectionIndexOk_ = selectionIndexSort_ + 1;
  add(new MenuItem("Ok"));

  // "hidden" Shot
  shotItem_ = new MenuItemShot(display, "Shot");
  shotItem_->disable();
  add(shotItem_);

  onRender(std::bind(&MenuItemShots::render, this, std::placeholders::_1));
  onButtonPushed([this](MenuItem &self) {
    uint16_t selectionIndex = self.getSelector().getIndex();
    if (selectionIndex == selectionIndexOk_) {
      goUp();
    } else {
      shotItem_->setShotIndex(selectionIndex);
      setActiveItem("Shot");
    }
    return false;
  });
}

//------------------------------------------------------------------------------
void MenuItemShots::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  const ShotRow &shots = *(Distributor::i.getShots());
  uint8_t size = shots.getCapacity();
  uint8_t i = 0;
  uint8_t x;
  uint8_t y;
  uint8_t selected = menu.getSelector().getIndex();

  // char buf[4];
  for (uint8_t col = 0; col < 3 && i < size; ++col) {
    y = (col * 16) + 2;
    for (uint8_t row = 0; row < 5 && i < size; ++row) {
      x = (row * 25) + 3;

      if (shots.getShotRaw(i).getEnabled()) {
        u8g2->drawBox(x, y, 20, 10);
      } else {
        u8g2->drawFrame(x, y, 20, 10);
      }

      if (i == selected) {
        u8g2->drawFrame(x - 2, y - 2, 24, 14);
      }
      // sprintf((char *)&buf, "%2d", i);
      // u8g2->drawStr(x, y, (char *)&buf);
      i++;
    }
  }

  // sort
  DisplayUtils::drawSymbolAt(u8g2, 00, 64, selected == selectionIndexSort_, DisplayUtils::Symbol::SORT);

  // ok
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 64, selected == selectionIndexOk_, DisplayUtils::Symbol::OK);

  u8g2->sendBuffer();
}