#include "display.h"

//------------------------------------------------------------------------------
Display::Display()
    : LOG("Display"),
      u8g2_(NULL),
      isBooting_(true),
      menu_("root"),
      menuLeveling_(NULL),
      menuSetBounds_(NULL),
      // mainState_(MainState::BOOT),
      // menuState_(MenuState::MENU),
      // // panoBoundsState_(PanoBoundsState::TOP),
      // dirty_(true),
      // mainSelection_(2, 4),
      // setPanoSelection_(10, 4),
      levelX_(0.5),
      levelY_(0.5),
      view_(1.234, 12.345, 123.456, 0.0)
      // newViewCallback_()
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
  boot->onRender(bind(&Display::renderBootScreen, this, _1, _2));

  // Main
  MenuItem *main = new MenuItem("main");
  main->onRender(bind(&Display::renderMainMenu, this, _1, _2));
  MenuItem *panoConfig = new MenuItem("Pano config");
  MenuItem *setBounds = new MenuItem("Set bounds");
  MenuItem *takePano = new MenuItem("Take pano");
  MenuItem *leveling = new MenuItem("Leveling");
  leveling->onRender(bind(&Display::renderLeveling, this, _1, _2));
  leveling->onButtonPushed([](MenuItem &self) { self.goUp(); });

  // Pano Config
  MenuItem *delayAfterMove = new MenuItem("Delay after move");
  MenuItem *delayBetweenShots = new MenuItem("Delay between shots");
  MenuItem *shotCount = new MenuItem("Shot count");
  MenuItem *shots = new MenuItem("shots");

  menu_.add(boot);
  menu_.add(main);
  main->add(panoConfig);
  panoConfig->add(delayAfterMove);
  panoConfig->add(delayBetweenShots);
  panoConfig->add(shots);
  panoConfig->add(shotCount);
  main->add(setBounds);
  main->add(takePano);
  main->add(leveling);

  menuLeveling_ = leveling;
  menuSetBounds_ = setBounds;

  return true;
}

//------------------------------------------------------------------------------
void Display::statistics()
//------------------------------------------------------------------------------
{
  LOG.d("Selected: %s", menu_.getActivePath().c_str());
  // LOG.d("Main.State: %s", stateToString(mainState_));

  // LOG.d("Menu.State %s", stateToString(menuState_));
  // LOG.d("Menu.Selected %d", mainSelection_.getIndex());

  // LOG.d("Bounds.Selected %i", setPanoSelection_.getIndex());

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
  // if (menuState_ == MenuState::LEVELING) {
  //   requireRedraw();
  // }
}

//------------------------------------------------------------------------------
void Display::setPosition(double revX, double revY)
//------------------------------------------------------------------------------
{
  menuSetBounds_->requireRepaint();
}

//------------------------------------------------------------------------------
void Display::renderBootScreen(MenuItem &menu, RangeCounter &counter)
//------------------------------------------------------------------------------
{
  u8g2_->setContrast(255);  // !!!

  u8g2_->clearBuffer();
  u8g2_->setFont(u8g2_font_ncenB10_tr);

  drawStringAt(0, 0, false, false, "Booting...");

  u8g2_->sendBuffer();
}

