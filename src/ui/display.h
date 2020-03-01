#pragma once

#include <Arduino.h>
// #include <functional>

#include <U8g2lib.h>

#include "menu/menuitem.h"
#include "pano/view.h"
#include "util/logger.h"

// #include "selection.h"

class Display {
 public:
  // enum class MainState : uint8_t { BOOT, MAIN };
  // enum class MainState : uint8_t { OFF, BOOT, MAIN };
  //   enum class MenuState : uint8_t { MENU, SET_BOUNDS, TAKE_PANO, LEVELING };
  //   enum class PanoBoundsState : uint8_t {
  //     TOP,
  //     TOP_RIGHT,
  //     RIGHT,
  //     BOTTOM_RIGHT,
  //     BOTTOM,
  //     BOTTOM_LEFT,
  //     LEFT,
  //     TOP_LEFT,
  //     PARTIAL,
  //     SAVE,
  //     CANCEL,
  //     MAX = CANCEL
  //   };

  //   typedef std::function<void(View view)> NewViewCallback_t;

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
  //   void moveSelection(int16_t diff);
  //   void onButton();

 private:
  //   const char* stateToString(MainState s) const;
  //   const char* stateToString(MenuState s) const;
  //   const char* stateToString(PanoBoundsState s) const;

  //   void setState(MainState newState);
  //   void setState(MenuState newState);
  //   // void setState(PanoBoundsState newState);

  // void showUnimplementedScreen();
  void renderBootScreen(MenuItem& menu, RangeCounter& counter);
  void renderMainMenu(MenuItem& menu, RangeCounter& counter);
  // void showLeveling();
  // void showSetPanoBounds();

  // bool isRedrawRequired();
  // void requireRedraw();
  // void resetRedrawRequirement();

  // void onNewView(NewViewCallback_t newViewCallback) { newViewCallback_ = newViewCallback; }

  void drawAngleAt(uint8_t x, uint8_t y, bool selected, bool invers, float angle);
  void drawStringAt(uint8_t x, uint8_t y, bool selected, bool invers, const char* text);

  Logger LOG;
  U8G2_SSD1306_128X64_NONAME_F_SW_I2C* u8g2_;
  bool isBooting_;
  MenuItem menu_;
  MenuItem* menuLeveling_;
  MenuItem* menuSetBounds_;

  // MainState mainState_;
  // MenuState menuState_;
  // PanoBoundsState panoBoundsState_;

  // bool dirty_;
  // Selection mainSelection_;
  // Selection setPanoSelection_;
  float levelX_;
  float levelY_;
  View view_;

  // NewViewCallback_t newViewCallback_;
};