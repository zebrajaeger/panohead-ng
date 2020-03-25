#include "app.h"

#include <Wire.h>
#include <driver/ledc.h>

#include "hal/motor_driver.h"
#include "menu/menuitem.h"
#include "pano/panoutils.h"
#include "util/singletimer.h"

// --------------------------------------------------------------------------------
App::App()
    : LOG(LoggerFactory::getLogger("APP")),
      kvStore_(),
      statistic_(),
      io_(),
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
  setupKVStore("app");
  setupI2C(19, 18, 2000000);
  setupIO();
  setupLed(0);
  setupDisplay();
  setupEncoder(34, 39, 36);
  // 200 st/rev
  // 16µSteps
  // gear1: 1:5
  // gear2: 26+(103/121) ->  https://www.omc-stepperonline.com/download/11HS12-0674D1-PG27.pdf
  setupMotorDriver(16, 21, 20, (200.0 * 16.0 * 5.0 * (26.0 + (103.0 / 121.0))));
  setupCamera(6, 7);
  setupPanoAutomat();
  setupADC(0x4A);  // ADDR connected with SDA
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
  statusLed_.loop();
}

// --------------------------------------------------------------------------------
void App::setupKVStore(const char *name)
// --------------------------------------------------------------------------------
{
  if (kvStore_.begin(name)) {
    LOG.i("KVStore initialized");
    // ===== Read values =====

    // pic overlay x and y
    Distributor::i.getPicture().set(Picture());  // set default
    kvStore_.get64("pic.ovl.x", [this](const KVStore::kv64_t &value) {
      LOG.i("kv-get pic.ovl.x = %f %", value.f);
      (*Distributor::i.getPicture()).setOverlapWidth(value.f);
      Distributor::i.getPicture().propagateChange();
    });
    kvStore_.get64("pic.ovl.y", [this](const KVStore::kv64_t &value) {
      LOG.i("kv-get pic.ovl.y = %f %", value.f);
      (*Distributor::i.getPicture()).setOverlapHeight(value.f);
      Distributor::i.getPicture().propagateChange();
    });

    // timing
    Distributor::i.getMovementTiming().set(MovementTiming());  // set default
    kvStore_.get32("pano.delAfMov", [this](const KVStore::kv32_t &value) {
      LOG.i("kv-get pano.delAfMov = %d ms", value.i);
      (*Distributor::i.getMovementTiming()).setDelayAfterMoveMs(value.u);
      Distributor::i.getMovementTiming().propagateChange();
    });
    kvStore_.get32("pano.delBtSht", [this](const KVStore::kv32_t &value) {
      LOG.i("kv-get pano.delBtSht = %d ms", value.i);
      (*Distributor::i.getMovementTiming()).setDelayBetweenShotsMs(value.u);
      Distributor::i.getMovementTiming().propagateChange();
    });
    kvStore_.get32("pano.delLsSht", [this](const KVStore::kv32_t &value) {
      LOG.i("kv-get pano.delLsSht = %d ms", value.i);
      (*Distributor::i.getMovementTiming()).setDelayAfterLastShotMs(value.u);
      Distributor::i.getMovementTiming().propagateChange();
    });

    // ===== Write values =====

    // picture
    Distributor::i.getPicture().addListener([this](const Value<Picture> &value) {
      KVStore::kv64_t temp;
      temp.f = (*value).getOverlapWidth();
      LOG.i("kv-set pic.ovl.x = %f %", temp.f);
      kvStore_.set64("pic.ovl.x", temp);
      temp.f = (*value).getOverlapHeight();
      LOG.i("kv-set pic.ovl.y = %f %", temp.f);
      kvStore_.set64("pic.ovl.y", temp);
      kvStore_.commit();
    });

    // timing
    Distributor::i.getMovementTiming().addListener([this](const Value<MovementTiming> &movementTiming) {
      KVStore::kv32_t temp;
      temp.u = (*movementTiming).getDelayAfterMoveMs();
      kvStore_.get32("pano.delAfMov", temp);
      temp.u = (*movementTiming).getDelayBetweenShotsMs();
      kvStore_.get32("pano.delBtSht", temp);
      temp.u = (*movementTiming).getDelayAfterLastShotMs();
      kvStore_.get32("pano.delLsSht", temp);
      kvStore_.commit();
    });

  } else {
    LOG.e("KVStore failed");
  }
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
void App::setupIO()
// --------------------------------------------------------------------------------
{
  if (io_.begin()) {
    LOG.i("IO initialized");
  } else {
    LOG.e("IO failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupLed(uint8_t statusLedPin)
// --------------------------------------------------------------------------------
{
  if (statusLed_.begin(&io_, statusLedPin)) {
    LOG.i("Status LED initialized");
    statusLed_.blink();
  } else {
    LOG.e("Status LED failed");
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
  encoder_.onButtonPushed([this] { display_.buttonPushed(); });
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
        panoAutomat_.triggerMoveDone();
        Distributor::i.getAxisMovingX().set(axisMoving[0]);
        Distributor::i.getAxisMovingY().set(axisMoving[1]);
      }
    });

    motorDriver_.onPosChange([this](uint8_t axisIndex, double pos) {
      switch (axisIndex) {
        case 0:
          // display.setPositionX(pos);
          Distributor::i.getPosition().get().setX(pos);
          Distributor::i.getPosition().propagateChange();
          break;
        case 1:
          // display.setPositionY(pos);
          Distributor::i.getPosition().get().setY(pos);
          Distributor::i.getPosition().propagateChange();
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
void App::setupCamera(uint8_t ioFocus, uint8_t ioTrigger)
// --------------------------------------------------------------------------------
{
  if (camera_.begin(&io_, ioFocus, ioTrigger)) {
    LOG.i("Camera initialized");
    camera_.setFocus(false);
    camera_.setTrigger(true);
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
      LOG.d("Camera: %d, %d", focus, trigger);
      camera_.set(focus, trigger);
      Distributor::i.getCameraFocus().set(focus);
      Distributor::i.getCameraTrigger().set(trigger);
    });

    panoAutomat_.onMove([this](Position pos) {
      LOG.d("GoTo: %f, %f", pos.getX(), pos.getY());
      motorDriver_.goTo(0, pos.getX());
      motorDriver_.goTo(1, pos.getY());
    });

    panoAutomat_.onStatus([this](const Pano &pano, const PanoState &panoState) {
      LOG.d("PanoState: %s", stateToName(panoState.getAutomatState()));
      Distributor::i.getPanoState().set(panoState);
    });
  } else {
    LOG.e("PanoAutomat failed");
  }
}

// --------------------------------------------------------------------------------
void App::setupADC(uint8_t i2cadress)
// --------------------------------------------------------------------------------
{
  if (adc_.begin(i2cadress, 5)) {
    LOG.i("ADC found");
    adc_.onResult([this](uint8_t channel, uint16_t value) {
      // LOG.i("ADC : %u[%4u]", channel, value);
      switch (channel) {
        case 3: {
          if (joystick_.getXAxis().isCalibrated()) {
            joystick_.setRawX(value);
            // LOG.d("joy.x value @ %4u", value);
          } else {
            joystick_.setRawCenterX(value);
            LOG.d("joy.x center calibrated @ %u", value);
          }
        } break;
        case 2: {
          if (joystick_.getYAxis().isCalibrated()) {
            joystick_.setRawY(value);
            // LOG.d("joy.y value @ %4u", value);
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
  if (joystick_.begin(0.0045, 8000, true)) {
    LOG.i("Joystick initialized");
    joystickTimer_.startMs(50, false, true, [this] {
      bool led = false;
      Position joyPos(0.0, 0.0);
      if (joystick_.getXAxis().hasValue()) {
        float v = joystick_.getXAxis().getValue();
        joyPos.setX(v);
        LOG.d("jogX: %f", v);
        motorDriver_.jogV(0, 0.05 * v * v * v);  // v³ for better handling on slowmo
        led = true;
      } else {
        motorDriver_.jogV(0, 0.0);
      }
      if (joystick_.getYAxis().hasValue()) {
        float v = joystick_.getYAxis().getValue();
        joyPos.setY(v);
        LOG.d("jogY: %f", v);
        motorDriver_.jogV(1, 0.05 * v * v * v);  // v³ for better handling on slowmo
        led = true;
      } else {
        motorDriver_.jogV(1, 0.0);
      }
      Distributor::i.getJoystick().set(joyPos);
      statusLed_.set(!led);  // low active
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
    positionSensor_.onData([this](PositionSensor &self) { Distributor::i.getLevel().set(Position(self.getRevX(), self.getRevX())); });
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
    powerSensor_.onData([this](const Power &power) { Distributor::i.getPower().set(power); });
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
      // encoder_.statistics();
      // motorDriver_.statistic();
      panoAutomat_.statistic();
      joystick_.statistics();
      display_.statistics();
      // analogSetAttenuation(ADC_0db);
      uint32_t freeHeap = heap_caps_get_free_size(MALLOC_CAP_8BIT);
      uint32_t largestBlock = heap_caps_get_largest_free_block(MALLOC_CAP_8BIT);
      LOG.i("Heap: available: %'d bytes, largest block: %'d", freeHeap, largestBlock);
      LOG.i("Stack.highwatermark: %d", uxTaskGetStackHighWaterMark(NULL));
    });
  } else {
    LOG.e("Statistic failed");
  }
}
