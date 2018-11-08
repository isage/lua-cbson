#include <lauxlib.h>
#include <inttypes.h>
#include <stdlib.h>

#include "cbson.h"
#include "cbson-uint.h"
#include "cbson-int.h"
#include "cbson-date.h"
#include "cbson-util.h"
#include "intpow.h"

enum {
  ENDIAN_LE,
  ENDIAN_BE
};

uint64_t cbson_uint64_check(lua_State *L, int index)
{
  if (lua_isuserdata(L, index) && (luaL_checkudata_ex(L, index, INT64_METATABLE) || luaL_checkudata_ex(L, index, UINT64_METATABLE) || luaL_checkudata_ex(L, index, DATE_METATABLE)))
  {
    return *(uint64_t*)lua_touserdata(L, index);
  }
  else if (lua_isnumber(L, index))
  {
    return lua_tonumber(L, index);
  }
  else if (lua_isstring(L, index))
  {
    return strtoull(lua_tostring(L, index), NULL, 10);
  }
  else
  {
    lua_pushstring(L, "Invalid operand. Expected 'cbson.int', 'cbson.uint', 'cbson.date', 'string' or 'number'");
    lua_error(L);
    return 0;
  }
}

int cbson_uint64_create(lua_State* L, uint64_t val)
{
  cbson_uint64_t* ud = lua_newuserdata(L, sizeof(cbson_uint64_t));

  *ud = val;

  luaL_getmetatable(L, UINT64_METATABLE);
  lua_setmetatable(L, -2);
  return 1;
}

int cbson_uint64_new(lua_State* L)
{
  return cbson_uint64_create(L, cbson_uint64_check(L,1));
}

int cbson_uint64_number(lua_State* L)
{
  cbson_int64_t a = cbson_uint64_check(L, 1);

  lua_pushnumber(L, a);
  return 1;
}


int cbson_uint64_tostring(lua_State* L)
{
  cbson_uint64_t a = cbson_uint64_check(L, 1);

  char buffer[sizeof(uint64_t)*8+1];
  sprintf(buffer, "%"PRIu64, a);

  lua_pushstring(L, buffer);
  return 1;
}

int cbson_uint64_add (lua_State* L) { return cbson_uint64_create( L, cbson_uint64_check(L,1) + cbson_uint64_check(L,2) ); }
int cbson_uint64_sub (lua_State* L) { return cbson_uint64_create( L, cbson_uint64_check(L,1) - cbson_uint64_check(L,2) ); }
int cbson_uint64_mul (lua_State* L) { return cbson_uint64_create( L, cbson_uint64_check(L,1) * cbson_uint64_check(L,2) ); }
int cbson_uint64_div (lua_State* L) { return cbson_uint64_create( L, cbson_uint64_check(L,1) / cbson_uint64_check(L,2) ); }
int cbson_uint64_mod (lua_State* L) { return cbson_uint64_create( L, cbson_uint64_check(L,1) % cbson_uint64_check(L,2) ); }
int cbson_uint64_pow (lua_State* L) { return cbson_uint64_create( L, cbson_pow( cbson_uint64_check(L,1), cbson_uint64_check(L,2) ) ); }
int cbson_uint64_unm (lua_State* L) { return cbson_uint64_create( L, -cbson_uint64_check(L,1) ); }
int cbson_uint64_eq  (lua_State* L) { lua_pushboolean( L, cbson_uint64_check(L,1) == cbson_uint64_check(L,2) ); return 1; }
int cbson_uint64_lt  (lua_State* L) { lua_pushboolean( L, cbson_uint64_check(L,1) <  cbson_uint64_check(L,2) ); return 1; }
int cbson_uint64_le  (lua_State* L) { lua_pushboolean( L, cbson_uint64_check(L,1) <= cbson_uint64_check(L,2) ); return 1; }


int cbson_uint64_from_raw(lua_State* L)
{
  size_t length;
  const char* data = lua_tolstring(L, 1, &length);
  int endian = luaL_optnumber(L, 2, ENDIAN_LE);

  if ( length > 8 || length <= 0 )
  {
    lua_pushstring(L, "Invalid length");
    lua_error(L);
    return 0;
  }

  cbson_uint64_t result=0;
  int i;
  if (endian==ENDIAN_LE)
  {
    result = (uint8_t)data[length-1];
    for (i=length-2; i>=0; i--)
    {
      result=result*256+(uint8_t)data[i];
    }
  }
  else
  {
    result = (uint8_t)data[0];
    for (i=0; i<length; i++)
    {
      result=result*256+(uint8_t)data[i];
    }
  }

  return cbson_uint64_create(L, result);
  
}

int cbson_uint64_to_raw(lua_State* L)
{
  cbson_uint64_t a = cbson_uint64_check(L, 1);
  int length = luaL_optnumber(L, 2, 4);
  int endian = luaL_optnumber(L, 3, ENDIAN_LE);
  if ( length > 8 || length <= 0 ) length=8;

  uint8_t out[8];

  int i;

  if (endian==ENDIAN_LE)
  {
    for (i=0; i<length; i++)
    {
      out[i] = a;
      a = a / 256;
    }
  }
  else
  {
    for (i=length-1; i>=0; i--)
    {
      out[i] = a;
      a = a / 256;
    }
  }

  switch (length)
  {
    case 8:
      lua_pushlstring(L, (const char*)out, 8);
      break;
    case 4:
      lua_pushlstring(L, (const char*)out, 4);
      break;
    case 2:
      lua_pushlstring(L, (const char*)out, 2);
      break;
    case 1:
      lua_pushlstring(L, (const char*)out, 1);
      break;
    default:
      lua_pushstring(L, "Invalid length");
      lua_error(L);
      return 0;
      break;
  }
  return 1;
}


const struct luaL_Reg cbson_uint64_meta[] = {
  { "__add",     cbson_uint64_add },
  { "__sub",     cbson_uint64_sub },
  { "__mul",     cbson_uint64_mul },
  { "__div",     cbson_uint64_div },
  { "__mod",     cbson_uint64_mod },
  { "__pow",     cbson_uint64_pow },
  { "__unm",     cbson_uint64_unm },
  { "__eq",      cbson_uint64_eq  },
  { "__lt",      cbson_uint64_lt  },
  { "__le",      cbson_uint64_le  },
  {"__tostring", cbson_uint64_tostring},
  {NULL, NULL}
};

const struct luaL_Reg cbson_uint64_methods[] = {
  {"number", cbson_uint64_number},
  {NULL, NULL}
};
