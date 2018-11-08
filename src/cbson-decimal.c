#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-decimal.h"

DEFINE_CHECK(DECIMAL, decimal)

int cbson_decimal_create(lua_State* L, const bson_decimal128_t* decimal)
{
  cbson_decimal_t* ud = lua_newuserdata(L, sizeof(cbson_decimal_t));
  if (decimal) {
    ud->dec.high = decimal->high;
    ud->dec.low = decimal->low;
  } else {
    ud->dec.high = 0;
    ud->dec.low = 0;
  }

  luaL_getmetatable(L, DECIMAL_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_decimal_new(lua_State* L)
{
  const char* decimal = luaL_checkstring(L, 1);
  if (decimal) {
    bson_decimal128_t dec;
    bool success = bson_decimal128_from_string(decimal, &dec);

    if (!success) {
      lua_pushstring(L, "Invalid decimal value");
      lua_error(L);
      return 1;
    }
    return cbson_decimal_create(L, &dec);
  } else {
    return cbson_decimal_create(L, NULL);
  }
}

int cbson_decimal_tostring(lua_State* L)
{
  cbson_decimal_t* a = check_cbson_decimal(L, 1);

  char dec_str[BSON_DECIMAL128_STRING];
  bson_decimal128_to_string(&a->dec, dec_str);

  lua_pushstring(L, dec_str);
  return 1;
}


const struct luaL_Reg cbson_decimal_meta[] = {
  {"__tostring", cbson_decimal_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_decimal_methods[] = {
  {NULL, NULL}
};
