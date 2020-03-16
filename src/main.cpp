#include <Arduino.h>

#include "app.h"

App app;

Logger &LOG(LoggerFactory::getLogger("MAIN"));

#if __has_include("mywifi.h")
#include <WiFi.h>
#include "net/ota.h"
#include "net/wifiutils.h"
OTA ota;

// --------------------------------------------------------------------------------
void beginWiFi()
// --------------------------------------------------------------------------------
{
// WiFI
// WiFi.begin("SSID", "PW");
#include "mywifi.h"

  WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
    LOG.i("WiFi event: %u -> %s\n", event, getWifiEventName(event));
    switch (event) {
      case SYSTEM_EVENT_STA_GOT_IP:
        LOG.i("WiFi connected");
        LOG.i("IP is: %s", WiFi.localIP().toString().c_str());
        break;
      case SYSTEM_EVENT_STA_DISCONNECTED:
        LOG.i("WiFi disconnected, Reason: %u -> %s\n", info.disconnected.reason, getWifiFailReason(info.disconnected.reason));
        if (info.disconnected.reason == 202) {
          LOG.i("WiFi Bug, REBOOT/SLEEP!");
          esp_sleep_enable_timer_wakeup(10);
          esp_deep_sleep_start();
          delay(100);
        }
        break;
      default:
        break;
    }
  });

  ota.begin();
}

// --------------------------------------------------------------------------------
bool loopWiFi() {
  // --------------------------------------------------------------------------------
  ota.loop();
  return !ota.isUpdating();
}
#else
void beginWiFi() {}
bool loopWiFi() { return true; }
#endif

// --------------------------------------------------------------------------------
void setup()
// --------------------------------------------------------------------------------
{
  delay(1100);  // lets the visual studio code/platformIO console work...
  Serial.begin(115200);

  LOG.i("========================================");
  LOG.i("Booting...");
  LOG.i("========================================");

  beginWiFi();

  app.setup();
  LOG.i("========================================");
  LOG.i("Run App...");
  LOG.i("========================================");
}

// --------------------------------------------------------------------------------
void loop()
// --------------------------------------------------------------------------------
{
  if (loopWiFi()) {
    app.loop();
  }
}
