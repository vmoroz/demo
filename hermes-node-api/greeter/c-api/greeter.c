#include <node_api.h>
#include <stdio.h>

static napi_value SayHello(napi_env env, const napi_callback_info info) {
  size_t argc = 1;
  napi_value args[1] = {0};
  napi_valuetype arg_type = napi_undefined;
  char name[256] = {0};
  char message[512] = {0};
  napi_value result = NULL;

  napi_get_cb_info(env, info, &argc, args, NULL, NULL);
  napi_get_value_string_utf8(env, args[0], name, sizeof(name), NULL);
  snprintf(message, sizeof(message), "C API: Hello, %s", name);
  napi_create_string_utf8(env, message, NAPI_AUTO_LENGTH, &result);

  return result;
}

static napi_value Init(napi_env env, napi_value exports) {
  napi_property_descriptor prop_desc = {
      "sayHello", NULL, SayHello, NULL, NULL, NULL, napi_default, NULL};
  napi_define_properties(env, exports, 1, &prop_desc);
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, Init)

// NAPI_MODULE(NODE_GYP_MODULE_NAME, Init) is expanded to
//
// NAPI_MODULE_EXPORT int32_t node_api_module_get_api_version_v1() {
//   return NAPI_VERSION;
// }
//
// NAPI_MODULE_EXPORT napi_value napi_register_module_v1(
//     napi_env env, napi_value exports) {
//   return Init(env, exports);
// }
