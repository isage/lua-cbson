#include <lua.h>
#include <lauxlib.h>
#include <bson.h>
#include <string.h>
#include <resolv.h>
#include <stdint.h>
#include <inttypes.h>

#include "cbson.h"
#include "cbson-decode.h"
#include "cbson-oid.h"
#include "cbson-regex.h"
#include "cbson-binary.h"
#include "cbson-symbol.h"
#include "cbson-code.h"
#include "cbson-timestamp.h"
#include "cbson-ref.h"
#include "cbson-misc.h"
#include "cbson-int.h"
#include "cbson-date.h"
#include "cbson-decimal.h"

typedef struct {
  uint32_t count;
  bool keys;
  uint32_t depth;
  lua_State *L;
} cbson_state_t;

bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data);
bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data);

bool cbson_visit_before(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  s->count++;

  if (s->keys)
  {
    lua_pushstring(s->L, key);
  }

  return false;
}

bool cbson_visit_after(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  if (s->keys)
  {
    lua_rawset(s->L, -3);
  }
  else
  {
    lua_rawseti(s->L, -2, s->count);
  }

  return false;
}

bool cbson_visit_utf8(const bson_iter_t *iter, const char *key, size_t v_utf8_len, const char *v_utf8, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_utf8, v_utf8_len);

  return false;
}

bool cbson_visit_int32(const bson_iter_t *iter, const char *key, int32_t v_int32, void *data)
{
  cbson_state_t *s = data;

  cbson_int64_create(s->L, v_int32);

  return false;
}

bool cbson_visit_int64(const bson_iter_t *iter, const char *key, int64_t v_int64, void *data)
{
  cbson_state_t *s = data;
  
  cbson_int64_create(s->L, v_int64);

  return false;
}

bool cbson_visit_double(const bson_iter_t *iter, const char *key, double v_double, void *data)
{
  cbson_state_t *s = data;

  lua_pushnumber(s->L, v_double);

  return false;
}

bool cbson_visit_undefined(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_undefined_create(s->L);

  return false;
}

bool cbson_visit_null(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

//  lua_pushnil(s->L);
  cbson_null_create(s->L);

  return false;
}

bool cbson_visit_bool(const bson_iter_t *iter, const char *key, bool v_bool, void *data)
{
  cbson_state_t *s = data;

  lua_pushboolean(s->L, v_bool);

  return false;
}

bool cbson_visit_oid(const bson_iter_t *iter, const char *key, const bson_oid_t *oid, void *data)
{
  cbson_state_t *s = data;
  char str[25];

  bson_oid_to_string (oid, str);

  cbson_oid_create(s->L, str);

  return false;
}

bool cbson_visit_regex(const bson_iter_t *iter, const char *key, const  char *v_reg, const char *v_options, void *data)
{
  cbson_state_t *s = data;

  cbson_regex_create(s->L, v_reg, v_options);

  return false;
}

bool cbson_visit_date_time(const bson_iter_t *iter, const char *key, int64_t msec_since_epoch, void *data)
{
  cbson_state_t *s = data;

  cbson_date_create(s->L, msec_since_epoch);

  return false;
}

bool cbson_visit_minkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_minkey_create(s->L);

  return false;
}

bool cbson_visit_maxkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  cbson_maxkey_create(s->L);

  return false;
}

bool cbson_visit_code(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, void *data)
{
  cbson_state_t *s = data;

  cbson_code_create(s->L, v_code);

  return false;
}

bool cbson_visit_symbol(const bson_iter_t *iter, const char *key, size_t v_symbol_len, const char *v_symbol, void *data)
{
  cbson_state_t *s = data;

  cbson_symbol_create(s->L, v_symbol);

  return false;
}

bool cbson_visit_codewscope(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, const bson_t *v_scope, void *data)
{
  cbson_state_t *s = data;

  cbson_codewscope_create(s->L, v_code);

  return false;
}

bool cbson_visit_dbpointer(const bson_iter_t *iter, const char *key, size_t v_collection_len, const char *v_collection, const bson_oid_t *v_oid, void *data)
{
  cbson_state_t *s = data;
  char str[25]="";

  if (v_oid)
  {
    bson_oid_to_string (v_oid, str);
  }

  cbson_ref_create(s->L, v_collection, str);

  return false;
}

