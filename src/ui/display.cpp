#include "display.h"

#include <functional>

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
  MenuItem *boot = new MenuItem("boot");
  boot->onRender(bind(&Display::renderBootScreen, this, _1));
  menu_.add(boot);

  // Main
  MenuItem *main = new MenuItem("main");
  main->onRender(bind(&Display::renderMainMenu, this, _1));
  menu_.add(main);

  // -- Pano Config
  MenuItem *panoConfig = new MenuItem("Pano config");
  main->add(panoConfig);
  MenuItem *delayAfterMove = new MenuItem("Delay after move");
  panoConfig->add(delayAfterMove);
  MenuItem *delayBetweenShots = new MenuItem("Delay between shots");
  panoConfig->add(delayBetweenShots);
  MenuItem *shotCount = new MenuItem("Shot count");
  panoConfig->add(shotCount);
  MenuItem *shots = new MenuItem("shots");
  panoConfig->add(shots);

  // -- Set Bounds
  MenuItem *setBounds = new MenuItem("Set bounds");
  setBounds->getCounter().setMax((uint16_t)PanoBoundsState::MAX);
  setBounds->onRender(bind(&Display::renderSetBounds, this, _1));
  setBounds->onButtonPushed(bind(&Display::pushButtonSetBounds, this, _1));
  main->add(setBounds);
  menuSetBounds_ = setBounds;

  // -- Make Pano
  MenuItem *takePano = new MenuItem("Take pano");
  main->add(takePano);

  // -- Leveling
  MenuItem *leveling = new MenuItem("Leveling");
  leveling->onRender(bind(&Display::renderLeveling, this, _1));
  leveling->onButtonPushed([](MenuItem &self) { self.goUp(); });
  main->add(leveling);
  menuLeveling_ = leveling;

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
void Display::setPosition(double revX, double revY)
//------------------------------------------------------------------------------
{
  menuSetBounds_->requireRepaint();
}

//------------------------------------------------------------------------------
void Display::renderBootScreen(MenuItem &menu)
//------------------------------------------------------------------------------
{
  u8g2_->setContrast(255);  // !!!

  u8g2_->clearBuffer();
  u8g2_->setFont(u8g2_font_ncenB10_tr);

  drawStringAt(0, 0, false, false, "Booting...");

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::renderMainMenu(MenuItem &menu)
//------------------------------------------------------------------------------
{
  u8g2_->clearBuffer();
  u8g2_->setFont(u8g2_font_ncenB10_tr);

  uint8_t selected = menu.getCounter().getIndex();
  const std::vector<MenuItem *> &items = menu.getItems();
  for (std::size_t i = 0; i < items.size(); ++i) {
    const MenuItem &item = *items[i];
    drawStringAt(0, i * 16, (i == selected), false, item.getName().c_str());
  }

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::drawAngleAt(uint8_t x, uint8_t y, bool selected, bool invers, float angle)
//------------------------------------------------------------------------------
{
  char buf[20];
  sprintf(buf, "%06.2f", angle);
  drawStringAt(x, y, selected, invers, (char *)&buf);
}

//------------------------------------------------------------------------------
void Display::drawStringAt(uint8_t x, uint8_t y, bool selected, bool invers, const char *text)
//------------------------------------------------------------------------------
{
  uint8_t stringWidth = u8g2_->getStrWidth(text);
  uint8_t height = u8g2_->getMaxCharHeight();

  if (invers) {
    u8g2_->setFontMode(1);
    u8g2_->setDrawColor(1);
    u8g2_->drawBox(x + 2, y + 2, stringWidth + 1, height - 1);
    u8g2_->setDrawColor(0);
  } else {
    u8g2_->setFontMode(0);
    u8g2_->setDrawColor(1);
  }
  u8g2_->drawStr(x + 3, y + height - 2, text);

  if (selected) {
    u8g2_->setDrawColor(1);
    u8g2_->drawHLine(x, y, stringWidth + 4);
    u8g2_->drawHLine(x, y + height + 1, stringWidth + 4);
    u8g2_->drawVLine(x, y, height + 1);
    u8g2_->drawVLine(x + stringWidth + 4, y, height + 1);
  }
}

// //------------------------------------------------------------------------------
// void Display::showUnimplementedScreen()
// //------------------------------------------------------------------------------
// {
//   u8g2_->clearBuffer();
//   u8g2_->setFont(u8g2_font_ncenB10_tr);

//   drawStringAt(50, 0, false, false, "!!!!");
//   drawStringAt(0, 16, false, false, "Please");
//   drawStringAt(0, 32, false, false, "implement me");
//   drawStringAt(50, 48, false, false, "!!!!");

//   u8g2_->sendBuffer();
// }

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
void Display::renderSetBounds(MenuItem &menu)
//------------------------------------------------------------------------------
{
  u8g2_->clearBuffer();

  PanoBoundsState i = (PanoBoundsState)menu.getCounter().getIndex();
  bool isTop = i == PanoBoundsState::TOP || i == PanoBoundsState::TOP_LEFT || i == PanoBoundsState::TOP_RIGHT;
  bool isBottom = i == PanoBoundsState::BOTTOM || i == PanoBoundsState::BOTTOM_LEFT || i == PanoBoundsState::BOTTOM_RIGHT;
  bool isLeft = i == PanoBoundsState::LEFT || i == PanoBoundsState::TOP_LEFT || i == PanoBoundsState::BOTTOM_LEFT;
  bool isRight = i == PanoBoundsState::RIGHT || i == PanoBoundsState::TOP_RIGHT || i == PanoBoundsState::BOTTOM_RIGHT;
  bool isPartial = i == PanoBoundsState::PARTIAL;
  bool isSave = i == PanoBoundsState::SAVE;
  bool isCancel = i == PanoBoundsState::CANCEL;

  u8g2_->setFont(u8g2_font_ncenB10_tr);

  drawAngleAt(50, 0, isTop, false, view_.getX1());
  drawAngleAt(50, 48, isBottom, false, view_.getX2());
  drawAngleAt(0, 24, isLeft, false, view_.getY1());
  drawAngleAt(83, 24, isRight, false, view_.getY2());

  drawStringAt(52, 16, isPartial, view_.isPartial(), "Prt");
  drawStringAt(50, 32, isSave, false, "O");
  drawStringAt(67, 32, isCancel, false, "X");

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::pushButtonSetBounds(MenuItem &menu)
//------------------------------------------------------------------------------
{
  switch ((PanoBoundsState)menu.getCounter().getIndex()) {
    case PanoBoundsState::PARTIAL:
      view_.setPartial(!view_.isPartial());
      menu.requireRepaint();
      break;
    case PanoBoundsState::SAVE:
      // TODO save
      menu.goUp();
      break;
    case PanoBoundsState::CANCEL:
      menu.goUp();
      break;
    default:
      break;
  }
}