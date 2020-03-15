#include "powersensor.h"

// --------------------------------------------------------------------------------
PowerSensor::PowerSensor()
    : LOG(LoggerFactory::getLogger("PowerSensor")),
      timer_("PowerSensor"),
      conversionCallback_(),
      ina219_(0x40)
// --------------------------------------------------------------------------------
{}

// --------------------------------------------------------------------------------
bool PowerSensor::begin(uint32_t periodMs)
// --------------------------------------------------------------------------------
{
  Wire.beginTransmission(0x40);  // default INA219 address
  if (Wire.endTransmission() == 0) {
    ina219_.begin();
    timer_.startMs(periodMs, false, true, std::bind(&PowerSensor::onTimer, this));
    return true;
  } else {
    return false;
  }
}

// --------------------------------------------------------------------------------
void PowerSensor::onTimer()
// --------------------------------------------------------------------------------
{
  if (conversionCallback_) {
    conversionCallback_(Power(ina219_.getBusVoltage_V(), -ina219_.getCurrent_mA(), ina219_.getPower_mW()));
  }
}

// --------------------------------------------------------------------------------
void PowerSensor::statistics()
// --------------------------------------------------------------------------------
{
  float shuntvoltage = ina219_.getShuntVoltage_mV();
  float busvoltage = ina219_.getBusVoltage_V();

  LOG.i("Bus: %f V", busvoltage);
  LOG.i("Shunt: %f V", shuntvoltage);
  LOG.i("Load: %f V", busvoltage + (shuntvoltage / 1000));
  LOG.i("Current: %f mA", ina219_.getCurrent_mA());
  LOG.i("Power: %f mW", ina219_.getPower_mW());
}