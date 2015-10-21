#include <lua.h>
#include <lauxlib.h>
#include <bson.h>
#include <string.h>
#include <resolv.h>

#ifndef CBSON_MODNAME
#define CBSON_MODNAME   "cbson"
#endif

#ifndef CBSON_VERSION
#define CBSON_VERSION   "1.0"
#endif

#ifndef BSON_MAX_RECURSION
# define BSON_MAX_RECURSION 100
#endif


typedef struct {
    uint32_t count;
    bool keys;
    uint32_t depth;
    lua_State *L;
} cbson_state_t;

static bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data);
static bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data);

static bool cbson_visit_before(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  s->count++;

  if (s->keys)
  {
    lua_pushstring(s->L, key);
  }

  return false;
}

static bool cbson_visit_after(const bson_iter_t *iter, const char *key, void *data)
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

static bool cbson_visit_utf8(const bson_iter_t *iter, const char *key, size_t v_utf8_len, const char *v_utf8, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_utf8, v_utf8_len);

  return false;
}

static bool cbson_visit_int32(const bson_iter_t *iter, const char *key, int32_t v_int32, void *data)
{
  cbson_state_t *s = data;

  lua_pushnumber(s->L, v_int32);

  return false;
}

static bool cbson_visit_int64(const bson_iter_t *iter, const char *key, int64_t v_int64, void *data)
{
  cbson_state_t *s = data;

  lua_pushnumber(s->L, v_int64);

  return false;
}

static bool cbson_visit_double(const bson_iter_t *iter, const char *key, double v_double, void *data)
{
  cbson_state_t *s = data;

  lua_pushnumber(s->L, v_double);

  return false;
}

static bool cbson_visit_undefined(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$undefined", 10);
  lua_pushboolean(s->L, true);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_null(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  lua_pushnil(s->L);

  return false;
}

static bool cbson_visit_bool(const bson_iter_t *iter, const char *key, bool v_bool, void *data)
{
  cbson_state_t *s = data;

  lua_pushboolean(s->L, v_bool);

  return false;
}

static bool cbson_visit_oid(const bson_iter_t *iter, const char *key, const bson_oid_t *oid, void *data)
{
  cbson_state_t *s = data;
  char str[25];

  bson_oid_to_string (oid, str);

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$oid", 4);
  lua_pushlstring(s->L, str, 24);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_regex(const bson_iter_t *iter, const char *key, const  char *v_reg, const char *v_options, void *data)
{
  cbson_state_t *s = data;

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$regex", 6);
  lua_pushstring(s->L, v_reg);
  lua_rawset(s->L, -3);

  lua_pushlstring(s->L, "$options", 8);
  lua_pushstring(s->L, v_options);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_date_time(const bson_iter_t *iter, const char *key, int64_t msec_since_epoch, void *data)
{
  cbson_state_t *s = data;

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$date", 5);
  lua_pushnumber(s->L, msec_since_epoch);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_minkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$minKey", 7);
  lua_pushnumber(s->L, 1);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_maxkey(const bson_iter_t *iter, const char *key, void *data)
{
  cbson_state_t *s = data;

  lua_newtable(s->L);
  lua_pushlstring(s->L, "$maxKey", 7);
  lua_pushnumber(s->L, 1);
  lua_rawset(s->L, -3);

  return false;
}

static bool cbson_visit_code(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_code, v_code_len);

  return false;
}

static bool cbson_visit_symbol(const bson_iter_t *iter, const char *key, size_t v_symbol_len, const char *v_symbol, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_symbol, v_symbol_len);

  return false;
}

static bool cbson_visit_codewscope(const bson_iter_t *iter, const char *key, size_t v_code_len, const char *v_code, const bson_t *v_scope, void *data)
{
  cbson_state_t *s = data;

  lua_pushlstring(s->L, v_code, v_code_len);

  return false;
}

static bool cbson_visit_dbpointer(const bson_iter_t *iter, const char *key, size_t v_collection_len, const char *v_collection, const bson_oid_t *v_oid, void *data)
{
   cbson_state_t *s = data;
   char str[25];

   lua_newtable(s->L);
   lua_pushlstring(s->L, "$ref", 4);
   lua_pushstring(s->L, v_collection);
   lua_rawset(s->L, -3);

   if (v_oid) {
    bson_oid_to_string (v_oid, str);
    lua_pushlstring(s->L, "$id", 3);
    lua_pushstring(s->L, str);
    lua_rawset(s->L, -3);
   }

   return false;
}

static bool cbson_visit_binary(const bson_iter_t *iter, const char *key, bson_subtype_t v_subtype, size_t v_binary_len, const uint8_t *v_binary, void *data)
{
   cbson_state_t *s = data;
   size_t b64_len;
   char *b64;
   char type[3];

   b64_len = (v_binary_len / 3 + 1) * 4 + 1;
   b64 = malloc(b64_len);
   b64_ntop(v_binary, v_binary_len, b64, b64_len);

   sprintf(type,"%02x", v_subtype);

   lua_newtable(s->L);
   lua_pushlstring(s->L, "$type", 5);
   lua_pushstring(s->L, type);
   lua_rawset(s->L, -3);

   lua_pushlstring(s->L, "$binary", 7);
   lua_pushstring(s->L, b64);
   lua_rawset(s->L, -3);
   free(b64);

   return false;
}

static bool cbson_visit_timestamp(const bson_iter_t *iter, const char *key, uint32_t v_timestamp, uint32_t v_increment, void *data)
{
   cbson_state_t *s = data;

   lua_newtable(s->L);
   lua_pushlstring(s->L, "$timestamp", 10);
   
   lua_newtable(s->L);

   lua_pushlstring(s->L, "t", 1);
   lua_pushnumber(s->L, v_timestamp);
   lua_rawset(s->L, -3);

   lua_pushlstring(s->L, "i", 1);
   lua_pushnumber(s->L, v_increment);
   lua_rawset(s->L, -3);

   lua_rawset(s->L, -3);

   return false;
}

static const bson_visitor_t cbson_visitors = {
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
     cbson_visit_minkey
};

static bool cbson_visit_document(const bson_iter_t *iter, const char *key, const bson_t *v_document, void *data){
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

static bool cbson_visit_array(const bson_iter_t *iter, const char *key, const bson_t *v_array, void *data){
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


static int bson_decode(lua_State *L)
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


int luaopen_cbson(lua_State *L)
{
  luaL_Reg cbsonlib[] = {
    { "decode", bson_decode },
    { NULL, NULL }
  };

  /* cbson module table */
  lua_newtable(L);

#if LUA_VERSION_NUM >= 502
  luaL_setfuncs(L,  cbsonlib, 0);
#else
  luaL_register(L, NULL, cbsonlib);
#endif

  lua_pushliteral(L, CBSON_MODNAME);
  lua_setfield(L, -2, "_NAME");
  lua_pushliteral(L, CBSON_VERSION);
  lua_setfield(L, -2, "_VERSION");

  return 1;
}
