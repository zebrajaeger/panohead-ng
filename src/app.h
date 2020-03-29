#pragma once

#include <Arduino.h>

#include <Adafruit_INA219.h>

#include "distributor.h"

#include "ui/display.h"
#include "ui/joystick.h"
#include "ui/position_sensor.h"
#include "util/encoder.h"
#include "util/kvstore.h"
#include "util/loggerfactory.h"
#include "util/statistic.h"

// #include "hal/adc_esp32.h"
#include "hal/adc_ads1115.h"
#include "hal/camera.h"
#include "hal/led.h"
#include "hal/motor_driver.h"
#include "hal/pcf8574.h"
#include "hal/powersensor.h"

#include "pano/pano_automat.h"

class App {
 public:
  App();
  void setup();
  void loop();

 private:
  void setupKVStore(const char* name);
  void setupKVStorePicOverlap(const char* name);
  void setupKVStoreTiming(const char* name);
  void setupKVStoreShots(const char* name);
  void setupI2C(uint8_t sda, uint8_t scl, uint32_t speed);
  void setupIO();
  void setupLed(uint8_t statusLedPin);
  void setupDisplay();
  void setupEncoder(uint8_t gpioA, uint8_t gpioB, uint8_t gpioButton);
  void setupMotorDriver(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, double microstepsPerRevolution);
  void setupCamera(uint8_t ioFocus, uint8_t ioTrigger);
  void setupPanoAutomat();
  void setupADC(uint8_t i2cadress);
  void setupJoystick();
  void setupPositionSensor();
  void setupPowerSensor();
  void setupStatistics();

  Logger &LOG;
  KVStore kvStore_;
  
  Statistic statistic_;
  PCF8574 io_;
  Display display_;
  Joystick joystick_;
  SingleTimer joystickTimer_;
  Encoder encoder_;
  MotorDriver motorDriver_;
  PanoAutomat panoAutomat_;
  Camera camera_;
  ADC_ADS1115 adc_;
  PositionSensor positionSensor_;
  PowerSensor powerSensor_;
  Led statusLed_;
};