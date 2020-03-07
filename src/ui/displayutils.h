#pragma once

#include <Arduino.h>

#include <U8g2lib.h>

class DisplayUtils {
 public:
   //------------------------------------------------------------------------------
  static void drawSymbolAt(U8G2 *u8g2, uint8_t x, uint8_t y, bool selected, uint16_t symbol)
  //------------------------------------------------------------------------------
  {
    u8g2->setFont(u8g2_font_open_iconic_all_2x_t);
    u8g2->setFontMode(0);
    u8g2->setDrawColor(1);
    u8g2->drawGlyph(x + 2, y - 2, symbol);

    if (selected) {
      u8g2->drawFrame(x, y - 20, 16 + 4, 16 + 4);
    }
  }

  //------------------------------------------------------------------------------
  static void drawAngleAt(U8G2 *u8g2, uint8_t x, uint8_t y, bool selected, bool invers, float angle)
  //------------------------------------------------------------------------------
  {
    char buf[20];
    sprintf(buf, "%6.2f", angle);
    drawStringAt(u8g2, x, y, selected, invers, (char *)&buf);
  }

  //------------------------------------------------------------------------------
  static void drawStringAt(U8G2 *u8g2, uint8_t x, uint8_t y, bool selected, bool invers, const char *text)
  //------------------------------------------------------------------------------
  {
    uint8_t stringWidth = u8g2->getStrWidth(text);
    uint8_t height = u8g2->getMaxCharHeight() - 3;

    if (invers) {
      u8g2->setFontMode(1);
      u8g2->setDrawColor(1);
      u8g2->drawBox(x + 2, y + 2, stringWidth + 1, height - 2);
      u8g2->setDrawColor(0);
    } else {
      u8g2->setFontMode(0);
      u8g2->setDrawColor(1);
    }
    u8g2->drawStr(x + 2, y + height - 2, text);

    if (selected) {
      u8g2->setDrawColor(1);
      u8g2->drawFrame(x, y, stringWidth + 4, height + 1);
    }
  }

  //------------------------------------------------------------------------------
  static void renderSetTime(U8G2 *u8g2, const char *title, int32_t timeMs, bool selSecs, bool selOneTenthSecs, bool selOk, bool selCancel)
  //------------------------------------------------------------------------------
  {
    u8g2->clearBuffer();

    // headline
    u8g2->setFont(u8g2_font_timR10_tf);
    drawStringAt(u8g2, 0, 0, false, false, title);

    int32_t sec = timeMs / 1000;
    int32_t mSec = (timeMs - (sec * 1000)) / 100;

    char buf[20];
    sprintf(buf, " %5d", sec);
    drawStringAt(u8g2, 35, 24, selSecs, false, (char *)&buf);

    drawStringAt(u8g2, 60, 24, false, false, ".");

    sprintf(buf, "%1d", mSec);
    drawStringAt(u8g2, 65, 24, selOneTenthSecs, false, (char *)&buf);

    // ok
    drawSymbolAt(u8g2, 64 + 8, 60, selOk, 0x73);

    // cancel
    u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
    drawSymbolAt(u8g2, 64 + 8 + 24, 60, selCancel, 0x11b);

    u8g2->sendBuffer();
  }

  //------------------------------------------------------------------------------
  static void renderMenu(U8G2 *u8g2, MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    u8g2->clearBuffer();

    uint8_t h = u8g2->getMaxCharHeight() - 2;
    uint8_t selected = menu.getSelector().getIndex();
    const std::vector<MenuItem *> &items = menu.getItems();
    for (std::size_t i = 0; i < items.size(); ++i) {
      const MenuItem &item = *items[i];
      if (item.isEnabled()) {
        u8g2->setFont(u8g2_font_timR10_tf);
        drawStringAt(u8g2, 0, i * h, (i == selected), false, item.getName().c_str());
      } else {
        u8g2->setFont(u8g2_font_timR08_tf);
        std::string sep1 = "  ";
        std::string sep2 = "  ";
        drawStringAt(u8g2, 0, i * h + 2, (i == selected), false, (sep1 + item.getName() + sep2).c_str());
      }
    }

    u8g2->sendBuffer();
  }
};
