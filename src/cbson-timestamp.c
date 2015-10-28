#include <lauxlib.h>
#include <inttypes.h>

#include "cbson.h"
#include "cbson-timestamp.h"

DEFINE_CHECK(TIMESTAMP, timestamp)

int cbson_timestamp_create(lua_State* L, uint32_t timestamp, uint32_t increment)
{
  cbson_timestamp_t* ud = lua_newuserdata(L, sizeof(cbson_timestamp_t));

  ud->timestamp = timestamp;
  ud->increment = increment;

  luaL_getmetatable(L, TIMESTAMP_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_timestamp_new(lua_State* L)
{
  uint32_t timestamp = luaL_checknumber(L, 1);
  uint32_t increment = luaL_checknumber(L, 2);

  return cbson_timestamp_create(L, timestamp, increment);
}

int cbson_timestamp_tostring(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);

  char buffer[(sizeof(int32_t)*8+1)*2];
  sprintf(buffer, "%"PRId32" %"PRId32, a->timestamp, a->increment);

  lua_pushstring(L, buffer);
  return 1;
}

int cbson_timestamp_timestamp(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);
  uint32_t timestamp = luaL_optint(L, 2, 0);

  if (lua_gettop(L) == 2)
  {
    a->timestamp = timestamp;
  }

  lua_pushnumber(L, a->timestamp);
  return 1;
}

int cbson_timestamp_increment(lua_State* L)
{
  cbson_timestamp_t* a = check_cbson_timestamp(L, 1);
  uint32_t increment = luaL_optint(L, 2, 0);

  if (lua_gettop(L) == 2)
  {
    a->increment = increment;
  }

  lua_pushnumber(L, a->increment);
  return 1;
}


const struct luaL_Reg cbson_timestamp_meta[] = {
  {"__tostring", cbson_timestamp_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_timestamp_methods[] = {
  {"timestamp", cbson_timestamp_timestamp},
  {"increment", cbson_timestamp_increment},
  {NULL, NULL}
};