//------------------------------------------------------------------------------
void Display::renderMainMenu(MenuItem &menu, RangeCounter &counter)
//------------------------------------------------------------------------------
{
  u8g2_->clearBuffer();
  u8g2_->setFont(u8g2_font_ncenB10_tr);

  uint8_t selected = counter.getIndex();
  LOG.d("renderMainMenu %d", selected);
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
// void Display::loop()
// //------------------------------------------------------------------------------
// {
//   if (isRedrawRequired()) {
//     resetRedrawRequirement();
//     switch (mainState_) {
//       case MainState::OFF:
//         // nothing to do
//         break;

//       case MainState::BOOT:
//         showBootScreen();
//         break;

//       case MainState::MAIN:
//         switch (menuState_) {
//           case MenuState::MENU:
//             showMainMenu();
//             break;
//           case MenuState::SET_BOUNDS:
//             showSetPanoBounds();
//             break;
//           case MenuState::LEVELING:
//             showLeveling();
//             break;
//           case MenuState::TAKE_PANO:
//             showUnimplementedScreen();
//             break;
//         }
//     }
//   }
// }

// //------------------------------------------------------------------------------
// void Display::bootDone()
// //------------------------------------------------------------------------------
// {
//   setState(MainState::MAIN);
//   setState(MenuState::MENU);
// }

// //------------------------------------------------------------------------------
// void Display::moveSelection(int16_t diff)
// //------------------------------------------------------------------------------
// {
//   switch (mainState_) {
//     case MainState::MAIN:
//       switch (menuState_) {
//         case MenuState::MENU:
//           if (mainSelection_.add(diff)) {
//             requireRedraw();
//           }
//           break;

//         case MenuState::SET_BOUNDS:
//           if (setPanoSelection_.add(diff)) {
//             requireRedraw();
//           }
//           break;

//         default:
//           break;
//       }

//       break;

//     default:
//       break;
//   }
// }

// //------------------------------------------------------------------------------
// void Display::onButton()
// //------------------------------------------------------------------------------
// {
//   switch (mainState_) {
//     case MainState::OFF:
//     case MainState::BOOT:
//       break;

//     case MainState::MAIN: {
//       switch (menuState_) {
//         case MenuState::MENU:
//           switch (mainSelection_.getIndex()) {
//             case 0:
//               setState(MenuState::SET_BOUNDS);
//               break;
//             case 1:
//               setState(MenuState::TAKE_PANO);
//               break;
//             case 2:
//               setState(MenuState::LEVELING);
//               break;
//             default:
//               break;
//           }

//         case MenuState::TAKE_PANO:
//           // TODO
//           break;

//         case MenuState::LEVELING:
//           setState(MenuState::MENU);
//           requireRedraw();
//           break;

//         case MenuState::SET_BOUNDS:
//           switch ((PanoBoundsState)setPanoSelection_.getIndex()) {
//             case PanoBoundsState::PARTIAL:
//               view_.setPartial(!view_.isPartial());
//               requireRedraw();
//               break;
//             case PanoBoundsState::SAVE:
//               if (newViewCallback_) {
//                 newViewCallback_(view_);
//               }
//               setState(MenuState::MENU);
//               break;
//             case PanoBoundsState::CANCEL:
//               setState(MenuState::MENU);
//               break;
//             default:
//               break;
//           }
//           break;
//       }
//       break;

//       default:
//         break;
//     }
//   }
// }

// //------------------------------------------------------------------------------
// const char* Display::stateToString(MainState s) const
// //------------------------------------------------------------------------------
// {
//   switch (s) {
//     case MainState::OFF:
//       return "OFF";
//     case MainState::BOOT:
//       return "BOOT";
//     case MainState::MAIN:
//       return "MAIN";
//     default:
//       return "?unknown?";
//   }
// }
// //------------------------------------------------------------------------------
// const char* Display::stateToString(MenuState s) const
// //------------------------------------------------------------------------------
// {
//   switch (s) {
//     case MenuState::MENU:
//       return "MENU";
//     case MenuState::LEVELING:
//       return "LEVELING";
//     case MenuState::SET_BOUNDS:
//       return "SET_BOUNDS";
//     case MenuState::TAKE_PANO:
//       return "TAKE_PANO";
//     default:
//       return "?unknown?";
//   }
// }

// //------------------------------------------------------------------------------
// const char* Display::stateToString(PanoBoundsState s) const
// //------------------------------------------------------------------------------
// {
//   switch (s) {
//     case PanoBoundsState::TOP_LEFT:
//       return "TOP_LEFT";
//     case PanoBoundsState::TOP:
//       return "TOP";
//     case PanoBoundsState::TOP_RIGHT:
//       return "TOP_RIGHT";
//     case PanoBoundsState::BOTTOM_LEFT:
//       return "BOTTOM_LEFT";
//     case PanoBoundsState::BOTTOM:
//       return "BOTTOM";
//     case PanoBoundsState::BOTTOM_RIGHT:
//       return "BOTTOM_RIGHT";
//     case PanoBoundsState::LEFT:
//       return "LEFT";
//     case PanoBoundsState::RIGHT:
//       return "RIGHT";
//     case PanoBoundsState::PARTIAL:
//       return "PARTIAL";
//     case PanoBoundsState::CANCEL:
//       return "CANCEL";
//     case PanoBoundsState::SAVE:
//       return "SAVE";
//     default:
//       return "?unknown?";
//   }
// }

// //------------------------------------------------------------------------------
// void Display::setState(MainState newState)
// //------------------------------------------------------------------------------
// {
//   LOG.d("Main %s -> %s", stateToString(mainState_), stateToString(newState));
//   requireRedraw();
//   mainState_ = newState;
// }
// //------------------------------------------------------------------------------
// void Display::setState(MenuState newState)
// //------------------------------------------------------------------------------
// {
//   LOG.d("Menu %s -> %s", stateToString(menuState_), stateToString(newState));
//   requireRedraw();
//   menuState_ = newState;
// }

// // //------------------------------------------------------------------------------
// // void Display::setState(PanoBoundsState newState)
// // //------------------------------------------------------------------------------
// // {
// //   LOG.d("SetBounds %s -> %s", stateToString(menuState_), stateToString(newState));
// //   requireRedraw();
// // }

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
void Display::renderLeveling(MenuItem &menu, RangeCounter &counter)
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

// //------------------------------------------------------------------------------
// void Display::showSetPanoBounds()
// //------------------------------------------------------------------------------
// {
//   u8g2_->clearBuffer();

//   PanoBoundsState i = (PanoBoundsState)setPanoSelection_.getIndex();
//   bool isTop = i == PanoBoundsState::TOP || i == PanoBoundsState::TOP_LEFT || i == PanoBoundsState::TOP_RIGHT;
//   bool isBottom = i == PanoBoundsState::BOTTOM || i == PanoBoundsState::BOTTOM_LEFT || i == PanoBoundsState::BOTTOM_RIGHT;
//   bool isLeft = i == PanoBoundsState::LEFT || i == PanoBoundsState::TOP_LEFT || i == PanoBoundsState::BOTTOM_LEFT;
//   bool isRight = i == PanoBoundsState::RIGHT || i == PanoBoundsState::TOP_RIGHT || i == PanoBoundsState::BOTTOM_RIGHT;
//   bool isPartial = i == PanoBoundsState::PARTIAL;
//   bool isSave = i == PanoBoundsState::SAVE;
//   bool isCancel = i == PanoBoundsState::CANCEL;

//   u8g2_->setFont(u8g2_font_ncenB10_tr);

//   drawAngleAt(50, 0, isTop, false, view_.getX1());
//   drawAngleAt(50, 48, isBottom, false, view_.getX2());
//   drawAngleAt(0, 24, isLeft, false, view_.getY1());
//   drawAngleAt(83, 24, isRight, false, view_.getY2());

//   drawStringAt(52, 16, isPartial, view_.isPartial(), "Prt");
//   drawStringAt(50, 32, isSave, false, "O");
//   drawStringAt(67, 32, isCancel, false, "X");

//   u8g2_->sendBuffer();
// }

// //------------------------------------------------------------------------------
// bool Display::isRedrawRequired()
// //------------------------------------------------------------------------------
// {
//   return dirty_;
// }

// //------------------------------------------------------------------------------
// void Display::requireRedraw()
// //------------------------------------------------------------------------------
// {
//   dirty_ = true;
// }

// //------------------------------------------------------------------------------
// void Display::resetRedrawRequirement()
// //------------------------------------------------------------------------------
// {
//   dirty_ = false;
// }
