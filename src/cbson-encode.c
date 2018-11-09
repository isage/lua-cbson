#include <lua.h>
#include <lauxlib.h>
#include <bson.h>
#include <string.h>
#include <resolv.h>
#include <stdint.h>
#include <inttypes.h>

#include "cbson.h"
#include "cbson-util.h"
#include "cbson-encode.h"
#include "cbson-oid.h"
#include "cbson-regex.h"
#include "cbson-binary.h"
#include "cbson-symbol.h"
#include "cbson-code.h"
#include "cbson-timestamp.h"
#include "cbson-ref.h"
#include "cbson-misc.h"
#include "cbson-int.h"
#include "cbson-uint.h"
#include "cbson-date.h"
#include "cbson-decimal.h"


// encoding
static int is_array(lua_State *L, int index)
{
  double k;
  int cnt = 0;

  lua_pushvalue(L, index);
  lua_pushnil(L);

  while (lua_next(L, -2))
  {
     if (lua_type(L, -2) == LUA_TNUMBER && (k = lua_tonumber(L, -2)))
     {
       if ((float)(int)(float)(k) == (float)k && k >= 1) // probably int
       {
         lua_pop(L, 1); // pop value
         cnt++;
         continue;
       }
     }

     lua_pop(L, 3); // pop value, key, table copy
     return 0;
  }

  lua_pop(L, 1); // pop table copy
  if (cnt>0) // non-empty array
  {
    return 1;
  }
  else // empty table. treat as map
  {
    return 0;
  }
}

static void iterate_table(lua_State *L, int index, bson_t* bson, int use_keys, int level, const char* firstkey);


void switch_value(lua_State *L, int index, bson_t* bson, int level, const char* key)
{
    switch(lua_type(L, index))
    {
      case LUA_TTABLE:
      {
        int is_a=is_array(L, index);

        if (is_a)
        {
          bson_t child;
          //start array
          BSON_APPEND_ARRAY_BEGIN(bson, key, &child);
          iterate_table(L, index, &child, 0, level+1, NULL);
          bson_append_array_end(bson, &child);
        }
        else
        {
          bson_t child;
          //start map
          BSON_APPEND_DOCUMENT_BEGIN(bson, key, &child);
          iterate_table(L, index, &child, 1, level+1, NULL);
          bson_append_document_end(bson, &child);
        }
        break;
      }

      case LUA_TNIL:
      {
        BSON_APPEND_NULL(bson, key);
        break;
      }

      case LUA_TNUMBER:
      {
        BSON_APPEND_DOUBLE(bson, key, lua_tonumber(L, index));
        break;
      }

      case LUA_TBOOLEAN:
      {
        BSON_APPEND_BOOL(bson, key, lua_toboolean(L, index));
        break;
      }

      case LUA_TSTRING:
      {
        BSON_APPEND_UTF8(bson, key, lua_tostring(L, index));
        break;
      }

      case LUA_TUSERDATA:
      {
        // switch userdata type
        if (luaL_checkudata_ex(L, index, REGEX_METATABLE))
        {
          cbson_regex_t* regex = check_cbson_regex(L, index);

          BSON_APPEND_REGEX (bson, key, regex->regex, regex->options);
        }
        else if (luaL_checkudata_ex(L, index, OID_METATABLE))
        {
          cbson_oid_t* oid = check_cbson_oid(L, index);
          bson_oid_t boid;

          bson_oid_init_from_string (&boid, oid->oid);

          BSON_APPEND_OID (bson, key, &boid);
        }
        else if (luaL_checkudata_ex(L, index, BINARY_METATABLE))
        {
          cbson_binary_t* bin = check_cbson_binary(L, index);

          BSON_APPEND_BINARY(bson, key, bin->type, (const uint8_t*)bin->data, bin->size);
        }
        else if (luaL_checkudata_ex(L, index, SYMBOL_METATABLE))
        {
          cbson_symbol_t* sym = check_cbson_symbol(L, index);

          BSON_APPEND_SYMBOL(bson, key, sym->symbol);
        }
        else if (luaL_checkudata_ex(L, index, REF_METATABLE))
        {
          cbson_ref_t* ref = check_cbson_ref(L, index);

          bson_oid_t boid;
          bson_oid_init_from_string (&boid, ref->id);

          BSON_APPEND_DBPOINTER(bson, key, ref->ref, &boid);
        }
        else if (luaL_checkudata_ex(L, index, MINKEY_METATABLE))
        {
          check_cbson_minkey(L, index);
          BSON_APPEND_MINKEY(bson, key);
        }
        else if (luaL_checkudata_ex(L, index, MAXKEY_METATABLE))
        {
          check_cbson_maxkey(L, index);
          BSON_APPEND_MAXKEY(bson, key);
        }
        else if (luaL_checkudata_ex(L, index, TIMESTAMP_METATABLE))
        {
          cbson_timestamp_t* time = check_cbson_timestamp(L, index);
          BSON_APPEND_TIMESTAMP(bson, key, time->timestamp, time->increment);
        }
        else if (luaL_checkudata_ex(L, index, INT64_METATABLE) || luaL_checkudata_ex(L, index, UINT64_METATABLE))
        {
          cbson_int64_t i = cbson_int64_check(L, index);
          if (i < INT32_MIN || i > INT32_MAX)
          {
            BSON_APPEND_INT64(bson, key, i);
          }
          else
          {
            BSON_APPEND_INT32(bson, key, (int32_t)i);
          }
        }
        else if (luaL_checkudata_ex(L, index, CODE_METATABLE))
        {
          cbson_code_t* code = check_cbson_code(L, index);
          BSON_APPEND_CODE(bson, key, code->code);
        }
        else if (luaL_checkudata_ex(L, index, CODEWSCOPE_METATABLE))
        {
          cbson_codewscope_t* code = check_cbson_codewscope(L, index);
          BSON_APPEND_CODE_WITH_SCOPE(bson, key, code->code, NULL);
        }
        else if (luaL_checkudata_ex(L, index, UNDEFINED_METATABLE))
        {
          check_cbson_undefined(L, index);
          BSON_APPEND_UNDEFINED(bson, key);
        }
        else if (luaL_checkudata_ex(L, index, CBNULL_METATABLE))
        {
          check_cbson_null(L, index);
          BSON_APPEND_NULL(bson, key);
        }
        else if (luaL_checkudata_ex(L, index, DATE_METATABLE))
        {
          BSON_APPEND_DATE_TIME(bson, key, cbson_date_check(L, index));
        }
        else if (luaL_checkudata_ex(L, index, ARRAY_METATABLE))
        {
          bson_t child;
          BSON_APPEND_ARRAY_BEGIN(bson, key, &child);
          bson_append_array_end(bson, &child);
        }
        else if (luaL_checkudata_ex(L, index,  DECIMAL_METATABLE))
        {
          cbson_decimal_t * dec = check_cbson_decimal(L, index);
          BSON_APPEND_DECIMAL128(bson, key, &dec->dec);
        }
        break;
      }
      case LUA_TFUNCTION:
      case LUA_TTHREAD:
      case LUA_TLIGHTUSERDATA:
      default:
        break; // or bail out?
    }

}

