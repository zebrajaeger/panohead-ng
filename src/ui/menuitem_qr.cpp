#include "menuitem_qr.h"

#include <ArduinoJson.h>
#include <qrcode.h>

#include "distributor.h"

#include "pano/panoutils.h"

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
  // maybe use LOCK_VERSION
  uint8_t version = 11;  // 61 pixel size
  QRCode qrcode;
  uint8_t qrcodeData[qrcode_getBufferSize(version)];
  String json;

  Distributor::getInstance().getView();

  PanoUtils::toJson(json, Distributor::getInstance().getView(), Distributor::getInstance().getPicture(),
                    Distributor::getInstance().getRaster(), Distributor::getInstance().getShots());
  qrcode_initText(&qrcode, qrcodeData, version, ECC_HIGH, json.c_str());

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

  if (!Distributor::getInstance().getRaster()) {
    DisplayUtils::drawStringAt(getU8g2(), 70, 10, false, false, "no raster");
  }

  if (!Distributor::getInstance().getShots()) {
    DisplayUtils::drawStringAt(getU8g2(), 70, 30, false, false, "no shots");
  }

  u8g2->sendBuffer();
}
