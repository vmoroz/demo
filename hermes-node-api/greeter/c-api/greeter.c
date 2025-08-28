#include <node_api.h>
#include <stdio.h>

#define CHECK_STATUS(expr)                                                     \
  do {                                                                         \
    napi_status status = (expr);                                               \
    if (status != napi_ok) {                                                   \
      return NULL;                                                             \
    }                                                                          \
  } while (0)

static napi_value SayHello(napi_env env, const napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1] = {};
  napi_valuetype arg_type = napi_undefined;
  char name[256] = {};
  char message[512] = {};
  napi_value result = NULL;

  CHECK_STATUS(napi_get_cb_info(env, info, &argc, args, NULL, NULL));
  if (argc < 1) {
    napi_throw_error(env, NULL, "Invalid number of arguments");
    return NULL;
  }
  CHECK_STATUS(napi_typeof(env, args[0], &arg_type));
  if (arg_type != napi_string) {
    napi_throw_error(env, NULL, "Expected string argument");
    return NULL;
  }

  CHECK_STATUS(
      napi_get_value_string_utf8(env, args[0], name, sizeof(name), NULL));

  snprintf(message, sizeof(message), "C: Hello, %s!", name);

  CHECK_STATUS(
      napi_create_string_utf8(env, message, NAPI_AUTO_LENGTH, &result));

  return result;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor prop_desc = {
      "sayHello", NULL, SayHello, NULL, NULL, NULL, napi_default, NULL};
  CHECK_STATUS(napi_define_properties(env, exports, 1, &prop_desc));
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)
