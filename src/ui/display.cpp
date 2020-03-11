
#include "display.h"

#include "menuitem_boot.h"
#include "menuitem_main.h"

Display::Display() : LOG(LoggerFactory::getLogger("Display")), u8g2_(NULL), isBooting_(true), menu_("root"){};

Display::~Display() {
  if (u8g2_) delete u8g2_;
}

//------------------------------------------------------------------------------
bool Display::begin(uint8_t sclGpio, uint8_t sdaGpio)
//------------------------------------------------------------------------------
{
  if (u8g2_) {
    return false;
  }

  u8g2_ = new U8G2_SSD1306_128X64_NONAME_F_HW_I2C(U8G2_R0);
  u8g2_->setI2CAddress(0x3c * 2);

  if (!u8g2_->begin()) {
    return false;
  }

  LOG.d("OLED I²C adr is %d", u8g2_GetI2CAddress(u8g2_->getU8g2()));

  u8g2_->setContrast(255);

  menu_.add(new MenuItemBoot(this, "boot"));
  menu_.add(new MenuItemMain(this, "main"));

  return true;
}

void Display::statistics() { LOG.d("Selected: %s", menu_.getActivePath().c_str()); }
void Display::bootStart() { menu_.setActiveItem("boot"); }
void Display::bootFinished() { menu_.setActiveItem("main"); }
void Display::loop() { menu_.loop(); }
void Display::encoderChanged(int16_t diff) { menu_.encoderChanged(diff); }
void Display::buttonPushed() { menu_.buttonPushed(); }

U8G2* Display::getU8g2() { return u8g2_; }
