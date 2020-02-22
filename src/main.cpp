#include <Arduino.h>
// #include <WiFi.h>
#include <Wire.h>
#include <driver/ledc.h>

// #include "net/ota.h"
// #include "wifiutils.h"
#include "display.h"
#include "joystick.h"
#include "statistic.h"
#include "util/encoder.h"
#include "util/logger.h"

#include "pano/adc.h"
#include "pano/camera.h"
#include "pano/motor_driver.h"
#include "pano/pano_automat.h"
#include "pano/pano_calculator.h"

Logger LOG("MAIN");

Statistic statistic;
Display display;
Joystick joystick;
Encoder encoder;
MotorDriver motorDriver;
PanoAutomat panoAutomat;
Camera camera;
ADC adc;

// OTA ota;

// ESP -> TMC (PIN)
// GPIO 16 (CS) -> 9(CS)
// GPIO 14 (SCK) -> 10(SCK)
// GPIO 2 (MOSI) -> 11(DI)
// GPIO 15 (MISO) -> 14(DO)
// GPIO 21 (ClockSource) -> 7(CLK)

// ESP -> ENCODER (on Vcc)
// GPIO 36 -> A (+ pull-down)
// GPIO 39 -> B (+ pull-down)
// GPIO 34 -> Button (+ pull-down)

// ESP -> OLED I²C
// GPIO 17 -> SCL
// GPIO 17 -> SDA

// ESP -> I²C (ADC,FRAM,)
// GPIO 18 -> SCL
// GPIO 19 -> SDA
#define I2C_SPEED 400000
#define I2C_SCL 18
#define I2C_SDA 19

// ESP -> Camera
// GPIO 22 -> Focus
// GPIO 23 -> Trigger

// // --------------------------------------------------------------------------------
// void setupWiFi()
// // --------------------------------------------------------------------------------
// {
//   // WiFI
//   WiFi.begin("zebrafarm", "Schnee25");

//   WiFi.onEvent([](WiFiEvent_t event, WiFiEventInfo_t info) {
//     LOG.i("WiFi event: %u -> %s\n", event, getWifiEventName(event));
//     switch (event) {
//       case SYSTEM_EVENT_STA_GOT_IP:
//         LOG.i("WiFi connected");
//         LOG.i("IP is: %s", WiFi.localIP().toString().c_str());
//         break;
//       case SYSTEM_EVENT_STA_DISCONNECTED:
//         LOG.i("WiFi disconnected, Reason: %u -> %s\n", info.disconnected.reason, getWifiFailReason(info.disconnected.reason));
//         if (info.disconnected.reason == 202) {
//           LOG.i("WiFi Bug, REBOOT/SLEEP!");
//           esp_sleep_enable_timer_wakeup(10);
//           esp_deep_sleep_start();
//           delay(100);
//         }
//         break;
//       default:
//         break;
//     }
//   });
// }

