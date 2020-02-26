#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "util/logger.h"

#include "selection.h"

class Display {
 public:
  typedef enum { OFF, BOOTSCREEN, MAIN, PANO_CONFIG, SET_BOUNDS, TAKE_PANO, LEVELING } menuState_t;

  Display() : LOG("Display"), u8g2_(NULL), menuState_(MAIN), dirty_(true), mainSelection_(2, 8), levelX_(0.5), levelY_(0.5){};

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

  void loop() {
    if (isRedrawRequired()) {
      resetRedrawRequirement();
      switch (menuState_) {
        case OFF:
          // nothing to do
          break;
        case BOOTSCREEN:
          showBootScreen();
          break;
        case MAIN:
          showMainMenu();
          break;
        case LEVELING:
          showLeveling();
          break;
        default:
          showUnimplementedScreen();
      }
    }
  }

  menuState_t getMenuState() const { return menuState_; }
  void setLeveling(float x, float y) {
    if (x < -0.5) x = -0.5;
    if (x > 0.5) x = 0.5;
    if (y < -0.5) x = -0.5;
    if (y > 0.5) y = 0.5;

    levelX_ = x;
    levelY_ = y;
    if (menuState_ == LEVELING) {
      requireRedraw();
    }
  }

  void moveSelection(int16_t diff) {
    LOG.d("DIFF: %d", diff);
    switch (menuState_) {
      case MAIN:
        mainSelection_.add(diff);
        requireRedraw();
        break;
      default:
        break;
    }
  }

  void onButton() {
    switch (menuState_) {
      case MAIN: {
        switch (mainSelection_.getIndex()) {
          case 0:
            setState(SET_BOUNDS);
            requireRedraw();
            break;
          case 1:
            setState(TAKE_PANO);
            requireRedraw();
            break;
          case 2:
            setState(LEVELING);
            requireRedraw();
            break;
          default:
            break;
        }
      } break;

      case LEVELING:
        setState(MAIN);
        requireRedraw();
        break;

      default:
        break;
    }
  }

 private:
  const char* stateToName(menuState_t s) {
    switch (s) {
      case OFF:
        return "OFF";
      case BOOTSCREEN:
        return "BOOTSCREEN";
      case MAIN:
        return "MAIN";
      case PANO_CONFIG:
        return "PANO_CONFIG";
      case SET_BOUNDS:
        return "SET_BOUNDS";
      case TAKE_PANO:
        return "TAKE_PANO";
      case LEVELING:
        showLeveling();
        return "LEVELING";
      default:
        return "?unknown?";
    }
  }

  void setState(menuState_t newState) {
    LOG.d("statechange %s -> %s", stateToName(menuState_), stateToName(newState));
    menuState_ = newState;
  }

  void showUnimplementedScreen() {
    u8g2_->clearBuffer();
    u8g2_->setFont(u8g2_font_ncenB10_tr);

    drawString("!!!!", 0, false);
    drawString("Please", 1, false);
    drawString("implement me", 2, false);
    drawString("!!!!", 3, false);

    u8g2_->sendBuffer();
  }

  void showBootScreen() {
    u8g2_->setContrast(255);  // !!!

    u8g2_->clearBuffer();
    u8g2_->setFont(u8g2_font_ncenB10_tr);

    drawString("Booting...", 0, false);

    u8g2_->sendBuffer();
  }

  void showMainMenu() {
    u8g2_->clearBuffer();
    u8g2_->setFont(u8g2_font_ncenB10_tr);

    drawString("Set bounds", 0, mainSelection_.getIndex() == 0);
    drawString("Take pano", 1, mainSelection_.getIndex() == 1);
    drawString("Levelin", 2, mainSelection_.getIndex() == 2);

    u8g2_->sendBuffer();
  }

  void showLeveling() {
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

  bool isRedrawRequired() { return dirty_; }
  void requireRedraw() { dirty_ = true; }
  void resetRedrawRequirement() { dirty_ = false; }

  void drawString(const char* text, uint8_t lineIndex, bool inverted) {
    if (inverted) {
      u8g2_->setFontMode(1);
      u8g2_->setDrawColor(1);
      u8g2_->drawBox(0, lineIndex * 16, 128, 16);
      u8g2_->setDrawColor(0);
    } else {
      u8g2_->setFontMode(0);
      u8g2_->setDrawColor(1);
    }
    u8g2_->drawStr(5, (lineIndex * 16) + 14, text);
  }

  Logger LOG;
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C* u8g2_;

  menuState_t menuState_;
  bool dirty_;
  Selection mainSelection_;
  float levelX_;
  float levelY_;
};