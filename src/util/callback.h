#pragma once

#include <functional>

// thats why i hate c++
// many thx to https://stackoverflow.com/questions/1000663/using-a-c-class-member-function-as-a-c-callback-function/56943930#56943930
template <typename T>
struct Callback;

template <typename Ret, typename... Params>
struct Callback<Ret(Params...)> {
  template <typename... Args>
  static Ret callback(Args... args) {
    return func(args...);
  }

  static std::function<Ret(Params...)> func;
};

template <typename Ret, typename... Params>
std::function<Ret(Params...)> Callback<Ret(Params...)>::func;