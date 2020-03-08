#include "menuitem_qr.h"

#include <ArduinoJson.h>
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
  const size_t capacity = JSON_OBJECT_SIZE(8);
  DynamicJsonDocument doc(capacity);

  doc["left"] = 1.2345678;
  doc["top"] = 1.2345678;
  doc["right"] = 1.2345678;
  doc["bottom"] = 1.2345678;
  doc["countX"] = 100;
  doc["countY"] = 100;
  doc["partial"] = true;
  doc["shots"] = 100;

  char buffer[256];
  serializeJson(doc, &buffer, 256);

  // maybe use LOCK_VERSION
  uint8_t version = 11;  // 61 pixel size
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  qrcode_initText(&qrcode, qrcodeData, version, ECC_HIGH, buffer);

  //   Serial.printf("| Version | Buffer | Size |\n");
  //   Serial.printf("| --- | --- | --- |\n");
  //   for (uint8_t i = 1; i < 30; ++i) {
  // uint8_t version = i;
  // uint8_t qrcodeData[qrcode_getBufferSize(version)];
  // qrcode_initText(&qrcode, qrcodeData, version, ECC_LOW, "HELLO WORLD");
  // Serial.printf("| %d | %d | %d |\n", i, qrcode_getBufferSize(version), qrcode.size);
  //   }

  U8G2 *u8g2 = getU8g2();
  u8g2->clearBuffer();

  for (uint8_t y = 0; y < qrcode.size; y++) {
    for (uint8_t x = 0; x < qrcode.size; x++) {
      if (qrcode_getModule(&qrcode, x, y)) {
        u8g2->drawPixel(x, y);
      }
    }
  }

  u8g2->sendBuffer();
}