// --------------------------------------------------------------------------------
void setup()
// --------------------------------------------------------------------------------
{
  delay(1500); // lets the visual studio code/platformIO console work...

  Serial.begin(115200);

  LOG.i("========================================");
  LOG.i("Booting...");
  LOG.i("========================================");

  // Display
  if (display.begin(17, 5)) {
    LOG.i("Display started");
    display.showBootScreen();
  } else {
    LOG.i("Display failed");
  }

  // Encoder
  encoder.begin(36, 39, 34);
  encoder.onValueChanged([](int16_t oldValue, int16_t newValue) { LOG.d("ENCODER: %d -> %d", oldValue, newValue); });
  encoder.onButtonChanged([](Encoder::ButtonState buttonState) { LOG.d("ENCODER-BUTTON: %s", Encoder::buttonStateToName(buttonState)); });

  // I²C
  if (Wire.begin(I2C_SDA, I2C_SCL, I2C_SPEED)) {
    LOG.i("I²C initialized");
    for (byte i = 8; i < 120; i++) {
      Wire.beginTransmission(i);
      if (Wire.endTransmission() == 0) {
        LOG.i("I²C device found @ 0x%02x", i);
      }
    }
  } else {
    LOG.e("I²C failed");
  }

  // MotorDriver
  LambdaTranslator *translator = new LambdaTranslator([](double pos) {
    // https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf
    return pos * (double)(200 * 32 * 5 * 26 * 103) / (double)121;
  });
  if (motorDriver.begin(16, 21, 10, {(Translator *)translator, (Translator *)translator, (Translator *)NULL})) {
    LOG.i("MotorDriver initialized");
    motorDriver.onStatusChange([](uint8_t axisIndex, const std::array<bool, 3> &axisMoving) {
      LOG.d("onStatusChange: %d (%d, %d, %d)", axisIndex, axisMoving[0], axisMoving[1], axisMoving[2]);

      if (!axisMoving[0] && !axisMoving[1]) {
        LOG.i("MOVE DONE");
        panoAutomat.moveDone();
      }
    });
  } else {
    LOG.e("MotorDriver failed");
  }

  // Camera
  if (camera.begin(22, 23)) {
    LOG.i("Camera initialized");
  } else {
    LOG.e("Camera failed");
  }

  // PanoAutomat
  if (panoAutomat.begin()) {
    LOG.i("PanoAutomat initialized");
    panoAutomat.onCamera([](bool focus, bool trigger) {
      LOG.d("Camera: %d, %d", focus, trigger);
      camera.set(focus, trigger);
    });
    panoAutomat.onMove([](pano::Position pos) {
      LOG.d("GoTo: %f, %f", pos.getX(), pos.getY());
      motorDriver.goTo(0, pos.getX());
      motorDriver.goTo(1, pos.getY());
    });
  } else {
    LOG.e("PanoAutomat failed");
  }

  // Create raster, shots and start pano
  PanoCalculator calc;
  pano::View view(pano::degToRev(30), pano::degToRev(330), pano::degToRev(30), pano::degToRev(150));
  pano::Picture picture(pano::degToRev(30), pano::degToRev(40), pano::degToRev(12), pano::degToRev(16));
  pano::Raster *raster = calc.createMatrixRasterForPano(view, picture);

  pano::Shots shots(1000, 500, 0);
  shots += {500, 500};
  shots += {500, 1000};
  shots += {500, 2000};
  panoAutomat.start(raster, shots);

  // ADC
  if (adc.begin()) {
    LOG.i("ADS1115 found");
    adc.onResult([](uint8_t channel, uint16_t value) {
      // LOG.i("ON.CH: %u, v: %u", channel, value);
      switch (channel) {
        case 0: {
          if (joystick.getXAxis().isCalibrated()) {
            joystick.setRawX(value);
          } else {
            joystick.setCenterX(value);
          }
          //       int32_t from = tmc429.getTargetPosition(0);
          //       int32_t to = from + (joystick.setRawX(value) * 1000);
          //       tmc429.setTargetPosition(0, to);
        } break;
        case 1: {
          if (joystick.getYAxis().isCalibrated()) {
            joystick.setRawY(value);
          } else {
            joystick.setCenterY(value);
          }

          //       int32_t from = tmc429.getTargetPosition(0);
          //       int32_t to = from + (joystick.setRawY(value) * 1000);
          //       tmc429.setTargetPosition(1, to);
        } break;
        case 2:
          break;
        case 3:
          break;
      }
    });
  } else {
    LOG.e("ADS1115 NOT found");
  }

  // Joystick
  if (joystick.begin()) {
    LOG.i("Joystick initialized");
  } else {
    LOG.e("Joystick failed");
  }

  // Net
  // setupWiFi();
  // ota.begin();

  // Statistics
  if (statistic.begin()) {
    LOG.i("Statistic initialized");
    statistic.onStatistic([]() {
      encoder.statistics();
      motorDriver.statistic();
      panoAutomat.statistic();
    });
  } else {
    LOG.e("Statistic failed");
  }
}

// --------------------------------------------------------------------------------
void loop()
// --------------------------------------------------------------------------------
{
  // ota.loop();
  // if(!ota.isUpdating()){
  adc.loop();
  statistic.loop();
  motorDriver.loop();
  panoAutomat.loop();
  encoder.loop();
  // }
}