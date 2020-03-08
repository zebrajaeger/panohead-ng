#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "util/logger.h"
#include "menu/menuitem.h"

#include "panodata.h"

class Display {
 public:
  Display();
  ~Display();

  bool begin(uint8_t sclGpio, uint8_t sdaGpio);

  void setLeveling(float x, float y);
  void setPositionX(double revX);
  void setPositionY(double revY);

  void statistics();
  void bootStart();
  void bootFinished(); 
  void loop();
  void encoderChanged(int16_t diff);
  void buttonPushed();

  const PanoData& getPanoData() const;
  PanoData& getPanoData();
  U8G2* getU8g2();

 private:
  Logger LOG;
  U8G2* u8g2_;
  bool isBooting_;
  MenuItem menu_;
  PanoData panoData_;
};