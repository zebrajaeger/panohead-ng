#pragma once

#include <Arduino.h>
#include <functional>

class Translator {
 public:
  virtual int64_t revolutionToSteps(double revolution) = 0;

 private:
};

class LambdaTranslator {
 public:
  LambdaTranslator(std::function<int64_t(double)> lambda) : lambda_(lambda) {}
  virtual int64_t revolutionToSteps(double revolution) { return lambda_(revolution); }

 private:
  std::function<int32_t(double)> lambda_;
};