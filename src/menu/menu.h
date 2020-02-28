#pragma once

#include <Arduino.h>

#include "menuitem.h"

class Menu {
 public:
  Menu() : root_(NULL) {}
  void begin(MenuItem* root) { root_ = root; }

  void pushEncoder(int16_t diff) {
    if (root_) {
      root_->pushEncoder(diff);
    }
  }

  void pushButton() {
    if (root_) {
      root_->pushButton();
    }
  }

  void pushLoop() {
    if (root_) {
      root_->pushLoop();
    }
  }

 private:
  MenuItem* root_;
};