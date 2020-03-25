#pragma once

#include <Arduino.h>

#include <functional>

#include "util/debouncer.h"
#include "util/loggerfactory.h"

// based on https://github.com/igorantolic/ai-esp32-rotary-encoder
class Encoder {
 public:
  typedef enum { DOWN = 0, PUSHED = 1, UP = 2, RELEASED = 3 } ButtonState;

  typedef std::function<void(int16_t oldValue, int16_t newValue)> ValueCallback_t;
  typedef std::function<void(bool newButtonState)> ButtonCallback_t;
  typedef std::function<void()> ButtonPushCallback_t;

  Encoder();

  bool begin(uint8_t encoderAPin, uint8_t encoderBPin, uint8_t encoderButtonPin = -1, uint32_t buttonDebounceTimeMs = 20);
  void loop();
  void statistics();

  // button
  void onButtonChanged(ButtonCallback_t cb) { buttonCallback_ = cb; }
  void onButtonPushed(ButtonPushCallback_t cb) { buttonPushCallback_ = cb; }
  ButtonState getButtonState(bool peek = false);
  static const char* buttonStateToName(ButtonState buttonState);

  // encoder
  void onValueChanged(ValueCallback_t cb) { valueCallback_ = cb; }
  int16_t getValue() const { return encoderPos_; }
  void setValue(int16_t v) { encoderPos_ = v; }

 protected:
  typedef void (*IsrCallback_t)();
  void IRAM_ATTR isr();

 private:
  Logger& LOG;
  portMUX_TYPE mux_;

  uint8_t encoderAPin_;
  uint8_t encoderBPin_;
  uint8_t encoderButtonPin_;

  volatile int16_t encoderPos_;
  int16_t previousEncoderPos_;
  uint8_t previousAB_;
  int8_t encoderStates_[16];

  bool previousButtonState_;
  Debouncer<bool> debouncer_;

  ValueCallback_t valueCallback_;
  ButtonCallback_t buttonCallback_;
  ButtonPushCallback_t buttonPushCallback_;
};
