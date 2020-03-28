#include "menuitem_power.h"

#include "distributor.h"

#include "pano/panoutils.h"

//------------------------------------------------------------------------------
MenuItemPower::MenuItemPower(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemPower::render, this, _1));

  onButtonPushed([](MenuItem &self) {
    self.goUp();
    return false;
  });
  Distributor::i.getPower().addListener([this](const Value<Power> &value) { requireRepaint(); });
}

//------------------------------------------------------------------------------
void MenuItemPower::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  const Power &power = Distributor::i.getPower().get();
  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  char buf[20];
  u8g2->setFont(u8g2_font_timR10_tf);
  sprintf(buf, "U: %8.3f V", power.getBusvoltage_V());
  DisplayUtils::drawStringAt(getU8g2(), 0, 0, false, false, (char *)&buf);

  sprintf(buf, "I: %8.1f mA", power.getCurrent_mA());
  DisplayUtils::drawStringAt(getU8g2(), 0, 16, false, false, (char *)&buf);

  sprintf(buf, "P: %6.0f mW", power.getPower_mW());
  DisplayUtils::drawStringAt(getU8g2(), 0, 32, false, false, (char *)&buf);

  u8g2->sendBuffer();
}