static void iterate_table(lua_State *L, int index, bson_t* bson, int use_keys, int level, const char* firstkey)
{

  if (firstkey!=NULL)
  {
    lua_getfield(L, index, firstkey);
    switch_value(L, -1, bson, level, firstkey);
    lua_pop(L,1);
  }

  lua_pushvalue(L, index);
  // stack: -1 => table

  lua_pushnil(L);
  // stack: -1 => nil; -2 => table

  int k=0;


  while (lua_next(L, -2))
  {
    // stack: -1 => value; -2 => key; -3 => table
    lua_pushvalue(L, -2);
    // stack: -1 => key; -2 => value; -3 => key; -4 => table

    const char *key;

    if (!use_keys)
    {
      char ckey[512];
      snprintf(ckey, 512, "%d", k);
      key = ckey;
    }
    else
    {
      key = lua_tostring(L, -1);
    }

    if (level == 0 && firstkey != NULL && strcmp(firstkey, key) == 0)
    {
      lua_pop(L,2);
      continue;
    }

    switch_value(L, -2, bson, level, key);

    lua_pop(L, 2);
    // stack: -1 => key; -2 => table
    k++;
  }
  // stack: -1 => table
  lua_pop(L, 1);
}

int cbson_encode(lua_State *L)
{
  bson_t bson = BSON_INITIALIZER;

  luaL_checktype(L, 1, LUA_TTABLE);

  iterate_table(L, 1,  &bson, 1, 0, NULL);

  const uint8_t* data=bson_get_data(&bson);
  lua_pushlstring(L, (const char*)data, bson.len);
  bson_destroy(&bson);
  return 1;
}

int cbson_encode_first(lua_State *L)
{
  bson_t bson = BSON_INITIALIZER;

  const char* key = luaL_checkstring(L,1);

  luaL_checktype(L, 2, LUA_TTABLE);

  iterate_table(L, 2,  &bson, 1, 0, key);

  const uint8_t* data=bson_get_data(&bson);
  lua_pushlstring(L, (const char*)data, bson.len);
  bson_destroy(&bson);
  return 1;
}

int cbson_from_json(lua_State *L)
{
  bson_t *bson;
  size_t len;
  bson_error_t error;

  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &len);

  bson = bson_new_from_json(data, len, &error);

  if (bson)
  {
    data = bson_get_data(bson);
    lua_pushlstring(L, (const char*)data, bson->len);
    bson_destroy(bson);
  }
  else
  {
    luaL_error(L, error.message);
  }
  return 1;
}
