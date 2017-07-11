#include <lauxlib.h>

#include "cbson.h"
#include "cbson-minmax.h"

// UNDEFINED

DEFINE_CHECK(UNDEFINED, undefined)

int cbson_undefined_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_undefined_t));

  luaL_getmetatable(L, UNDEFINED_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_undefined_new(lua_State* L)
{
  return cbson_undefined_create(L);
}

int cbson_undefined_tostring(lua_State* L)
{
  check_cbson_undefined(L, 1);

  lua_pushstring(L, "undefined");
  return 1;
}

const struct luaL_Reg cbson_undefined_meta[] = {
  {"__tostring", cbson_undefined_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_undefined_methods[] = {
  {NULL, NULL}
};

// NULL

DEFINE_CHECK(CBNULL, null)

int cbson_null_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_null_t));

  luaL_getmetatable(L, CBNULL_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_null_new(lua_State* L)
{
  return cbson_null_create(L);
}

int cbson_null_tostring(lua_State* L)
{
  check_cbson_null(L, 1);

  lua_pushstring(L, "null");
  return 1;
}

int cbson_null_eq  (lua_State* L) { lua_pushboolean( L, (check_cbson_null(L,1)!=NULL) && (check_cbson_null(L,2)!=NULL) ); return 1; }

const struct luaL_Reg cbson_null_meta[] = {
  { "__eq",      cbson_null_eq  },
  {"__tostring", cbson_null_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_null_methods[] = {
  {NULL, NULL}
};

// MINKEY

DEFINE_CHECK(MINKEY, minkey)

int cbson_minkey_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_minkey_t));

  luaL_getmetatable(L, MINKEY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_minkey_new(lua_State* L)
{
  return cbson_minkey_create(L);
}

int cbson_minkey_tostring(lua_State* L)
{
  check_cbson_minkey(L, 1);

  lua_pushstring(L, "minkey");
  return 1;
}

const struct luaL_Reg cbson_minkey_meta[] = {
  {"__tostring", cbson_minkey_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_minkey_methods[] = {
  {NULL, NULL}
};

// MAXKEY


DEFINE_CHECK(MAXKEY, maxkey)

int cbson_maxkey_create(lua_State* L)
{
  lua_newuserdata(L, sizeof(cbson_maxkey_t));

  luaL_getmetatable(L, MAXKEY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_maxkey_new(lua_State* L)
{
  return cbson_maxkey_create(L);
}

int cbson_maxkey_tostring(lua_State* L)
{
  check_cbson_maxkey(L, 1);

  lua_pushstring(L, "maxkey");
  return 1;
}

const struct luaL_Reg cbson_maxkey_meta[] = {
  {"__tostring", cbson_maxkey_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_maxkey_methods[] = {
  {NULL, NULL}
};

