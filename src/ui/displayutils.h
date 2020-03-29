#pragma once

#include <Arduino.h>
#include <U8g2lib.h>

class DisplayUtils {
 public:
  enum Symbol {
    PAUSE = 0xd2,
    PLAY = 0xd3,
    STOP = 0xda,
    BATTERY_EMPTY = 0x05A,
    BATTERY_FULL = 0x05B,
    OK = 0x73,
    CHECKED = 0x78,
    UNCHECKED = 0x79,
    CLOCK = 0x7B,
    COMPASS = 0x88,
    LOCKED = 0xca,
    UNLOCKED = 0xcb,
    PIN = 0xe8,
    ONOFFSWITCH = 0xeb,
    SORT = 0xf2,
    TRASH = 0x10f,
    WARN = 0x118,
    REPAIR = 0x11a,
    CANCEL = 0x11b,

    PANO_PARTIAL=0x8c,
    PANO_FULL=0xf6,
  };

  enum NumberSelection { NONE, FULL, PART, ALL };

  //------------------------------------------------------------------------------
  static void drawSymbolAt(U8G2 *u8g2, uint8_t x, uint8_t y, bool selected, Symbol symbol)
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
  static void drawNumberAt(U8G2 *u8g2, uint8_t x, uint8_t y, NumberSelection selection, bool invers, uint32_t value)
  //------------------------------------------------------------------------------
  {
    uint8_t height = u8g2->getMaxCharHeight() - 3;

    int32_t full = value / 1000;
    int32_t part = (value - (full * 1000)) / 100;

    char buf[20];
    sprintf(buf, " %5d", full);
    DisplayUtils::drawStringAt(u8g2, x, y, selection == FULL, false, (char *)&buf);

    DisplayUtils::drawStringAt(u8g2, x + 25, y, false, false, ".");

    sprintf(buf, "%1d", part);
    DisplayUtils::drawStringAt(u8g2, x + 30, y, selection == PART, false, (char *)&buf);

    if (selection == ALL) {
      u8g2->drawFrame(x, y, 50, height + 1);
    }
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
  static void renderMenu(U8G2 *u8g2, MenuItem &menu)
  //------------------------------------------------------------------------------
  {
    u8g2->clearBuffer();

    const std::vector<MenuItem *> &items = menu.getItems();
    u8g2->setFont(u8g2_font_timR10_tf);
    uint8_t h = u8g2->getMaxCharHeight() - 2;
    uint8_t selected = menu.getSelector().getIndex();
    uint8_t size = items.size();
    uint8_t page = selected / 8;

    uint8_t start = page * 8;
    uint8_t end = size;
    if ((end - start) > 8) {
      end = start + 8;
    }

    uint16_t x = 0;
    uint16_t y = 0;

    for (std::size_t i = start; i < end; ++i) {
      const MenuItem &item = *items[i];
      if (item.isEnabled()) {
        u8g2->setFont(u8g2_font_timR10_tf);
        drawStringAt(u8g2, x, y, (i == selected), false, item.getName().c_str());
      } else {
        u8g2->setFont(u8g2_font_timR08_tf);
        std::string sep1 = "  ";
        std::string sep2 = "  ";
        drawStringAt(u8g2, x, y + 2, (i == selected), false, (sep1 + item.getName() + sep2).c_str());
      }

      y += h;
      if (y > 64 - h) {
        y = 0;
        x += 64;
      }
    }

    u8g2->sendBuffer();
  }
};
