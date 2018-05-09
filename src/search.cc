#include "search.h"

namespace nis {

napi_value  createString(napi_env env, const char *str) {
  napi_status status;
  napi_value  string;

  status = napi_create_string_utf8(env, str, NAPI_AUTO_LENGTH, &string);
  if (status != napi_ok) {
    status = napi_throw_error(env, nullptr, "Couldn't create string");
    return nullptr;
  }
  return string;
}

void        checkSingleProp(napi_env env, napi_value obj, napi_value prop) {
    napi_status status;
    bool        result;

    status = napi_has_property(env, obj, prop, &result);
    if (status != napi_ok) {
        napi_throw_error(env, nullptr, "Couldn't check property.");
    }
    if (!result) {
        napi_throw_error(env, nullptr, "One of the argument's format is bad.");
    }
}

void        checkProps(napi_env env, napi_value obj) {
  checkSingleProp(env, obj, createString(env, "height"));
  checkSingleProp(env, obj, createString(env, "width"));
  checkSingleProp(env, obj, createString(env, "channels"));
  checkSingleProp(env, obj, createString(env, "data"));
}

napi_value  Search(napi_env env, napi_callback_info info) {
  size_t      argc = 2;
  napi_value  argv[2];
  napi_value  m1;
  napi_value  m2;
  napi_status status;

  status = napi_get_cb_info(env, info, &argc, argv, nullptr, nullptr);
  if (status != napi_ok) return nullptr;
  if (argc != 2) {
    status = napi_throw_error(env, nullptr, "Wrong number of arguments, expected 2.");
    return nullptr;
  }
  m1 = argv[0];
  m2 = argv[1];
  checkProps(env, m1);
  checkProps(env, m2);
  return m1;
}

napi_value  init(napi_env env, napi_value exports) {
  napi_status status;
  napi_value  fn;

  status = napi_create_function(env, nullptr, 0, Search, nullptr, &fn);
  if (status != napi_ok) return nullptr;

  status = napi_set_named_property(env, exports, "search", fn);
  if (status != napi_ok) return nullptr;
  return exports;
}

NAPI_MODULE(NODE_GYP_MODULE_NAME, init);

}
