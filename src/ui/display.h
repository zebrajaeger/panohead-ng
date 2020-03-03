#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "menu/menuitem.h"
#include "pano/view.h"
#include "util/logger.h"

class Display {
 public:
  Display();
  ~Display();

  bool begin(uint8_t sclGpio, uint8_t sdaGpio);
  void loop();
  void statistics();

  void bootStart();
  void bootFinished();

  void setLeveling(float x, float y);
  void setPositionX(double revX);
  void setPositionY(double revY);

  void encoderChanged(int16_t diff);
  void buttonPushed();

 private:
  void renderBootScreen(MenuItem& menu);
  void renderMainMenu(MenuItem& menu);
  void renderLeveling(MenuItem& menu);
  void renderSetPanoBounds(MenuItem& menu);
  void renderSetPanoBounds_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel);
  bool pushButtonSetBounds(MenuItem& menu);

  void drawSymbolAt(uint8_t x, uint8_t y, bool selected, uint16_t symbol);
  void drawAngleAt(uint8_t x, uint8_t y, bool selected, bool invers, float angle);
  void drawStringAt(uint8_t x, uint8_t y, bool selected, bool invers, const char* text);

  Logger LOG;
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C* u8g2_;
  bool isBooting_;
  MenuItem menu_;
  MenuItem* menuLeveling_;
  MenuItem* menuSetBounds_;
  float levelX_;
  float levelY_;
  float posRevX_;
  float posRevY_;
  View view_;
};