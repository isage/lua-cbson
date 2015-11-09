#include <lauxlib.h>
#include <string.h>
#include <resolv.h>
#include <stdint.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-binary.h"

DEFINE_CHECK(BINARY, binary)

int cbson_binary_create(lua_State* L, uint8_t type, const char* binary)
{
  cbson_binary_t* ud = lua_newuserdata(L, sizeof(cbson_binary_t));

  ud->data = malloc(strlen(binary)+1);
  ud->type = type;
  strcpy(ud->data, binary);

  luaL_getmetatable(L, BINARY_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_binary_new(lua_State* L)
{
  const char* binary = luaL_checkstring(L, 1);
  uint8_t type = luaL_optnumber(L, 2, 0);

  return cbson_binary_create(L, type, binary);
}

int cbson_binary_destroy(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);

  free(a->data);

  return 0;
}

int cbson_binary_tostring(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);

  lua_pushfstring(L, "%d:%s", a->type, a->data);
  return 1;
}

int cbson_binary_data(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  const char* binary = luaL_optstring(L, 2, NULL);

  if (binary)
  {
    free(a->data);
    a->data = malloc(strlen(binary)+1);
    strcpy(a->data, binary);
  }

  lua_pushstring(L, a->data);
  return 1;
}

int cbson_binary_raw(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  const char* binary = luaL_optstring(L, 2, NULL);
  int binary_len = luaL_optnumber(L, 3, -1);

  if (binary && binary_len >=0)
  {
    free(a->data);

    size_t b64_len;
    char *b64;

    b64_len = (binary_len / 3 + 1) * 4 + 1;
    b64 = malloc(b64_len);
    b64_ntop((unsigned const char*)binary, binary_len, b64, b64_len);

    a->data = malloc(strlen(b64)+1);
    strcpy(a->data, b64);
    free(b64);
  }

  binary_len = b64_pton (a->data, NULL, 0);
  unsigned char* buf=malloc(binary_len+1);
  b64_pton(a->data, buf, binary_len+1);

  lua_pushlstring(L, (char*)buf, binary_len);

  free(buf);
  return 1;
}

int cbson_binary_type(lua_State* L)
{
  cbson_binary_t* a = check_cbson_binary(L, 1);
  int8_t type = luaL_optint(L, 2, -1);

  if (type>=0)
  {
    a->type = type;
  }

  lua_pushnumber(L, a->type);
  return 1;
}


const struct luaL_Reg cbson_binary_meta[] = {
  {"__tostring", cbson_binary_tostring},
  {"__gc",       cbson_binary_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_binary_methods[] = {
  {"data",   cbson_binary_data},
  {"type",   cbson_binary_type},
  {"raw",    cbson_binary_raw},
  {NULL, NULL}
};

