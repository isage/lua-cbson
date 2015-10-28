#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-code.h"

DEFINE_CHECK(CODE, code)

int cbson_code_create(lua_State* L, const char* code)
{
  cbson_code_t* ud = lua_newuserdata(L, sizeof(cbson_code_t));

  ud->code = malloc(strlen(code)+1);
  strcpy(ud->code, code);

  luaL_getmetatable(L, CODE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_code_new(lua_State* L)
{
  const char* code = luaL_checkstring(L, 1);

  return cbson_code_create(L, code);
}

int cbson_code_destroy(lua_State* L)
{
  cbson_code_t* a = check_cbson_code(L, 1);

  free(a->code);

  return 0;
}

int cbson_code_tostring(lua_State* L)
{
  cbson_code_t* a = check_cbson_code(L, 1);

  lua_pushstring(L, a->code);
  return 1;
}

const struct luaL_Reg cbson_code_meta[] = {
  {"__tostring", cbson_code_tostring},
  {"__gc",       cbson_code_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_code_methods[] = {
  {NULL, NULL}
};

// CODEWSCOPE

DEFINE_CHECK(CODEWSCOPE, codewscope)

int cbson_codewscope_create(lua_State* L, const char* code)
{
  cbson_codewscope_t* ud = lua_newuserdata(L, sizeof(cbson_codewscope_t));

  ud->code = malloc(strlen(code)+1);
  strcpy(ud->code, code);

  luaL_getmetatable(L, CODEWSCOPE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_codewscope_new(lua_State* L)
{
  const char* code = luaL_checkstring(L, 1);

  return cbson_codewscope_create(L, code);
}

int cbson_codewscope_destroy(lua_State* L)
{
  cbson_codewscope_t* a = check_cbson_codewscope(L, 1);

  free(a->code);

  return 0;
}

int cbson_codewscope_tostring(lua_State* L)
{
  cbson_codewscope_t* a = check_cbson_codewscope(L, 1);

  lua_pushstring(L, a->code);
  return 1;
}

const struct luaL_Reg cbson_codewscope_meta[] = {
  {"__tostring", cbson_codewscope_tostring},
  {"__gc",       cbson_codewscope_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_codewscope_methods[] = {
  {NULL, NULL}
};

