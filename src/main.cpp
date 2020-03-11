#include <Arduino.h>
#include <Wire.h>
#include <driver/ledc.h>

#include <Adafruit_INA219.h>

#include "ui/display.h"
#include "ui/joystick.h"
#include "ui/position_sensor.h"
#include "util/encoder.h"
#include "util/logger.h"
#include "util/singletimer.h"
#include "util/statistic.h"

#include "hal/adc_esp32.h"
#include "hal/camera.h"
#include "hal/motor_driver.h"
#include "menu/menuitem.h"
#include "pano/pano_automat.h"
#include "pano/pano_calculator.h"
#include "pano/panoutils.h"

Logger LOG("MAIN");

Statistic statistic;
Display display;
Joystick joystick;
SingleTimer joystickTimer("Joystick");
Encoder encoder;
MotorDriver motorDriver;
PanoAutomat panoAutomat;
Camera camera;
ADC_ESP32 adc;
PositionSensor position;
Adafruit_INA219 ina219;

// 200 st/rev
// 16µSteps
// gear1: 1:5
// gear2: 26+(103/121) ->  https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf
#define MICROSTEPS_PER_REVOLUITION (200.0 * 16.0 * 5.0 * (26.0 + (103.0 / 121.0)));

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
#define I2C_SPEED 2000000
#define I2C_SCL 18
#define I2C_SDA 19

// ESP -> Camera
// GPIO 22 -> Focus
// GPIO 23 -> Trigger

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

bool loopWiFi() {
  ota.loop();
  return !ota.isUpdating();
}
#else
void beginWiFi() {}
bool loopWiFi() { return true; }
#endif

