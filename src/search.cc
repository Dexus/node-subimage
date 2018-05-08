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

bool        checkProps(napi_env env, napi_value obj, napi_value rows, napi_value cols, napi_value data, napi_value channels) {
  bool        result;
  napi_status status;

  status = napi_has_property(env, obj, rows, &result);
  if (status != napi_ok) {
    status = napi_throw_error(env, nullptr, "Couldn't check property");
  }
  if (!result) return 0;
  status = napi_has_property(env, obj, cols, &result);
  if (status != napi_ok) {
    status = napi_throw_error(env, nullptr, "Couldn't check property");
  }
  if (!result) return 0;
  status = napi_has_property(env, obj, data, &result);
  if (status != napi_ok) {
    status = napi_throw_error(env, nullptr, "Couldn't check property");
  }
  if (!result) return 0;
  status = napi_has_property(env, obj, channels, &result);
  if (status != napi_ok) {
    status = napi_throw_error(env, nullptr, "Couldn't check property");
  }
  if (!result) return 0;
  return 1;
}

void        setProps(napi_env env, napi_value obj, napi_value *dest, napi_value rows, napi_value cols, napi_value data, napi_value channels) {
  napi_value  val;
  napi_status status;

  status = napi_get_property(env, obj, rows, &val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't get property");
  status = napi_set_property(env, *dest, rows, val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't set property");
  status = napi_get_property(env, obj, cols, &val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't get property");
  status = napi_set_property(env, *dest, cols, val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't set property");
  status = napi_get_property(env, obj, data, &val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't get property");
  status = napi_set_property(env, *dest, data, val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't set property");
  status = napi_get_property(env, obj, channels, &val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't get property");
  status = napi_set_property(env, *dest, channels, val);
  if (status != napi_ok) napi_throw_error(env, nullptr, "Couldn't set property");
}

void        getProps(napi_env env, napi_value obj[2], napi_value *m1, napi_value *m2) {
  bool        result;
  napi_value  rows = createString(env, "rows");
  napi_value  cols = createString(env, "cols");
  napi_value  data = createString(env, "data");
  napi_value  channels = createString(env, "channels");
  napi_status status;

  result = checkProps(env, obj[0], rows, cols, data, channels);
  if (!result) {
    status = napi_throw_error(env, nullptr, "Bad argument 'imgMatrix'");
  }
  result = checkProps(env, obj[1], rows, cols, data, channels);
  if (!result) {
    status = napi_throw_error(env, nullptr, "Bad argument 'tplMatrix'");
  }
  setProps(env, obj[0], m1, rows, cols, data, channels);
  setProps(env, obj[1], m2, rows, cols, data, channels);
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

  status = napi_create_object(env, &m1);
  if (status != napi_ok) return nullptr;
  status = napi_create_object(env, &m2);
  if (status != napi_ok) return nullptr;
  getProps(env, argv, &m1, &m2);

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
