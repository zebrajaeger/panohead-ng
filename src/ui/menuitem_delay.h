#pragma once

#include <functional>

#include "menuitem_base.h"
#include "displayutils.h"

class MenuItemDelay : public MenuItemBase {
 public:
  typedef std::function<void(int32_t timeMs)> SaveCallback_t;

  enum Index { S = 0, MS = 1, OK = 2, CANCEL = 3 };

  //------------------------------------------------------------------------------
  MenuItemDelay(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name),
        timeMs_(0),
        saveCallback_()
  //------------------------------------------------------------------------------
  {
    using namespace std;
    using namespace std::placeholders;

    add(new MenuItemBase(u8g2, "s"))
        ->onRender(bind(&MenuItemDelay::renderS, this, _1))
        ->onIndexChanged(bind(&MenuItemDelay::encoderS, this, _1, _2))
        ->onButtonPushed([](MenuItem &self) {
          self.goUp();
          return false;
        });
    add(new MenuItemBase(u8g2, "ms"))
        ->onRender(bind(&MenuItemDelay::renderMs, this, _1))
        ->onIndexChanged(bind(&MenuItemDelay::encoderMs, this, _1, _2))
        ->onButtonPushed([](MenuItem &self) {
          self.goUp();
          return false;
        });
    add(new MenuItemBase(u8g2, "ok"));
    add(new MenuItemBase(u8g2, "cancel"));

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
  void onSave(SaveCallback_t saveCallback)
  //------------------------------------------------------------------------------
  {
    saveCallback_ = saveCallback;
  }

  //------------------------------------------------------------------------------
  void render(MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    switch ((Index)menu.getSelector().getIndex()) {
      case Index::S:
        DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, true, false, false, false);
        break;
      case Index::MS:
        DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, false, true, false, false);
        break;
      case Index::OK:
        DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, false, false, true, false);
        break;
      case Index::CANCEL:
        DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, false, false, false, true);
        break;
      default:
        break;
    }
  }

  //------------------------------------------------------------------------------
  void renderS(MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, true, false, false, false);
  }

  //------------------------------------------------------------------------------
  void renderMs(MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    DisplayUtils::renderSetTime(u8g2_, getName().c_str(), timeMs_, false, true, false, false);
  }

  //------------------------------------------------------------------------------
  bool addToMs(int16_t diff)
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
  void encoderS(MenuItem &self, Incrementor::Step step)
  //------------------------------------------------------------------------------
  {
    addToMs(1000 * step);
    self.requireRepaint();
  }

  //------------------------------------------------------------------------------
  void encoderMs(MenuItem &self, Incrementor::Step step)
  //------------------------------------------------------------------------------
  {
    addToMs(100 * step);
    self.requireRepaint();
  }

 private:
  int32_t timeMs_;
  SaveCallback_t saveCallback_;
};