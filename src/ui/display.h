#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "util/loggerfactory.h"
#include "menu/menuitem.h"

class Display {
 public:
  Display();
  ~Display();

  bool begin();

  void statistics();
  void bootStart();
  void bootFinished(); 
  void loop();
  void encoderChanged(int16_t diff);
  void buttonPushed();

  U8G2* getU8g2();

 private:
  Logger &LOG;
  U8G2* u8g2_;
  bool isBooting_;
  MenuItem menu_;
};