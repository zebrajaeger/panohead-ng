#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

#include "menu/menuitem.h"
#include "pano/view.h"
#include "util/loggerfactory.h"

class Display {
 public:
  enum class PanoBoundsState : uint8_t {
    TOP,
    TOP_RIGHT,
    RIGHT,
    BOTTOM_RIGHT,
    BOTTOM,
    BOTTOM_LEFT,
    LEFT,
    TOP_LEFT,
    PARTIAL,
    SAVE,
    CANCEL,
    MAX = CANCEL
  };

  Display();
  ~Display();

  bool begin(uint8_t sclGpio, uint8_t sdaGpio);
  void loop();
  void statistics();

  void bootStart();
  void bootFinished();

  void setLeveling(float x, float y);
  void setPosition(double revX, double revY);

  void encoderChanged(int16_t diff);
  void buttonPushed();

 private:
  void renderBootScreen(MenuItem& menu);
  void renderMainMenu(MenuItem& menu);
  void renderLeveling(MenuItem& menu);
  void renderSetBounds(MenuItem& menu);
  void pushButtonSetBounds(MenuItem& menu);

  void drawAngleAt(uint8_t x, uint8_t y, bool selected, bool invers, float angle);
  void drawStringAt(uint8_t x, uint8_t y, bool selected, bool invers, const char* text);

  Logger &LOG;
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C* u8g2_;
  bool isBooting_;
  MenuItem menu_;
  MenuItem* menuLeveling_;
  MenuItem* menuSetBounds_;
  float levelX_;
  float levelY_;
  View view_;
};