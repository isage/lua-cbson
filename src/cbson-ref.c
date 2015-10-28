#include <lua.h>
#include <lauxlib.h>
#include <string.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-ref.h"

DEFINE_CHECK(REF, ref)

int cbson_ref_create(lua_State* L, const char* ref, const char* id)
{
  cbson_ref_t* ud = lua_newuserdata(L, sizeof(cbson_ref_t));

  ud->ref = malloc(strlen(ref)+1);
  ud->id = malloc(strlen(id)+1);
  strcpy(ud->ref, ref);
  strcpy(ud->id, id);

  luaL_getmetatable(L, REF_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_ref_new(lua_State* L)
{
  const char* ref = luaL_checkstring(L, 1);
  const char* id = luaL_checkstring(L, 2);

  return cbson_ref_create(L,ref, id);
}

int cbson_ref_destroy(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);

  free(a->ref);
  free(a->id);

  return 0;
}

int cbson_ref_tostring(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);

  lua_pushfstring(L, "%s/%s", a->ref, a->id);
  return 1;
}

int cbson_ref_ref(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);
  const char* ref = luaL_optstring(L, 2, NULL);

  if (ref)
  {
    free(a->ref);
    a->ref = malloc(strlen(ref)+1);
    strcpy(a->ref, ref);
  }

  lua_pushstring(L, a->ref);
  return 1;
}

int cbson_ref_id(lua_State* L)
{
  cbson_ref_t* a = check_cbson_ref(L, 1);
  const char* id = luaL_optstring(L, 2, NULL);

  if (id)
  {
    free(a->id);
    a->id = malloc(strlen(id)+1);
    strcpy(a->id, id);
  }

  lua_pushstring(L, a->id);
  return 1;
}


const struct luaL_Reg cbson_ref_meta[] = {
  {"__tostring", cbson_ref_tostring},
  {"__gc",       cbson_ref_destroy},
  {NULL, NULL}
};

const struct luaL_Reg cbson_ref_methods[] = {
  {"ref",   cbson_ref_ref},
  {"id", cbson_ref_id},
  {NULL, NULL}
};

