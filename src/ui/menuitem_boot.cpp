#include "menuitem_boot.h"

#include "displayutils.h"

//------------------------------------------------------------------------------
MenuItemBoot::MenuItemBoot(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemBoot::render, this, _1));
}

//------------------------------------------------------------------------------
void MenuItemBoot::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();
  u8g2->setFont(u8g2_font_timR10_tf);

  DisplayUtils::drawStringAt(u8g2, 0, 0, false, false, "Booting...");

  u8g2->sendBuffer();
}