#include <hal/ads1115.h>

#include <Wire.h>

//------------------------------------------------------------------------------
ADS1115::ADS1115()
    : LOG("ADS1115"),
      i2cAddress_(0),
      gain_(GAIN_ONE),
      conversionDelayMicros_(1163),
      conversionEnd_(0),
      requestedChannel_(),
      cb_(NULL)
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool ADS1115::begin(uint8_t i2cAddress)
//------------------------------------------------------------------------------
{
  i2cAddress_ = i2cAddress;
  uint16_t temp;
  return getLastConversionResult_(temp);  // dummy read to check that ADS1115 is available
}

//------------------------------------------------------------------------------
void ADS1115::loop()
//------------------------------------------------------------------------------
{
  if (conversionEnd_ && micros() >= conversionEnd_) {
    conversionEnd_ = 0;
    uint16_t r;
    if (cb_ && getLastConversionResult_(r)) {
      cb_(requestedChannel_, r);
    }
  }
}

//------------------------------------------------------------------------------
void ADS1115::onResult(conversionCallback_t cb)
//------------------------------------------------------------------------------
{
  cb_ = cb;
}

//------------------------------------------------------------------------------
bool ADS1115::readSync(uint8_t channel, uint16_t& resultValue)
//------------------------------------------------------------------------------
{
  bool result = triggerConversion(channel);
  if (result) {
    delayMicroseconds(conversionDelayMicros_);
    result = getLastConversionResult_(resultValue);
  }
  return result;
}

//------------------------------------------------------------------------------
bool ADS1115::triggerConversion(uint8_t channel)
//------------------------------------------------------------------------------
{
  if (channel > 3) {
    return 0;
  }

  requestedChannel_ = channel;
  bool result = trigger_(channel);
  // conversionEnd_ = result ? micros() + conversionDelayMicros_ : 0;
  conversionEnd_ = micros() + conversionDelayMicros_;
  return result;
}

//------------------------------------------------------------------------------
bool ADS1115::trigger_(uint8_t channel)
//------------------------------------------------------------------------------
{
  if (channel > 3) {
    return 0;
  }

  // Start with default values
  uint16_t config = ADS1015_REG_CONFIG_CQUE_NONE |     // Disable the comparator (default val)
                    ADS1015_REG_CONFIG_CLAT_NONLAT |   // Non-latching (default val)
                    ADS1015_REG_CONFIG_CPOL_ACTVLOW |  // Alert/Rdy active low   (default val)
                    ADS1015_REG_CONFIG_CMODE_TRAD |    // Traditional comparator (default val)
                    ADS1015_REG_CONFIG_DR_860SPS |    // 1600 samples per second (default)
                    ADS1015_REG_CONFIG_MODE_SINGLE;    // Single-shot mode (default)

  // Set PGA/voltage range
  config |= gain_;

  // Set single-ended input channel
  switch (channel) {
    case (0):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_0;
      break;
    case (1):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_1;
      break;
    case (2):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_2;
      break;
    case (3):
      config |= ADS1015_REG_CONFIG_MUX_SINGLE_3;
      break;
  }

  // Set 'start single-conversion' bit
  config |= ADS1015_REG_CONFIG_OS_SINGLE;

  return writeRegister_(ADS1015_REG_POINTER_CONFIG, config);
}

//------------------------------------------------------------------------------
bool ADS1115::getLastConversionResult_(uint16_t& result)
//------------------------------------------------------------------------------
{
  return readRegister_(ADS1015_REG_POINTER_CONVERT, &result);
}

//------------------------------------------------------------------------------
bool ADS1115::readRegister_(uint8_t reg, uint16_t* result)
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2cAddress_);
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.write(reg);
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.endTransmission();
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.requestFrom(i2cAddress_, (uint8_t)2);
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  *result = ((Wire.read() << 8) | Wire.read());
  return true;
}

//------------------------------------------------------------------------------
bool ADS1115::writeRegister_(uint8_t reg, uint16_t value)
//------------------------------------------------------------------------------
{
  Wire.beginTransmission(i2cAddress_);
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.write((uint8_t)reg);
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.write((uint8_t)(value >> 8));
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.write((uint8_t)(value & 0xFF));
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }

  Wire.endTransmission();
  if (Wire.lastError() != I2C_ERROR_OK) {
    LOG.e(Wire.getErrorText(Wire.lastError()));
    return false;
  }
  return true;
}