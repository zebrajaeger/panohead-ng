#pragma once

#include <Arduino.h>

#include <Adafruit_INA219.h>

#include "distributor.h"

#include "ui/display.h"
#include "ui/joystick.h"
#include "ui/position_sensor.h"
#include "util/encoder.h"
#include "util/loggerfactory.h"
#include "util/statistic.h"

#include "hal/adc_esp32.h"
#include "hal/camera.h"
#include "hal/motor_driver.h"
#include "hal/powersensor.h"

#include "pano/pano_automat.h"

class App {
 public:
  App();
  void setup();
  void loop();

 private:
  void setupI2C(uint8_t sda, uint8_t scl, uint32_t speed);
  void setupDisplay();
  void setupEncoder(uint8_t gpioA, uint8_t gpioB, uint8_t gpioButton);
  void setupMotorDriver(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, double microstepsPerRevolution);
  void setupCamera(uint8_t gpioFocus, uint8_t gpioTrigger);
  void setupPanoAutomat();
  void setupADC(uint8_t gpioX, uint8_t gpioY);
  void setupJoystick();
  void setupPositionSensor();
  void setupPowerSensor();
  void setupStatistics();

  Logger &LOG;

  Distributor &distributor_ = Distributor::getInstance();
  Statistic statistic_;
  Display display_;
  Joystick joystick_;
  SingleTimer joystickTimer_;
  Encoder encoder_;
  MotorDriver motorDriver_;
  PanoAutomat panoAutomat_;
  Camera camera_;
  ADC_ESP32 adc_;
  PositionSensor positionSensor_;
  PowerSensor powerSensor_;
};