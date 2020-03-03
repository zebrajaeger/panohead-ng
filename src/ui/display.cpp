#include "display.h"

#include <functional>

#include "pano/panoutils.h"

//------------------------------------------------------------------------------
Display::Display()
    : LOG("Display"),
      u8g2_(NULL),
      isBooting_(true),
      menu_("root"),
      menuLeveling_(NULL),
      menuSetBounds_(NULL),
      levelX_(0.5),
      levelY_(0.5),
      posRevX_(0.0),
      posRevY_(0.0),
      view_(1.234, 12.345, 123.456, 0.0)
      //------------------------------------------------------------------------------
      {};

//------------------------------------------------------------------------------
Display::~Display()
//------------------------------------------------------------------------------
{
  if (u8g2_) {
    delete u8g2_;
  }
}

//------------------------------------------------------------------------------
bool Display::begin(uint8_t sclGpio, uint8_t sdaGpio)
//------------------------------------------------------------------------------
{
  if (u8g2_) {
    return false;
  }

  u8g2_ = new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, sclGpio, sdaGpio, /* reset=*/U8X8_PIN_NONE);
  if (!u8g2_->begin()) {
    return false;
  }

  using namespace std;
  using namespace std::placeholders;

  // Boot
  MenuItem *boot = menu_.add(new MenuItem("boot"));
  boot->onRender(bind(&Display::renderBootScreen, this, _1));

  // Main
  MenuItem *main = menu_.add(new MenuItem("main"));
  main->onRender(bind(&Display::renderMainMenu, this, _1));

  // -- Pano Config
  MenuItem *panoConfig = main->add(new MenuItem("Pano config"));
  panoConfig->add(new MenuItem("Delay after move"));
  panoConfig->add(new MenuItem("Delay between shots"));
  panoConfig->add(new MenuItem("Shot count"));
  panoConfig->add(new MenuItem("shots"));

  // -- Set Bounds
  menuSetBounds_ = main->add(new MenuItem("Set bounds"))
                       ->onButtonPushed(bind(&Display::pushButtonSetBounds, this, _1))
                       ->onRender(bind(&Display::renderSetPanoBounds, this, _1));
  menuSetBounds_->add(new MenuItem("TogglePartial"));
  menuSetBounds_->add(new MenuItem("Top"));
  menuSetBounds_->add(new MenuItem("Top Right"));
  menuSetBounds_->add(new MenuItem("Right"));
  menuSetBounds_->add(new MenuItem("Bottom Right"));
  menuSetBounds_->add(new MenuItem("Bottom"));
  menuSetBounds_->add(new MenuItem("Bottom Left"));
  menuSetBounds_->add(new MenuItem("Left"));
  menuSetBounds_->add(new MenuItem("Top Left"));
  menuSetBounds_->add(new MenuItem("Ok"));
  menuSetBounds_->add(new MenuItem("Cancel"));

  // -- Make Pano
  MenuItem *takePano = main->add(new MenuItem("Take pano"));
  takePano->disable();

  // -- Leveling
  MenuItem *leveling = main->add(new MenuItem("Leveling"));
  menuLeveling_ = leveling;
  leveling->onRender(bind(&Display::renderLeveling, this, _1));
  leveling->onButtonPushed([](MenuItem &self) {
    self.goUp();
    return false;
  });

  return true;
}

//------------------------------------------------------------------------------
void Display::statistics()
//------------------------------------------------------------------------------
{
  LOG.d("Selected: %s", menu_.getActivePath().c_str());
  // LOG.d("Level %f, %f", levelX_, levelY_);
  // LOG.d("View x:(%f - %f) y:(%f - %f)", view_.getX1(), view_.getX2(), view_.getY1(), view_.getY2());
}

//------------------------------------------------------------------------------
void Display::bootStart()
//------------------------------------------------------------------------------
{
  menu_.setActiveItem("boot");
}
//------------------------------------------------------------------------------
void Display::bootFinished()
//------------------------------------------------------------------------------
{
  menu_.setActiveItem("main");
}

