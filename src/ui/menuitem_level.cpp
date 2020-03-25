#include "menuitem_level.h"

#include "displayutils.h"

#include "distributor.h"

//------------------------------------------------------------------------------
MenuItemLevel::MenuItemLevel(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemLevel::render, this, _1));

  onButtonPushed([](MenuItem &self) {
    self.goUp();
    return false;
  });

  Distributor::i.getLevel().addListener([this](const Value<Position> &value) { requireRepaint(); });
}

//------------------------------------------------------------------------------
void MenuItemLevel::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  u8g2->setDrawColor(1);
  u8g2->drawHLine(33, 32, 63);
  u8g2->drawVLine(64, 1, 63);
  u8g2->drawCircle(64, 32, 31);
  u8g2->drawCircle(64, 32, 3);

  Position &pos = Distributor::i.getLevel().get();
  // Serial.printf("RENDER %f %f\n", pos.getX(), pos.getY());
  double x = pos.getX();
  double y = pos.getY();
  if (x < -0.5) x = -0.5;
  if (x > 0.5) x = 0.5;
  if (y < -0.5) x = -0.5;
  if (y > 0.5) y = 0.5;
  float posX = 64 + (x * 128.0);
  float posY = 32 + (y * 128.0);
  u8g2->drawCircle(posX, posY, 5);

  u8g2->sendBuffer();
}
