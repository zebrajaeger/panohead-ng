#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

class Display {
 public:
  Display() : u8g2_(NULL){};

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

 private:
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C *u8g2_;
  ;
};