//------------------------------------------------------------------------------
void Display::loop()
//------------------------------------------------------------------------------
{
  menu_.loop();
}

//------------------------------------------------------------------------------
void Display::encoderChanged(int16_t diff)
//------------------------------------------------------------------------------
{
  menu_.encoderChanged(diff);
}

//------------------------------------------------------------------------------
void Display::buttonPushed()
//------------------------------------------------------------------------------
{
  menu_.buttonPushed();
}

//------------------------------------------------------------------------------
void Display::setLeveling(float x, float y)
//------------------------------------------------------------------------------
{
  if (x < -0.5) x = -0.5;
  if (x > 0.5) x = 0.5;
  if (y < -0.5) x = -0.5;
  if (y > 0.5) y = 0.5;

  levelX_ = x;
  levelY_ = y;
  menuLeveling_->requireRepaint();
}

//------------------------------------------------------------------------------
void Display::setPositionX(double revX)
//------------------------------------------------------------------------------
{
  posRevX_ = revX;
  menuSetBounds_->requireRepaint();
}
//------------------------------------------------------------------------------
void Display::setPositionY( double revY)
//------------------------------------------------------------------------------
{
  posRevY_ = revY;
  menuSetBounds_->requireRepaint();
}

//------------------------------------------------------------------------------
void Display::renderBootScreen(MenuItem &menu)
//------------------------------------------------------------------------------
{
  u8g2_->setContrast(255);  // !!!

  u8g2_->clearBuffer();
  u8g2_->setFont(u8g2_font_timR10_tf);

  drawStringAt(0, 0, false, false, "Booting...");

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::renderMainMenu(MenuItem &menu)
//------------------------------------------------------------------------------
{
  u8g2_->clearBuffer();

  uint8_t h = u8g2_->getMaxCharHeight() - 2;
  uint8_t selected = menu.getCounter().getIndex();
  const std::vector<MenuItem *> &items = menu.getItems();
  for (std::size_t i = 0; i < items.size(); ++i) {
    const MenuItem &item = *items[i];
    if (item.isEnabled()) {
      u8g2_->setFont(u8g2_font_timR10_tf);
      drawStringAt(0, i * h, (i == selected), false, item.getName().c_str());
    } else {
      u8g2_->setFont(u8g2_font_timR08_tf);
      std::string sep1 = "  ";
      std::string sep2 = "  ";
      drawStringAt(0, i * h + 2, (i == selected), false, (sep1 + item.getName() + sep2).c_str());
    }
  }

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::drawSymbolAt(uint8_t x, uint8_t y, bool selected, uint16_t symbol)
//------------------------------------------------------------------------------
{
  u8g2_->setFont(u8g2_font_open_iconic_all_2x_t);
  u8g2_->setFontMode(0);
  u8g2_->setDrawColor(1);
  u8g2_->drawGlyph(x + 2, y - 2, symbol);

  if (selected) {
    u8g2_->drawFrame(x, y - 20, 16 + 4, 16 + 4);
  }
}

//------------------------------------------------------------------------------
void Display::drawAngleAt(uint8_t x, uint8_t y, bool selected, bool invers, float angle)
//------------------------------------------------------------------------------
{
  char buf[20];
  sprintf(buf, "%6.2f", angle);
  drawStringAt(x, y, selected, invers, (char *)&buf);
}

//------------------------------------------------------------------------------
void Display::drawStringAt(uint8_t x, uint8_t y, bool selected, bool invers, const char *text)
//------------------------------------------------------------------------------
{
  uint8_t stringWidth = u8g2_->getStrWidth(text);
  uint8_t height = u8g2_->getMaxCharHeight() - 3;

  if (invers) {
    u8g2_->setFontMode(1);
    u8g2_->setDrawColor(1);
    u8g2_->drawBox(x + 2, y + 2, stringWidth + 1, height - 2);
    u8g2_->setDrawColor(0);
  } else {
    u8g2_->setFontMode(0);
    u8g2_->setDrawColor(1);
  }
  u8g2_->drawStr(x + 2, y + height - 2, text);

  if (selected) {
    u8g2_->setDrawColor(1);
    u8g2_->drawFrame(x, y, stringWidth + 4, height + 1);
  }
}

//------------------------------------------------------------------------------
void Display::renderLeveling(MenuItem &menu)
//------------------------------------------------------------------------------
{
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

//------------------------------------------------------------------------------
void Display::renderSetPanoBounds(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch (menu.getCounter().getIndex()) {
    case 0:
      renderSetPanoBounds_(true, false, false, false, false, false, false);
      break;
    case 1:
      renderSetPanoBounds_(false, true, false, false, false, false, false);
      break;
    case 2:
      renderSetPanoBounds_(false, true, true, false, false, false, false);
      break;
    case 3:
      renderSetPanoBounds_(false, false, true, false, false, false, false);
      break;
    case 4:
      renderSetPanoBounds_(false, false, true, true, false, false, false);
      break;
    case 5:
      renderSetPanoBounds_(false, false, false, true, false, false, false);
      break;
    case 6:
      renderSetPanoBounds_(false, false, false, true, true, false, false);
      break;
    case 7:
      renderSetPanoBounds_(false, false, false, false, true, false, false);
      break;
    case 8:
      renderSetPanoBounds_(false, true, false, false, true, false, false);
      break;
    case 9:
      renderSetPanoBounds_(false, false, false, false, false, true, false);
      break;
    case 10:
      renderSetPanoBounds_(false, false, false, false, false, false, true);
      break;
  }
}

//------------------------------------------------------------------------------
void Display::renderSetPanoBounds_(bool togglePartial, bool top, bool right, bool bottom, bool left, bool ok, bool cancel)
//------------------------------------------------------------------------------
{
  // LOG.d("renderSetPanoBounds(%d, %d, %d, %d, %d, %d, %d)", togglePartial, top, right, bottom, left, ok, cancel);
  u8g2_->clearBuffer();

  // partial / full pano
  drawSymbolAt(22, 22 + 16, togglePartial, view_.isPartial() ? 0x8c : 0xf6);

  // top
  if (top) {
    u8g2_->drawBox(10, 0, 44, 10);
  } else {
    u8g2_->drawFrame(10, 0, 44, 10);
  }

  // right
  if (right) {
    u8g2_->drawBox(54, 10, 10, 44);
  } else {
    u8g2_->drawFrame(54, 10, 10, 44);
  }

  // bottom
  if (bottom) {
    u8g2_->drawBox(10, 53, 44, 10);
  } else {
    u8g2_->drawFrame(10, 53, 44, 10);
  }

  // left
  if (left) {
    u8g2_->drawBox(0, 10, 10, 44);
  } else {
    u8g2_->drawFrame(0, 10, 10, 44);
  }

  // pos:
  u8g2_->setFont(u8g2_font_timR10_tf);
  u8g2_->drawStr(71, 8, "x:");
  drawAngleAt(90, -2, false, false, PanoUtils::revToDeg(posRevX_));
  u8g2_->drawStr(71, 22, "y:");
  drawAngleAt(90, 12, false, false, PanoUtils::revToDeg(posRevY_));

  // ok
  drawSymbolAt(64 + 8, 60, ok, 0x73);

  // cancel
  u8g2_->setFont(u8g2_font_open_iconic_check_2x_t);
  drawSymbolAt(64 + 8 + 24, 60, cancel, 0x11b);

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
bool Display::pushButtonSetBounds(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch (menu.getCounter().getIndex()) {
    case 0:
      LOG.d("PARTIAL");
      view_.setPartial(!view_.isPartial());
      menu.requireRepaint();
      break;
    case 9:
      // TODO save
      LOG.d("SAVE");
      menu.goUp();
      break;
    case 10:
      LOG.d("CANCEL");
      menu.goUp();
      break;
    default:
      break;
  }
  return false;
}