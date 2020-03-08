#include "menuitem_qr.h"

#include <qrcode.h>

//------------------------------------------------------------------------------
MenuItemQR::MenuItemQR(Display *display, const std::string &name)
    : MenuItemBase(display, name)
//------------------------------------------------------------------------------
{
  using namespace std;
  using namespace std::placeholders;

  onRender(bind(&MenuItemQR::render, this, _1));

  onButtonPushed([](MenuItem &self) {
    self.goUp();
    return false;
  });
}

//------------------------------------------------------------------------------
void MenuItemQR::render(MenuItem &menu)
//------------------------------------------------------------------------------
{
  // 10: b:407, s:57
  // 11: b:466, s:61
  // 12: b:529, s:65

  Serial.printf("| Version | Buffer | Size |\n");
  Serial.printf("| --- | --- | --- |\n");
  QRCode qrcode;
  for (uint8_t i = 1; i < 30; ++i) {
    uint8_t version = i;
    uint8_t qrcodeData[qrcode_getBufferSize(version)];
    qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, "HELLO WORLD");
    Serial.printf("| %d | %d | %d |\n", i, qrcode_getBufferSize(version), qrcode.size);
  }

  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  //   for (uint8_t y = 0; y < qrcode.size; y++) {
  //     for (uint8_t x = 0; x < qrcode.size; x++) {
  //       if (qrcode_getModule(&qrcode, x, y)) {
  //         u8g2->drawPixel(x, y);
  //       }
  //     }
  //   }

  u8g2->sendBuffer();
}
