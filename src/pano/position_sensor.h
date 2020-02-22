#pragma once

#include <Arduino.h>
#include <Wire.h>

#include <MPU9250.h>

class PositionSensor {
 public:
  PositionSensor() : IMU(Wire, 0x68){};

  bool begin() {
    int16_t s = IMU.begin();
    if (s == 1) {
      IMU.setAccelRange(MPU9250::ACCEL_RANGE_2G);
      IMU.setGyroRange(MPU9250::GYRO_RANGE_250DPS);
      IMU.setDlpfBandwidth(MPU9250::DLPF_BANDWIDTH_184HZ);
      IMU.setSrd(19);
      return true;
    }
    return false;
  }

  void loop() {
    IMU.readSensor();
    // display the data
    Serial.print(IMU.getAccelX_mss(), 6);
    Serial.print("\t");
    Serial.print(IMU.getAccelY_mss(), 6);
    Serial.print("\t");
    Serial.print(IMU.getAccelZ_mss(), 6);
    Serial.print("\t");
    Serial.print(IMU.getGyroX_rads(), 6);
    Serial.print("\t");
    Serial.print(IMU.getGyroY_rads(), 6);
    Serial.print("\t");
    Serial.print(IMU.getGyroZ_rads(), 6);
    Serial.print("\t");
    Serial.print(IMU.getMagX_uT(), 6);
    Serial.print("\t");
    Serial.print(IMU.getMagY_uT(), 6);
    Serial.print("\t");
    Serial.print(IMU.getMagZ_uT(), 6);
    Serial.print("\t");
    Serial.println(IMU.getTemperature_C(), 6);
  }

 private:
  MPU9250 IMU;
};