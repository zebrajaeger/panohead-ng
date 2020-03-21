#include "menuitem_takepano.h"

#include "distributor.h"
#include "pano/panoutils.h"

MenuItemTakePano::MenuItemTakePano(Display *display, const std::string &name) : MenuItemBase(display, name) {
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemTakePano::render, this, _1));

  disable();
  Distributor::i.getPanoComplete().addListener(Value<bool>::CALL_IF_VALID, [this](const Value<bool> &complete) {
    setEnabled(*complete);
    requireRepaintParent();
  });
}

void MenuItemTakePano::render(MenuItem &menu) {
  U8G2 *u8g2 = getU8g2();

  u8g2->clearBuffer();

  // pos:
  u8g2->setFont(u8g2_font_timR10_tf);
  u8g2->drawStr(71, 8, "x:");
  //   DisplayUtils::drawAngleAt(u8g2, 87, -2, false, false, PanoUtils::revToDeg(posRevX_));
  u8g2->drawStr(71, 22, "y:");
  //   DisplayUtils::drawAngleAt(u8g2, 87, 12, false, false, PanoUtils::revToDeg(posRevY_));

  // cancel
  u8g2->setFont(u8g2_font_open_iconic_check_2x_t);
  //   DisplayUtils::drawSymbolAt(u8g2, 64 + 8 + 24, 60, cancel, 0x11b);

  u8g2->sendBuffer();
}