#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "util/logger.h"

class Display {
 public:
  Display() : LOG("Display"), u8g2_(NULL){};

  ~Display() {
    if (u8g2_) {
      delete u8g2_;
    }
  }

  bool begin(uint8_t sclGpio, uint8_t sdaGpio) {
    if (u8g2_) {
      return false;
    }
    u8g2_ = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, sclGpio, sdaGpio, /* reset=*/U8X8_PIN_NONE);
    return u8g2_->begin();
  }

  void showBootScreen() {
    u8g2_->clearBuffer();
    u8g2_->setFont(u8g2_font_ncenB10_tr);
    u8g2_->drawStr(0, 20, "Booting...");
    u8g2_->sendBuffer();
  }

  // paramters: [-0.25 - +0.25]
  void showLeveling(float revX, float revY) {
    u8g2_->clearBuffer();
    u8g2_->drawHLine(33, 32, 63);
    u8g2_->drawVLine(64, 1, 63);
    u8g2_->drawCircle(64, 32, 31);
    u8g2_->drawCircle(64, 32, 3);
    if (revX < -0.5) revX = -0.5;
    if (revX > 0.5) revX = 0.5;
    if (revY < -0.5) revY = -0.5;
    if (revY > 0.5) revY = 0.5;

    float posX = 64 + (revX * 128.0);
    float posY = 32 + (revY * 128.0);
    u8g2_->drawCircle(posX, posY, 5);

    u8g2_->sendBuffer();
  }

 private:
  Logger LOG;
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C *u8g2_;
};