bool cbson_visit_binary(const bson_iter_t *iter, const char *key, bson_subtype_t v_subtype, size_t v_binary_len, const uint8_t *v_binary, void *data)
{
  cbson_state_t *s = data;
  cbson_binary_create(s->L, v_subtype, (const char *)v_binary, (unsigned int)v_binary_len);

  return false;
}

bool cbson_visit_decimal128(const bson_iter_t *iter, const char *key, const bson_decimal128_t *v_decimal128, void *data)
{
  cbson_state_t *s = data;
  cbson_decimal_create(s->L, v_decimal128);

  return false;
}

bool cbson_visit_timestamp(const bson_iter_t *iter, const char *key, uint32_t v_timestamp, uint32_t v_increment, void *data)
{
  cbson_state_t *s = data;

  cbson_timestamp_create(s->L, v_timestamp, v_increment);

  return false;
}

const bson_visitor_t cbson_visitors = {
  cbson_visit_before,
  cbson_visit_after,
  NULL, /* visit_corrupt */
  cbson_visit_double,
  cbson_visit_utf8,
  cbson_visit_document,
  cbson_visit_array,
  cbson_visit_binary,
  cbson_visit_undefined,
  cbson_visit_oid,
  cbson_visit_bool,
  cbson_visit_date_time,
  cbson_visit_null,
  cbson_visit_regex,
  cbson_visit_dbpointer,
  cbson_visit_code,
  cbson_visit_symbol,
  cbson_visit_codewscope,
  cbson_visit_int32,
  cbson_visit_timestamp,
  cbson_visit_int64,
  cbson_visit_maxkey,
  cbson_visit_minkey,
  NULL, /* visit_unsupported */
  cbson_visit_decimal128
};

bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data)
{
  bson_iter_t iter_new;
  cbson_state_t *s = data;
  cbson_state_t cs = {0, true};

  if (s->depth >= BSON_MAX_RECURSION)
  {
    lua_pushlstring(s->L, "...", 3);
    return false;
  }

  if (bson_iter_init(&iter_new, v_document))
  {
    lua_newtable(s->L);
    cs.depth = s->depth + 1;
    cs.L = s->L;
    bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
  }
  else
  {
    luaL_error(s->L, "Can't init bson iterator.");
  }

  return false;
}

bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data)
{
  bson_iter_t iter_new;
  cbson_state_t *s = data;
  cbson_state_t cs = {0, false};

  if (s->depth >= BSON_MAX_RECURSION)
  {
    lua_pushlstring(s->L, "...", 3);
    return false;
  }

  if (bson_iter_init(&iter_new, v_array))
  {
    lua_newtable(s->L);
    cs.depth = s->depth + 1;
    cs.L = s->L;
    bson_iter_visit_all(&iter_new, &cbson_visitors, &cs);
  }
  else
  {
    luaL_error(s->L, "Can't init bson iterator.");
  }

  return false;
}


int cbson_decode(lua_State *L)
{
  bson_t *bson;
  cbson_state_t s = {0, true, 0};
  bson_iter_t iter;
  size_t len;

  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &len);

  bson = bson_new_from_data(data, len);

  if (bson)
  {
    lua_pushnil(L);
    if (bson_iter_init(&iter, bson))
    {
      lua_newtable(L);
      s.L = L;
      bson_iter_visit_all(&iter, &cbson_visitors, &s);
    }
    else
    {
      luaL_error(L, "Can't init bson iterator.");
    }
  }
  else
  {
    luaL_error(L, "Can't init bson from data.");
  }

  bson_destroy(bson);
  return 1;
}

int cbson_to_json(lua_State *L)
{
  bson_t *bson;
  size_t len;

  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &len);

  bson = bson_new_from_data(data, len);

  if (bson)
  {
    const char* str = bson_as_json(bson, &len);
    lua_pushlstring(L, str, len);
    bson_free((void *)str);
    bson_destroy(bson);
  }
  else
  {
    luaL_error(L, "Can't init bson from data.");
  }
  return 1;
}

int cbson_to_relaxed_json(lua_State *L)
{
  bson_t *bson;
  size_t len;

  const uint8_t* data = (uint8_t*)luaL_checklstring(L, 1, &len);

  bson = bson_new_from_data(data, len);

  if (bson)
  {
    const char* str = bson_as_relaxed_extended_json(bson, &len);
    lua_pushlstring(L, str, len);
    bson_free((void *)str);
    bson_destroy(bson);
  }
  else
  {
    luaL_error(L, "Can't init bson from data.");
  }
  return 1;
}
