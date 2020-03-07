#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "util/logger.h"

#include "menuitem_boot.h"
#include "menuitem_main.h"

class Display {
 public:
  Display() : LOG("Display"), u8g2_(NULL), isBooting_(true), menu_("root"){};
  ~Display() {
    if (u8g2_) delete u8g2_;
  }

  //------------------------------------------------------------------------------
  bool begin(uint8_t sclGpio, uint8_t sdaGpio)
  //------------------------------------------------------------------------------
  {
    if (u8g2_) {
      return false;
    }

    u8g2_ = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
    u8g2_->setI2CAddress(0x3c * 2);

    if (!u8g2_->begin()) {
      return false;
    }

    LOG.d("OLED I²C adr is %d", u8g2_GetI2CAddress(u8g2_->getU8g2()));

    u8g2_->setContrast(255);

    menu_.add(new MenuItemBoot(u8g2_, "boot"));
    menu_.add(new MenuItemMain(u8g2_, "main"));

    return true;
  }

  void setLeveling(float x, float y) { ((MenuItemBase*)menu_.getActivePathItem())->setLeveling(x, y); }
  void setPositionX(double revX) { ((MenuItemBase*)menu_.getActivePathItem())->setPositionX(revX); }
  void setPositionY(double revY) { ((MenuItemBase*)menu_.getActivePathItem())->setPositionY(revY); }

  void statistics() { LOG.d("Selected: %s", menu_.getActivePath().c_str()); }
  void bootStart() { menu_.setActiveItem("boot"); }
  void bootFinished() { menu_.setActiveItem("main"); }
  void loop() { menu_.loop(); }
  void encoderChanged(int16_t diff) { menu_.encoderChanged(diff); }
  void buttonPushed() { menu_.buttonPushed(); }

 private:
  Logger LOG;
  U8G2* u8g2_;
  bool isBooting_;
  MenuItem menu_;
};