void statisticsIna219() {
  float shuntvoltage = 0;
  float busvoltage = 0;
  float current_mA = 0;
  float loadvoltage = 0;
  float power_mW = 0;

  shuntvoltage = ina219.getShuntVoltage_mV();
  busvoltage = ina219.getBusVoltage_V();
  current_mA = ina219.getCurrent_mA();
  power_mW = ina219.getPower_mW();
  loadvoltage = busvoltage + (shuntvoltage / 1000);

  Serial.print("Bus Voltage:   ");
  Serial.print(busvoltage);
  Serial.println(" V");
  Serial.print("Shunt Voltage: ");
  Serial.print(shuntvoltage);
  Serial.println(" mV");
  Serial.print("Load Voltage:  ");
  Serial.print(loadvoltage);
  Serial.println(" V");
  Serial.print("Current:       ");
  Serial.print(current_mA);
  Serial.println(" mA");
  Serial.print("Power:         ");
  Serial.print(power_mW);
  Serial.println(" mW");
  Serial.println("");
}

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

  // Display
  if (display.begin(17, 5)) {
    LOG.i("Display started");
    display.bootStart();
    display.loop();  // show first screen. Otherwise we have to wait until control reaches main loop code
  } else {
    LOG.i("Display failed");
  }

  // Encoder
  encoder.begin(36, 39, 34);
  encoder.onValueChanged([](int16_t oldValue, int16_t newValue) { display.encoderChanged(newValue - oldValue); });
  encoder.onButtonChanged([](Encoder::ButtonState buttonState) {
    if (buttonState == Encoder::ButtonState::PUSHED) {
      display.buttonPushed();
    }
  });

  // MotorDriver

  // limits: min: 10Steps/s; max: 7 revs/s
  MotorDriver::Limit_t limit = {16 * 10, 200 * 16 * 7, 75000};
  LambdaTranslator::RevolutionToSteps_t rts = [](double rev) { return rev * MICROSTEPS_PER_REVOLUITION; };
  LambdaTranslator::StepsToRevolution_t str = [](int64_t steps) { return (double)steps / MICROSTEPS_PER_REVOLUITION; };

  LambdaTranslator *translator = new LambdaTranslator(str, rts);
  if (motorDriver.begin(16, 21, 20, {&limit, &limit, &limit},
                        {(Translator *)translator, (Translator *)translator, (Translator *)translator})) {
    LOG.i("MotorDriver initialized");
    motorDriver.onStatusChange([](uint8_t axisIndex, const std::array<bool, 3> &axisMoving) {
      if (!axisMoving[0] && !axisMoving[1] && !axisMoving[2]) {
        panoAutomat.moveDone();
      }
    });
    motorDriver.onPosChange([](uint8_t axisIndex, double pos) {
      switch (axisIndex) {
        case 0:
          display.setPositionX(pos);
          break;
        case 1:
          display.setPositionY(pos);
          break;
        default:
          break;
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
    panoAutomat.onMove([](Position pos) {
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
  View view(PanoUtils::degToRev(30), PanoUtils::degToRev(330), PanoUtils::degToRev(30), PanoUtils::degToRev(150));
  Picture picture(PanoUtils::degToRev(30), PanoUtils::degToRev(40), PanoUtils::degToRev(12), PanoUtils::degToRev(16));
  Raster *raster = calc.createMatrixRasterForPano(view, picture);

  pano::Shots shots(500, 500, 0);
  shots += {500, 500};
  // shots += {500, 1000};
  // shots += {500, 2000};
  // panoAutomat.start(raster, shots);

  // ADC
  // if (adc.begin({36,37,38,39,32,33,34,35},250)) { // ADC_CH[0..7]
  if (adc.begin({32, 35})) {  // ADC_CH[4,7]
    LOG.i("ADC found");
    adc.onResult([](uint8_t channel, uint16_t value) {
      // LOG.i("ADC : %u[%u]", channel, value);
      switch (channel) {
        case 0: {
          if (joystick.getXAxis().isCalibrated()) {
            joystick.setRawX(value);
            // LOG.d("joy.x value @ %u", value);
          } else {
            joystick.setRawCenterX(value);
            LOG.d("joy.x center calibrated @ %u", value);
          }
        } break;
        case 1: {
          if (joystick.getYAxis().isCalibrated()) {
            joystick.setRawY(value);
            // LOG.d("joy.y value @ %u", value);
          } else {
            joystick.setRawCenterY(value);
            LOG.d("joy.y center calibrated @ %u", value);
          }
        } break;
        default:
          break;
      }
    });
  } else {
    LOG.e("ADC NOT found");
  }

  // Joystick
  if (joystick.begin(0.05, 1000, true)) {
    LOG.i("Joystick initialized");
    joystickTimer.startMs(50, false, true, [] {
      if (joystick.getXAxis().hasValue()) {
        float v = joystick.getXAxis().getValue();
        LOG.d("jogX: %f", v);
        motorDriver.jogV(0, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver.jogV(0, 0.0);
      }
      if (joystick.getYAxis().hasValue()) {
        float v = joystick.getYAxis().getValue();
        LOG.d("jogY: %f", v);
        motorDriver.jogV(1, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver.jogV(1, 0.0);
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

  // INA219
  ina219.begin();

  // Statistics
  if (statistic.begin()) {
    LOG.i("Statistic initialized");
    statistic.onStatistic([]() {
      // encoder.statistics();
      // motorDriver.statistic();
      // panoAutomat.statistic();
      // joystick.statistics();
      display.statistics();
      statisticsIna219();
      analogReadResolution(12);
      // analogSetAttenuation(ADC_0db);

      // LOG.d("X VAL: %d", analogRead(36)); // a or b
      LOG.d("X VAL: %d", analogRead(35));  // a or b
    });
  } else {
    LOG.e("Statistic failed");
  }

  LOG.i("========================================");
  LOG.i("Run App...");
  LOG.i("========================================");
  display.bootFinished();
}

// --------------------------------------------------------------------------------
void loop()
// --------------------------------------------------------------------------------
{
  if (loopWiFi()) {
    adc.loop();
    statistic.loop();
    motorDriver.loop();
    panoAutomat.loop();
    encoder.loop();
    joystickTimer.loop();
    position.loop();
    display.loop();
  }
}