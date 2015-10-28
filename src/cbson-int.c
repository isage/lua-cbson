#include <lauxlib.h>
#include <inttypes.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-int.h"
#include "intpow.h"

int64_t cbson_int64_check(lua_State *L, int index)
{
  if (lua_isuserdata(L, index) && luaL_checkudata(L, index, INT64_METATABLE))
  {
    return *(int64_t*)lua_touserdata(L, index);
  }
  else if (lua_isnumber(L, index))
  {
    return lua_tonumber(L, index);
  }
  else if (lua_isstring(L, index))
  {
    return strtoll(lua_tostring(L, index), NULL, 10);
  }
  else
  {
    lua_pushstring(L, "Invalid operand. Expected 'cbson.int', 'string' or 'number'");
    lua_error(L);
    return 0;
  }
}

int cbson_int64_create(lua_State* L, int64_t val)
{
  cbson_int64_t* ud = lua_newuserdata(L, sizeof(cbson_int64_t));

  *ud = val;

  luaL_getmetatable(L, INT64_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_int64_new(lua_State* L)
{
  return cbson_int64_create(L, cbson_int64_check(L,1));
}

int cbson_int64_tostring(lua_State* L)
{
  cbson_int64_t a = cbson_int64_check(L, 1);

  char buffer[sizeof(int64_t)*8+1];
  sprintf(buffer, "%"PRId64, a);

  lua_pushstring(L, buffer);
  return 1;
}

int cbson_int64_add (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) + cbson_int64_check(L,2) ); }
int cbson_int64_sub (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) - cbson_int64_check(L,2) ); }
int cbson_int64_mul (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) * cbson_int64_check(L,2) ); }
int cbson_int64_div (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) / cbson_int64_check(L,2) ); }
int cbson_int64_mod (lua_State* L) { return cbson_int64_create( L, cbson_int64_check(L,1) % cbson_int64_check(L,2) ); }
int cbson_int64_pow (lua_State* L) { return cbson_int64_create( L, cbson_pow( cbson_int64_check(L,1), cbson_int64_check(L,2) ) ); }
int cbson_int64_unm (lua_State* L) { return cbson_int64_create( L, -cbson_int64_check(L,1) ); }
int cbson_int64_eq  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) == cbson_int64_check(L,2) ); return 1; }
int cbson_int64_lt  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) <  cbson_int64_check(L,2) ); return 1; }
int cbson_int64_le  (lua_State* L) { lua_pushboolean( L, cbson_int64_check(L,1) <= cbson_int64_check(L,2) ); return 1; }

const struct luaL_Reg cbson_int64_meta[] = {
  { "__add",     cbson_int64_add },
  { "__sub",     cbson_int64_sub },
  { "__mul",     cbson_int64_mul },
  { "__div",     cbson_int64_div },
  { "__mod",     cbson_int64_mod },
  { "__pow",     cbson_int64_pow },
  { "__unm",     cbson_int64_unm },
  { "__eq",      cbson_int64_eq  },
  { "__lt",      cbson_int64_lt  },
  { "__le",      cbson_int64_le  },
  {"__tostring", cbson_int64_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_int64_methods[] = {
  {NULL, NULL}
};
