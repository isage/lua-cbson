#include <lauxlib.h>
#include <inttypes.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-date.h"
#include "cbson-int.h"
#include "cbson-uint.h"
#include "cbson-util.h"
#include "intpow.h"

int64_t cbson_date_check(lua_State *L, int index)
{
  if (lua_isuserdata(L, index) && (luaL_checkudata_ex(L, index, INT64_METATABLE) || luaL_checkudata_ex(L, index, UINT64_METATABLE) || luaL_checkudata_ex(L, index, DATE_METATABLE)))
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
    lua_pushstring(L, "Invalid operand. Expected 'cbson.date', 'string' or 'number'");
    lua_error(L);
    return 0;
  }
}

int cbson_date_create(lua_State* L, int64_t val)
{
  cbson_date_t* ud = lua_newuserdata(L, sizeof(cbson_date_t));

  *ud = val;

  luaL_getmetatable(L, DATE_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_date_new(lua_State* L)
{
  return cbson_date_create(L, cbson_date_check(L,1));
}

int cbson_date_tostring(lua_State* L)
{
  cbson_date_t a = cbson_date_check(L, 1);

  char buffer[sizeof(int64_t)*8+1];
  sprintf(buffer, "%"PRId64, a);

  lua_pushstring(L, buffer);
  return 1;
}

int cbson_date_add (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) + cbson_date_check(L,2) ); }
int cbson_date_sub (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) - cbson_date_check(L,2) ); }
int cbson_date_mul (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) * cbson_date_check(L,2) ); }
int cbson_date_div (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) / cbson_date_check(L,2) ); }
int cbson_date_mod (lua_State* L) { return cbson_date_create( L, cbson_date_check(L,1) % cbson_date_check(L,2) ); }
int cbson_date_pow (lua_State* L) { return cbson_date_create( L, cbson_pow( cbson_date_check(L,1), cbson_date_check(L,2) ) ); }
int cbson_date_unm (lua_State* L) { return cbson_date_create( L, -cbson_date_check(L,1) ); }
int cbson_date_eq  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) == cbson_date_check(L,2) ); return 1; }
int cbson_date_lt  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) <  cbson_date_check(L,2) ); return 1; }
int cbson_date_le  (lua_State* L) { lua_pushboolean( L, cbson_date_check(L,1) <= cbson_date_check(L,2) ); return 1; }

const struct luaL_Reg cbson_date_meta[] = {
  { "__add",     cbson_date_add },
  { "__sub",     cbson_date_sub },
  { "__mul",     cbson_date_mul },
  { "__div",     cbson_date_div },
  { "__mod",     cbson_date_mod },
  { "__pow",     cbson_date_pow },
  { "__unm",     cbson_date_unm },
  { "__eq",      cbson_date_eq  },
  { "__lt",      cbson_date_lt  },
  { "__le",      cbson_date_le  },
  {"__tostring", cbson_date_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_date_methods[] = {
  {NULL, NULL}
};

