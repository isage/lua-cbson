#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-symbol.h"

DEFINE_CHECK(SYMBOL, symbol)

int cbson_symbol_create(lua_State* L, const char* symbol)
{
  cbson_symbol_t* ud = lua_newuserdata(L, sizeof(cbson_symbol_t));

  ud->symbol = malloc(strlen(symbol)+1);
  strcpy(ud->symbol, symbol);

  luaL_getmetatable(L, SYMBOL_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_symbol_new(lua_State* L)
{
  const char* symbol = luaL_checkstring(L, 1);

  return cbson_symbol_create(L, symbol);
}

int cbson_symbol_destroy(lua_State* L)
{
  cbson_symbol_t* a = check_cbson_symbol(L, 1);

  free(a->symbol);

  return 0;
}

int cbson_symbol_tostring(lua_State* L)
{
  cbson_symbol_t* a = check_cbson_symbol(L, 1);

  lua_pushstring(L, a->symbol);
  return 1;
}

const struct luaL_Reg cbson_symbol_meta[] = {
  {"__tostring", cbson_symbol_tostring},
  {"__gc",       cbson_symbol_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_symbol_methods[] = {
  {NULL, NULL}
};

