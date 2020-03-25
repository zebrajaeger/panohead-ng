
#include "encoder.h"

#include "callback.h"

//------------------------------------------------------------------------------
void IRAM_ATTR Encoder::isr()
//------------------------------------------------------------------------------
{
  portENTER_CRITICAL_ISR(&(this->mux_));

  this->previousAB_ <<= 2;  // remember previous state

  int8_t ENC_PORT = ((digitalRead(this->encoderBPin_)) ? (1 << 1) : 0) | ((digitalRead(this->encoderAPin_)) ? (1 << 0) : 0);
  this->previousAB_ |= (ENC_PORT & 0x03);  // add current state
  this->encoderPos_ += (this->encoderStates_[(this->previousAB_ & 0x0f)]);

  portEXIT_CRITICAL_ISR(&(this->mux_));
}

//------------------------------------------------------------------------------
Encoder::Encoder()
    : LOG(LoggerFactory::getLogger("Encoder")),
      mux_(portMUX_INITIALIZER_UNLOCKED),
      encoderAPin_(-1),
      encoderBPin_(-1),
      encoderButtonPin_(-1),
      encoderPos_(0),
      previousEncoderPos_(0),
      previousAB_(0),
      encoderStates_{0, -1, 1, 0, 1, 0, 0, -1, -1, 0, 0, 1, 0, 1, -1, 0},
      previousButtonState_(false),
      debouncer_(),
      valueCallback_(),
      buttonCallback_(),
      buttonPushCallback_()
//------------------------------------------------------------------------------
{}

//------------------------------------------------------------------------------
bool Encoder::begin(uint8_t encoder_APin, uint8_t encoder_BPin, uint8_t encoder_ButtonPin, uint32_t buttonDebounceTimeMs)
//------------------------------------------------------------------------------
{
  // A
  encoderAPin_ = encoder_APin;
  pinMode(encoderAPin_, INPUT_PULLDOWN);

  // B
  encoderBPin_ = encoder_BPin;
  pinMode(encoderBPin_, INPUT_PULLDOWN);

  // Button
  encoderButtonPin_ = encoder_ButtonPin;
  previousButtonState_ = 0;
  if (encoderButtonPin_ >= 0) {
    pinMode(encoderButtonPin_, INPUT_PULLDOWN);
  }

  // Button debouncing
  debouncer_.begin(buttonDebounceTimeMs);
  debouncer_.onChange([this](bool newState) {
    if (buttonCallback_) {
      buttonCallback_(newState);
    }
    if (newState && buttonPushCallback_) {
      buttonPushCallback_();
    }
  });

  // Interrrupts
  Callback<void()>::func = std::bind(&Encoder::isr, this);
  IsrCallback_t func = static_cast<IsrCallback_t>(Callback<void()>::callback);

  attachInterrupt(digitalPinToInterrupt(this->encoderAPin_), func, CHANGE);
  attachInterrupt(digitalPinToInterrupt(this->encoderBPin_), func, CHANGE);

  return true;
}

//------------------------------------------------------------------------------
void Encoder::loop()
//------------------------------------------------------------------------------
{
  if (valueCallback_) {
    int16_t encoderPos = getValue();
    if (encoderPos != this->previousEncoderPos_) {
      valueCallback_(this->previousEncoderPos_, encoderPos);
      this->previousEncoderPos_ = encoderPos;
    }
  }

  ButtonState s = getButtonState();
  if (s == PUSHED) {
    debouncer_.triggerChange(true);
  } else if (s == RELEASED) {
    debouncer_.triggerChange(false);
  }

  debouncer_.loop();
}

//------------------------------------------------------------------------------
void Encoder::statistics()
//------------------------------------------------------------------------------
{
  LOG.i("LastValue:%d Value: %d", previousEncoderPos_, encoderPos_);
  LOG.i("ButtonState:%s", buttonStateToName(getButtonState(true)));
  LOG.i("PinState A:%d B:%d K:%d", digitalRead(encoderAPin_), digitalRead(encoderBPin_), digitalRead(encoderButtonPin_));
}

//------------------------------------------------------------------------------
Encoder::ButtonState Encoder::getButtonState(bool peek)
//------------------------------------------------------------------------------
{
  uint8_t butt_state = digitalRead(encoderButtonPin_);

  if (butt_state && !previousButtonState_) {
    if (!peek) {
      previousButtonState_ = true;
    }
    return PUSHED;
  }

  if (!butt_state && previousButtonState_) {
    if (!peek) {
      previousButtonState_ = false;
    }
    return RELEASED;
  }

  return (butt_state ? DOWN : UP);
}

//------------------------------------------------------------------------------
const char* Encoder::buttonStateToName(ButtonState buttonState)
//------------------------------------------------------------------------------
{
  switch (buttonState) {
    case DOWN:
      return "DOWN";
    case PUSHED:
      return "PUSHED";
    case UP:
      return "UP";
    case RELEASED:
      return "RELEASED";
    default:
      return "??UNKNOWN??";
  }
}
