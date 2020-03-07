#pragma once

#include <functional>

#include "menuitem_base.h"
#include "displayutils.h"

class MenuItemLevel : public MenuItemBase {
 public:
  //------------------------------------------------------------------------------
  MenuItemLevel(U8G2 *u8g2, const std::string &name)
      : MenuItemBase(u8g2, name)
  //------------------------------------------------------------------------------
  {
    using namespace std;
    using namespace std::placeholders;

    onRender(bind(&MenuItemLevel::render, this, _1));

    onButtonPushed([](MenuItem &self) {
      self.goUp();
      return false;
    });
  }

  //------------------------------------------------------------------------------
  virtual void setLeveling(float x, float y) override
  //------------------------------------------------------------------------------
  {
    if (x < -0.5) x = -0.5;
    if (x > 0.5) x = 0.5;
    if (y < -0.5) x = -0.5;
    if (y > 0.5) y = 0.5;

    levelX_ = x;
    levelY_ = y;
    requireRepaint();
  }

 private:
  //------------------------------------------------------------------------------
  void render(MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    u8g2_->clearBuffer();

    u8g2_->setDrawColor(1);
    u8g2_->drawHLine(33, 32, 63);
    u8g2_->drawVLine(64, 1, 63);
    u8g2_->drawCircle(64, 32, 31);
    u8g2_->drawCircle(64, 32, 3);

    float posX = 64 + (levelX_ * 128.0);
    float posY = 32 + (levelY_ * 128.0);
    u8g2_->drawCircle(posX, posY, 5);

    u8g2_->sendBuffer();
  }

  float levelX_;
  float levelY_;
};