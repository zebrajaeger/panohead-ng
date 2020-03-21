#include "menuitem_joystick.h"

#include "displayutils.h"
#include "distributor.h"

//------------------------------------------------------------------------------
MenuItemJoystick::MenuItemJoystick(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemJoystick::render, this, _1));

  onButtonPushed([](MenuItem &self) {
    self.goUp();
    return false;
  });

  Distributor::i.getJoystick() += [this](const Value<Position> &value) { requireRepaint(); };
}

//------------------------------------------------------------------------------
void MenuItemJoystick::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  u8g2->setDrawColor(1);
  u8g2->drawHLine(33, 32, 63);
  u8g2->drawVLine(64, 1, 63);
  u8g2->drawCircle(64, 32, 31);
  u8g2->drawCircle(64, 32, 3);

  Position &pos = Distributor::getInstance().getJoystick().get();
  double x = pos.getX();
  double y = pos.getY();
  if (x < -1.0) x = -1.0;
  if (x > 1.0) x = 1.0;
  if (y < -1.0) x = -1.0;
  if (y > 1.0) y = 1.0;
  float posX = 64 + (x * 64.0);
  float posY = 32 + (y * 32.0);
  u8g2->drawCircle(posX, posY, 5);

  u8g2->sendBuffer();
}
