#pragma once

#include <Arduino.h>
#include <functional>

class Translator {
 public:
  virtual int64_t revolutionToSteps(double revolution) = 0;
  virtual double stepsToRevolution(int64_t steps) = 0;

 private:
};

class LambdaTranslator {
 public:
  typedef std::function<double(int64_t)> StepsToRevolution_t;
  typedef std::function<int64_t(double)> RevolutionToSteps_t;

  LambdaTranslator(StepsToRevolution_t str, RevolutionToSteps_t rts) : stepsToRevolution_(str), revolutionToSteps_(rts) {}
  virtual int64_t revolutionToSteps(double revolution) { return revolutionToSteps_(revolution); }
  virtual double stepsToRevolution(int64_t steps) { return stepsToRevolution_(steps); }

 private:
  StepsToRevolution_t stepsToRevolution_;
  RevolutionToSteps_t revolutionToSteps_;
};