#include <lua.h>
#include <lauxlib.h>

#include "cbson.h"
#include "cbson-oid.h"
#include "cbson-regex.h"
#include "cbson-binary.h"
#include "cbson-symbol.h"
#include "cbson-code.h"
#include "cbson-timestamp.h"
#include "cbson-ref.h"
#include "cbson-misc.h"
#include "cbson-int.h"
#include "cbson-uint.h"
#include "cbson-date.h"
#include "cbson-decimal.h"

#include "cbson-encode.h"
#include "cbson-decode.h"


#if LUA_VERSION_NUM >= 502

#define DECLARE_CLASS(state, name, type) \
  luaL_newmetatable(state, name##_METATABLE); \
  luaL_setfuncs(state, cbson_##type##_meta , 0); \
\
  lua_newtable(state); \
  luaL_setfuncs(state, cbson_##type##_methods , 0); \
\
  lua_setfield(state, -2, "__index")

#else

#define DECLARE_CLASS(state, name, type) \
  luaL_newmetatable(state, name##_METATABLE); \
  luaL_register(state, NULL, cbson_##type##_meta); \
  lua_pushliteral(state, #name ); \
  lua_setfield(state, -2, "_NAME"); \
\
  lua_newtable(state); \
  luaL_register(state, NULL, cbson_##type##_methods); \
\
  lua_setfield(state, -2, "__index")

#endif


int luaopen_cbson(lua_State *L)
{
  luaL_Reg cbsonlib[] = {
    { "decode",          cbson_decode },
    { "encode",          cbson_encode },
    { "encode_first",    cbson_encode_first },
    { "to_json",         cbson_to_json },
    { "to_relaxed_json", cbson_to_relaxed_json },
    { "from_json",       cbson_from_json },
    { "regex",           cbson_regex_new },
    { "oid",             cbson_oid_new },
    { "binary",          cbson_binary_new },
    { "symbol",          cbson_symbol_new },
    { "code",            cbson_code_new },
    { "codewscope",      cbson_codewscope_new },
    { "undefined",       cbson_undefined_new },
    { "null",            cbson_null_new },
    { "array",           cbson_array_new },
    { "minkey",          cbson_minkey_new },
    { "maxkey",          cbson_maxkey_new },
    { "ref",             cbson_ref_new },
    { "timestamp",       cbson_timestamp_new },
    { "int",             cbson_int64_new },
    { "uint",            cbson_uint64_new },
    { "decimal",         cbson_decimal_new },
    { "date",            cbson_date_new },
    { "int_to_raw",      cbson_int64_to_raw },
    { "raw_to_int",      cbson_int64_from_raw },
    { "uint_to_raw",     cbson_uint64_to_raw },
    { "raw_to_uint",     cbson_uint64_from_raw },
    { NULL, NULL }
  };

  // types
  DECLARE_CLASS(L, REGEX,      regex);
  DECLARE_CLASS(L, OID,        oid);
  DECLARE_CLASS(L, BINARY,     binary);
  DECLARE_CLASS(L, SYMBOL,     symbol);
  DECLARE_CLASS(L, CODE,       code);
  DECLARE_CLASS(L, CODEWSCOPE, codewscope);
  DECLARE_CLASS(L, UNDEFINED,  undefined);
  DECLARE_CLASS(L, CBNULL,     null);
  DECLARE_CLASS(L, ARRAY,      array);
  DECLARE_CLASS(L, MINKEY,     minkey);
  DECLARE_CLASS(L, MAXKEY,     maxkey);
  DECLARE_CLASS(L, REF,        ref);
  DECLARE_CLASS(L, TIMESTAMP,  timestamp);
  DECLARE_CLASS(L, INT64,      int64);
  DECLARE_CLASS(L, DECIMAL,    decimal);
  DECLARE_CLASS(L, DATE,       date);
  DECLARE_CLASS(L, UINT64,     uint64);

  // cbson module
  lua_newtable(L);

#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L,  cbsonlib, 0);
#else
  luaL_register(L, NULL, cbsonlib);
#endif

  lua_pushliteral(L, CBSON_MODNAME);
  lua_setfield(L, -2, "_NAME");
  lua_pushliteral(L, CBSON_VERSION);
  lua_setfield(L, -2, "_VERSION");

  return 1;
}
