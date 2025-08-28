#include <napi.h>

#define NODE_API_ASSERT(cond, message) \
  if (!(cond)) { \
    Napi::Error::New(info.Env(), message) \
        .ThrowAsJavaScriptException(); \
    return Napi::Value(info.Env(), nullptr); \
  }

static Napi::Value SayHello(const Napi::CallbackInfo& info) {
  NODE_API_ASSERT(info.Length() >= 1, "Invalid number of arguments");
  NODE_API_ASSERT(info[0].IsString(), "Expected string argument");

  std::string name = info[0].As<Napi::String>();
  std::string message = Napi::details::StringFormat("C++: Hello, %s!", name.c_str());
  return Napi::String::From(info.Env(), message);
}

Napi::Object Init(Napi::Env env, Napi::Object exports) {
  exports.DefineProperties(
      { Napi::PropertyDescriptor::Function("sayHello", SayHello) });
  return exports;
}

NODE_API_MODULE(greeter, Init)
