#include "app.h"

#include <Wire.h>
#include <driver/ledc.h>

#include "util/singletimer.h"

#include "hal/adc_esp32.h"
#include "hal/camera.h"
#include "hal/motor_driver.h"
#include "menu/menuitem.h"

#include "pano/pano_calculator.h"
#include "pano/panoutils.h"

// --------------------------------------------------------------------------------
App::App()
    : LOG(LoggerFactory::getLogger("APP")),
      distributor_(Distributor::getInstance()),
      statistic_(),
      display_(),
      joystick_(),
      joystickTimer_("Joystick"),
      encoder_(),
      motorDriver_(),
      panoAutomat_(),
      camera_(),
      adc_(),
      positionSensor_(),
      powerSensor_()
// --------------------------------------------------------------------------------
{}

// --------------------------------------------------------------------------------
void App::setup()
// --------------------------------------------------------------------------------
{
  setupI2C(19, 18, 2000000);
  setupDisplay();
  setupEncoder(34, 39, 36);
  // 200 st/rev
  // 16µSteps
  // gear1: 1:5
  // gear2: 26+(103/121) ->  https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf
  setupMotorDriver(16, 21, 20, (200.0 * 16.0 * 5.0 * (26.0 + (103.0 / 121.0))));
  setupCamera(22, 23);
  setupPanoAutomat();
  setupADC(35, 32);
  setupJoystick();
  setupPositionSensor();
  setupPowerSensor();
  setupStatistics();

  display_.bootFinished();
}

// --------------------------------------------------------------------------------
void App::loop()
// --------------------------------------------------------------------------------
{
  adc_.loop();
  statistic_.loop();
  motorDriver_.loop();
  panoAutomat_.loop();
  encoder_.loop();
  joystickTimer_.loop();
  positionSensor_.loop();
  display_.loop();
  powerSensor_.loop();
}

