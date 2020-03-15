#pragma once

class Power {
 public:
  Power() : busvoltage_V_(0.0), current_mA_(0.0), power_mW_(0.0){};
  Power(float busvoltage_V, float current_mA, float power_mW)
      : busvoltage_V_(busvoltage_V), current_mA_(current_mA), power_mW_(power_mW){};
  float getBusvoltage_V() const { return busvoltage_V_; }
  float getCurrent_mA() const { return current_mA_; }
  float getPower_mW() const { return power_mW_; }
  bool operator==(const Power& b) const {
    return busvoltage_V_ == b.busvoltage_V_ && current_mA_ == b.current_mA_ && power_mW_ == b.power_mW_;
  }

 private:
  float busvoltage_V_;
  float current_mA_;
  float power_mW_;
};