#include <Arduino.h>
// #include <WiFi.h>
#include <Wire.h>
#include <driver/ledc.h>

// #include "net/ota.h"
// #include "net/wifiutils.h"
#include "ui/display.h"
#include "ui/joystick.h"
#include "ui/position_sensor.h"
#include "util/encoder.h"
#include "util/logger.h"
#include "util/singletimer.h"
#include "util/statistic.h"

#include "hal/adc.h"
#include "hal/camera.h"
#include "hal/motor_driver.h"
#include "pano/pano_automat.h"
#include "pano/pano_calculator.h"

Logger LOG("MAIN");

Statistic statistic;
Display display;
Joystick joystick;
SingleTimer joystickTimer("Joystick");
Encoder encoder;
MotorDriver motorDriver;
PanoAutomat panoAutomat;
Camera camera;
ADC adc;
PositionSensor position;
// OTA ota;

// test
// SingleTimer timer("TEST");
// double posX = 0;

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
  delay(1500);  // lets the visual studio code/platformIO console work...

  Serial.begin(115200);

  LOG.i("========================================");
  LOG.i("Booting...");
  LOG.i("========================================");

  // Display
  if (display.begin(17, 5)) {
    LOG.i("Display started");
    display.loop();  // show first screen. Otherwise we have to wait until control reaches main loop code
  } else {
    LOG.i("Display failed");
  }

  // Encoder
  encoder.begin(36, 39, 34);
  encoder.onValueChanged([](int16_t oldValue, int16_t newValue) { display.moveSelection(newValue - oldValue); });
  encoder.onButtonChanged([](Encoder::ButtonState buttonState) {
    if (buttonState == Encoder::ButtonState::PUSHED) {
      display.onButton();
    }
  });

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

  // limits: min: 10Steps/s; max: 7 revs/s
  MotorDriver::Limit_t limit = {16 * 10, 200 * 16 * 7, 75000};
  LambdaTranslator *translator = new LambdaTranslator([](double pos) {
    // 200 st/rev
    // 16µSteps
    // gear1: 1:5
    // gear2: 26+(103/121) ->  https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf
    return pos * 200.0 * 16.0 * 5.0 * (26.0 + (103.0 / 121.0));
  });
  if (motorDriver.begin(16, 21, 20, {&limit, &limit, &limit},
                        {(Translator *)translator, (Translator *)translator, (Translator *)translator})) {
    LOG.i("MotorDriver initialized");
    motorDriver.onStatusChange([](uint8_t axisIndex, const std::array<bool, 3> &axisMoving) {
      // LOG.d("onStatusChange: %d (%d, %d, %d)", axisIndex, axisMoving[0], axisMoving[1], axisMoving[2]);

      if (!axisMoving[0] && !axisMoving[1] && !axisMoving[2]) {
        // LOG.i("MOVE DONE");
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
      // LOG.d("Camera: %d, %d", focus, trigger);
      camera.set(focus, trigger);
    });
    panoAutomat.onMove([](pano::Position pos) {
      // LOG.d("GoTo: %f, %f", pos.getX(), pos.getY());
      motorDriver.goTo(0, pos.getX());
      motorDriver.goTo(1, pos.getY());
    });
    panoAutomat.onStatus(
        [](uint32_t columnIndex, uint32_t rowIndex, uint32_t shotIndex, PanoAutomat::state_t stateFrom, PanoAutomat::state_t stateTo) {
          LOG.d("@%d,%d[%d], %s -> %s", columnIndex, rowIndex, shotIndex, PanoAutomat::stateToName(stateFrom),
                PanoAutomat::stateToName(stateTo));
        });
  } else {
    LOG.e("PanoAutomat failed");
  }

  // Create raster, shots and start pano
  PanoCalculator calc;
  pano::View view(pano::degToRev(30), pano::degToRev(330), pano::degToRev(30), pano::degToRev(150));
  pano::Picture picture(pano::degToRev(30), pano::degToRev(40), pano::degToRev(12), pano::degToRev(16));
  pano::Raster *raster = calc.createMatrixRasterForPano(view, picture);

  pano::Shots shots(500, 500, 0);
  shots += {500, 500};
  // shots += {500, 1000};
  // shots += {500, 2000};
  // panoAutomat.start(raster, shots);

  // ADC
  if (adc.begin()) {
    LOG.i("ADS1115 found");
    adc.onResult([](uint8_t channel, uint16_t value) {
      // LOG.i("ADC : %u[%u]", channel, value);
      switch (channel) {
        case 0: {
          if (joystick.getXAxis().isCalibrated()) {
            joystick.setRawX(value);
          } else {
            joystick.setRawCenterX(value);
            LOG.d("joy.x calibrated");
          }
        } break;
        case 1: {
          if (joystick.getYAxis().isCalibrated()) {
            joystick.setRawY(value);
          } else {
            joystick.setRawCenterY(value);
            LOG.d("joy.y calibrated");
          }
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
  if (joystick.begin(0.05, 5000, true)) {
    LOG.i("Joystick initialized");
    joystickTimer.startMs(50, false, true, [] {
      if (joystick.getXAxis().hasValue()) {
        float v = joystick.getXAxis().getValue();
        motorDriver.jogV(0, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver.jogV(0, 0.0);
      }
      if (joystick.getYAxis().hasValue()) {
        float v = joystick.getYAxis().getValue();
        motorDriver.jogV(1, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver.jogV(0, 0.0);
      }
    });
  } else {
    LOG.e("Joystick failed");
  }

  // Position
  if (position.begin()) {
    LOG.i("Position sensor initialized");
    position.onData([] { display.setLeveling(position.getRevX(), position.getRevY()); });
  } else {
    LOG.e("Position sensor failed");
  }
  // Net
  // setupWiFi();
  // ota.begin();

  // Statistics
  if (statistic.begin()) {
    LOG.i("Statistic initialized");
    statistic.onStatistic([]() {
      // encoder.statistics();
      motorDriver.statistic();
      panoAutomat.statistic();
      joystick.statistics();
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
  joystickTimer.loop();
  position.loop();
  display.loop();
  // timer.loop();
  // }
}