// --------------------------------------------------------------------------------
void App::setupI2C(uint8_t sda, uint8_t scl, uint32_t speed)
// --------------------------------------------------------------------------------
{
  if (Wire.begin(sda, scl, speed)) {
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
}

// --------------------------------------------------------------------------------
void App::setupDisplay()
// --------------------------------------------------------------------------------
{
  if (display_.begin()) {
    LOG.i("Display started");
    display_.bootStart();
    display_.loop();  // show first screen. Otherwise we have to wait until control reaches main loop code
  } else {
    LOG.i("Display failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupEncoder(uint8_t gpioA, uint8_t gpioB, uint8_t gpioButton)
// --------------------------------------------------------------------------------
{
  encoder_.begin(gpioA, gpioB, gpioButton);
  encoder_.onValueChanged([this](int16_t oldValue, int16_t newValue) { display_.encoderChanged(newValue - oldValue); });
  encoder_.onButtonChanged([this](Encoder::ButtonState buttonState) {
    if (buttonState == Encoder::ButtonState::PUSHED) {
      display_.buttonPushed();
    }
  });
}

// --------------------------------------------------------------------------------
void App::setupMotorDriver(uint8_t pinCS, uint8_t pinClockSource, uint8_t clockSpeedMHz, double microstepsPerRevolution)
// --------------------------------------------------------------------------------
{
  // limits: min: 10Steps/s; max: 7 revs/s
  LambdaTranslator::RevolutionToSteps_t rts = [microstepsPerRevolution](double rev) { return (int64_t)(rev * microstepsPerRevolution); };
  LambdaTranslator::StepsToRevolution_t str = [microstepsPerRevolution](int64_t steps) {
    return (double)(steps / microstepsPerRevolution);
  };

  LambdaTranslator *translator = new LambdaTranslator(str, rts);
  MotorDriver::Limit_t limit = {16 * 10, 200 * 16 * 7, 75000};

  if (motorDriver_.begin(pinCS, pinClockSource, clockSpeedMHz, {&limit, &limit, &limit},
                         {(Translator *)translator, (Translator *)translator, (Translator *)translator})) {
    LOG.i("MotorDriver initialized");

    motorDriver_.onStatusChange([this](uint8_t axisIndex, const std::array<bool, 3> &axisMoving) {
      if (!axisMoving[0] && !axisMoving[1] && !axisMoving[2]) {
        panoAutomat_.moveDone();
        distributor_.getAxisMovingX().set(axisMoving[0]);
        distributor_.getAxisMovingY().set(axisMoving[1]);
      }
    });

    motorDriver_.onPosChange([this](uint8_t axisIndex, double pos) {
      switch (axisIndex) {
        case 0:
          // display.setPositionX(pos);
          distributor_.getPosition().get().setX(pos);
          distributor_.getPosition().propagateChange();
          break;
        case 1:
          // display.setPositionY(pos);
          distributor_.getPosition().get().setY(pos);
          distributor_.getPosition().propagateChange();
          break;
        default:
          break;
      }
    });
  } else {
    LOG.e("MotorDriver failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupCamera(uint8_t gpioFocus, uint8_t gpioTrigger)
// --------------------------------------------------------------------------------
{
  if (camera_.begin(gpioFocus, gpioTrigger)) {
    LOG.i("Camera initialized");
  } else {
    LOG.e("Camera failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupPanoAutomat()
// --------------------------------------------------------------------------------
{
  if (panoAutomat_.begin()) {
    LOG.i("PanoAutomat initialized");

    panoAutomat_.onCamera([this](bool focus, bool trigger) {
      // LOG.d("Camera: %d, %d", focus, trigger);
      camera_.set(focus, trigger);
      distributor_.getCameraFocus().set(focus);
      distributor_.getCameraTrigger().set(trigger);
    });

    panoAutomat_.onMove([this](Position pos) {
      // LOG.d("GoTo: %f, %f", pos.getX(), pos.getY());
      motorDriver_.goTo(0, pos.getX());
      motorDriver_.goTo(1, pos.getY());
    });

    panoAutomat_.onStatus(
        [this](uint32_t columnIndex, uint32_t rowIndex, uint32_t shotIndex, PanoAutomat::state_t stateFrom, PanoAutomat::state_t stateTo) {
          const char *stateName = PanoAutomat::stateToName(stateTo);
          LOG.d("@%d,%d[%d], %s -> %s", columnIndex, rowIndex, shotIndex, PanoAutomat::stateToName(stateFrom),
                PanoAutomat::stateToName(stateTo));
          distributor_.getPanoAutomatStatus().set(stateName);
          distributor_.getPanoAutomatColumn().set(columnIndex);
          distributor_.getPanoAutomatRow().set(rowIndex);
          distributor_.getPanoAutomatShot().set(shotIndex);
        });
  } else {
    LOG.e("PanoAutomat failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupADC(uint8_t gpioX, uint8_t gpioY)
// --------------------------------------------------------------------------------
{
  // if (adc.begin({36,37,38,39,32,33,34,35},250)) { // ADC_CH[0..7]
  if (adc_.begin({gpioX, gpioY})) {  // ADC_CH[4,7]
    LOG.i("ADC found");
    adc_.onResult([this](uint8_t channel, uint16_t value) {
      // LOG.i("ADC : %u[%u]", channel, value);
      switch (channel) {
        case 0: {
          if (joystick_.getXAxis().isCalibrated()) {
            joystick_.setRawX(value);
            // LOG.d("joy.x value @ %u", value);
          } else {
            joystick_.setRawCenterX(value);
            LOG.d("joy.x center calibrated @ %u", value);
          }
        } break;
        case 1: {
          if (joystick_.getYAxis().isCalibrated()) {
            joystick_.setRawY(value);
            // LOG.d("joy.y value @ %u", value);
          } else {
            joystick_.setRawCenterY(value);
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
}

// --------------------------------------------------------------------------------
void App::setupJoystick()
// --------------------------------------------------------------------------------
{
  if (joystick_.begin(0.05, 1000, true)) {
    LOG.i("Joystick initialized");
    joystickTimer_.startMs(50, false, true, [this] {
      Position joyPos(0.0, 0.0);
      if (joystick_.getXAxis().hasValue()) {
        float v = joystick_.getXAxis().getValue();
        joyPos.setX(v);
        // LOG.d("jogX: %f", v);
        motorDriver_.jogV(0, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver_.jogV(0, 0.0);
      }
      if (joystick_.getYAxis().hasValue()) {
        float v = joystick_.getYAxis().getValue();
        joyPos.setY(v);
        // LOG.d("jogY: %f", v);
        motorDriver_.jogV(1, 0.05 * v * v * v);  // v³ for better handling on slowmo
      } else {
        motorDriver_.jogV(1, 0.0);
      }
      distributor_.getJoystick().set(joyPos);
    });
  } else {
    LOG.e("Joystick failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupPositionSensor()
// --------------------------------------------------------------------------------
{
  if (positionSensor_.begin()) {
    LOG.i("Position sensor initialized");
    positionSensor_.onData([this](PositionSensor &self) { distributor_.getLevel().set(Position(self.getRevX(), self.getRevX())); });
  } else {
    LOG.e("Position sensor failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupPowerSensor()
// --------------------------------------------------------------------------------
{
  if (powerSensor_.begin()) {
    LOG.i("Power sensor initialized");
    powerSensor_.onData([this](const Power &power) { distributor_.getPower().set(power); });
  } else {
    LOG.e("Power sensor failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupStatistics()
// --------------------------------------------------------------------------------
{
  if (statistic_.begin()) {
    LOG.i("Statistic initialized");
    statistic_.onStatistic([this]() {
      // encoder.statistics();
      // motorDriver.statistic();
      // panoAutomat.statistic();
      joystick_.statistics();
      display_.statistics();
      // statisticsIna219();
      // analogReadResolution(12);
      // analogSetAttenuation(ADC_0db);

      // LOG.d("X VAL: %d", analogRead(36)); // a or b
      LOG.d("X VAL: %d", analogRead(35));  // a or b
    });
  } else {
    LOG.e("Statistic failed");
  }
}