#include "menuitem_number.h"

#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemNumber::MenuItemNumber(Display *display, const std::string &name, const std::string &postfix, int32_t maxValue)
    : MenuItemBase(display, name),
      value_(0),
      maxValue_(maxValue),
      postfix_(postfix),
      saveCallback_()
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  add(new MenuItemBase(display, "full"))
      ->onRender(bind(&MenuItemNumber::renderFull, this, _1))
      ->onIndexChanged(bind(&MenuItemNumber::encoderFull, this, _1, _2))
      ->onButtonPushed([](MenuItem &self) {
        self.goUp(false);
        return false;
      });
  add(new MenuItemBase(display, "part"))
      ->onRender(bind(&MenuItemNumber::renderPart, this, _1))
      ->onIndexChanged(bind(&MenuItemNumber::encoderPart, this, _1, _2))
      ->onButtonPushed([](MenuItem &self) {
        self.goUp(false);
        return false;
      });
  add(new MenuItemBase(display, "ok"));
  add(new MenuItemBase(display, "cancel"));

  onRender(bind(&MenuItemNumber::render, this, _1));
  onButtonPushed([this](MenuItem &self) {
    int16_t sel = getSelector().getIndex();
    if (sel == Index::OK) {
      if (saveCallback_) {
        saveCallback_(*this);
      }
      goUp();
      return false;
    } else if (sel == Index::CANCEL) {
      goUp();
      return false;
    }
    return true;
  });
}

//------------------------------------------------------------------------------
MenuItemNumber *MenuItemNumber::onSave(SaveCallback_t saveCallback)
//------------------------------------------------------------------------------
{
  saveCallback_ = saveCallback;
  return this;
}

//------------------------------------------------------------------------------
void MenuItemNumber::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch ((Index)menu.getSelector().getIndex()) {
    case Index::FULL:
      render_(true, false, false, false);
      break;
    case Index::PART:
      render_(false, true, false, false);
      break;
    case Index::OK:
      render_(false, false, true, false);
      break;
    case Index::CANCEL:
      render_(false, false, false, true);
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
bool MenuItemNumber::addToValue(int16_t diff)
//------------------------------------------------------------------------------
{
  if (diff > 0) {
    int32_t newVal = value_ + diff;
    if (newVal <= maxValue_) {
      value_ = newVal;
      return true;
    }
  } else if (diff < 0) {
    int32_t newVal = value_ + diff;
    if (newVal >= 0) {
      value_ = newVal;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
void MenuItemNumber::encoderFull(MenuItem &self, Incrementor::Step step)
//------------------------------------------------------------------------------
{
  addToValue(1000 * step);
  self.requireRepaint();
}

//------------------------------------------------------------------------------
void MenuItemNumber::encoderPart(MenuItem &self, Incrementor::Step step)
//------------------------------------------------------------------------------
{
  addToValue(100 * step);
  self.requireRepaint();
}

//------------------------------------------------------------------------------
void MenuItemNumber::render_(bool selFull, bool selPart, bool selOk, bool selCancel)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();

  u8g2->clearBuffer();

  // headline
  u8g2->setFont(u8g2_font_timR10_tf);
  DisplayUtils::drawStringAt(u8g2, 0, 0, false, false, getName().c_str());

  // number
  int32_t full = value_ / 1000;
  int32_t part = (value_ - (full * 1000)) / 100;

  char buf[20];
  sprintf(buf, " %5d", full);
  DisplayUtils::drawStringAt(u8g2, 35, 24, selFull, false, (char *)&buf);

  DisplayUtils::drawStringAt(u8g2, 60, 24, false, false, ".");

  sprintf(buf, "%1d", part);
  DisplayUtils::drawStringAt(u8g2, 65, 24, selPart, false, (char *)&buf);

  // postfix
  DisplayUtils::drawStringAt(u8g2, 75, 24, false, false, postfix_.c_str());

  // ok
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8, 60, selOk, DisplayUtils::Symbol::OK);

  // cancel
  u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
  DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, selCancel, DisplayUtils::Symbol::CANCEL);

  u8g2->sendBuffer();
}