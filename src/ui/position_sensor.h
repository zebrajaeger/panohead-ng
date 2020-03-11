#pragma once

#include <Arduino.h>
#include <Wire.h>

// requires https://platformio.org/lib/show/5602/Bolder%20Flight%20Systems%20MPU9250
#include <MPU9250.h>

#include "data/position.h"

#include "util/loggerfactory.h"
#include "util/singletimer.h"

class PositionSensor {
 public:
  typedef std::function<void(PositionSensor &self)> PositionSensorCallback_t;

  PositionSensor() : LOG(LoggerFactory::getLogger("PositionSensor")), periosUs_(1000 * 50), imu_(Wire, 0x68), timer_("PositionSensor"){};

  bool begin(uint64_t periodUs = 1000 * 50) {
    periosUs_ = periodUs;
    int16_t s = imu_.begin();
    if (s == 1) {
      imu_.setAccelRange(MPU9250::ACCEL_RANGE_2G);
      imu_.setGyroRange(MPU9250::GYRO_RANGE_250DPS);
      imu_.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_184HZ);
      imu_.setSrd(19);

      timer_.startUs(periosUs_, false, true, [this]() {
        if (imu_.readSensor() == 1 && cb_) {
          cb_(*this);
        }
      });

      return true;
    }
    return false;
  }

  void loop() { timer_.loop(); }

  void onData(PositionSensorCallback_t cb) { cb_ = cb; }

  float getAngleX() { return atan(imu_.getAccelX_mss() / imu_.getAccelZ_mss()) * 57.2957; }
  float getAngleY() { return atan(imu_.getAccelY_mss() / imu_.getAccelZ_mss()) * 57.2957; }

  float getRevX() { return atan(imu_.getAccelX_mss() / imu_.getAccelZ_mss()) / TWO_PI; }
  float getRevY() { return atan(imu_.getAccelY_mss() / imu_.getAccelZ_mss()) / TWO_PI; }

  // display the data
  // Serial.print(IMU.getAccelX_mss(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getAccelY_mss(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getAccelZ_mss(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getGyroX_rads(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getGyroY_rads(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getGyroZ_rads(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getMagX_uT(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getMagY_uT(), 6);
  // Serial.print("\t");
  // Serial.print(IMU.getMagZ_uT(), 6);
  // Serial.print("\t");
  // Serial.println(IMU.getTemperature_C(), 6);

 private:
  Logger &LOG;
  uint64_t periosUs_;
  MPU9250 imu_;
  SingleTimer timer_;
  PositionSensorCallback_t cb_;
};