#include "menuitem_base.h"

U8G2 *MenuItemBase::getU8g2() { return display_->getU8g2(); }
Display *MenuItemBase::getDisplay() { return display_; }
