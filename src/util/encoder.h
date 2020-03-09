#pragma once

#include <Arduino.h>
#include <functional>

#include "util/loggerfactory.h"

// based on https://github.com/igorantolic/ai-esp32-rotary-encoder
class Encoder {
 public:
  typedef enum { DOWN = 0, PUSHED = 1, UP = 2, RELEASED = 3 } ButtonState;

  typedef std::function<void(int16_t oldValue, int16_t newValue)> ValueCallback_t;
  typedef std::function<void(ButtonState buttonState)> ButtonCallback_t;

  Encoder();
  bool begin(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderButtonPin = -1, uint8_t encoderVccPin = -1);
  void loop();
  void statistics();

  void onValueChanged(ValueCallback_t cb);
  void onButtonChanged(ButtonCallback_t cb);

  ButtonState getButtonState(bool peek = false);
  static const char* buttonStateToName(ButtonState buttonState);
  int16_t getValue() const;
  void setValue(int16_t v);

 protected:
  typedef void (*IsrCallback_t)();
  void IRAM_ATTR isr();

 private:
  Logger& LOG;
  portMUX_TYPE mux_;

  uint8_t encoderAPin_;
  uint8_t encoderBPin_;
  uint8_t encoderButtonPin_;
  uint8_t encoderVccPin_;

  volatile int16_t encoderPos_;
  int16_t previousEncoderPos_;
  uint8_t previousAB_;

  bool previousButtonState_;

  int8_t encoderStates_[16];

  ValueCallback_t valueCallback_;
  ButtonCallback_t buttonCallback_;
};
