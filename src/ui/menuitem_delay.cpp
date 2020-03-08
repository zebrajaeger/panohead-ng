#include "menuitem_delay.h"

#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemDelay::MenuItemDelay(Display *display, const std::string &name)
    : MenuItemBase(display, name),
      timeMs_(0),
      saveCallback_()
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  add(new MenuItemBase(display, "s"))
      ->onRender(bind(&MenuItemDelay::renderS, this, _1))
      ->onIndexChanged(bind(&MenuItemDelay::encoderS, this, _1, _2))
      ->onButtonPushed([](MenuItem &self) {
        self.goUp();
        return false;
      });
  add(new MenuItemBase(display, "ms"))
      ->onRender(bind(&MenuItemDelay::renderMs, this, _1))
      ->onIndexChanged(bind(&MenuItemDelay::encoderMs, this, _1, _2))
      ->onButtonPushed([](MenuItem &self) {
        self.goUp();
        return false;
      });
  add(new MenuItemBase(display, "ok"));
  add(new MenuItemBase(display, "cancel"));

  onRender(bind(&MenuItemDelay::render, this, _1));
  onButtonPushed([this](MenuItem &self) {
    int16_t sel = getSelector().getIndex();
    if (sel == Index::OK) {
      if (saveCallback_) {
        saveCallback_(timeMs_);
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
MenuItemBase *MenuItemDelay::onSave(SaveCallback_t saveCallback)
//------------------------------------------------------------------------------
{
  saveCallback_ = saveCallback;
  return this;
}

//------------------------------------------------------------------------------
void MenuItemDelay::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();

  switch ((Index)menu.getSelector().getIndex()) {
    case Index::S:
      DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, true, false, false, false);
      break;
    case Index::MS:
      DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, false, true, false, false);
      break;
    case Index::OK:
      DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, false, false, true, false);
      break;
    case Index::CANCEL:
      DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, false, false, false, true);
      break;
    default:
      break;
  }
}

//------------------------------------------------------------------------------
void MenuItemDelay::renderS(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, true, false, false, false);
}

//------------------------------------------------------------------------------
void MenuItemDelay::renderMs(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  DisplayUtils::renderSetTime(u8g2, getName().c_str(), timeMs_, false, true, false, false);
}

//------------------------------------------------------------------------------
bool MenuItemDelay::addToMs(int16_t diff)
//------------------------------------------------------------------------------
{
  if (diff > 0) {
    int32_t newVal = timeMs_ + diff;
    if (newVal <= 10000) {
      timeMs_ = newVal;
      return true;
    }
  } else if (diff < 0) {
    int32_t newVal = timeMs_ + diff;
    if (newVal >= 0) {
      timeMs_ = newVal;
      return true;
    }
  }
  return false;
}

//------------------------------------------------------------------------------
void MenuItemDelay::encoderS(MenuItem &self, Incrementor::Step step)
//------------------------------------------------------------------------------
{
  addToMs(1000 * step);
  self.requireRepaint();
}

//------------------------------------------------------------------------------
void MenuItemDelay::encoderMs(MenuItem &self, Incrementor::Step step)
//------------------------------------------------------------------------------
{
  addToMs(100 * step);
  self.requireRepaint();
}
