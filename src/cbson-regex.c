#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-regex.h"

DEFINE_CHECK(REGEX, regex)

int cbson_regex_create(lua_State* L, const char* regex, const char* options)
{
  cbson_regex_t* ud = lua_newuserdata(L, sizeof(cbson_regex_t));

  ud->regex = malloc(strlen(regex)+1);
  ud->options = malloc(strlen(options)+1);
  strcpy(ud->regex, regex);
  strcpy(ud->options, options);

  luaL_getmetatable(L, REGEX_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_regex_new(lua_State* L)
{
  const char* regex = luaL_checkstring(L, 1);
  const char* options = luaL_checkstring(L, 2);

  return cbson_regex_create(L,regex, options);
}

int cbson_regex_destroy(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);

  free(a->regex);
  free(a->options);

  return 0;
}

int cbson_regex_tostring(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);

  lua_pushfstring(L, "/%s/%s", a->regex, a->options);
  return 1;
}

int cbson_regex_regex(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);
  const char* regex = luaL_optstring(L, 2, NULL);

  if (regex)
  {
    free(a->regex);
    a->regex = malloc(strlen(regex)+1);
    strcpy(a->regex, regex);
  }

  lua_pushstring(L, a->regex);
  return 1;
}

int cbson_regex_options(lua_State* L)
{
  cbson_regex_t* a = check_cbson_regex(L, 1);
  const char* opt = luaL_optstring(L, 2, NULL);

  if (opt)
  {
    free(a->options);
    a->options = malloc(strlen(opt)+1);
    strcpy(a->options, opt);
  }

  lua_pushstring(L, a->options);
  return 1;
}


const struct luaL_Reg cbson_regex_meta[] = {
  {"__tostring", cbson_regex_tostring},
  {"__gc",       cbson_regex_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_regex_methods[] = {
  {"regex",   cbson_regex_regex},
  {"options", cbson_regex_options},
  {NULL, NULL}
};

