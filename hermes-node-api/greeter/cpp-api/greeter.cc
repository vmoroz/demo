#include <napi.h>

static Napi::Value SayHello(const Napi::CallbackInfo& info) {
  std::string name = info[0].As<Napi::String>();
  std::string message =
      Napi::details::StringFormat("C++ API: Hello, %s!", name.c_str());
  return Napi::String::From(info.Env(), message);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.DefineProperties(
      {Napi::PropertyDescriptor::Function("sayHello", SayHello)});
  return exports;
}

NODE_API_MODULE(greeter, Init)
