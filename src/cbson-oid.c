#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "cbson.h"
#include "cbson-oid.h"

DEFINE_CHECK(OID, oid)

int cbson_oid_create(lua_State* L, const char* oid)
{
  if (strlen(oid) != 24)
  {
    lua_pushstring(L, "Invalid operand. OID should be hex-string 24-chars long");
    lua_error(L);
    return 1;
  }

  cbson_oid_t* ud = lua_newuserdata(L, sizeof(cbson_oid_t));

  ud->oid = malloc(strlen(oid)+1);
  strcpy(ud->oid, oid);

  luaL_getmetatable(L, OID_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_oid_new(lua_State* L)
{
  const char* oid = luaL_checkstring(L, 1);

  return cbson_oid_create(L, oid);
}

int cbson_oid_destroy(lua_State* L)
{
  cbson_oid_t* a = check_cbson_oid(L, 1);

  free(a->oid);

  return 0;
}

int cbson_oid_tostring(lua_State* L)
{
  cbson_oid_t* a = check_cbson_oid(L, 1);

  lua_pushstring(L, a->oid);
  return 1;
}

int cbson_oid_timestamp(lua_State* L)
{
  cbson_oid_t* a = check_cbson_oid(L, 1);
  char hex[9];
  uint32_t number;

  strncpy(hex, a->oid, 8);
  hex[8] = '\n';
  number = (uint32_t)strtol(hex, NULL, 16);
  lua_pushnumber(L, number);
  return 1;
}


const struct luaL_Reg cbson_oid_meta[] = {
  {"__tostring", cbson_oid_tostring},
  {"__gc",       cbson_oid_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_oid_methods[] = {
  {"timestamp",   cbson_oid_timestamp},
  {NULL, NULL}
};
