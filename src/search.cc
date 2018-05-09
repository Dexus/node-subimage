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

void        checkChannels(napi_env env, napi_value m1, napi_value m2, napi_value m1Data, napi_value m2Data) {
  napi_value  m1ChannelsValue;
  napi_value  m2ChannelsValue;
  /*napi_value  m1DataLengthValue;
  napi_value  m2DataLengthValue;*/
  uint32_t    m1Channels;
  uint32_t    m2Channels;
  /*uint32_t    m1DataLength;
  uint32_t    m2DataLength;*/
  napi_status status;

  status = napi_get_property(env, m1, createString(env, "channels"), &m1ChannelsValue);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get m1 channels property.");
  }
  status = napi_get_value_uint32(env, m1ChannelsValue, &m1Channels);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get uint32 value.");
  }
  status = napi_get_property(env, m2, createString(env, "channels"), &m2ChannelsValue);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get m2 channels property.");
  }
  status = napi_get_value_uint32(env, m2ChannelsValue, &m2Channels);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get uint32 value.");
  }
  if (m1Channels < 1 || m1Channels > 4 || m2Channels < 1 || m2Channels > 4) {
    napi_throw_error(env, nullptr, "Bad number of channels.");
  }
  if (abs(m1Channels - m2Channels) > 1) {
    napi_throw_error(env, nullptr, "Channel mismatch.");
  }
  /*status = napi_get_property(env, m1Data, createString(env, "length"), &m1DataLengthValue);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get m1Data length property.");
  }
  status = napi_get_value_uint32(env, m1DataLengthValue, &m1DataLength);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get uint32 value.");
  }
  status = napi_get_property(env, m2Data, createString(env, "length"), &m2DataLengthValue);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get m2Data length property.");
  }
  status = napi_get_value_uint32(env, m2DataLengthValue, &m2DataLength);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get uint32 value.");
  }
  if (m1Channels != m1DataLength) {
    napi_throw_error(env, nullptr, "Bad argument 'imgMatrix'.");
  }
  if (m2Channels != m2DataLength) {
    napi_throw_error(env, nullptr, "Bad argument 'tplMatrix'.");
  }*/
}

void        setMatrixData(napi_env env, napi_value obj, napi_value *matrix) {
  napi_value  dataValue;
  napi_status status;

  status = napi_get_property(env, obj, createString(env, "data"), &dataValue);
  if (status != napi_ok) {
    napi_throw_error(env, nullptr, "Couldn't get data property.");
  }
  *matrix = dataValue;
}

napi_value  Search(napi_env env, napi_callback_info info) {
  size_t      argc = 2;
  napi_value  argv[2];
  napi_value  m1;
  napi_value  m2;
  napi_value  m1Data;
  napi_value  m2Data;
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
  setMatrixData(env, m1, &m1Data);
  setMatrixData(env, m2, &m2Data);
  checkChannels(env, m1, m2, m1Data, m2Data);
  return m1Data;
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
